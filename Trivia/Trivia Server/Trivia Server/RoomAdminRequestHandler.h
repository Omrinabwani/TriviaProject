#pragma once
#include "RoomManager.h"
#include "LoggedUser.h"
#include "RequestHandlerFactory.h"
#include "RequestResult.h"
#include "RoomRequestHandler.h"
	class RoomAdminRequestHandler : public RoomRequestHandler 
	{
	private:
		//methods
		RequestResult closeRoom(RequestInfo requestInfo);
		RequestResult startGame(RequestInfo requestInfo);


		//helper function to send data to the users
		void sendData(const SOCKET soc, const Buffer& msg);

	public:
		RoomAdminRequestHandler(Room room, LoggedUser user, RoomManager& roomManager, RequestHandlerFactory& factory);
		bool isRequestRelevant(const RequestInfo& requestInfo) override;
		RequestResult handleRequest(const RequestInfo& requestInfo) override;


	};