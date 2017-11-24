// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Protocol/OSC/BackChannelOSCBundle.h"
#include "BackChannel/Private/BackChannelCommon.h"

PRAGMA_DISABLE_OPTIMIZATION


FBackChannelOSCBundle::FBackChannelOSCBundle(OSCPacketMode InMode)
{
	Mode = InMode;
}

FBackChannelOSCBundle::~FBackChannelOSCBundle()
{
}


FBackChannelOSCBundle::FBackChannelOSCBundle(FBackChannelOSCBundle&& RHS)
{
	*this = MoveTemp(RHS);
}

FBackChannelOSCBundle& FBackChannelOSCBundle::operator=(FBackChannelOSCBundle&& RHS)
{
	Mode = RHS.Mode;
	TimeTag = RHS.TimeTag;

	return *this;
}

int32 FBackChannelOSCBundle::GetSize() const
{
	return 0;
}




PRAGMA_ENABLE_OPTIMIZATION