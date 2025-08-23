#include "RequestHandlerFactory.h"
#include "MenuRequestHandler.h"
#include "LoginRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"

RequestHandlerFactory::RequestHandlerFactory(IDatabase* database) :
    m_database(database), m_loginManager(database), m_statisticsManager(database), m_gameManager(database)
{
    
}

LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler(const SOCKET socket)
{
    return new LoginRequestHandler(*this, socket);
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
    return m_loginManager;
}

MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(const LoggedUser& loggedUser)
{
    return new MenuRequestHandler(*this, loggedUser);
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
    return m_statisticsManager;
}

RoomManager& RequestHandlerFactory::getRoomManager()
{
    return m_roomManager;
}

RoomAdminRequestHandler* RequestHandlerFactory::createRoomAdminRequestHandler(const LoggedUser& user, const Room& room)
{
    return new RoomAdminRequestHandler(room, user, m_roomManager, *this);
}

RoomMemberRequestHandler* RequestHandlerFactory::createRoomMemberRequestHandler(const LoggedUser& user, const Room& room)
{
    return new RoomMemberRequestHandler(room, user, m_roomManager, *this);
}

GameRequestHandler* RequestHandlerFactory::createGameRequestHandler(LoggedUser& user, Game& game)
{
    return new GameRequestHandler(*this, user, game);
}

GameManager& RequestHandlerFactory::getGameManager()
{
    return m_gameManager;
}
