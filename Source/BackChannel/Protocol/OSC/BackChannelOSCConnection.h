// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "BackChannel/Protocol/OSC/BackChannelOSCDispatch.h"

class IBackChannelConnection;
class FBackChannelOSCPacket;

class BACKCHANNEL_API FBackChannelOSCConnection : FRunnable
{
public:

	FBackChannelOSCConnection(TSharedRef<IBackChannelConnection> InConnection);

	~FBackChannelOSCConnection();

public:

	bool Start();

	// Begin FRunnable overrides
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable overrides
	
	bool IsConnected() const;

	bool SendPacket(FBackChannelOSCPacket& Packet);
	bool SendPacketData(const void* Data, const int32 DataLen);	

	FBackChannelOSCDispatch& GetDispatchMap();

	void DispatchMessages();

	FString Description()
	{
		return TEXT("BackChannelConnection");
	}

protected:

	TSharedRef<IBackChannelConnection>  Connection;

	FBackChannelOSCDispatch				DispatchMap;

	TArray<TSharedPtr<FBackChannelOSCPacket>> ReceivedPackets;

	FThreadSafeBool		ExitRequested;
	FThreadSafeBool		IsRunning;

	FCriticalSection	ReceivedPacketsMutex;
};