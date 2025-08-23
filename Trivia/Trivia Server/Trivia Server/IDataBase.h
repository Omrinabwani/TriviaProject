#pragma once
#include <iostream>
#include "Question.h"
#include "UserStatistics.h"
class Game;
struct GameData;

class IDatabase
{
public:

	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual int doesUserExist(const std::string& username) = 0;
	virtual int doesPasswordMatch(const std::string& username, const std::string& password) = 0;
	virtual int addNewUser(const std::string& username, const std::string& password, const std::string& email) = 0;

	virtual std::vector<Question> getQuestions(const int numQuestions) = 0;

	virtual float getPlayerAverageAnswerTime(const std::string& username) = 0;
	virtual int getNumOfCorrectAnswers(const std::string& username) = 0;
	virtual int getNumOfTotalAnswers(const std::string& username) = 0;
	virtual int getNumOfPlayerGames(const std::string& username) = 0;
	virtual int getPlayerScore(const std::string& username) = 0;
	virtual std::vector<std::pair<std::string, int>> getHighScores() = 0;
	virtual UserStatistics getUserStatistics(const  std::string username) = 0;

	virtual int submitGameStatistics(const std::string& username, GameData gameData) = 0;
	//void getSecurityKey(std::string);
	virtual void updateStatisticsFromGameTable(const std::string& username) = 0;
	// Admin only
	virtual int addNewQuestion(const Question& question) = 0;
	virtual int deleteQuestion(const std::string& questionString) = 0;
};