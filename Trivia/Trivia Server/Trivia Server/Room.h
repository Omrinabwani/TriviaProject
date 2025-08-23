#pragma once
#include <string>
#include <vector>
#include "LoggedUser.h"

enum RoomStatus : unsigned int
{
	ACTIVE = 0,
	NOT_ACTIVE = 1
};

struct RoomData
{
	unsigned int id{};
	std::string name{};
	unsigned int maxPlayers{};
	unsigned int numOfQuestionsInGame{};
	unsigned int timePerQuestion{};
	bool isActive;

};

class Room
{
public:
	Room();
	Room(const LoggedUser& loggedUser, const RoomData& roomData);

	std::vector<LoggedUser> getAllLoggedUsers() const;
	void addUser(const LoggedUser& loggedUser);
	void removeUser(const LoggedUser& loggedUser);

	std::vector<std::string> getAllUsers() const;
	RoomData getData() const;
	void setIsActive(const bool isActive);

private:
	RoomData m_metadata;
	std::vector<LoggedUser> m_users;

};
