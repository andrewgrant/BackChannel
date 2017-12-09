// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.


#pragma once

#include "IBackChannelConnection.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FBackChannelConnectionRequest, TSharedRef<IBackChannelConnection>)

class IBackChannelListener
{
	public:

		virtual ~IBackChannelListener() {}

		virtual bool Listen(const uint16 Port) = 0;
		virtual void Close() = 0;
	
		virtual FBackChannelConnectionRequest& GetOnConnectionRequestDelegate() = 0;

protected:

	IBackChannelListener() {}

};
