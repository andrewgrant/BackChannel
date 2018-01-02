// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"
#include "Common/TcpSocketBuilder.h"

FBackChannelConnection::FBackChannelConnection()
{
	Socket = nullptr;
}

FBackChannelConnection::~FBackChannelConnection()
{
	if (Socket)
	{
		Close();
	}
}

bool FBackChannelConnection::Connect(const TCHAR* InEndPoint)
{
	FSocket* NewSocket = FTcpSocketBuilder(TEXT("BackChannel Client"));

	if (NewSocket == nullptr)
	{
		return false;
	}

	FIPv4Endpoint EndPoint;
	FIPv4Endpoint::Parse(InEndPoint, EndPoint);

	//NewSocket->SetNonBlocking(false);

	if (!NewSocket->Connect(*EndPoint.ToInternetAddr()))
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(NewSocket);

		return false;
	}

	bool bFoundConnecton(false);
	FTimespan WaitTime = FTimespan(0, 0, 5);

	if (NewSocket->WaitForPendingConnection(bFoundConnecton, WaitTime))
	{
		if (bFoundConnecton)
		{
			return Attach(NewSocket);
		}
	}

	//NewSocket->SetNonBlocking(true);

	return false;
}

bool FBackChannelConnection::Attach(FSocket* InSocket)
{
	Socket = InSocket;
	return true;
}

void FBackChannelConnection::Close()
{
	if (Socket)
	{
		Socket->Close();
		Socket = nullptr;
	}
}

bool FBackChannelConnection::IsConnected() const
{
	return Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

int32 FBackChannelConnection::SendData(const void* InData, const int32 InSize)
{
	if (!Socket)
	{
		return -1;
	}

	int32 BytesSent(0);
	Socket->Send((const uint8*)InData, InSize, BytesSent);
	return BytesSent;
}

int32 FBackChannelConnection::ReceiveData(void* OutBuffer, const int32 BufferSize)
{
	if (!Socket)
	{
		return 0;
	}

	int32 BytesRead(0);
	Socket->Recv((uint8*)OutBuffer, BufferSize, BytesRead, ESocketReceiveFlags::None);
	return BytesRead;
}
