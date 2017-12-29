// Copyright 2017 Andrew Grant
// Licensed under BSD License 2.0. 
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Protocol/OSC/BackChannelOSCDispatch.h"


FBackChannelOSCDispatch::FBackChannelOSCDispatch()
{

}

FBackChannelDispatchDelegate& FBackChannelOSCDispatch::GetAddressHandler(const TCHAR* Path)
{
	
	FString LowerPath = FString(Path).ToLower();

	if (DispatchMap.Contains(LowerPath) == false)
	{
		DispatchMap.Add(LowerPath);
	}

	return DispatchMap.FindChecked(LowerPath);

}


void FBackChannelOSCDispatch::DispatchMessage(FBackChannelOSCMessage& Message)
{
	FString LowerAddress = Message.GetAddress().ToLower();

	for (const auto& KV : DispatchMap)
	{
		FString LowerPath = KV.Key.ToLower();

		if (LowerAddress.StartsWith(LowerPath))
		{
			KV.Value.Broadcast(Message, *this);
			Message.ResetRead();
		}
	}
}