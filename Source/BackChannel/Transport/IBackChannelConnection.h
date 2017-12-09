// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.


#pragma once

class IBackChannelConnection
{
public:

	// todo concept of 'connection' should be some sort of persistent connection subclass?
	virtual void Close() = 0;

	virtual bool Connect(const TCHAR* InEndPoint) = 0;

	virtual bool IsConnected() const = 0;

	virtual int32 SendData(const void* InData, const int32 InSize) = 0;
	
	virtual int32 ReceiveData(void* OutBuffer, const int32 BufferSize) = 0;

protected:

	IBackChannelConnection() {}
	virtual ~IBackChannelConnection() {}
};

