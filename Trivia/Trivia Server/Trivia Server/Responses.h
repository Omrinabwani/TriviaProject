#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Room.h"
#include <map>
#include "UserStatistics.h"

enum ResponseId : unsigned int
{
	// The values represent the version of Trivia (10, 11... for 1.0.x, 20, 21.. for for 2.0.x, etc.)
	// They are also sorted by the order they appear in the UML
	ERROR_RESPONSE = 10,
	LOGIN_RESPONSE = 11,
	SIGNUP_RESPONSE = 12,

	LOGOUT_RESPONSE = 20,
	GET_ROOMS_RESPONSE = 21,
	GET_PLAYERS_IN_ROOM_RESPONSE = 22,
	GET_HIGHSCORE_RESPONSE = 23,
	GET_PERSONAL_STATS_RESPONSE = 24,
	JOIN_ROOM_RESPONSE = 25,
	CREATE_ROOM_RESPONSE = 26,

	LEAVE_ROOM_RESPONSE = 30,
	CLOSE_ROOM_RESPONSE = 31,
	START_GAME_RESPONSE = 32,
	GET_ROOM_STATE_RESPONSE = 33,
	MEMBER_START_GAME_RESPONSE = 34,

	GET_QUESTION_RESPONSE = 40,
	GET_GAME_RESULT_RESPONSE = 41,
	SUBMIT_ANSWER_RESPONSE = 42,
	LEAVE_GAME_RESPONSE = 43
};

enum ResponseStatus : unsigned int
{
	SUCCESS = 0,
	FAILED = 1,
};


struct ErrorResponse
{
	std::string message = "ERROR";
};

struct LoginResponse
{
	int status;
};

struct SignupResponse
{
	ResponseStatus status{};
};

struct LogoutResponse
{
	ResponseStatus status{};
};

struct GetRoomsResponse
{
	ResponseStatus status{};
	std::vector<RoomData> rooms{};
};

struct GetPlayersInRoomResponse
{
	std::vector<std::string> players{};
};

struct GetHighScoreResponse
{
	ResponseStatus status{};
	std::vector<std::pair<std::string, int>> statistics{};
};

struct GetPersonalStatsResponse
{
	ResponseStatus status{};
	UserStatistics statistics{};
};

struct JoinRoomResponse
{
	ResponseStatus status{};
};

struct CreateRoomResponse
{
	ResponseStatus status{};
	int roomID{};
};

struct LeaveRoomResponse
{
	unsigned int status;
	std::string type;
}typedef LeaveRoomResponse;

struct CloseRoomResponse
{
	unsigned int status;
}typedef CloseRoomResponse;

struct StartGameResponse
{
	unsigned int status;
	std::string type;
}typedef StartGameResponse;

struct GetRoomStateResponse
{
	unsigned int status;
	bool hasGameBegun;
	std::vector<std::string> players;
	unsigned int questionCount;
	unsigned int answerTimeout;

}typedef GetRoomStateResponse;


struct LeaveGameResponse
{
	unsigned int status;
}typedef LeaveGameResponse;


struct GetQuestionResponse
{
	unsigned int status;
	std::string question;
	std::map<unsigned int, std::string> answers;

}typedef GetQuestionResponse;
struct PlayerResults
{
	std::string username;
	unsigned int correctAnswerCount;
	unsigned int wrongAnswerCount;
	float averageAnswerTime;
	int score;

}typedef PlayerResults;

struct GetGameResultsResponse
{
	unsigned int status;
	std::vector<PlayerResults> results;

}typedef GetGameResultsResponse;

struct SubmitAnswerResponse
{
	unsigned int status;
}typedef GetSubmitAnswerResponse;

struct GetScoresResponse
{
	unsigned int status;
	std::vector<std::string> scores;
}typedef GetScoresResponse;