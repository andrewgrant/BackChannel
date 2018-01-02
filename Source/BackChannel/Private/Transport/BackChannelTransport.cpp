// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

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

