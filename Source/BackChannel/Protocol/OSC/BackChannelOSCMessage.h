// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "BackChannel/Protocol/OSC/BackChannelOSCPacket.h"


class FBackChannelOSCMessage : public FBackChannelOSCPacket
{
public:

	FBackChannelOSCMessage(OSCPacketMode InMode);
	virtual ~FBackChannelOSCMessage();

	FBackChannelOSCMessage(FBackChannelOSCMessage&& RHS);

	FBackChannelOSCMessage& operator=(FBackChannelOSCMessage&& RHS);

	virtual OSCPacketType GetType() const override { return OSCPacketType::Message; }

	virtual int32 GetSize() const override;

	const FString& GetAddress() const
	{
		return Address;
	}

	const FString& GetTags() const
	{
		return TagString;
	}

	const int32 GetArgumentSize() const
	{
		return Buffer.Num();
	}

	void	SetAddress(const TCHAR* Address);

	FBackChannelOSCMessage& operator << (int32& Value)
	{
		SerializeInt32(Value);
		return *this;
	}

	FBackChannelOSCMessage& operator << (float& Value)
	{
		SerializeFloat(Value);
		return *this;
	}

	FBackChannelOSCMessage& operator << (FString& Value)
	{
		SerializeString(Value);
		return *this;
	}

	template <typename T>
	FBackChannelOSCMessage& operator << (TArray<T>& Value)
	{
		SerializeBlob(Value.GetData(), Value.Num() * sizeof(T));
		return *this;
	}

	void SerializeInt32(int32& Value)
	{
		Serialize(TEXT('i'), &Value, sizeof(Value));
	}

	void SerializeFloat(float& Value)
	{
		Serialize(TEXT('f'), &Value, sizeof(Value));
	}

	void SerializeBlob(void* InBlob, int32 BlobSize)
	{
		Serialize(TEXT('b'), InBlob, BlobSize);
	}

	void SerializeString(FString& Value);

	static int32 RoundedArgumentSize(int32 ArgSize)
	{
		return ((ArgSize + 3) / 4) * 4;
	}

	
	void WriteToBuffer(TArray<uint8>& Buffer);

	static TSharedPtr<FBackChannelOSCMessage> CreateFromBuffer(const void* Data, int32 DataLength);

protected:

	void Serialize(const TCHAR Code, void* InData, int32 InSize);

	void SerializeRead(const TCHAR Code, void* InData, int32 InSize);
	void SerializeWrite(const TCHAR Code, void* InData, int32 InSize);

protected:

	OSCPacketMode		Mode;
	FString				Address;
	FString				TagString;
	int					TagIndex;
	int					BufferIndex;
	TArray<uint8>		Buffer;
	
};