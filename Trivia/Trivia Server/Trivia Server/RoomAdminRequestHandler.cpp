#include "RoomAdminRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "MenuRequestHandler.h"
#include <vector>
#include "GameRequestHandler.h"

RoomAdminRequestHandler::RoomAdminRequestHandler(Room room, LoggedUser user, RoomManager& roomManager, RequestHandlerFactory& factory)
	: RoomRequestHandler(room, user, roomManager, factory)
{

}

RequestResult RoomAdminRequestHandler::closeRoom(RequestInfo requestInfo)
{
	LeaveRoomResponse leaveResponse;
	CloseRoomResponse closeResponse;
	RequestResult res;
	Buffer buffer;


	this->m_roomManager.deleteRoom(m_room.getData().id);

	closeResponse.status = SUCCESS;
	res.response = JsonResponsePacketSerializer::serializeResponse(closeResponse);
	res.newHandler = m_handlerFactory.createMenuRequestHandler(this->m_user);
	return res;
}

RequestResult RoomAdminRequestHandler::startGame(RequestInfo requestInfo)
{
	std::vector<LoggedUser> users = m_roomManager.getRoom(m_room.getData().id)->getAllLoggedUsers();
	StartGameResponse startGame;
	RequestResult res;
	Buffer buffer;
	Game& game = m_handlerFactory.getGameManager().createGame(*m_roomManager.getRoom(m_room.getData().id));

	m_roomManager.getRoom(m_room.getData().id)->setIsActive(true);
	startGame.status = SUCCESS;
	res.response = JsonResponsePacketSerializer::serializeResponse(startGame);
	res.newHandler = m_handlerFactory.createGameRequestHandler(m_user, game);
	return res;
}

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	if (requestInfo.id == CLOSE_ROOM_REQUEST || requestInfo.id == START_GAME_REQUEST
		|| requestInfo.id == GET_ROOM_STATE_REQUEST)
	{
		return true;
	}
	return false;
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	RequestResult requestResult;
	if (!isRequestRelevant(requestInfo))
	{
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
		requestResult.newHandler = this;
		return requestResult;
	}
	switch (requestInfo.id)
	{
	case CLOSE_ROOM_REQUEST:
		requestResult = closeRoom(requestInfo);
		break;
	case START_GAME_REQUEST:
		requestResult = startGame(requestInfo);
		break;
	case GET_ROOM_STATE_REQUEST:
		requestResult = getRoomState(requestInfo);
		break;
	}
	return requestResult;
}

