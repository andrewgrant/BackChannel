// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "BackChannel/Transport/IBackChannelConnection.h"
#include "EngineMinimal.h"

class FSocket;

/**
* BackChannelClient implementation.
*
*/
class FBackChannelConnection : public IBackChannelConnection
{
public:

	FBackChannelConnection();
	~FBackChannelConnection();

	bool Connect(const TCHAR* InEndPoint) override;

	bool Attach(FSocket* InSocket);

	virtual void Close() override;

	virtual int32 SendData(const void* InData, const int32 InSize) override;

	virtual int32 ReceiveData(void* OutBuffer, const int32 BufferSize) override;

private:

	FSocket*				Socket;
};
