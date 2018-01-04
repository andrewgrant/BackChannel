// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "BackChannel/Transport/IBackChannelConnection.h"
#include "EngineMinimal.h"

class FSocket;

/**
* BackChannelClient implementation.
*
*/
class BACKCHANNEL_API FBackChannelConnection : public IBackChannelConnection
{
public:

	FBackChannelConnection();
	~FBackChannelConnection();

	/* Connect to the specified end-point */
	virtual void Connect(const TCHAR* InEndPoint, double InTimeout, TFunction<void()> InDelegate) override;

	/* Attach this connection to the provided socket */
	bool Attach(FSocket* InSocket, const FString& InDescription);

	/* Close the connection */
	virtual void Close() override;

	/* Return our current connection state */
	virtual bool IsConnected() const;

	/* Send data over our connection. The number of bytes sent is returned */
	virtual int32 SendData(const void* InData, const int32 InSize) override;

	/* Read data from our remote connection. The number of bytes received is returned */
	virtual int32 ReceiveData(void* OutBuffer, const int32 BufferSize) override;

	/* Return a string describing this connection */
	virtual FString	GetDescription() const override { return Description; }

	/* Return the underlying socket (if any) for this connection */
	virtual FSocket* GetSocket() override { return Socket; }

private:

	FThreadSafeBool			IsAttemptingConnection;
	FCriticalSection		SocketMutex;
	FSocket*				Socket;
	FString					Description;
};
