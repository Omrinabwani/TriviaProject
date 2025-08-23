#include "GameRequestHandler.h"
#include "Requests.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "Game.h"
#include "MenuRequestHandler.h"
#include "Question.h"
#include <map>
#include <algorithm>
#include "SqliteDataBase.h"

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& requestInfo)
{
	SubmitAnswerRequest submitAnswerRequest = JsonRequestPacketDeserializer::deserializerSubmitAnswerRequest(requestInfo.buffer);
	m_game.submitAnswer(m_user.getUsername(), submitAnswerRequest.answerId, submitAnswerRequest.answerTime);
	SubmitAnswerResponse submitAnswerResponse;
	submitAnswerResponse.status = 0;
	RequestResult requestResult;
	requestResult.response = JsonResponsePacketSerializer::serializeSubmitAnswerResponse(submitAnswerResponse);
	requestResult.newHandler = this;
	return requestResult;
}


RequestResult GameRequestHandler::getQuestion(const RequestInfo& requestInfo)
{
	GetQuestionResponse questionResponse;
	RequestResult requestResult;
	Game* game = m_gameManager.getGameById(m_game.getGameId());
	
	Question question = game->getQuestionForUser(m_user.getUsername());
	questionResponse.question = question.getQuestion();
	const std::vector<std::string>& answers = question.getPossibleAnswers();

	for (int i = 0; i < answers.size(); i++)
	{
		questionResponse.answers[i] = answers[i];
	}
	questionResponse.status = SUCCESS;
	requestResult.response = JsonResponsePacketSerializer::serializeGetQuestionResponse(questionResponse);
	requestResult.newHandler = this;
	return requestResult;
}
RequestResult GameRequestHandler::getGameResults(const RequestInfo& requestInfo)
{
	GetGameResultsResponse gameResponse;
	PlayerResults playerResults;
	RequestResult requestResult;
	Game* game = m_gameManager.getGameById(m_game.getGameId());

	if (!game)
	{
		ErrorResponse err;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(err);
		requestResult.newHandler = this;
		return requestResult;
	}
	if (game->getFinishedPlayers() != game->getPlayers().size())
	{
		gameResponse.status = FAILED;
		requestResult.response = JsonResponsePacketSerializer::serializeGetGameResultsResponse(gameResponse);
		requestResult.newHandler = this;
		return requestResult;
	}
	

	std::map<std::string, GameData> players = game->getPlayers();
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		playerResults.username = it->first;
		playerResults.averageAnswerTime = std::round(it->second.averageAnswerTime * 1000.0f) / 1000.0f;
		playerResults.correctAnswerCount = it->second.correctAnswerCount;
		playerResults.wrongAnswerCount = it->second.wrongAnswerCount;
		playerResults.score = it->second.correctAnswerCount * 10;
		gameResponse.results.push_back(playerResults);
	}
	std::sort(gameResponse.results.begin(), gameResponse.results.end(), compareByScoreDescending);

	gameResponse.status = SUCCESS;
	requestResult.response = JsonResponsePacketSerializer::serializeGetGameResultsResponse(gameResponse);
	requestResult.newHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	m_game.incrementUsersInGameResults();
	if (m_game.getPlayersInGameResult() == m_game.getPlayers().size())
	{
		this->m_handlerFactory.getRoomManager().deleteRoom(m_game.getGameId());
		this->m_gameManager.deleteGame(m_game.getGameId());
	}
	m_gameManager.updateStatistcs(m_user.getUsername());
	return requestResult;
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& requestInfo)
{
	LeaveGameResponse leaveGame;
	RequestResult requestResult;
	Game* game = m_gameManager.getGameById(m_game.getGameId());
	if (!game)
	{
		ErrorResponse err;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(err);
		requestResult.newHandler = this;
		return requestResult;
	}
	game->removePlayer(m_user.getUsername());
	if (game->getPlayers().size() == 0)
	{
		m_handlerFactory.getRoomManager().deleteRoom(m_game.getGameId());
		m_gameManager.deleteGame(m_game.getGameId());
	}
	leaveGame.status = SUCCESS;
	requestResult.response = JsonResponsePacketSerializer::serializeLeaveGameResponse(leaveGame);
	requestResult.newHandler = m_handlerFactory.createMenuRequestHandler(m_user);
	m_gameManager.updateStatistcs(m_user.getUsername());
	return requestResult;
}

bool GameRequestHandler::compareByScoreDescending(const PlayerResults& a, const PlayerResults& b)
{
	return a.score > b.score;
}


GameRequestHandler::GameRequestHandler(RequestHandlerFactory& factory, LoggedUser user, Game& game) :
	m_handlerFactory(factory), m_gameManager(factory.getGameManager()), m_user(user), m_game(game)
{
}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	if (requestInfo.id == GET_ROOM_STATE_REQUEST ||
		requestInfo.id == LEAVE_GAME_REQUEST ||
		requestInfo.id == GET_GAME_RESULT_REQUEST ||
		requestInfo.id == GET_QUESTION_REQUEST ||
		requestInfo.id == SUBMIT_ANSWER_REQUEST)
	{
		return true;
	}
	return false;
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	RequestResult requestResult;
	if (!isRequestRelevant(requestInfo))
	{
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
		requestResult.newHandler = this;
		return requestResult;
	}
	StartGameResponse start;
	switch (requestInfo.id)
	{
	case GET_ROOM_STATE_REQUEST:
		start.status = SUCCESS;
		start.type = "startGame";
		requestResult.newHandler = this;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(start);
		return requestResult;

	case SUBMIT_ANSWER_REQUEST:
		return submitAnswer(requestInfo);
	case LEAVE_GAME_REQUEST:
		return leaveGame(requestInfo);
	case GET_QUESTION_REQUEST:
		return getQuestion(requestInfo);
	case GET_GAME_RESULT_REQUEST:
		return getGameResults(requestInfo);
	}
}
