// Copyright 2017 Andrew Grant
// This file is part of BackChannel and is freely licensed for commercial and 
// non-commercial use under an MIT license
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Utils/BackChannelThreadedConnection.h"


FBackChannelThreadedListener::FBackChannelThreadedListener()
{
}

FBackChannelThreadedListener::~FBackChannelThreadedListener()
{
	Stop();
}

bool FBackChannelThreadedListener::IsRunning() const
{
	return bIsRunning;
}

void FBackChannelThreadedListener::Start(TSharedRef<IBackChannelConnection> InConnection, FBackChannelListenerDelegate InDelegate)
{
	Connection = InConnection;
	Delegate = InDelegate;

	bIsRunning = true;
	bExitRequested = false;

	FRunnableThread::Create(this, TEXT("FBackChannelSocketThread"), 32 * 1024);
}

uint32 FBackChannelThreadedListener::Run()
{
	while (bExitRequested == false)
	{
		FScopeLock RunningLock(&RunningCS);

		Connection->WaitForConnection(1, [this](TSharedRef<IBackChannelConnection> NewConnection) {
			return Delegate.Execute(NewConnection);
		});
	}

	bIsRunning = false;
	return 0;
}

void FBackChannelThreadedListener::Stop()
{
	bExitRequested = true;

	if (IsRunning())
	{
		FScopeLock RunLock(&RunningCS);
	}

	bExitRequested = false;
}
