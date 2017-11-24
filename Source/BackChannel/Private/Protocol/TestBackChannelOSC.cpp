// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Protocol/OSC/BackChannelOSC.h"
#include "EngineMinimal.h"

PRAGMA_DISABLE_OPTIMIZATION

#if WITH_DEV_AUTOMATION_TESTS

class FBackChannelTestOSCBase : public FAutomationTestBase
{

public:

	FBackChannelTestOSCBase(const FString& InName, const bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask) {}

	

};

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBackChannelTestOSCMessage, FBackChannelTestOSCBase, "BackChannel.TestOSCMessage", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBackChannelTestOSCBundle, FBackChannelTestOSCBase, "BackChannel.TestOSCBundle", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FBackChannelTestOSCMessage::RunTest(const FString& Parameters)
{
	FBackChannelOSCMessage Message(OSCPacketMode::Write);
	
	Message.SetAddress(TEXT("/foo"));
	
	int32 IntValue = 1;
	float FloatValue = 2.5;
	FString StringValue = TEXT("Hello!");
	TArray<int8> AnswerArray;

	const int kArraySize = 33;
	const int kArrayValue = 42;

	for (int i = 0; i < kArraySize; i++)
	{
		AnswerArray.Add(kArrayValue);
	}

	Message << IntValue << FloatValue << StringValue << AnswerArray;

	FString Address = Message.GetAddress();
	FString Tags = Message.GetTags();
	const int32 ArgSize = Message.GetArgumentSize();

	const int RoundedStringSize = FBackChannelOSCMessage::RoundedArgumentSize(StringValue.Len() + 1);
	const int RoundedArraySize = FBackChannelOSCMessage::RoundedArgumentSize(kArraySize);

	const int ExpectedArgSize = 4 + 4 + RoundedStringSize + RoundedArraySize;
	const int ExpectedBufferSize = ExpectedArgSize + FBackChannelOSCMessage::RoundedArgumentSize(Address.Len() + 1) + FBackChannelOSCMessage::RoundedArgumentSize(Tags.Len() + 1);

	// verify this address and tags...
	check(Address == TEXT("/foo"));
	check(Tags == TEXT("ifsb"));
	check(ArgSize == ExpectedArgSize);

	TArray<uint8> Buffer;

	Message.WriteToBuffer(Buffer);

	check(Buffer.Num() == ExpectedBufferSize);
	check(FBackChannelOSCPacket::GetType(Buffer.GetData(), Buffer.Num()) == OSCPacketType::Message);

	TSharedPtr<FBackChannelOSCMessage> NewMessage = FBackChannelOSCMessage::CreateFromBuffer(Buffer.GetData(), Buffer.Num());

	// read them back
	int32 OutIntValue(0);
	float OutFloatValue(0);
	FString OutStringValue;
	TArray<uint8> OutArray;

	OutArray.AddUninitialized(kArraySize);

	*NewMessage << OutIntValue << OutFloatValue << OutStringValue << OutArray;

	check(OutIntValue == IntValue);
	check(OutFloatValue == OutFloatValue);
	check(OutStringValue == OutStringValue);
	
	for (int i = 0; i < OutArray.Num(); i++)
	{
		check(OutArray[i] == kArrayValue);
	}

	return true;
}


bool FBackChannelTestOSCBundle::RunTest(const FString& Parameters)
{
	FBackChannelOSCBundle Bundle(OSCPacketMode::Write);

	return true;
}



#endif // WITH_DEV_AUTOMATION_TESTS

PRAGMA_ENABLE_OPTIMIZATION