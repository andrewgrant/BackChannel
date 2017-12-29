// Copyright 2017 Andrew Grant
// Licensed under BSD License 2.0. 
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
	bool Connect(const TCHAR* InEndPoint) override;

	/* Attach this connection to the provided socket */
	bool Attach(FSocket* InSocket);

	/* Close the connection */
	virtual void Close() override;

	/* Return our current connection state */
	virtual bool IsConnected() const;

	/* Send data over our connection. The number of bytes sent is returned */
	virtual int32 SendData(const void* InData, const int32 InSize) override;

	/* Read data from our remote connection. The number of bytes received is returned */
	virtual int32 ReceiveData(void* OutBuffer, const int32 BufferSize) override;

private:

	FSocket*				Socket;
};
