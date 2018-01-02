// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
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