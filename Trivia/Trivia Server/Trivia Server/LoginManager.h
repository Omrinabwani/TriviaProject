#pragma once
#include <vector>
#include "IDataBase.h"
#include "LoggedUser.h"
#include "Responses.h"

/* 
The purpose of the LoginManager is to call the relevant m_database methods
Some of the methods return a ResponseStatus which indicates a successful or an unsuccessful login or signup
*/
class LoginManager
{
public:
	// Constructor
	LoginManager(IDatabase* database);

	// Methods - including not just login, but also the related
	ResponseStatus signup(const std::string username, const std::string password, const std::string email, SOCKET socket);
	int login(const std::string username, const std::string password, SOCKET socket);
	ResponseStatus logout(const std::string username);

	

private:
	IDatabase* m_database;
	std::vector<LoggedUser> m_loggedUsers;
	bool isUserLoggedIn(const std::string& username);
	
};
