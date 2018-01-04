// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "IBackChannelConnection.h"

/*
 *	Delegate for incoming connections. The new connection is provided as a SharedRef and users should
 *	return true to accept the connection, or false to have it closed.
 */
DECLARE_DELEGATE_RetVal_OneParam(bool, FBackChannelConnectionRequest, TSharedRef<IBackChannelConnection>)

/*
 *	A class that listens for incoming connections and fires the connection-request dialog when
 *	they arrive. Listening occurs on a background thread but delegate requests occur on the gamethread
 */
class IBackChannelListener
{
	public:

		virtual ~IBackChannelListener() {}

		/* Start listening on the specified port */
		virtual bool Listen(const uint16 Port) = 0;

		/* Close this connection */
		virtual void Close() = 0;
	
		/* Get the delegate that is fired on each incoming request */
		virtual FBackChannelConnectionRequest& GetOnConnectionRequestDelegate() = 0;

protected:

	IBackChannelListener() {}

};
