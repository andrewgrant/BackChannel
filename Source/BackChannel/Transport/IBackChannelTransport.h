// Copyright 2017 Andrew Grant
// Licensed under BSD License 2.0. 
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "EngineMinimal.h"
#include "IBackChannelListener.h"
#include "IBackChannelConnection.h"

/*
 *	Main module and factory interface for Backchannel connections
 */
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
