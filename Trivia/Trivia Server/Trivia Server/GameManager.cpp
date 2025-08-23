#include "GameManager.h"
#include <vector>
#include "Game.h"
GameManager::GameManager(IDatabase* database)
: m_database(database) {}
void GameManager::updateStatistcs(const std::string& username)
{
	m_database->updateStatisticsFromGameTable(username);
}
Game& GameManager::createGame(const Room& room)
{
	std::vector<Question> q = m_database->getQuestions(room.getData().numOfQuestionsInGame);
	std::vector<std::string> players = room.getAllUsers();

	int roomId = room.getData().id;
	m_games[roomId] = Game(q, players, roomId, m_database);
	return m_games[roomId];
}

void GameManager::deleteGame(int gameId)
{
	m_games.erase(gameId);
}

Game* GameManager::getGameById(int gameId)
{
	auto it = m_games.find(gameId);
	if (it != m_games.end())
	{
		return &(it->second);
	}
	return nullptr;
}

