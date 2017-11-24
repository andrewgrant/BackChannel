// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

#include "BackChannel/Transport/IBackChannelTransport.h"
#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Private/Transport/BackChannelConnection.h"
#include "Common/TcpSocketBuilder.h"

FBackChannelConnection::FBackChannelConnection()
{
	Socket = nullptr;
}

FBackChannelConnection::~FBackChannelConnection()
{
	if (Socket)
	{
		Close();
	}
}

bool FBackChannelConnection::Connect(const TCHAR* InEndPoint)
{
	FSocket* Socket = FTcpSocketBuilder(TEXT("BackChannel Client"));

	if (Socket == nullptr)
	{
		return false;
	}

	FIPv4Endpoint EndPoint;
	FIPv4Endpoint::Parse(InEndPoint, EndPoint);

	if (!Socket->Connect(*EndPoint.ToInternetAddr()))
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);

		return false;
	}

	return Attach(Socket);
}

bool FBackChannelConnection::Attach(FSocket* InSocket)
{
	Socket = InSocket;
	return true;
}

void FBackChannelConnection::Close()
{
	if (Socket)
	{
		Socket->Close();
		Socket = nullptr;
	}
}

int32 FBackChannelConnection::SendData(const void* InData, const int32 InSize)
{
	if (!Socket)
	{
		return 0;
	}

	int32 BytesSent(0);
	Socket->Send((const uint8*)InData, InSize, BytesSent);
	return BytesSent;
}

int32 FBackChannelConnection::ReceiveData(void* OutBuffer, const int32 BufferSize)
{
	if (!Socket)
	{
		return 0;
	}

	int32 BytesRead(0);
	Socket->Recv((uint8*)OutBuffer, BufferSize, BytesRead, ESocketReceiveFlags::None);
	return BytesRead;
}
