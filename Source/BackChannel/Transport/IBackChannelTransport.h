// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "EngineMinimal.h"
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

	virtual TSharedPtr<IBackChannelConnection> CreateConnection(const int32 Type) = 0;

public:

	static const int TCP;

protected:

	IBackChannelTransport() {}
	virtual ~IBackChannelTransport() {}
};
