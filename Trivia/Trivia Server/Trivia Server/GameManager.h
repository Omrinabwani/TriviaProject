#pragma once
#include <vector>
#include "IDatabase.h"
#include "Room.h"
#include "Game.h"
#include <map>

class GameManager {
public:
	GameManager(IDatabase* database);

	void updateStatistcs(const std::string& username);
	Game& createGame(const Room& room);
	void deleteGame(int gameId);
	Game* getGameById(int gameId);

private:
	IDatabase* m_database{};
	std::map<int, Game> m_games;
};

