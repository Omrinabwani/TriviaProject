#include "RoomRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "MenuRequestHandler.h"

RoomRequestHandler::RoomRequestHandler(Room room, LoggedUser user, RoomManager& roomManager, RequestHandlerFactory& handlerFactory)
	: m_room(room),
	m_user(user),
	m_roomManager(roomManager),
	m_handlerFactory(handlerFactory)
{

}

RequestResult RoomRequestHandler::getRoomState(const RequestInfo& requestInfo)
{
	RequestResult req;

	GetRoomStateResponse res = m_roomManager.getRoomStateResponse(m_room.getData().id);
	req.response = JsonResponsePacketSerializer::serializeResponse(res);

	req.newHandler = this;
	return req;
}