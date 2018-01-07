// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "BackChannel/Transport/IBackChannelConnection.h"
#include "HAL/Runnable.h"
#include "EngineMinimal.h"

class FSocket;

class IBackChannelThreadedConnectionDelegate
{
public:

	virtual bool OnIncomingConnection(TSharedRef<IBackChannelConnection>& NewConnection) { return false; }

	virtual void OnRecvData(TSharedPtr<IBackChannelConnection>& NewConnection) { return; }

};

/**
* BackChannelClient implementation.
*
*/
class BACKCHANNEL_API FBackChannelThreadedConnection : public IBackChannelConnection, public FRunnable, public TSharedFromThis<FBackChannelThreadedConnection>
{
public:

	FBackChannelThreadedConnection();
	~FBackChannelThreadedConnection();

	void Start(TSharedRef<IBackChannelConnection> InConnection,
		TSharedRef<IBackChannelThreadedConnectionDelegate> InDelegate,
		EThreadPriority Priority = TPri_Normal);

	virtual uint32 Run() override;

	virtual void Stop() override;

	bool IsRunning() const;

	/* Return our current connection state */
	virtual bool IsConnected() const override;

	/* Returns true if this connection is currently listening for incoming connections */
	virtual bool IsListening() const override;

	/* Return a string describing this connection */
	virtual FString	GetDescription() const override;

	/* Return the underlying socket (if any) for this connection */
	virtual FSocket* GetSocket() override;

	/* Connect to the specified end-point */
	virtual bool Connect(const TCHAR* InEndPoint) override;

	virtual bool Listen(const int16 Port) override;

	/* Close the connection */
	virtual void Close() override;

	virtual bool WaitForConnection(double InTimeout, TFunction<bool(TSharedRef<IBackChannelConnection>)> InDelegate) override;

	/* Attach this connection to the provided socket */
	bool Attach(FSocket* InSocket);

	/* Send data over our connection. The number of bytes sent is returned */
	virtual int32 SendData(const void* InData, const int32 InSize) override;

	/* Read data from our remote connection. The number of bytes received is returned */
	virtual int32 ReceiveData(void* OutBuffer, const int32 BufferSize) override;


private:

	IBackChannelConnection*						RawConnection;
	IBackChannelThreadedConnectionDelegate*		RawDelegate;

	TSharedPtr<IBackChannelConnection>					Connection;
	TWeakPtr<IBackChannelThreadedConnectionDelegate>	Delegate;
	
	FThreadSafeBool		bExitRequested;
	FThreadSafeBool		bIsRunning;

	FCriticalSection	RunningCS;
	FCriticalSection	SocketMutex;

};
