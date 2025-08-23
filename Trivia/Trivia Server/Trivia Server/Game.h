#pragma once
#include <vector>
#include <string>
#include <map>
#include "LoggedUser.h"
#include "Question.h"
#include "IDatabase.h"

struct GameData
{
	Question currentQuestion;
	unsigned int correctAnswerCount{};
	unsigned int wrongAnswerCount{};
	float averageAnswerTime{};
};

class Game 
{
public:
	Game();
	Game(const std::vector<Question>& questions,const std::vector<std::string>& players, unsigned int id, IDatabase* m_DB);
	std::vector<Question> getQuestionsForUser(const std::string& username);
	void submitAnswer(const std::string& username, const int answerId, const float answerTime);
	void removePlayer(const std::string& username);
	Question getQuestionForUser(const std::string& username);
	std::map<std::string, GameData> getPlayers();
	unsigned int getGameId();
	unsigned int getFinishedPlayers();
	void incrementUsersInGameResults();
	unsigned int getPlayersInGameResult();

private:
	IDatabase* m_DB{};
	std::vector<Question> m_questions;
	std::map<std::string, GameData> m_players;
	unsigned int m_gameId;
	unsigned int m_playersFinished;
	unsigned int m_playersInGameResults;
	// Submits the game stats for the specified player
	void submitGameStatsToDatabase(const std::string& username, const GameData& gameData);
};
