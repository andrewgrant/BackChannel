// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "IBackChannelListener.h"
#include "IBackChannelConnection.h"

class BACKCHANNEL_API IBackChannelTransport : public IModuleInterface
{
public:

	static inline bool IsAvailable(void)
	{
		return Get() != nullptr;
	}

	static inline IBackChannelTransport* Get(void)
	{
		return FModuleManager::LoadModulePtr<IBackChannelTransport>("BackChannel");
	}

	virtual TSharedPtr<IBackChannelListener> CreateListener(const int32 Type) = 0;

	virtual TSharedPtr<IBackChannelConnection> CreateConnection(const int32 Type) = 0;

public:

	static const int TCP;

protected:

	IBackChannelTransport() {}
	virtual ~IBackChannelTransport() {}
};
