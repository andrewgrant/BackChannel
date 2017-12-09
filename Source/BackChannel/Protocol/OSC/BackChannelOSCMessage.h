// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "BackChannel/Protocol/OSC/BackChannelOSCPacket.h"


class BACKCHANNEL_API FBackChannelOSCMessage : public FBackChannelOSCPacket
{
public:

	FBackChannelOSCMessage(OSCPacketMode InMode);

	FBackChannelOSCMessage(const TCHAR* Address);

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

	void	ResetRead();

	//! Int32 read/write

	void Write(const int32 Value)
	{
		check(IsWriting());
		SerializeWrite(TEXT('i'), &Value, sizeof(Value));
	}

	void Read(int32& Value)
	{
		check(IsReading());
		SerializeRead(TEXT('i'), &Value, sizeof(Value));
	}

	//! Float read/write

	void Write(const float Value)
	{
		check(IsWriting());
		SerializeWrite(TEXT('f'), &Value, sizeof(Value));
	}

	void Read(float& OutValue)
	{
		check(IsReading());
		SerializeRead(TEXT('f'), &OutValue, sizeof(OutValue));
	}

	//! String read/write (multiple forms of write for TCHAR*'s

	void Write(const FString& Value)
	{
		Write(*Value);
	}

	void Write(const TCHAR* Value)
	{
		SerializeWrite(TEXT('s'), TCHAR_TO_ANSI(Value), FCString::Strlen(Value) + 1);
	}

	void Read(FString& OutValue);

	//! Raw data blobs

	void Write(const void* InBlob, int32 BlobSize)
	{
		check(IsWriting());
		SerializeWrite(TEXT('b'), InBlob, BlobSize);
	}

	void Read(void* InBlob, int32 BlobSize)
	{
		check(IsReading());
		SerializeRead(TEXT('b'), InBlob, BlobSize);
	}

	template<typename T>
	void Write(const TArray<T>& Value)
	{
		Write(Value.Num());
		Write(Value.GetData(), Value.Num() * sizeof(T));
	}

	template<typename T>
	void Read(TArray<T>& Value)
	{
		int32 ArraySize(0);
		Read(ArraySize);

		Value.Empty();
		Value.AddUninitialized(ArraySize);
		Read(Value.GetData(), Value.Num() * sizeof(T));
	}
	
	template<typename T>
	void Serialize(T& Value)
	{
		if (IsWriting())
		{
			Write(Value);
		}
		else
		{
			Read(Value);
		}
	}

	void Serialize(void* InBlob, int32 BlobSize)
	{
		if (IsReading())
		{
			Read(InBlob, BlobSize);
		}
		else
		{
			Write(InBlob, BlobSize);
		}
	}

	static int32 RoundedArgumentSize(int32 ArgSize)
	{
		return ((ArgSize + 3) / 4) * 4;
	}

	virtual TArray<uint8> WriteToBuffer() const override;

	virtual void WriteToBuffer(TArray<uint8>& Buffer) const override;

	static TSharedPtr<FBackChannelOSCMessage> CreateFromBuffer(const void* Data, int32 DataLength);

	bool IsWriting() const { return Mode == OSCPacketMode::Write; }
	bool IsReading() const { return Mode == OSCPacketMode::Read; }

protected:

	void Serialize(const TCHAR Code, void* InData, int32 InSize);

	void SerializeRead(const TCHAR Code, void* InData, int32 InSize);
	void SerializeWrite(const TCHAR Code, const void* InData, int32 InSize);

protected:

	OSCPacketMode		Mode;
	FString				Address;
	FString				TagString;
	int					TagIndex;
	int					BufferIndex;
	TArray<uint8>		Buffer;
};

BACKCHANNEL_API FBackChannelOSCMessage& operator << (FBackChannelOSCMessage& Msg, int32& Value);

BACKCHANNEL_API FBackChannelOSCMessage& operator << (FBackChannelOSCMessage& Msg, float& Value);

BACKCHANNEL_API FBackChannelOSCMessage& operator << (FBackChannelOSCMessage& Msg, bool& Value);

BACKCHANNEL_API FBackChannelOSCMessage& operator << (FBackChannelOSCMessage& Msg, TCHAR& Value);

BACKCHANNEL_API FBackChannelOSCMessage& operator << (FBackChannelOSCMessage& Msg, FString& Value);

template <typename T>
FBackChannelOSCMessage& operator << (FBackChannelOSCMessage& Msg, TArray<T>& Value)
{
	if (Msg.IsWriting())
	{
		Msg.Write(Value);
	}
	else
	{
		Msg.Read(Value);
	}

	return Msg;
}

template <typename T>
FBackChannelOSCMessage& SerializeOut(FBackChannelOSCMessage& Msg, const T& Value)
{
	T Tmp = Value;
	Msg << Tmp;
	return Msg;
}