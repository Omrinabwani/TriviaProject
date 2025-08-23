#include "Game.h"
#include <vector>
#include <string>
#include <map>
#include "Question.h"

Game::Game()
{
}

Game::Game(const std::vector<Question>& questions, const std::vector<std::string>& players, unsigned int id, IDatabase* DB)
	: m_questions(questions), m_gameId(id), m_playersFinished(0), m_DB(DB)
{
	m_playersInGameResults = 0;
	m_playersFinished = 0;
	Question firstQuestion = m_questions[0];

	for (const auto& player : players)
	{
		GameData data{ firstQuestion, 0, 0, 0.0f};

		m_players[player] = data;
	}
}


std::vector<Question> Game::getQuestionsForUser(const std::string& username)
{
	return m_questions;
}

void Game::submitAnswer(const std::string& username, const int answerId, const float answerTime)
{
	GameData& player = m_players.at(username);
	int questionCount = player.correctAnswerCount + player.wrongAnswerCount;
	int i = 0;
	if (answerId == player.currentQuestion.getCorrectAnswerId())
	{
		player.correctAnswerCount++;
		player.averageAnswerTime = (player.averageAnswerTime * questionCount + answerTime) / (questionCount + 1); // Calculate average answer time
	}
	else
	{
		player.wrongAnswerCount++;
		player.averageAnswerTime = (player.averageAnswerTime * questionCount + answerTime) / (questionCount + 1); // Calculate average answer time
	}
	for (i = 0; i < m_questions.size(); i++)
	{
		if (m_questions[i] == player.currentQuestion)
		{
			if (i + 1 < m_questions.size())
			{
				player.currentQuestion = m_questions[i + 1];
			}
			else
			{
				Question question("", std::vector<std::string>(), 99);
				player.currentQuestion = question;
				m_playersFinished++;
			}
			break;
		}
	}
	submitGameStatsToDatabase(username, m_players.at(username));
}

void Game::removePlayer(const std::string& username)
{
	m_players.erase(username);
	// We don't remove the player from the database because we need to show that player game data later on
}

Question Game::getQuestionForUser(const std::string& username)
{
	return m_players.at(username).currentQuestion;
}

std::map<std::string, GameData> Game::getPlayers()
{
	return m_players;
}

unsigned int Game::getGameId()
{
	return m_gameId;
}

unsigned int Game::getFinishedPlayers()
{
	return m_playersFinished;
}

void Game::incrementUsersInGameResults()
{
	m_playersInGameResults++;
}

unsigned int Game::getPlayersInGameResult()
{
	return m_playersInGameResults;
}

void Game::submitGameStatsToDatabase(const std::string& username, const GameData& gameData)
{
	m_DB->submitGameStatistics(username, gameData);
}
