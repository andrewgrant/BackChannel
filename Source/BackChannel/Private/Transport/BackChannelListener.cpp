// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/Transport/BackChannelListener.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"


DEFINE_LOG_CATEGORY_STATIC(LogBackChannelServer, Log, All);

FBackChannelListener::FBackChannelListener()
{
}

FBackChannelListener::~FBackChannelListener()
{
	if (Listener.IsValid())
	{
		Close();
	}
}

FBackChannelConnectionRequest& FBackChannelListener::GetOnConnectionRequestDelegate()
{
	return ConnectionRequestDelegate;
}

bool FBackChannelListener::Listen(const uint16 Port)
{
	if (Listener.IsValid())
	{
		return false;
	}

	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);

	Listener = MakeShareable(new FTcpListener(Endpoint));

	Listener->OnConnectionAccepted().BindLambda([this](FSocket* InSocket, const FIPv4Endpoint& EndPoint) -> bool
	{
		// We are called from a background thread, so to make things easier on client code we'll take the socket,
		// ask them  by calling their delegate on the main loop, and close it if they don't want it
		FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this, InSocket, EndPoint](float) {

			bool SocketAccepted = false;

			UE_LOG(LogBackChannel, Verbose, TEXT("Connection request from %s"), *EndPoint.ToString());

			if (ConnectionRequestDelegate.IsBound())
			{
				TSharedRef<FBackChannelConnection> NewConnection = MakeShareable(new FBackChannelConnection());

				NewConnection->Attach(InSocket);
				SocketAccepted = ConnectionRequestDelegate.Execute(NewConnection);
			}

			if (!SocketAccepted)
			{
				InSocket->Close();
				UE_LOG(LogBackChannel, Log, TEXT("Rejected connection from %s"), *EndPoint.ToString());
			}
			else
			{
				UE_LOG(LogBackChannel, Log, TEXT("Accepted connection from %s"), *EndPoint.ToString());
			}

			return false;

		}), 0.0f);

		// say accept it, if the listener on the game thread doesn't want it we'll just close it
		return true;
	});

	UE_LOG(LogBackChannel, Log, TEXT("Started server on port %d"), Port);

	return true;
}

void FBackChannelListener::Close() 
{
	if (Listener.IsValid())
	{
		UE_LOG(LogBackChannel, Log, TEXT("Stopped server"));
		Listener->Stop();
		Listener = nullptr;
	}
	else
	{
		UE_LOG(LogBackChannel, Error, TEXT("Close() called on non-running server"));
	}
}

