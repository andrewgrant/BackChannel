// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

// todo (agrant 17/12/29): concept of 'connection' should be a base class with persistent connection subclass?

/*
 *	Base class that describes a back-channel connection. The underlying behavior will depend on the type
 *	of connection that was requested from the factory
 */
class IBackChannelConnection
{
public:
	
	// todo (agrant 17/12/29): Should remove 'Connect' and instead return a connected (or null..) socket
	// from the factory

	/* Connect to the specified endpoint */
	virtual bool Connect(const TCHAR* InEndPoint) = 0;

	/* Close our connection */
	virtual void Close() = 0;

	/* Check if this object is connected */
	virtual bool IsConnected() const = 0;

	/* Send data via our connection */
	virtual int32 SendData(const void* InData, const int32 InSize) = 0;
	
	/* Receive data from our connection. The returned value is the number of bytes read, to a max of BufferSize */
	virtual int32 ReceiveData(void* OutBuffer, const int32 BufferSize) = 0;

protected:

	IBackChannelConnection() {}
	virtual ~IBackChannelConnection() {}
};

