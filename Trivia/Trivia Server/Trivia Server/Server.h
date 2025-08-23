#pragma once
#include "IDataBase.h"
#include "Communicator.h"
#include "RequestHandlerFactory.h"

class Server
{
public:
	// Constructor - used to init the server variables and open the DB
	Server();

	// Destructor - used to close the DB and free memory
	~Server();

	void run();
private:
	IDatabase* m_database;
	RequestHandlerFactory m_handlerFactory;
	

};

