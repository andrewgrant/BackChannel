// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Utils/BackChannelThreadedConnection.h"


FBackChannelThreadedConnection::FBackChannelThreadedConnection()
{
	RawConnection = nullptr;
	RawDelegate = nullptr;
}

FBackChannelThreadedConnection::~FBackChannelThreadedConnection()
{
	Close();
}

/* Close the connection */
void FBackChannelThreadedConnection::Close()
{
	Stop();

	if (RawConnection)
	{
		RawConnection->Close();
		RawConnection = nullptr;
	}
	RawDelegate = nullptr;

	Connection = nullptr;
	Delegate = nullptr;
}

bool FBackChannelThreadedConnection::IsRunning() const
{
	return bIsRunning;
}

void FBackChannelThreadedConnection::Start(TSharedRef<IBackChannelConnection> InConnection,
	TSharedRef<IBackChannelThreadedConnectionDelegate> InDelegate,
	EThreadPriority Priority /*= TPri_Normal*/)
{
	Close();

	Connection = InConnection;
	Delegate = InDelegate;

	// these are accessed via threads
	RawConnection = &InConnection.Get();
	RawDelegate = &InDelegate.Get();

	bIsRunning = true;
	bExitRequested = false;

	FRunnableThread::Create(this, TEXT("FBackChannelSocketThread"), 32 * 1024, Priority);
}

uint32 FBackChannelThreadedConnection::Run()
{
	FScopeLock RunningLock(&RunningCS);

	while (bExitRequested == false)
	{
		if (IsListening())
		{
			bool Success = WaitForConnection(0, [this](TSharedRef<IBackChannelConnection> NewConnection) {
				return RawDelegate->OnIncomingConnection(NewConnection);
			});


		}

	}

	bIsRunning = false;

	return 0;
}

void FBackChannelThreadedConnection::Stop()
{
	bExitRequested = true;

	if (IsRunning())
	{
		FScopeLock RunLock(&RunningCS);
	}

	bExitRequested = false;
}



bool FBackChannelThreadedConnection::Connect(const TCHAR* InEndPoint)
{
	UE_LOG(LogBackChannel, Error, TEXT("Calling Connect on ThreadedConnection is invalid!"));
	return false;
}

bool FBackChannelThreadedConnection::Listen(const int16 Port)
{
	UE_LOG(LogBackChannel, Error, TEXT("Calling Listen on ThreadedConnection is invalid!"));
	return false;
}

/* Attach this connection to the provided socket */
bool FBackChannelThreadedConnection::Attach(FSocket* InSocket)
{
	UE_LOG(LogBackChannel, Error, TEXT("Calling Attach on ThreadedConnection is invalid!"));
	return false;
}

/* Return our current connection state */
bool FBackChannelThreadedConnection::IsConnected() const
{
	check(RawConnection);
	return RawConnection->IsConnected();
}

/* Returns true if this connection is currently listening for incoming connections */
bool FBackChannelThreadedConnection::IsListening() const
{
	check(RawConnection);
	return RawConnection->IsListening();
}

/* Return a string describing this connection */
FString	FBackChannelThreadedConnection::GetDescription() const 
{
	check(RawConnection);
	return RawConnection->GetDescription();
}

/* Return the underlying socket (if any) for this connection */
FSocket* FBackChannelThreadedConnection::GetSocket()  
{ 
	check(RawConnection);
	FScopeLock Lock(&SocketMutex);
	return RawConnection->GetSocket();
}

bool FBackChannelThreadedConnection::WaitForConnection(double InTimeout, TFunction<bool(TSharedRef<IBackChannelConnection>)> InDelegate)
{
	check(RawConnection);
	FScopeLock Lock(&SocketMutex);
	return RawConnection->WaitForConnection(InTimeout, InDelegate);
}

/* Send data over our connection. The number of bytes sent is returned */
int32 FBackChannelThreadedConnection::SendData(const void* InData, const int32 InSize)
{
	check(RawConnection);
	FScopeLock Lock(&SocketMutex);
	return RawConnection->SendData(InData, InSize);
}

/* Read data from our remote connection. The number of bytes received is returned */
int32 FBackChannelThreadedConnection::ReceiveData(void* OutBuffer, const int32 BufferSize)
{
	check(RawConnection);
	FScopeLock Lock(&SocketMutex);
	return RawConnection->ReceiveData(OutBuffer, BufferSize);
}
