#include "JsonRequestPacketDeserializer.h"

#define REQUEST_CODE_NUM_BYTES 1
#define REQUEST_MSG_LEN_NUM_BYTES 4

std::string JsonRequestPacketDeserializer::convertBufferToJsonString(const Buffer& bufferedRequest)
{
    std::string jsonString = "";
    
    // Starting from the 5th byte, since that's where the actual json message begins
    for (auto it = bufferedRequest.begin() + REQUEST_CODE_NUM_BYTES + REQUEST_MSG_LEN_NUM_BYTES;
        it != bufferedRequest.end(); ++it)
    {
        jsonString += (unsigned char)*it;
    }

    return jsonString;
}

int JsonRequestPacketDeserializer::convertBytesToInt(const unsigned char byte1, const unsigned char byte2, const unsigned char byte3, const unsigned char byte4)
{
    // Similiar to what we did JsonResponsePacketSerializer::BuildBuffer just the other way around
    return (int(byte1 << 24)) |
        (int(byte2) << 16) |
        (int(byte3) << 8) |
        (int(byte4));
}


LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const Buffer& bufferedLoginRequest)
{
    std::string jsonString = convertBufferToJsonString(bufferedLoginRequest);

    nlohmann::json content = nlohmann::json::parse(jsonString); // This makes the json file as string to become an actual json file
    std::string username = content.at("username");
    std::string password = content.at("password");

    LoginRequest loginRequest;
    loginRequest.username = username;
    loginRequest.password = password;

    return loginRequest;
}

SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const Buffer& bufferedSingupRequest)
{
    std::string jsonString = convertBufferToJsonString(bufferedSingupRequest);

    nlohmann::json content = nlohmann::json::parse(jsonString);
    std::string username = content.at("username");
    std::string password = content.at("password");
    std::string email = content.at("email");

    SignupRequest signupRequest;
    signupRequest.username = username;
    signupRequest.password = password;
    signupRequest.email = email;

    return signupRequest;
}



GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersRequest(const Buffer& bufferedGetPlayersRequest)
{
    GetPlayersInRoomRequest getPlayersInRoomRequest;
    std::string jsonString = convertBufferToJsonString(bufferedGetPlayersRequest);
    nlohmann::json content = nlohmann::json::parse(jsonString);

    getPlayersInRoomRequest.roomId = content.at("roomId");

    return getPlayersInRoomRequest;
}

JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const Buffer& bufferedJoinRoomReqeust)
{
    JoinRoomRequest joinRoomRequest;

    std::string jsonString = convertBufferToJsonString(bufferedJoinRoomReqeust);
    nlohmann::json content = nlohmann::json::parse(jsonString);
    joinRoomRequest.roomId = content.at("roomId");

    return joinRoomRequest;
}

CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const Buffer& bufferedCreateRoomRequest)
{
    CreateRoomRequest createRoomRequest;
    std::string jsonString = convertBufferToJsonString(bufferedCreateRoomRequest);
    nlohmann::json content = nlohmann::json::parse(jsonString);

    createRoomRequest.roomName = content.at("roomName");
    createRoomRequest.maxUsers = content.at("maxUsers");
    createRoomRequest.questionCount = content.at("questionCount");
    createRoomRequest.answerTimeout = content.at("answerTimeout");

    return createRoomRequest;
}

SubmitAnswerRequest JsonRequestPacketDeserializer::deserializerSubmitAnswerRequest(const Buffer& bufferedSubmitAnswerRequest)
{
    std::string jsonString = convertBufferToJsonString(bufferedSubmitAnswerRequest);
    nlohmann::json j = nlohmann::json::parse(jsonString);
    unsigned int answerId = j.at("answerId");
    float answerTime = j.at("answerTime");
    
    SubmitAnswerRequest submitAnswer = { answerId, answerTime };
    return submitAnswer;

}

SubmitQuestionRequest JsonRequestPacketDeserializer::deserializerSubmitQuestionRequest(const Buffer& bufferedSubmitQuestionRequest)
{
    std::string jsonString = convertBufferToJsonString(bufferedSubmitQuestionRequest);
    nlohmann::json j = nlohmann::json::parse(jsonString);
    std::string question = j.at("question");
    std::string correctAns = j.at("correctAns");
    std::string secAns = j.at("secAns");
    std::string thirdAns = j.at("thirdAns");
    std::string fourthAns = j.at("fourthAns");
    SubmitQuestionRequest submitQuestion = { question, correctAns, secAns, thirdAns, fourthAns };
    return submitQuestion;
}
