#pragma once
#include "RoomRequestHandler.h"
#include "RoomManager.h"
#include "IRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
class RoomMemberRequestHandler : public RoomRequestHandler
{
public:
	RequestResult startGame(const RequestInfo& requestInfo);
	RoomMemberRequestHandler(Room room, LoggedUser user, RoomManager& roomManager, RequestHandlerFactory& factory);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;


private:
	RequestResult leaveRoom(const RequestInfo& requestInfo);

};