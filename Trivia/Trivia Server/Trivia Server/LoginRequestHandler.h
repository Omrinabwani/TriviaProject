#pragma once
#include "IRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "LoginManager.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

// This class doesn't take care just of login requests, but also for signup requests
class LoginRequestHandler : public IRequestHandler 
{
public:
	// Constructor - used only by the RequestHandlerFactory
	LoginRequestHandler(RequestHandlerFactory& requestHandlerFactory, SOCKET socket);

	virtual RequestResult handleRequest(const RequestInfo& requestInfo) override;
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) override; // Checks if the  request is of a login or a singup

private:
	// Although the LoginRequestHandler is created by RequestHandlerFactory, some things, like LoginManager are accessible 
	// Only by the RequestHandlerFactory
	RequestHandlerFactory& m_handlerFactory;

	// Only used to pass the socket to logged user
	SOCKET m_socket; 
	// Helper methods
	RequestResult login(const RequestInfo& loginRequestInfo, SOCKET socket);
	RequestResult signup(const RequestInfo& signupRequestInfo, SOCKET socket);

};