#include "JsonResponsePacketSerializer.h"
#include "json.hpp"
#include "Room.h"
#include <vector>
#include <string>


Buffer JsonResponsePacketSerializer::serializeResponse(const LoginResponse& loginResponse)
{
    nlohmann::json content;

    content["status"] = loginResponse.status; // Adding a key to the json named "status" with its value

    return buildBuffer(LOGIN_RESPONSE, content);
}


Buffer JsonResponsePacketSerializer::serializeResponse(const SignupResponse& signupResponse)
{
    nlohmann::json content;

    content["status"] = signupResponse.status;

    return buildBuffer(SIGNUP_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& errorResponse)
{
    nlohmann::json content;

    content["message"] = errorResponse.message;

    return buildBuffer(ERROR_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& logoutResponse)
{
    nlohmann::json content;

    content["status"] = logoutResponse.status;

    return buildBuffer(LOGOUT_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetRoomsResponse& getRoomsResponse)
{
    nlohmann::json content;
    nlohmann::json roomsJson = nlohmann::json::array();

    for (const RoomData& room : getRoomsResponse.rooms)
    {
        roomsJson.push_back(roomDataToJson(room));
    }

    content["status"] = getRoomsResponse.status;
    content["rooms"] = roomsJson;

    return buildBuffer(GET_ROOMS_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& getPlayersInRoomsResponse)
{
    nlohmann::json content;

    content["players"] = getPlayersInRoomsResponse.players;

    return buildBuffer(GET_PLAYERS_IN_ROOM_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& joinRoomResponse)
{
    nlohmann::json content;

    content["status"] = joinRoomResponse.status;
    return buildBuffer(JOIN_ROOM_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& createRoomResponse)
{
    nlohmann::json content;

    content["status"] = createRoomResponse.status;
    content["roomID"] = createRoomResponse.roomID;

    return buildBuffer(CREATE_ROOM_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetHighScoreResponse& getHighScoreResponse)
{
    nlohmann::json content;

    content["status"] = getHighScoreResponse.status;
    content["statistics"] = getHighScoreResponse.statistics;

    return buildBuffer(GET_HIGHSCORE_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetPersonalStatsResponse& getPersonalStatsResponse)
{
    nlohmann::json content;

    content["status"] = getPersonalStatsResponse.status;
    content["statistics"] = nlohmann::json::parse(userStatisticsToJsonString(getPersonalStatsResponse.statistics)); // the last error

    return buildBuffer(GET_PERSONAL_STATS_RESPONSE, content);
}

Buffer JsonResponsePacketSerializer::serializeResponse(const LeaveRoomResponse& leaveRoomResponse)
{
    nlohmann::json j;
    j["status"] = leaveRoomResponse.status;
    j["type"] = leaveRoomResponse.type;
    return buildBuffer(LEAVE_ROOM_RESPONSE, j);
}
Buffer JsonResponsePacketSerializer::serializeResponse(const CloseRoomResponse& closeRoomResponse)
{
    nlohmann::json j;
    j["status"] = closeRoomResponse.status;
    return buildBuffer(CLOSE_ROOM_RESPONSE, j);
}
Buffer JsonResponsePacketSerializer::serializeResponse(const StartGameResponse& startGameResponse)
{
    nlohmann::json j;
    j["status"] = startGameResponse.status;
    j["type"] = startGameResponse.type;
    return buildBuffer(START_GAME_RESPONSE, j);
}




Buffer JsonResponsePacketSerializer::serializeResponse(const GetRoomStateResponse& res)
{
    nlohmann::json j;
    j["status"] = res.status;
    j["hasGameBegun"] = res.hasGameBegun;
    j["questionCount"] = res.questionCount;
    j["answerTimeout"] = res.answerTimeout;
    j["players"] = res.players;
    return buildBuffer(GET_ROOM_STATE_RESPONSE, j);
    
}


Buffer JsonResponsePacketSerializer::serializeGetQuestionResponse(const GetQuestionResponse& res)
{
    nlohmann::json responseJson;
    responseJson["status"] = res.status;
    responseJson["question"] = res.question;

    nlohmann::json answersJson = nlohmann::json::object();
    for (const auto& pair : res.answers)
    {
        answersJson[std::to_string(pair.first)] = pair.second;
    }

    responseJson["answers"] = answersJson;
    return buildBuffer(GET_QUESTION_RESPONSE, responseJson);
}

Buffer JsonResponsePacketSerializer::serializeGetGameResultsResponse(GetGameResultsResponse& res)
{
    nlohmann::json j;
    j["status"] = res.status;

    nlohmann::json playerResultsArray = nlohmann::json::array();

    for (std::vector<PlayerResults>::iterator it = res.results.begin(); it != res.results.end(); ++it)
    {
        nlohmann::json playerJson;
        playerJson["username"] = it->username;
        playerJson["correctAnswerCount"] = it->correctAnswerCount;
        playerJson["wrongAnswerCount"] = it->wrongAnswerCount;
        playerJson["averageAnswerTime"] = it->averageAnswerTime;
        playerJson["score"] = it->score;

        playerResultsArray.push_back(playerJson);
    }
    j["results"] = playerResultsArray;
    return buildBuffer(GET_GAME_RESULT_RESPONSE, j);
}

Buffer JsonResponsePacketSerializer::serializeSubmitAnswerResponse(const SubmitAnswerResponse& res)
{
    nlohmann::json j;
    j["status"] = res.status;
    return buildBuffer(SUBMIT_ANSWER_RESPONSE, j);
}

Buffer JsonResponsePacketSerializer::serializeLeaveGameResponse(const LeaveGameResponse& res)
{
    nlohmann::json j;
    j["status"] = res.status;
    return buildBuffer(LEAVE_GAME_RESPONSE, j);
}




std::string JsonResponsePacketSerializer::roomDataToJsonString(const RoomData& roomData)
{
    nlohmann::json json;

    json["id"] = roomData.id;
    json["name"] = roomData.name;
    json["maxPlayers"] = roomData.maxPlayers;
    json["questionCount"] = roomData.numOfQuestionsInGame;
    json["timePerQuestion"] = roomData.timePerQuestion;
    json["status"] = roomData.isActive;

    return json.dump();
}
nlohmann::json JsonResponsePacketSerializer::roomDataToJson(const RoomData& room)
{
    return {
        {"id", room.id},
        {"name", room.name},
        {"maxPlayers", room.maxPlayers},
        {"timePerQuestion", room.timePerQuestion},
        {"isActive", room.isActive}
    };
}


std::string JsonResponsePacketSerializer::userStatisticsToJsonString(const UserStatistics& userStatistics)
{
    nlohmann::json json;

    json["averageAnswerTime"] = userStatistics.averageAnswerTime;
    json["correctAnswers"] = userStatistics.correctAnswers;
    json["totalAnswers"] = userStatistics.totalAnswers;
    json["gamesPlayed"] = userStatistics.gamesPlayed;
    json["score"] = userStatistics.score;

    return json.dump();
}

// Buffer structure: [1 byte code] + [4 byte size] + [json string]
Buffer JsonResponsePacketSerializer::buildBuffer(const ResponseId code, const nlohmann::json& jsonFile)
{
    Buffer buffer;
    std::string jsonStr = jsonFile.dump();
    int length = jsonStr.length();

    
    // Message code (1 byte)
    buffer.push_back(code);

    // Message size (4 bytes, big endian)
    buffer.push_back((length >> 24) & 0xFF); // length >> 24 means shifting the length by 3 bytes to the left giving the MSB
    buffer.push_back((length >> 16) & 0xFF); // & 0xFF is a bitwise AND operator, making sure to zero out all the garbage values
    buffer.push_back((length >> 8) & 0xFF);
    buffer.push_back(length & 0xFF);

    // JSON string
    buffer.insert(buffer.end(), jsonStr.begin(), jsonStr.end());

    return buffer;
}