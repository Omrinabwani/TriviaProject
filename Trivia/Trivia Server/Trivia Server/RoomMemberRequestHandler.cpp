#include "RoomMemberRequestHandler.h"
#include "MenuRequestHandler.h"
#include "GameRequestHandler.h"
#include "Game.h"

RoomMemberRequestHandler::RoomMemberRequestHandler(Room room, LoggedUser user, RoomManager& roomManager, RequestHandlerFactory& factory)
    : RoomRequestHandler(room, user, roomManager, factory)
{

}

bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
    if (requestInfo.id == LEAVE_ROOM_REQUEST || requestInfo.id == GET_ROOM_STATE_REQUEST
        || requestInfo.id == MEMBER_START_GAME_REQUEST)
    {
        return true;
    }
    return false;
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
    RequestResult requestResult;
    if (!isRequestRelevant(requestInfo))
    {
        requestResult.newHandler = this;
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
        return requestResult;
    }
    switch (requestInfo.id)
    {
    case LEAVE_ROOM_REQUEST:
        requestResult = leaveRoom(requestInfo);
        break;
    case GET_ROOM_STATE_REQUEST:
        requestResult = getRoomState(requestInfo);
        break;

    case MEMBER_START_GAME_REQUEST:
        requestResult = startGame(requestInfo);

    }
    return requestResult;
}

RequestResult RoomMemberRequestHandler::startGame(const RequestInfo& requestInfo)
{
    RequestResult result;
    StartGameResponse response;

    response.status = SUCCESS;
    response.type = "startGame";
    Game* game = m_handlerFactory.getGameManager().getGameById(m_room.getData().id);

    result.response = JsonResponsePacketSerializer::serializeResponse(response);
    result.newHandler = m_handlerFactory.createGameRequestHandler(m_user, *game);

    return result;
}

RequestResult RoomMemberRequestHandler::leaveRoom(const RequestInfo& requestInfo)
{
    RequestResult res;
    m_roomManager.removeUserFromRoom(m_room.getData().id, m_user);

    LeaveRoomResponse leaveRes;
    leaveRes.status = SUCCESS;
    leaveRes.type = "leaveRoom";
    res.response = JsonResponsePacketSerializer::serializeResponse(leaveRes);
    res.newHandler = m_handlerFactory.createMenuRequestHandler(m_user);
    return res;
}
