#include <vector>
#include "MenuRequestHandler.h"
#include "Requests.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "RoomManager.h"
#include "LoginRequestHandler.h"
#include "RoomRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "RoomAdminRequestHandler.h"

MenuRequestHandler::MenuRequestHandler(RequestHandlerFactory& factory, const LoggedUser& user)
	: m_handlerFactory(factory), m_user(user)
{
}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == LOGOUT_REQUEST ||
		requestInfo.id == GET_ROOMS_REQUEST ||
		requestInfo.id == GET_PLAYERS_IN_ROOM_REQUEST ||
		requestInfo.id == GET_HIGHSCORE_REQUEST ||
		requestInfo.id == GET_PERSONAL_STATS_REQUEST ||
		requestInfo.id == JOIN_ROOM_REQUEST ||
		requestInfo.id == CREATE_ROOM_REQUEST ||
		requestInfo.id == GET_ROOM_STATE_REQUEST;
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	if (!isRequestRelevant(requestInfo))
	{
		RequestResult requestResult;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
		requestResult.newHandler = this;

		return requestResult;
	}

	switch (requestInfo.id)
	{
	case LOGOUT_REQUEST:
		return logout(requestInfo);

	case GET_ROOMS_REQUEST:
		return getRooms(requestInfo);

	case GET_PLAYERS_IN_ROOM_REQUEST:
		return getPlayersInRoom(requestInfo);

	case GET_HIGHSCORE_REQUEST:
		return getHighScore(requestInfo);

	case GET_PERSONAL_STATS_REQUEST:
		return getPersonalStats(requestInfo);

	case JOIN_ROOM_REQUEST:
		return joinRoom(requestInfo);

	case CREATE_ROOM_REQUEST:
		return createRoom(requestInfo);

	case GET_ROOM_STATE_REQUEST:
		RequestResult req;
		LeaveRoomResponse leave;
		leave.status = SUCCESS;
		leave.type = "leaveGame";
		req.newHandler = this;
		req.response = JsonResponsePacketSerializer::serializeResponse(leave);
		return req;
	}
}

RequestResult MenuRequestHandler::logout(const RequestInfo& requestInfo)
{
	LogoutResponse logoutResponse;
	RequestResult requestResult;
	logoutResponse.status = m_handlerFactory.getLoginManager().logout(m_user.getUsername());
	if (logoutResponse.status == FAILED)
	{
		requestResult.newHandler = this;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());

		return requestResult;
	}

	requestResult.newHandler = m_handlerFactory.createLoginRequestHandler(m_user.getSocket());
	requestResult.response = JsonResponsePacketSerializer::serializeResponse(logoutResponse);

	return requestResult;
}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& requestInfo)
{
	std::vector<RoomData> rooms = m_handlerFactory.getRoomManager().getRooms();

	GetRoomsResponse getRoomsResponse;
	RequestResult requestResult;

	getRoomsResponse.rooms = rooms;
	getRoomsResponse.status = SUCCESS;
	
	requestResult.newHandler = this;
	requestResult.response = JsonResponsePacketSerializer::serializeResponse(getRoomsResponse);

	return requestResult;
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& requestInfo)
{
	GetPlayersInRoomRequest getPlayersInRoomRequest = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(requestInfo.buffer);
	Room* room = nullptr;  
	GetPlayersInRoomResponse getPlayersInRoomResponse;
	RequestResult requestResult;

	room = m_handlerFactory.getRoomManager().getRoom(getPlayersInRoomRequest.roomId);
	getPlayersInRoomResponse.players = room->getAllUsers();

	requestResult.newHandler = this;
	requestResult.response = JsonResponsePacketSerializer::serializeResponse(getPlayersInRoomResponse);

	return requestResult;
}

RequestResult MenuRequestHandler::getPersonalStats(const RequestInfo& requestInfo)
{
	GetPersonalStatsResponse getPersonalStatsResponse;
	RequestResult requestResult;

	try
	{
		auto& statsManager = m_handlerFactory.getStatisticsManager();

		getPersonalStatsResponse.statistics = statsManager.getUserStatistics(m_user.getUsername());
		getPersonalStatsResponse.status = SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		getPersonalStatsResponse.status = FAILED;
	}

	requestResult.newHandler = this;
	requestResult.response = JsonResponsePacketSerializer::serializeResponse(getPersonalStatsResponse);

	return requestResult;
}

RequestResult MenuRequestHandler::getHighScore(const RequestInfo& requestInfo)
{
	GetHighScoreResponse getHighScoreResponse;
	RequestResult requestResult;

	try
	{
		getHighScoreResponse.statistics = m_handlerFactory.getStatisticsManager().getHighScore();
		getHighScoreResponse.status = SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		getHighScoreResponse.status = FAILED;
	}

	requestResult.newHandler = this;
	requestResult.response = JsonResponsePacketSerializer::serializeResponse(getHighScoreResponse);

	return requestResult;
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& requestInfo)
{

	JoinRoomRequest joinRequest = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(requestInfo.buffer);
	Room* room = nullptr; 
	JoinRoomResponse joinResponse;
	RequestResult requestResult;

	try
	{
		room = m_handlerFactory.getRoomManager().getRoom(joinRequest.roomId);

		if (room == nullptr || room->getAllUsers().size() == room->getData().maxPlayers)
		{
			requestResult.newHandler = this;
			joinResponse.status = FAILED;
		}
		else
		{
			room->addUser(m_user);
			joinResponse.status = SUCCESS;
			requestResult.newHandler = m_handlerFactory.createRoomMemberRequestHandler(m_user, *room);
			
		}
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(joinResponse);
	}
	catch (const std::exception& e)
	{

		std::cout << e.what() << std::endl;
		joinResponse.status = FAILED;
		requestResult.newHandler = this;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
	}


	return requestResult;
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo& requestInfo)
{

	RoomData roomData;
	CreateRoomResponse createRoomResponse;
	CreateRoomRequest createRequest = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(requestInfo.buffer);
	RequestResult requestResult;

	roomData.id = m_handlerFactory.getRoomManager().generateRoomId();

	roomData.name = createRequest.roomName;
	roomData.maxPlayers = createRequest.maxUsers;
	roomData.numOfQuestionsInGame = createRequest.questionCount;
	roomData.timePerQuestion = createRequest.answerTimeout;
	roomData.isActive = false; // Might be changed later
	if (m_handlerFactory.getRoomManager().doesRoomNameExist(roomData.name))
	{
		createRoomResponse.status = FAILED;
		createRoomResponse.roomID = -1;
		requestResult.newHandler = this;
	}
	else
	{
		m_handlerFactory.getRoomManager().createRoom(m_user, roomData);
		createRoomResponse.status = SUCCESS;
		createRoomResponse.roomID = roomData.id;
		requestResult.newHandler = m_handlerFactory.createRoomAdminRequestHandler(m_user, *m_handlerFactory.getRoomManager().getRoom(roomData.id));
	}	
	requestResult.response = JsonResponsePacketSerializer::serializeResponse(createRoomResponse);
	return requestResult;
}
