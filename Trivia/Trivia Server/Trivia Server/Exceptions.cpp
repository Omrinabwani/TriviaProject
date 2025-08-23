#include "Exceptions.h"

LoginException::LoginException(const std::string& message) :
	m_message("Login Exception: " + message)
{
}

char const* LoginException::what() const noexcept
{
	return m_message.c_str();
}

RoomException::RoomException(const std::string& message) :
	m_message("Room Exception: " + message)
{
}

char const* RoomException::what() const noexcept
{
	return m_message.c_str();
}

SqliteDatabaseException::SqliteDatabaseException(const std::string message):
	m_message("Sqlite Database Exception: " + message)
{
}

char const* SqliteDatabaseException::what() const noexcept
{
	return m_message.c_str();
}
