#pragma once
#include "json.hpp"
#include "Buffer.h"
#include "Responses.h"
#include "LoginManager.h"
#include "UserStatistics.h"

class JsonResponsePacketSerializer
{
public:
	static Buffer serializeResponse(const LoginResponse& loginResponse);
	static Buffer serializeResponse(const SignupResponse& signupResponse);
	static Buffer serializeResponse(const ErrorResponse& errrorResponse);
	
	static Buffer serializeResponse(const LogoutResponse& logoutResponse);
	static Buffer serializeResponse(const GetRoomsResponse& getRoomsResponse);
	static Buffer serializeResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse);
	static Buffer serializeResponse(const JoinRoomResponse& joinRoomResponse);
	static Buffer serializeResponse(const CreateRoomResponse& createRoomResponse);
	static Buffer serializeResponse(const GetHighScoreResponse& getHighScoreResponse);
	static Buffer serializeResponse(const GetPersonalStatsResponse& getPersonalStatsResponse);
	static Buffer serializeResponse(const CloseRoomResponse& closeRoomResponse);
	static Buffer serializeResponse(const LeaveRoomResponse& leaveRoomResponse);
	static Buffer serializeResponse(const StartGameResponse& startGameResponse);
	static Buffer serializeResponse(const GetRoomStateResponse& res);

	static Buffer serializeGetQuestionResponse(const GetQuestionResponse& res);
	static Buffer serializeGetGameResultsResponse(GetGameResultsResponse& res);
	static Buffer serializeSubmitAnswerResponse(const SubmitAnswerResponse& res);
	static Buffer serializeLeaveGameResponse(const LeaveGameResponse& res);

private:
	static nlohmann::json roomDataToJson(const RoomData& room);
	static std::string roomDataToJsonString(const RoomData& roomData);
	static std::string userStatisticsToJsonString(const UserStatistics& userStatistics);
	static Buffer buildBuffer(const ResponseId code, const nlohmann::json& j);
};
