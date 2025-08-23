#include "LoginRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "MenuRequestHandler.h"

LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory& requestHandlerFactory, SOCKET socket) :
    m_handlerFactory(requestHandlerFactory), m_socket(socket)
{
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
    if (requestInfo.id == LOGIN_REQUEST)
    {
        return login(requestInfo, m_socket);
    }
    else if (requestInfo.id == SIGNUP_REQUEST)
    {
        return signup(requestInfo, m_socket);
    }
    else
    {
        RequestResult requestResult;

        requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
        requestResult.newHandler = nullptr;

        return requestResult;
    }
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
    return requestInfo.id == LOGIN_REQUEST or requestInfo.id == SIGNUP_REQUEST;
}


RequestResult LoginRequestHandler::login(const RequestInfo& loginRequestInfo, const SOCKET socket)
{
    RequestResult requestResult;
    LoginRequest loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(loginRequestInfo.buffer);
    LoginResponse loginResponse{};
    ErrorResponse errorResponse;
    int loginStatus;
    
    try
    {
        // Getting the loginStatus using the login method of the LoginManager
        loginStatus = m_handlerFactory.getLoginManager().login(loginRequest.username, loginRequest.password, socket);

        // Setting the newHandler considering the loginStatus
        if (loginStatus == SUCCESS)
        {
            LoggedUser user(loginRequest.username, socket);
            requestResult.newHandler = m_handlerFactory.createMenuRequestHandler(user);
        }
        else if (loginStatus == FAILED || loginStatus == 2) // two is the code for already logged in user
        {
            requestResult.newHandler = m_handlerFactory.createLoginRequestHandler(socket);
        }

        // Setting the requestResult using the loginStatus once again
        loginResponse.status = loginStatus;
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(loginResponse);

    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
        errorResponse.message = e.what();
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        requestResult.newHandler = nullptr;
    }

    return requestResult;
}

RequestResult LoginRequestHandler::signup(const RequestInfo& signupRequestInfo, const SOCKET socket)
{
    RequestResult requestResult;
    SignupRequest singupRequest = JsonRequestPacketDeserializer::deserializeSignupRequest(signupRequestInfo.buffer);
    ErrorResponse errorResponse;
    SignupResponse signupResponse{};
    ResponseStatus signupStatus;

    // Using the same logic used in the login method
    try
    {
        signupStatus = m_handlerFactory.getLoginManager().signup(singupRequest.username, singupRequest.password, singupRequest.email, socket);

        if (signupStatus == SUCCESS)
        {
            LoggedUser user(singupRequest.username, socket);
            requestResult.newHandler = m_handlerFactory.createMenuRequestHandler(user);
        }
        else if (signupStatus == FAILED)
        {
            requestResult.newHandler = m_handlerFactory.createLoginRequestHandler(socket);
        }
        signupResponse.status = signupStatus;
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(signupResponse);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
        errorResponse.message = e.what();
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        requestResult.newHandler = nullptr;
    }

    return requestResult;
}