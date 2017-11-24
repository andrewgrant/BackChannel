// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "BackChannel/Transport/IBackChannelListener.h"
#include "EngineMinimal.h"
#include "Common/TcpListener.h"

/**
* BackChannelServer implementation.
*
*/
class FBackChannelListener : public IBackChannelListener
{
public:

	FBackChannelListener();
	~FBackChannelListener();

	/* IBackChannelServer implementation */
	virtual FBackChannelConnectionRequest& GetOnConnectionRequestDelegate() override;
	virtual bool Listen(const uint16 Port);
	virtual void Close() override;

protected:

protected:

	FBackChannelConnectionRequest	ConnectionRequestDelegate;
	TSharedPtr<FTcpListener>		Listener;
	TSharedPtr<FSocket>				Socket;
};
