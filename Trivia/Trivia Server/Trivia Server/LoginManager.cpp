#include "LoginManager.h"
#include "Exceptions.h"

LoginManager::LoginManager(IDatabase* database) :
	m_database(database)
{
}

ResponseStatus LoginManager::signup(const std::string username, const std::string password, const std::string email, SOCKET socket)
{
	if (m_database->doesUserExist(username))
	{
		return FAILED; // One can't sign up with a user that already exists in the DB
	}

	m_database->addNewUser(username, password, email);
	m_loggedUsers.push_back(LoggedUser(username, socket));

	return SUCCESS;
}

int LoginManager::login(const std::string username, const std::string password, SOCKET socket)
{
	if (!m_database->doesUserExist(username))
	{
		return FAILED; // One can't log in to a non-existing user
	}
	if (isUserLoggedIn(username))
	{
		return 2; // One can't log in to an already logged in user
	}
	if (!m_database->doesPasswordMatch(username, password))
	{
		return FAILED; // One can't sign in with an incorrect password
	}

	m_loggedUsers.push_back(LoggedUser(username, socket));

	return SUCCESS;
}

ResponseStatus LoginManager::logout(const std::string username)
{
	if (!isUserLoggedIn(username))
	{
		return FAILED;
		//throw LoginException("Error!, No user to logout!");
	}
	// Removing the LoggedUser from the m_loggedUsers thus logging the user out
	for (auto it = this->m_loggedUsers.begin(); it != this->m_loggedUsers.end();)
	{
		if (it->getUsername() == username)
		{
			it = this->m_loggedUsers.erase(it);
		}
		else
		{
			++it;
		}
	}

	return SUCCESS;
}

bool LoginManager::isUserLoggedIn(const std::string& username)
{
	for (const LoggedUser& user : m_loggedUsers)
	{
		if (user.getUsername() == username)
		{
			return true;
		}
	}
	return false;
}
