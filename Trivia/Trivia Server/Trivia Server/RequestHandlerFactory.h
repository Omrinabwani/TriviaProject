#pragma once
#include "IDatabase.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameManager.h"
#include "RequestHandlerFactory.h"
#include "LoginManager.h"
#include "GameManager.h"
#include "Game.h"
// Preventing the circular referencing with RequestHandlerFactory and LoginRequestHandler because
// Both of them use each other
class LoginRequestHandler;
class MenuRequestHandler;
class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class GameRequestHandler;

class RequestHandlerFactory
{
public:
	RequestHandlerFactory(IDatabase* database);

	LoginRequestHandler* createLoginRequestHandler(const SOCKET socket);
	LoginManager& getLoginManager();

	MenuRequestHandler* createMenuRequestHandler(const LoggedUser& loggedUser);
	StatisticsManager& getStatisticsManager();
	RoomManager& getRoomManager();

	RoomAdminRequestHandler* createRoomAdminRequestHandler(const LoggedUser& user, const Room& room);
	RoomMemberRequestHandler* createRoomMemberRequestHandler(const LoggedUser& user, const Room& room);

	GameRequestHandler* createGameRequestHandler(LoggedUser& user, Game& game);
	GameManager& getGameManager();

private:
	IDatabase* m_database;
	LoginManager m_loginManager;
	RoomManager m_roomManager;
	StatisticsManager m_statisticsManager;
	GameManager m_gameManager;
};
