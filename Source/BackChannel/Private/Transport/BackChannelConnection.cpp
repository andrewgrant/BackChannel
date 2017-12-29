// Copyright 2017 Andrew Grant
// Licensed under BSD License 2.0. 
// See https://github.com/andrewgrant/BackChannel for more info

#include "BackChannel/Private/BackChannelCommon.h"
#include "BackChannel/Transport/IBackChannelTransport.h"
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
	FSocket* NewSocket = FTcpSocketBuilder(TEXT("BackChannel Client"));

	if (NewSocket == nullptr)
	{
		return false;
	}

	FIPv4Endpoint EndPoint;
	FIPv4Endpoint::Parse(InEndPoint, EndPoint);

	NewSocket->SetNonBlocking(false);

	if (!NewSocket->Connect(*EndPoint.ToInternetAddr()))
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(NewSocket);

		return false;
	}

	NewSocket->SetNonBlocking(true);

	return Attach(NewSocket);
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

bool FBackChannelConnection::IsConnected() const
{
	return Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

int32 FBackChannelConnection::SendData(const void* InData, const int32 InSize)
{
	if (!Socket)
	{
		return -1;
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
