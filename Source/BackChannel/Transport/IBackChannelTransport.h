// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "IBackChannelListener.h"
#include "IBackChannelConnection.h"


class IBackChannelTransport : public IModuleInterface
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

	virtual TSharedPtr<IBackChannelListener> CreateListener() = 0;

	virtual TSharedPtr<IBackChannelConnection> CreateConnection() = 0;

protected:

	IBackChannelTransport() {}
	virtual ~IBackChannelTransport() {}
};
