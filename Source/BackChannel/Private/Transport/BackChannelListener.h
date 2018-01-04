// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info


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
