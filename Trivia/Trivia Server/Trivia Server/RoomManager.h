#pragma once
#include <map>
#include <vector>
#include "Room.h"
#include "LoggedUser.h"
#include "Responses.h"

class RoomManager
{
public:
	void createRoom(const LoggedUser& loggedUser, const RoomData& roomData);
	void deleteRoom(const int roomId);
	bool doesRoomNameExist(const std::string& roomName) const;
	std::vector<RoomData> getRooms() const;

	Room* getRoom(const int id); // Changed from optional to room* because optional wouldn't work, might change later though
	GetRoomStateResponse getRoomStateResponse(int roomId) const;
	int generateRoomId() const;
	void removeUserFromRoom(int roomId, const LoggedUser& user);
private:
	std::map<int, Room> m_rooms;
};
