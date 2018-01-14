// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#include "Private/BackChannelCommon.h"
#include "Protocol/OSC/BackChannelOSCPacket.h"
#include "Protocol/OSC/BackChannelOSCBundle.h"
#include "Protocol/OSC/BackChannelOSCMessage.h"



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
