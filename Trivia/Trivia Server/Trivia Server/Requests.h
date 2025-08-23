#pragma once
#include <iostream>
#include "Responses.h"

enum RequestId
{
    // The values here are the same of the according response to the request
    LOGIN_REQUEST = LOGIN_RESPONSE,
    SIGNUP_REQUEST = SIGNUP_RESPONSE,

    LOGOUT_REQUEST = LOGOUT_RESPONSE,
    GET_ROOMS_REQUEST = GET_ROOMS_RESPONSE,
    GET_PLAYERS_IN_ROOM_REQUEST = GET_PLAYERS_IN_ROOM_RESPONSE,
    GET_HIGHSCORE_REQUEST = GET_HIGHSCORE_RESPONSE,
    GET_PERSONAL_STATS_REQUEST = GET_PERSONAL_STATS_RESPONSE,
    JOIN_ROOM_REQUEST = JOIN_ROOM_RESPONSE,
    CREATE_ROOM_REQUEST = CREATE_ROOM_RESPONSE,
    LEAVE_ROOM_REQUEST = LEAVE_ROOM_RESPONSE,
    CLOSE_ROOM_REQUEST = CLOSE_ROOM_RESPONSE,
    START_GAME_REQUEST = START_GAME_RESPONSE,
    MEMBER_START_GAME_REQUEST = MEMBER_START_GAME_RESPONSE,
    GET_ROOM_STATE_REQUEST = GET_ROOM_STATE_RESPONSE,
    LEAVE_GAME_REQUEST = LEAVE_GAME_RESPONSE,
    GET_GAME_RESULT_REQUEST = GET_GAME_RESULT_RESPONSE,
    SUBMIT_ANSWER_REQUEST = SUBMIT_ANSWER_RESPONSE,
    GET_QUESTION_REQUEST = GET_QUESTION_RESPONSE
    
};

struct LoginRequest
{
    std::string username;
    std::string password;

};

struct SignupRequest
{
    std::string username;
    std::string password;
    std::string email;

};

struct GetPlayersInRoomRequest
{
    unsigned int roomId;
};

struct JoinRoomRequest
{
    unsigned int roomId;
};

struct CreateRoomRequest
{
    std::string roomName;
    unsigned int maxUsers;
    unsigned int questionCount;
    unsigned int answerTimeout;
};


typedef struct SubmitAnswerRequest {
    int answerId;
    float answerTime;
}SubmitAnswerRequest;

typedef struct SubmitQuestionRequest {
    std::string question;
    std::string correctAns;
    std::string secAns;
    std::string thirdAns;
    std::string fourthAns;
}SubmitQuestionRequest;