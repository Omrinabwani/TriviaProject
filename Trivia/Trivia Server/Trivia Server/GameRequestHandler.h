#pragma once
#include "IRequestHandler.h"
#include "Game.h"
#include "LoggedUser.h"
#include "GameManager.h"
#include "RequestHandlerFactory.h"

class GameRequestHandler : public IRequestHandler 
{
private:
	// fields
	LoggedUser m_user;
	GameManager& m_gameManager;
	RequestHandlerFactory& m_handlerFactory;
	Game& m_game;
	// methods
	RequestResult submitAnswer(const RequestInfo& requestInfo);
	RequestResult getQuestion(const RequestInfo& requestInfo);
	RequestResult getGameResults(const RequestInfo& requestInfo);
	RequestResult leaveGame(const RequestInfo& requestInfo);
	static bool compareByScoreDescending(const PlayerResults& a, const PlayerResults& b); // Helper Function To Sort Results 



public:
	GameRequestHandler(RequestHandlerFactory& factory, LoggedUser user, Game& game);
	bool isRequestRelevant(const RequestInfo& requestInfo);
	RequestResult handleRequest(const RequestInfo& requestInfo);
};
