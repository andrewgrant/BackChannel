// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Private/Transport/BackChannelListener.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"

DEFINE_LOG_CATEGORY(LogBackChannel);


class FBackChannelTransport : public IBackChannelTransport
{
public:
	virtual TSharedPtr<IBackChannelListener> CreateListener() override
	{
		TSharedPtr<FBackChannelListener> Listener = MakeShareable(new FBackChannelListener());
		return Listener;
	}

	virtual TSharedPtr<IBackChannelConnection> CreateConnection() override
	{
		TSharedPtr<FBackChannelConnection> Connection = MakeShareable(new FBackChannelConnection());
		return Connection;	
	}
};

IMPLEMENT_MODULE(FBackChannelTransport, BackChannel)

