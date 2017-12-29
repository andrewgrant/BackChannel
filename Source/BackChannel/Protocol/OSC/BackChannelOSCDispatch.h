// Copyright 2017 Andrew Grant
// Licensed under BSD License 2.0. 
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "CoreMinimal.h"

class FBackChannelOSCMessage;
class FBackChannelOSCDispatch;

DECLARE_MULTICAST_DELEGATE_TwoParams(FBackChannelDispatchDelegate, FBackChannelOSCMessage&, FBackChannelOSCDispatch&)

class BACKCHANNEL_API FBackChannelOSCDispatch
{
	
public:

	FBackChannelOSCDispatch();

	virtual ~FBackChannelOSCDispatch() {}


	FBackChannelDispatchDelegate& GetAddressHandler(const TCHAR* Path);

	void	DispatchMessage(FBackChannelOSCMessage& Message);


protected:

	TMap<FString, FBackChannelDispatchDelegate> DispatchMap;

};