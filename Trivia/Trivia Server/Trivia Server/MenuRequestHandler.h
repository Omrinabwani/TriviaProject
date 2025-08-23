#pragma once
#include "IRequestHandler.h"
#include "RequestResult.h"
#include "LoggedUser.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler
{
public:
	MenuRequestHandler(RequestHandlerFactory& factory, const LoggedUser& user);
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	LoggedUser m_user;
	RequestHandlerFactory& m_handlerFactory;

	// Helper methods
	RequestResult logout(const RequestInfo& requestInfo);
	RequestResult getRooms(const RequestInfo& requestInfo);
	RequestResult getPlayersInRoom(const RequestInfo& requestInfo);
	RequestResult getPersonalStats(const RequestInfo& requestInfo);
	RequestResult getHighScore(const RequestInfo& requestInfo);
	RequestResult joinRoom(const RequestInfo& requestInfo);
	RequestResult createRoom(const RequestInfo& requestInfo);

};
