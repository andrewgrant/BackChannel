// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/Transport/BackChannelListener.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"

const TCHAR* BackChannelTransport_TCP = TEXT("BackChannelTCP");

DEFINE_LOG_CATEGORY(LogBackChannel);

const int32 IBackChannelTransport::TCP=1;

class FBackChannelTransport : public IBackChannelTransport
{
public:
	virtual TSharedPtr<IBackChannelListener> CreateListener(const int32 Type) override
	{
		check(Type == IBackChannelTransport::TCP);
		TSharedPtr<FBackChannelListener> Listener = MakeShareable(new FBackChannelListener());
		return Listener;
	}

	virtual TSharedPtr<IBackChannelConnection> CreateConnection(const int32 Type) override
	{
		check(Type == IBackChannelTransport::TCP);
		TSharedPtr<FBackChannelConnection> Connection = MakeShareable(new FBackChannelConnection());
		return Connection;	
	}
};

IMPLEMENT_MODULE(FBackChannelTransport, BackChannel)

