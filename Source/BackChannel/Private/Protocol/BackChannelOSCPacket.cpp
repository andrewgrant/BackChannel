// Copyright 2017 Andrew Grant
// Licensed under BSD License 2.0. 
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Protocol/OSC/BackChannelOSCPacket.h"



OSCPacketType FBackChannelOSCPacket::GetType(const void* Data, int32 DataLength)
{
	const ANSICHAR* CharData = (const ANSICHAR*)Data;

	if (CharData == nullptr)
	{
		return OSCPacketType::Invalid;
	}

	const ANSICHAR* BundleIdentifier = "#bundle";
	const ANSICHAR* MessageIdentifier = "";		// need to check whether "" is valid or it requires at least /

	const int32 MessageIdentifierLen = FCStringAnsi::Strlen(MessageIdentifier);

	if (DataLength >= FCStringAnsi::Strlen(BundleIdentifier))
	{
		if (FCStringAnsi::Stricmp(CharData, BundleIdentifier) == 0)
		{
			return OSCPacketType::Bundle;
		}
	}

	if (DataLength >= MessageIdentifierLen)
	{
		if (FCStringAnsi::Strnicmp(CharData, MessageIdentifier, MessageIdentifierLen) == 0)
		{
			return OSCPacketType::Message;
		}
	}

	return OSCPacketType::Invalid;
}

TSharedPtr<FBackChannelOSCPacket> FBackChannelOSCPacket::CreateFromBuffer(const void * Data, int32 DataLength)
{
	OSCPacketType Type = GetType(Data, DataLength);

	if (Type == OSCPacketType::Bundle)
	{
		return FBackChannelOSCBundle::CreateFromBuffer(Data, DataLength);
	}
	else if (Type == OSCPacketType::Message)
	{
		return FBackChannelOSCMessage::CreateFromBuffer(Data, DataLength);
	}

	return nullptr;
}
