// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"
#include "Common/TcpSocketBuilder.h"

int32 GBackChannelLogPackets = 1;
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
	IsListener = false;
}

FBackChannelConnection::~FBackChannelConnection()
{
	if (Socket)
	{
		Close();
	}
}

bool FBackChannelConnection::IsConnected() const
{
	FBackChannelConnection* NonConstThis = const_cast<FBackChannelConnection*>(this);
	FScopeLock Lock(&NonConstThis->SocketMutex);
	return Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

bool FBackChannelConnection::IsListening() const
{
	return IsListener;
}

FString	FBackChannelConnection::GetDescription() const
{
	FBackChannelConnection* NonConstThis = const_cast<FBackChannelConnection*>(this);
	FScopeLock Lock(&NonConstThis->SocketMutex);

	return Socket ? Socket->GetDescription() : TEXT("No Socket");
}

void FBackChannelConnection::Close()
{
	FScopeLock Lock(&SocketMutex);
	if (Socket)
	{
		UE_LOG(LogBackChannel, Log, TEXT("Closing connection %s"), *Socket->GetDescription());
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void FBackChannelConnection::CloseWithError(const TCHAR* Error)
{
	const TCHAR* SocketErr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(SE_GET_LAST_ERROR_CODE);
	FString SockDesc = Socket != nullptr ? Socket->GetDescription() : TEXT("(No Socket)");

	UE_LOG(LogBackChannel, Error, TEXT("%s, Err: %s, Socket:%s"), Error, SocketErr, *SockDesc);

	Close();
}

bool FBackChannelConnection::Connect(const TCHAR* InEndPoint)
{
	FScopeLock Lock(&SocketMutex);

	if (IsConnected())
	{
		Close();
	}

	IsAttemptingConnection = true;

	FString LocalEndPoint = InEndPoint;

	FString Description = FString::Printf(TEXT("%s"), InEndPoint);

	FSocket* NewSocket = FTcpSocketBuilder(*Description)
		.AsReusable();

	if (NewSocket)
	{
		FIPv4Endpoint EndPoint;
		FIPv4Endpoint::Parse(LocalEndPoint, EndPoint);

		if (NewSocket->Connect(*EndPoint.ToInternetAddr()))
		{
			UE_LOG(LogBackChannel, Log, TEXT("Opening connection to %s (localport: %d)"), *NewSocket->GetDescription(), NewSocket->GetPortNo());
			Attach(NewSocket);
		}
		else
		{
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(NewSocket);
			CloseWithError(*FString::Printf(TEXT("Failed to open connection to %s."), InEndPoint));
		}
	}

	return Socket != nullptr;
}

bool FBackChannelConnection::Listen(const int16 Port)
{
	FScopeLock Lock(&SocketMutex);

	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);

	FSocket* NewSocket = FTcpSocketBuilder(TEXT("FBackChannelConnection ListenSocket"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8)
		.WithSendBufferSize(2 * 1024 * 1024);

	if (NewSocket == nullptr)
	{
		CloseWithError(*FString::Printf(TEXT("Failed to start listening on port %d"), Port));
	}
	else
	{
		Attach(NewSocket);
		IsListener = true;
	}

	return NewSocket != nullptr;
}

bool FBackChannelConnection::WaitForConnection(double InTimeout, TFunction<bool(TSharedRef<IBackChannelConnection>)> InDelegate)
{
	FScopeLock Lock(&SocketMutex);

	FTimespan SleepTime = FTimespan(0, 0, InTimeout);

	// handle incoming connections

	bool Pending = false;

	bool Success = Socket->WaitForPendingConnection(Pending, SleepTime);

	if (Success)
	{
		if (Pending)
		{
			UE_LOG(LogBackChannel, Log, TEXT("Found connection on %s"), *Socket->GetDescription());

			if (IsListener == false)
			{
				InDelegate(AsShared());
			}
			else
			{
				TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

				FSocket* ConnectionSocket = Socket->Accept(*RemoteAddress, TEXT("Incoming Connection"));

				if (ConnectionSocket != nullptr)
				{
					TSharedRef<FBackChannelConnection> BCConnection = MakeShareable(new FBackChannelConnection);
					BCConnection->Attach(ConnectionSocket);

					if (InDelegate(BCConnection) == false)
					{
						UE_LOG(LogBackChannel, Warning, TEXT("Calling code rejected connection on %s"), *Socket->GetDescription());
						BCConnection->Close();
						Pending = false;
					}
					else
					{
						UE_LOG(LogBackChannel, Warning, TEXT("Accepted connection on %s"), *Socket->GetDescription());
					}
				}
			}
		}
	}
	else
	{
		CloseWithError(TEXT("Connection Failed"));
	}

	return Success;
}

bool FBackChannelConnection::Attach(FSocket* InSocket)
{
	FScopeLock Lock(&SocketMutex);

	check(Socket == nullptr);

	Socket = InSocket;
	return true;
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
			
			UE_CLOG(GBackChannelLogErrors, LogBackChannel, Error, TEXT("Failed to send %d bytes of data to %s. Err: %s"), BytesSent, *GetDescription(), SocketErr);
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

	// todo - close connection on certain errors
	if (BytesRead > 0)
	{
		UE_CLOG(GBackChannelLogPackets, LogBackChannel, Log, TEXT("Received %d bytes of data"), BytesRead);
	}
	return BytesRead;
}
