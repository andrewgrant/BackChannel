// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Protocol/OSC/BackChannelOSCConnection.h"
#include "BackChannel/Protocol/OSC/BackChannelOSC.h"

FBackChannelOSCConnection::FBackChannelOSCConnection(TSharedRef<IBackChannelConnection> InConnection)
	: Connection(InConnection)
{

}

FBackChannelOSCConnection::~FBackChannelOSCConnection()
{
	if (IsRunning)
	{
		Stop();
	}

	Connection->Close();
}

bool FBackChannelOSCConnection::Start()
{
	check(IsRunning == false);

	ExitRequested = false;

	FRunnableThread* Thread = FRunnableThread::Create(this, TEXT("OSCHostConnection"));

	if (Thread)
	{
		IsRunning = true;
	}

	UE_LOG(LogBackChannel, Log, TEXT("Started OSS Connection"));

	return Thread != nullptr;
}

bool FBackChannelOSCConnection::Init()
{
	return true;
}

uint32 FBackChannelOSCConnection::Run()
{
	const int32 kDefaultBufferSize = 4096;

	TArray<uint8> Buffer;
	Buffer.AddUninitialized(kDefaultBufferSize);

	// OSC connections expect a size followed by payload for TCP connections
	int32 ExpectedDataSize = 4;

	while (ExitRequested == false)
	{
		FPlatformProcess::SleepNoStats(0);
		
		int32 Received = Connection->ReceiveData(Buffer.GetData(), ExpectedDataSize);

		// TODO - move to blocking sockets?
		if (Received)
		{
			if (Received != ExpectedDataSize)
			{
				UE_LOG(LogBackChannel, Error, TEXT("Received unexpected data. Will skip all data till next packet"), Received);
				ExpectedDataSize = 4;
			}
			else
			{
				if (ExpectedDataSize == 4)
				{
					int32 Size(0);
					FMemory::Memcpy(&Size, Buffer.GetData(), sizeof(int32));

					if (Size > Buffer.Num())
					{
						Buffer.AddUninitialized(Size - Buffer.Num());
					}

					ExpectedDataSize = Size;
				}
				else
				{
					// read packet
					TSharedPtr<FBackChannelOSCPacket> Packet = FBackChannelOSCPacket::CreateFromBuffer(Buffer.GetData(), ExpectedDataSize);

					if (Packet.IsValid())
					{
						FScopeLock Lock(&ReceivedPacketsMutex);
						ReceivedPackets.Add(Packet);
					}

					ExpectedDataSize = 4;
				}
			}
		}
	}

	IsRunning = false;
	return 0;
}

void FBackChannelOSCConnection::Stop()
{
	ExitRequested = true;

	while (IsRunning)
	{
		FPlatformProcess::SleepNoStats(0);
	}
}

bool FBackChannelOSCConnection::IsConnected() const
{
	return Connection->IsConnected();
}

bool FBackChannelOSCConnection::SendPacket(FBackChannelOSCPacket& Packet)
{
	TArray<uint8> Data = Packet.WriteToBuffer();
	return SendPacketData(Data.GetData(), Data.Num());
}

bool FBackChannelOSCConnection::SendPacketData(const void* Data, const int32 DataLen)
{
	// write size
	int32 Sent = 0;

	// TODO - differentiate between TCP / UDP 
	if (DataLen > 0)
	{
		Connection->SendData(&DataLen, sizeof(DataLen));
		Sent = Connection->SendData(Data, DataLen);

		if (Sent == DataLen)
		{
			UE_LOG(LogBackChannel, Verbose, TEXT("Sent %d bytes of data"), DataLen);
		}
		else
		{
			UE_LOG(LogBackChannel, Warning, TEXT("Only sent %d bytes of %d packet"), Sent, DataLen);
		}
	}

	return Sent > 0;
}


FBackChannelOSCDispatch& FBackChannelOSCConnection::GetDispatchMap()
{
	return DispatchMap;
}

void FBackChannelOSCConnection::DispatchMessages()
{
	FScopeLock Lock(&ReceivedPacketsMutex);

	for (auto& Packet : ReceivedPackets)
	{
		if (Packet->GetType() == OSCPacketType::Message)
		{
			TSharedPtr<FBackChannelOSCMessage> Msg = StaticCastSharedPtr<FBackChannelOSCMessage>(Packet);

			DispatchMap.DispatchMessage(*Msg.Get());
		}
	}

	ReceivedPackets.Empty();
}