#pragma once
#include <string>
#include <WinSock2.h>
#include <Windows.h>

class LoggedUser
{
public:
	LoggedUser(const std::string username, const SOCKET socket);

	std::string getUsername() const;
	SOCKET getSocket() const;
	bool operator==(const LoggedUser& other) const;

private:
	std::string m_username;
	SOCKET m_socket;
};
