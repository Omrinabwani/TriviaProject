#include <string>
#include "SqliteDatabase.h"
#include "Exceptions.h"
#include "Game.h"

#define DB_FILE_NAME "TriviaDB.sqlite"
#define QUESTION_ANSWERS 4
#define HIGH_SCORES 5

struct TempGameData {
	int correctAnswers = 0;
	int wrongAnswers = 0;
	float averageAnswerTime = 0.0f;
};

bool SqliteDatabase::open()
{
	// We check if the file exists before opening it because after opening it
	// the file will exist for sure
	std::string sqlStatement = "";
	int fileExists = _access(DB_FILE_NAME, 0); // 0 as parameter for checking whether the file exists
	int result = sqlite3_open(DB_FILE_NAME, &m_DB); // Opening the Database (or creating the .sqlite file if the DB isn't yet)
	char* errorMessage = nullptr;

	if (result != SQLITE_OK)
	{
		m_DB = nullptr;
		throw SqliteDatabaseException("Failed to open Sqlite Database");
	}

	
	if (fileExists == 0) // 0 means the file exists
	{
		return true;
	}
	
	// The DB didn't exist, we need to init it here

	// Creating the USERS table
	sqlStatement = 
		R"(
		CREATE TABLE USERS (
		NAME TEXT NOT NULL PRIMARY KEY, PASSWORD TEXT NOT NULL, EMAIL TEXT NOT NULL
		);
		)";

	result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);

	// Creating QUESTIONS table
	sqlStatement =
		R"(
		CREATE TABLE QUESTIONS (
		QUESTION TEXT NOT NULL PRIMARY KEY,
		ANSWER1 TEXT NOT NULL,
		ANSWER2 TEXT NOT NULL,
		ANSWER3 TEXT NOT NULL,
		ANSWER4 TEXT NOT NULL,
		CORRECT_ANSWER_ID INTEGER NOT NULL
		)
		)";


	result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);

	// Creating STATISTICS table
	sqlStatement =
		R"(
		CREATE TABLE STATISTICS (
		USERNAME TEXT NOT NULL PRIMARY KEY,
		AVERAGE_ANSWER_TIME FLOAT NOT NULL,
		CORRECT_ANSWERS INTEGER NOT NULL,
		TOTAL_ANSWERS INTEGER NOT NULL,
		GAMES_PLAYED INTEGER NOT NULL,
		SCORE INTEGER NOT NULL
		)
		)";


	result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);

	// Creating GAME table
	sqlStatement =
		R"(
		CREATE TABLE GAMES (
		USERNAME TEXT NOT NULL PRIMARY KEY,
		CORRECT_ANSWERS INTEGER NOT NULL,
		WRONG_ANSWERS INTEGER NOT NULL,
		AVERAGE_ANSWER_TIME FLOAT NOT NULL,
		FOREIGN KEY (USERNAME) REFERENCES USERS (NAME)
		)
		)";


	result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);

	return true;
}

bool SqliteDatabase::close()
{
	int result = sqlite3_close(m_DB);

	if (result != SQLITE_OK)
	{
		throw SqliteDatabaseException("Failed to close Sqlite Database");
	}

	// After closing the DB connection m_DB is not a pointer to memory that isn't being used - 
	// thus we set it to nullptr
	m_DB = nullptr;

	return true;
}

int SqliteDatabase::doesUserExist(const std::string& username)
{
	bool userExists = false;

	char* errorMessage;
	std::string sqlStatement = R"(SELECT COUNT(*) FROM USERS WHERE NAME = ")" + username + R"(";)";

	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), doesRowExistsCallback, &userExists, &errorMessage);
	checkSqliteResult(result, errorMessage);

	return userExists;
}

int SqliteDatabase::doesPasswordMatch(const std::string& username, const std::string& password)
{
	bool passwordMatches = false;

	char* errorMessage;
	std::string sqlStatement = R"(SELECT COUNT(*) FROM USERS WHERE NAME = ")" + username + R"(" AND PASSWORD = ")" + password + R"(";)";

	// We pass the &passwordMatches to the doesRowExistsCallback because if there is a row with the specified in the 
	// sqlStatement it means the passsword matches to the username
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), doesRowExistsCallback, &passwordMatches, &errorMessage);
	checkSqliteResult(result, errorMessage);

	return passwordMatches;
}


int SqliteDatabase::addNewUser(const std::string& username, const std::string& password, const std::string& email)
{
	std::string sqlStatement = 
		R"(INSERT INTO USERS (NAME, PASSWORD, EMAIL) VALUES (")" + username + R"(", ")" + password + R"(", ")" + email + R"(");)";
	std::string statsSql =
		R"(INSERT INTO STATISTICS (USERNAME, AVERAGE_ANSWER_TIME, CORRECT_ANSWERS, TOTAL_ANSWERS, GAMES_PLAYED, SCORE)
       VALUES (")" + username + R"(", 0.0, 0, 0, 0, 0);)";

	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);
	
	result = sqlite3_exec(m_DB, statsSql.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);

	return 0;
}

std::vector<Question> SqliteDatabase::getQuestions(const int numQuestions)
{
	std::vector<Question> questions;
	std::string sqlStatement = 
		R"(SELECT * FROM QUESTIONS ORDER BY RANDOM() LIMIT )" + std::to_string(numQuestions) + R"(;)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), getQuestionsCallback, &questions, &errorMessage);

	checkSqliteResult(result, errorMessage);
	
	return questions;
}

float SqliteDatabase::getPlayerAverageAnswerTime(const std::string& username)
{
	float averageAnswerTime = 0;
	std::string sqlStatement =
		R"(SELECT AVERAGE_ANSWER_TIME FROM STATISTICS WHERE USERNAME = ")" + username + R"(";)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), getOneFloatCallback, &averageAnswerTime, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return averageAnswerTime;
}

int SqliteDatabase::getNumOfCorrectAnswers(const std::string& username)
{
	int correctAnswers = 0;
	std::string sqlStatement =
		R"(SELECT CORRECT_ANSWERS FROM STATISTICS WHERE USERNAME = ")" + username + R"(";)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), getOneIntCallback, &correctAnswers, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return correctAnswers;
}

int SqliteDatabase::getNumOfTotalAnswers(const std::string& username)
{
	int totalAnswers = 0;
	std::string sqlStatement =
		R"(SELECT TOTAL_ANSWERS FROM STATISTICS WHERE USERNAME = ")" + username + R"(";)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), getOneIntCallback, &totalAnswers, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return totalAnswers;
}

int SqliteDatabase::getNumOfPlayerGames(const std::string& username)
{
	int gamesPlayed = 0;
	std::string sqlStatement =
		R"(SELECT GAMES_PLAYED FROM STATISTICS WHERE USERNAME = ")" + username + R"(";)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), getOneIntCallback, &gamesPlayed, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return gamesPlayed;
}

int SqliteDatabase::getPlayerScore(const std::string& username)
{
	int score = 0;
	std::string sqlStatement =
		R"(SELECT SCORE FROM STATISTICS WHERE USERNAME = ")" + username + R"(";)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), getOneIntCallback, &score, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return score;
}

std::vector<std::pair<std::string, int>> SqliteDatabase::getHighScores()
{
	std::vector<std::pair<std::string, int>> highScores;
	std::string sqlStatement =
		R"(SELECT USERNAME, SCORE FROM STATISTICS ORDER BY SCORE DESC LIMIT )" + std::to_string(HIGH_SCORES) + R"(;)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), highScoresCallback, &highScores, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return highScores;
}

UserStatistics SqliteDatabase::getUserStatistics(const std::string username)
{
	UserStatistics userStatistics;
	std::string sqlStatement =
		R"(SELECT AVERAGE_ANSWER_TIME, CORRECT_ANSWERS, TOTAL_ANSWERS, GAMES_PLAYED, SCORE FROM STATISTICS WHERE USERNAME = ")" + username + R"(";)";
	char* errorMessage = nullptr;

	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), userStatisticsCallback, &userStatistics, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return userStatistics;
}

int SqliteDatabase::addNewQuestion(const Question& question)
{
	std::string questionString = question.getQuestion();
	std::vector<std::string> possibleAnswers = question.getPossibleAnswers();
	char* errorMessage = nullptr;
	int correctAnswerId = question.getCorrectAnswerId();
	int result = 0;

	std::string sqlStatement =
		R"(INSERT INTO QUESTIONS (QUESTION, ANSWER1, ANSWER2, ANSWER3, ANSWER4, CORRECT_ANSWER_ID) VALUES (")" 
		+ questionString;
	for (int i = 0; i < QUESTION_ANSWERS; i++)
	{
		sqlStatement += R"(", ")" + possibleAnswers[i];
	}
	sqlStatement += R"(", ")" + std::to_string(correctAnswerId) + R"(");)";
	
	result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);

	return 0;
}

int SqliteDatabase::deleteQuestion(const std::string& questionString)
{
	std::string sqlStatement = R"(DELETE FROM QUESTION WHERE QUESTIONS = ")" + questionString + R"(";)";
	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return 0;
}

int SqliteDatabase::submitGameStatistics(const std::string& username, GameData gameData)
{
	
	std::string sqlStatement = R"(INSERT OR REPLACE INTO GAMES (USERNAME, CORRECT_ANSWERS, WRONG_ANSWERS, AVERAGE_ANSWER_TIME) VALUES (")" +
		username + R"(", ")" +
		std::to_string(gameData.correctAnswerCount) + R"(", ")" +
		std::to_string(gameData.wrongAnswerCount) + R"(", ")" +
		std::to_string(gameData.averageAnswerTime) + R"("))";

	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);

	checkSqliteResult(result, errorMessage);

	return 0;
}

int SqliteDatabase::doesRowExistsCallback(void* data, int argc, char** argv, char** azColName)
{
	bool* pRowExists = (bool*)data;

	if (std::stoi(argv[0]) == 0)
	{
		*pRowExists = false;
	}
	else
	{
		*pRowExists = true;
	}

	return 0;
}

int SqliteDatabase::getQuestionsCallback(void* data, int argc, char** argv, char** azColname)
{
	std::vector<Question>* questions = (std::vector<Question>*)data;
	
	std::string question = argv[0];
	std::vector<std::string> possibleAnswers = { argv[1], argv[2], argv[3], argv[4] };
	int correctAnswerId = std::stoi(argv[5]);
	questions->push_back(Question(question, possibleAnswers, correctAnswerId));

	return 0;
}

int SqliteDatabase::getOneFloatCallback(void* data, int argc, char** argv, char** azColName)
{
	float* num = (float*)data;

	*num = std::stof(argv[0]);

	return 0;
}

int SqliteDatabase::getOneIntCallback(void* data, int argc, char** argv, char** azColName)
{
	int* num = (int*)data;

	*num = std::stoi(argv[0]);

	return 0;
}

int SqliteDatabase::highScoresCallback(void* data, int argc, char** argv, char** azColName)
{
	std::vector<std::pair<std::string, int>>* highScores = (std::vector<std::pair<std::string, int>>*)data;
	std::pair<std::string, int> highScore;

	highScores->emplace_back(argv[0], std::stoi(argv[1]));

	return 0;
}


int SqliteDatabase::userStatisticsCallback(void* data, int argc, char** argv, char** azColName)
{
	int i = 0;
	UserStatistics* userStatistics = (UserStatistics*)data;
	if (argc < 5 || argv[0] == nullptr)
	{
		userStatistics->averageAnswerTime = 0;
		userStatistics->correctAnswers = 0;
		userStatistics->totalAnswers = 0;
		userStatistics->gamesPlayed = 0;
		userStatistics->score = 0;
		return 0;
	}

	// The order must be the following (as stored in the DB)
	userStatistics->averageAnswerTime = std::stof(argv[i++]);
	userStatistics->correctAnswers = std::stoi(argv[i++]);
	userStatistics->totalAnswers = std::stoi(argv[i++]);
	userStatistics->gamesPlayed = std::stoi(argv[i++]);
	userStatistics->score = std::stoi(argv[i]);

	return 0;
}

int gameDataCallback(void* data, int argc, char** argv, char** azColName)
{
	

	if (argc == 3 && argv[0] && argv[1] && argv[2])
	{
		TempGameData* gd = static_cast<TempGameData*>(data);
		gd->correctAnswers = std::stoi(argv[0]);
		gd->wrongAnswers = std::stoi(argv[1]);
		gd->averageAnswerTime = std::stof(argv[2]);
	}
	return 0;
}


void SqliteDatabase::updateStatisticsFromGameTable(const std::string& username)
{
	UserStatistics currentStats = getUserStatistics(username);

	std::string sqlGetGameData =
		R"(SELECT CORRECT_ANSWERS, WRONG_ANSWERS, AVERAGE_ANSWER_TIME FROM GAMES WHERE USERNAME = ")" + username + R"(";)";

	TempGameData gameData;

	char* errorMessage = nullptr;
	int result = sqlite3_exec(m_DB, sqlGetGameData.c_str(), gameDataCallback, &gameData, &errorMessage);
	checkSqliteResult(result, errorMessage);

	int newCorrectAnswers = currentStats.correctAnswers + gameData.correctAnswers;
	int newTotalAnswers = currentStats.totalAnswers + gameData.correctAnswers + gameData.wrongAnswers;
	int newGamesPlayed = currentStats.gamesPlayed + 1; 

	float totalOldTime = currentStats.averageAnswerTime * currentStats.totalAnswers;
	int gameTotalAnswers = gameData.correctAnswers + gameData.wrongAnswers;
	float totalNewTime = gameData.averageAnswerTime * gameTotalAnswers;
	float newAverageAnswerTime = 0.0f;

	if (newTotalAnswers > 0)
	{
		newAverageAnswerTime = (totalOldTime + totalNewTime) / newTotalAnswers;
	}

	int newScore = newCorrectAnswers * 10;

	std::string sqlUpdateStats =
		R"(UPDATE STATISTICS SET )"
		R"(CORRECT_ANSWERS = )" + std::to_string(newCorrectAnswers) + ", " +
		R"(TOTAL_ANSWERS = )" + std::to_string(newTotalAnswers) + ", " +
		R"(AVERAGE_ANSWER_TIME = )" + std::to_string(newAverageAnswerTime) + ", " +
		R"(GAMES_PLAYED = )" + std::to_string(newGamesPlayed) + ", " +
		R"(SCORE = )" + std::to_string(newScore) +
		R"( WHERE USERNAME = ")" + username + R"(";)";

	result = sqlite3_exec(m_DB, sqlUpdateStats.c_str(), nullptr, nullptr, &errorMessage);
	checkSqliteResult(result, errorMessage);
}


void SqliteDatabase::checkSqliteResult(const int sqliteResult, const char* errorMessage)
{	
	if (sqliteResult != SQLITE_OK)
	{
		throw SqliteDatabaseException(errorMessage);
	}
}
