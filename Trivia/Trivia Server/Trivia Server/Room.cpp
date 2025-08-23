#include <algorithm>
#include "Room.h"
#include "Exceptions.h"

Room::Room()
{
}

Room::Room(const LoggedUser& loggedUser, const RoomData& roomData) : m_metadata(roomData)
{
	if (m_users.size() < roomData.maxPlayers)
	{
		addUser(loggedUser);
	}
	else
	{
		throw RoomException("Error!, max players in room!");
	}
}

std::vector<LoggedUser> Room::getAllLoggedUsers() const
{
	return m_users;
}

void Room::addUser(const LoggedUser& loggedUser)
{
	m_users.push_back(loggedUser);
}

void Room::removeUser(const LoggedUser& loggedUser)
{
	m_users.erase(std::remove(this->m_users.begin(), this->m_users.end(), loggedUser), this->m_users.end());
}

std::vector<std::string> Room::getAllUsers() const
{
	std::vector<LoggedUser>::const_iterator usersIt;
	std::vector<std::string> usernames;

	for (auto usersIt = m_users.begin(); usersIt != this->m_users.end(); ++usersIt)
	{
		usernames.push_back(usersIt->getUsername());
	}

	return usernames;
}
RoomData Room::getData() const
{
	return this->m_metadata;
}

void Room::setIsActive(const bool isActive)
{
	this->m_metadata.isActive = isActive;
}
