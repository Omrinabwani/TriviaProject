#include <winsock2.h>
#include <Windows.h>
#include <thread>

#include "Communicator.h"
#include "LoginRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"


// using static const instead of macros 
static const unsigned short PORT = 55551;
static const unsigned int IFACE = INADDR_ANY;

Communicator::Communicator(RequestHandlerFactory& requestHandlerFactory):
	m_handlerFactory(requestHandlerFactory)
{
}

void Communicator::startHandleRequests()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed!" << std::endl;
		return;
	}

	bindAndListen();

	while (true) 
	{
		// The main thread is only accepting clients 
		// and adding them to the list of handlers
		TRACE("Accepting client...");

		SOCKET client_socket = accept(m_serverSocket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);
		
		TRACE("Client accepted !");

		// The first requestHandler for the client will be a login request handler
		IRequestHandler* loginRequestHandler = m_handlerFactory.createLoginRequestHandler(client_socket);
		m_clients[client_socket] = loginRequestHandler;

		std::thread tr(&Communicator::handleNewClient, this, client_socket);
		tr.detach();
	}
}
void Communicator::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;

	// again stepping out to the global namespace
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (::bind(m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
		throw std::exception(__FUNCTION__ " - bind");
	}

	TRACE("binded");

	if (::listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	TRACE("listening...");

}

void Communicator::handleNewClient(SOCKET socket)
{
	IRequestHandler* requestHandler = m_clients[socket];
	try
	{
		while (true)
		{
			requestHandler = m_clients[socket];
			RequestInfo requestInfo;
			RequestResult requestResult;
			Buffer bufferedRequest;
			Buffer bufferedResponse;
			char currChar = ' ';
			int requestContentLen = 0;
			int responseLen = 0;
			bool requestRelevant = false;
			int i = 0;


			// Building the requestInfo
			for (i = 0; i < 5; i++)
			{
				recv(socket, &currChar, 1, 0);
				bufferedRequest.push_back(currChar);
			}
			requestContentLen = JsonRequestPacketDeserializer::convertBytesToInt(bufferedRequest[1], bufferedRequest[2], bufferedRequest[3], bufferedRequest[4]);

			for (i; i < 5 + requestContentLen; i++)
			{
				recv(socket, &currChar, 1, 0);
				bufferedRequest.push_back(currChar);
			}
			requestInfo.id = (RequestId)bufferedRequest[0];
			requestInfo.buffer = bufferedRequest;
			requestInfo.receivalTime = std::time(nullptr); // Getting the current time formatted as time_t

			// After getting the requestInfo, we can now handle the request

			// Creating the response as a Buffer
			requestRelevant = requestHandler->isRequestRelevant(requestInfo);
			if (requestRelevant)
			{
				requestResult = requestHandler->handleRequest(requestInfo);
				bufferedResponse = requestResult.response;
			}
			else
			{
				requestResult.newHandler = requestHandler;
				bufferedResponse = JsonResponsePacketSerializer::serializeResponse(ErrorResponse());
			}

			// Sending the response byte after byte
			responseLen = bufferedResponse.size();
			for (i = 0; i < responseLen; i++)
			{
				currChar = bufferedResponse[i];
				send(socket, &currChar, 1, 0);
			}



			if (requestResult.newHandler != nullptr && requestHandler != requestResult.newHandler)
			{
				delete requestHandler;
				requestHandler = requestResult.newHandler;
				m_clients[socket] = requestHandler;
			}

			if (m_clients[socket] != requestHandler)
			{
				delete requestHandler; 
				requestHandler = m_clients[socket];
			}
		}
	}



	catch (const std::exception& e)
	{	
		std::cerr << e.what() << std::endl;
		closesocket(socket);
		delete requestHandler;
		m_clients.erase(socket);
	}
}
