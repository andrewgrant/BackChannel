// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#pragma once

#include "BackChannel/Transport/IBackChannelConnection.h"
#include "HAL/Runnable.h"
#include "EngineMinimal.h"

class FSocket;

DECLARE_DELEGATE_RetVal_OneParam(bool, FBackChannelListenerDelegate, TSharedRef<IBackChannelConnection>);

/**
* BackChannelClient implementation.
*
*/
class BACKCHANNEL_API FBackChannelThreadedListener : public FRunnable, public TSharedFromThis<FBackChannelThreadedListener>
{
public:

	FBackChannelThreadedListener();
	~FBackChannelThreadedListener();

	void Start(TSharedRef<IBackChannelConnection> InConnection, FBackChannelListenerDelegate InDelegate);

	virtual void Stop() override;

	bool IsRunning() const;

protected:

	virtual uint32 Run() override;

private:

	TSharedPtr<IBackChannelConnection>		Connection;
	FBackChannelListenerDelegate			Delegate;
	
	FThreadSafeBool							bExitRequested;
	FThreadSafeBool							bIsRunning;
	FCriticalSection						RunningCS;
};
