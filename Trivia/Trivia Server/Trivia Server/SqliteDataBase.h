#pragma once
#include <io.h>
#include<iostream>
#include <vector>
#include "IDatabase.h"
#include "sqlite3.h"


class SqliteDatabase : public IDatabase
{
public:
	virtual bool open() override;
	virtual bool close() override;
	virtual int doesUserExist(const std::string& username) override;
	virtual int doesPasswordMatch(const std::string& username, const std::string& password) override;
	virtual int addNewUser(const std::string& username, const std::string& password, const std::string& email) override;

	virtual std::vector<Question> getQuestions(const int numQuestions) override;
	
	virtual float getPlayerAverageAnswerTime(const std::string& username) override;
	virtual int getNumOfCorrectAnswers(const std::string& username) override;
	virtual int getNumOfTotalAnswers(const std::string& username) override;
	virtual int getNumOfPlayerGames(const std::string& username) override;
	virtual int getPlayerScore(const std::string& username) override;
	virtual std::vector<std::pair<std::string, int>> getHighScores() override;
	virtual UserStatistics getUserStatistics(const  std::string username) override;
	void updateStatisticsFromGameTable(const std::string& username) override;
	// Used only by admin commands using the server class
	virtual int addNewQuestion(const Question& question) override;
	virtual int deleteQuestion(const std::string& questionString) override;

	virtual int submitGameStatistics(const std::string& username, GameData gameData) override;
private:
	sqlite3* m_DB{};

	// Callbacks
	static int doesRowExistsCallback(void* data, int argc, char** argv, char** azColName);
	static int getQuestionsCallback(void* data, int argc, char** argv, char** azColname);
	static int getOneFloatCallback(void* data, int argc, char** argv, char** azColName);
	static int getOneIntCallback(void* data, int argc, char** argv, char** azColName);
	static int highScoresCallback(void* data, int argc, char** argv, char** azColName);
	static int userStatisticsCallback(void* data, int argc, char** argv, char** azColName);

	// Throws an SqliteDatabaseException containing the errorMessage if the sqliteResult is not SQLITE_OK
	void checkSqliteResult(const int sqliteResult, const char* errorMessage); 
};
