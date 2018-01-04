// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

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

	void AddElement(const void* InData, const int32 InSize);

	int32 GetElementCount() const;

	const TArray<uint8>& GetElement(const int32 Index) const;

	virtual TArray<uint8> WriteToBuffer() const override;

	virtual void WriteToBuffer(TArray<uint8>& Buffer) const override;

	static TSharedPtr<FBackChannelOSCBundle> CreateFromBuffer(const void* Data, int32 DataLength);

protected:

protected:

	typedef TArray<uint8> ElementData;

	OSCPacketMode			Mode;
	int64					TimeTag;
	TArray<ElementData>		Elements;

	static const ANSICHAR*		BundleHeader;
};