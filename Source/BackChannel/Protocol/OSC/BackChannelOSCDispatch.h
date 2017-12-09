// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

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