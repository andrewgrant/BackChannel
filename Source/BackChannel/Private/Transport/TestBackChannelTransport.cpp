// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repo
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"

#if WITH_DEV_AUTOMATION_TESTS

class FBackChannelTestTransport : public FAutomationTestBase
{

public:

	FBackChannelTestTransport(const FString& InName, const bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask) {}

	void CreateListener()
	{
		if (IBackChannelTransport* BC = IBackChannelTransport::Get())
		{
			BackChannelListener = BC->CreateListener(IBackChannelTransport::TCP);
		}
	}

	void CreateConnection1()
	{
		if (IBackChannelTransport* BC = IBackChannelTransport::Get())
		{
			BackChannelConnection1 = BC->CreateConnection(IBackChannelTransport::TCP);
		}
	}
	
	bool ConnectListenerAndConnection()
	{
		bool IsConnected = false;

		BackChannelListener->GetOnConnectionRequestDelegate().BindLambda([this, &IsConnected](auto NewConnection)->bool
		{
			BackChannelConnection2 = NewConnection;
			FPlatformMisc::MemoryBarrier();
			IsConnected = true;
			return true;
		});

		BackChannelListener->Listen(1313);

		BackChannelConnection1->Connect(TEXT("127.0.0.1:1313"));

		const double StartTime = FPlatformTime::Seconds();

		while (!IsConnected && (FPlatformTime::Seconds() - StartTime) < 10)
		{
			FPlatformProcess::SleepNoStats(1);
		}

		return BackChannelConnection2.IsValid();
	}

	TSharedPtr<IBackChannelListener>	BackChannelListener;
	TSharedPtr<IBackChannelConnection>	BackChannelConnection1;
	TSharedPtr<IBackChannelConnection>	BackChannelConnection2;

};

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBackChannelTestCreate, FBackChannelTestTransport, "BackChannel.TestTransport", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FBackChannelTestCreate::RunTest(const FString& Parameters)
{
	CreateListener();
	CreateConnection1();

	return BackChannelListener.IsValid() && BackChannelConnection1.IsValid();
}


IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBackChannelTestConnect, FBackChannelTestTransport, "Project.BackChannel.TestConnect", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBackChannelTestConnect::RunTest(const FString& Parameters)
{
	CreateListener();
	CreateConnection1();

	ConnectListenerAndConnection();

	return BackChannelListener.IsValid() && BackChannelConnection1.IsValid() && BackChannelConnection2.IsValid();	
}

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBackChannelTestSendReceive, FBackChannelTestTransport, "Project.BackChannel.TestConnect", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBackChannelTestSendReceive::RunTest(const FString& Parameters)
{
	CreateListener();
	CreateConnection1();
	ConnectListenerAndConnection();

	FString Msg = TEXT("Hello!");
	TCHAR MsgReceived[256] = { 0 };


	int32 MsgSize = Msg.Len() * sizeof(TCHAR);

	int32 Sent = BackChannelConnection1->SendData(*Msg, MsgSize);

	check(Sent == MsgSize);

	int32 Received = BackChannelConnection2->ReceiveData(MsgReceived, 256);

	check(Received == Sent);

	check(Msg == MsgReceived);

	return true;
}



#endif // WITH_DEV_AUTOMATION_TESTS