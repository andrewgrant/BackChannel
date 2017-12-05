// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class OSCPacketMode
{
	Read,
	Write
};

enum class OSCPacketType
{
	Invalid,
	Message,
	Bundle
};

class BACKCHANNEL_API FBackChannelOSCPacket
{
public:

	virtual ~FBackChannelOSCPacket() {}

	virtual int32	GetSize() const = 0;

	virtual OSCPacketType GetType() const = 0;

	static OSCPacketType GetType(const void* Data, int32 DataLength);

	static TSharedPtr<FBackChannelOSCPacket> CreateFromBuffer(const void* Data, int32 DataLength);
};

class FBackChannelOSCNullPacket : public FBackChannelOSCPacket
{
public:
	virtual int32 GetSize() const override { return 0; }

	virtual OSCPacketType GetType() const override { return OSCPacketType::Invalid;  }
};

