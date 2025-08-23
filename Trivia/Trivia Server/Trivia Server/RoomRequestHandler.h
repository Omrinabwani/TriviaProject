#pragma once
#include "IRequestHandler.h"
#include "LoggedUser.h"
#include "RoomManager.h"
#include "RequestHandlerFactory.h"
#include "RequestResult.h"
#include "RequestInfo.h"
class RoomRequestHandler : public IRequestHandler
{


public:
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) = 0;
	virtual RequestResult handleRequest(const RequestInfo& requestInfo) = 0;
	RoomRequestHandler(Room room, LoggedUser user, RoomManager& roomManager, RequestHandlerFactory& handlerFactory);

protected:
	Room m_room;
	LoggedUser m_user;
	RoomManager& m_roomManager;
	RequestHandlerFactory& m_handlerFactory;


	RequestResult getRoomState(const RequestInfo& requestInfo);

};

