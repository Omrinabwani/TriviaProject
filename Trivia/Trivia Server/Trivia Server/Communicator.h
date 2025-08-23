#pragma once
#include <map>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "IRequestHandler.h"
#include "ICryptoAlgorithm.h"
#include "RequestHandlerFactory.h"

class Communicator
{
public:
	Communicator(RequestHandlerFactory& requestHandlerFactory);
	void startHandleRequests();
	static Communicator& getInstance(RequestHandlerFactory& factory)
	{
		static Communicator instance(factory);
		return instance;
	}
private:
	std::map<SOCKET, IRequestHandler*> m_clients;
	RequestHandlerFactory& m_handlerFactory;
	SOCKET m_serverSocket{};
	//ICryptoAlgorithm m_cryptoAlgorithm;
	// methods

	void bindAndListen();
	void handleNewClient(SOCKET socket);
};



#ifdef _DEBUG // vs add this define in debug mode
#include <stdio.h>
// Q: why do we need traces ?
// A: traces are a nice and easy way to detect bugs without even debugging
// or to understand what happened in case we miss the bug in the first time
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
// for convenient reasons we did the traces in stdout
// at general we would do this in the error stream like that
// #define TRACE(msg, ...) fprintf(stderr, msg "\n", __VA_ARGS__);
#else // we want nothing to be printed in release version
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
#endif
