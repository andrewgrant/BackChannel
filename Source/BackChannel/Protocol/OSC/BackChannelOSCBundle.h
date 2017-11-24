// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "BackChannel/Protocol/OSC/BackChannelOSCPacket.h"


class FBackChannelOSCBundle : public FBackChannelOSCPacket
{
public:

	FBackChannelOSCBundle(OSCPacketMode InMode);
	virtual ~FBackChannelOSCBundle();

	FBackChannelOSCBundle(FBackChannelOSCBundle&& RHS);

	FBackChannelOSCBundle& operator=(FBackChannelOSCBundle&& RHS);

	virtual OSCPacketType GetType() const override { return OSCPacketType::Bundle;  }

	virtual int32 GetSize() const override;

protected:

protected:

	OSCPacketMode		Mode;
	int64				TimeTag;
	int32				ElementSize;
	TArray<uint8>		ElementData;
};