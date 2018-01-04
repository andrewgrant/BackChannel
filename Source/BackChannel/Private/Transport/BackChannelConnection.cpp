// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"
#include "Common/TcpSocketBuilder.h"

int32 GBackChannelLogPackets = 0;
static FAutoConsoleVariableRef BCCVarLogPackets(
	TEXT("backchannel.logpackets"), GBackChannelLogPackets,
	TEXT("Logs incoming packets"),
	ECVF_Default);

int32 GBackChannelLogErrors = 1;
static FAutoConsoleVariableRef BCCVarLogErrors(
	TEXT("backchannel.logerrors"), GBackChannelLogErrors,
	TEXT("Logs packet errors"),
	ECVF_Default);

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

void FBackChannelConnection::Connect(const TCHAR* InEndPoint, double InTimeout, TFunction<void()> InDelegate)
{
	while (IsAttemptingConnection)
	{
		FPlatformProcess::SleepNoStats(0);
	}

	if (IsConnected())
	{
		Close();
	}

	IsAttemptingConnection = true;

	FString LocalEndPoint = InEndPoint;

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, LocalEndPoint, InTimeout, InDelegate]()
	{
		bool bSuccess = false;

		FSocket* NewSocket = FTcpSocketBuilder(TEXT("BackChannel Client"));

		if (NewSocket)
		{
			FIPv4Endpoint EndPoint;
			FIPv4Endpoint::Parse(LocalEndPoint, EndPoint);

			if (NewSocket->Connect(*EndPoint.ToInternetAddr()))
			{
				bool bFoundConnecton(false);
				FTimespan WaitTime = FTimespan(0, 0, (int)InTimeout);

				NewSocket->WaitForPendingConnection(bFoundConnecton, WaitTime);

				if (bFoundConnecton)
				{
					bSuccess = true;
				}
			}

			if (bSuccess)
			{
				FString NewDescription = FString::Printf(TEXT("%s (localport:%d)"), *LocalEndPoint, NewSocket->GetPortNo());
				Attach(NewSocket, NewDescription);
			}
			else
			{
				ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(NewSocket);
			}
		}

		IsAttemptingConnection = false;

		AsyncTask(ENamedThreads::GameThread, [InDelegate]() {
			InDelegate();
		});
	

	});

	return;
}

bool FBackChannelConnection::Attach(FSocket* InSocket, const FString& InDescription)
{
	FScopeLock Lock(&SocketMutex);
	Socket = InSocket;
	Description = InDescription;
	return true;
}

void FBackChannelConnection::Close()
{
	FScopeLock Lock(&SocketMutex);
	if (Socket)
	{
		Socket->Close();
		Socket = nullptr;
	}
}

bool FBackChannelConnection::IsConnected() const
{
	FBackChannelConnection* NonConstThis = const_cast<FBackChannelConnection*>(this);
	FScopeLock Lock(&NonConstThis->SocketMutex);
	return Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

int32 FBackChannelConnection::SendData(const void* InData, const int32 InSize)
{
	FScopeLock Lock(&SocketMutex);
	if (!Socket)
	{
		return -1;
	}

	int32 BytesSent(0);
	Socket->Send((const uint8*)InData, InSize, BytesSent);

	if (BytesSent == -1)
	{
		if (GBackChannelLogErrors)
		{
			const TCHAR* SocketErr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(SE_GET_LAST_ERROR_CODE);
			
			UE_CLOG(GBackChannelLogErrors, LogBackChannel, Error, TEXT("Failed to send %d bytes of data to %s. Err: %s"), BytesSent, *Description, SocketErr);
		}
	}
	else
	{
		UE_CLOG(GBackChannelLogPackets, LogBackChannel, Log, TEXT("Sent %d bytes of data"), BytesSent);
	}
	return BytesSent;
}

int32 FBackChannelConnection::ReceiveData(void* OutBuffer, const int32 BufferSize)
{
	FScopeLock Lock(&SocketMutex);
	if (!Socket)
	{
		return 0;
	}

	int32 BytesRead(0);
	Socket->Recv((uint8*)OutBuffer, BufferSize, BytesRead, ESocketReceiveFlags::None);

	if (BytesRead > 0)
	{
		UE_CLOG(GBackChannelLogPackets, LogBackChannel, Log, TEXT("Received %d bytes of data"), BytesRead);
	}
	return BytesRead;
}
