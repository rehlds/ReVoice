#pragma once

#include "rehlsdk/engine/maintypes.h"
#include "rehlsdk/public/interface.h"

class IGameServerData : public IBaseInterface {
public:
	virtual ~IGameServerData() { };

	virtual void WriteDataRequest(const void *buffer, int bufferSize) = 0;

	virtual int ReadDataResponse(void *data, int len) = 0;
};

#define GAMESERVERDATA_INTERFACE_VERSION "GameServerData001"
