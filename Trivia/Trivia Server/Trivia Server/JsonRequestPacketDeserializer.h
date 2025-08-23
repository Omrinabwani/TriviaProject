#pragma once
#include <string>
#include <vector>
#include <chrono>

#include "json.hpp"
#include "Buffer.h"
#include "Requests.h"


class JsonRequestPacketDeserializer
{
private:
    static std::string convertBufferToJsonString(const Buffer& bufferedRequest);

public:
    static int convertBytesToInt(const unsigned char byte1, const unsigned char byte2, const unsigned char byte3, const unsigned char byte4); // Must be exactly 4 bytes
    static LoginRequest deserializeLoginRequest(const Buffer& bufferedLoginRequest);
    static SignupRequest deserializeSignupRequest(const Buffer& bufferedSingupRequest);

    static GetPlayersInRoomRequest deserializeGetPlayersRequest(const Buffer& bufferedGetPlayersRequest);
    static JoinRoomRequest deserializeJoinRoomRequest(const Buffer& bufferedjJoinRoomRequest);
    static CreateRoomRequest deserializeCreateRoomRequest(const Buffer& bufferedCreateRoomRequest);

    static SubmitAnswerRequest deserializerSubmitAnswerRequest(const Buffer& bufferedSubmitAnswerRequest);
    static SubmitQuestionRequest deserializerSubmitQuestionRequest(const Buffer& bufferedSubmitQuestionRequest);
    
};
