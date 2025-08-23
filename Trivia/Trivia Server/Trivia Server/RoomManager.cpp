#include "RoomManager.h"
#include "Exceptions.h"
#include "Responses.h"


void RoomManager::createRoom(const LoggedUser& loggedUser, const RoomData& roomData)
{
	m_rooms[roomData.id] = Room(loggedUser, roomData);
};
void RoomManager::deleteRoom(const int roomId)
{
	m_rooms.erase(roomId);
}

bool RoomManager::doesRoomNameExist(const std::string& roomName) const
{
	if (m_rooms.empty())
		return false;

	for (const auto& pair : m_rooms)
	{
		const Room& room = pair.second;
		if (room.getData().name == roomName)
			return true;
	}

	return false;
}


GetRoomStateResponse RoomManager::getRoomStateResponse(int roomId) const
{
	GetRoomStateResponse res;
	auto it = m_rooms.find(roomId);
	if (it == m_rooms.end())
	{
		res.status = FAILED;
		res.hasGameBegun = false;
		res.answerTimeout = 0;
		res.questionCount = 0;
		return res;
	}

	const RoomData& data = it->second.getData();


	res.hasGameBegun = data.isActive;
	res.answerTimeout = data.timePerQuestion;
	res.players = it->second.getAllUsers();
	res.questionCount = data.numOfQuestionsInGame;
	res.status = SUCCESS;

	return res;
}


std::vector<RoomData> RoomManager::getRooms() const
{
	std::map<int, Room>::const_iterator roomsIt;
	std::vector<RoomData> roomsData;

	for (roomsIt = m_rooms.begin(); roomsIt != m_rooms.end(); ++roomsIt)
	{
		roomsData.push_back(roomsIt->second.getData()); // We only push back the RoomData, not the Room
	}

	return roomsData;
}

Room* RoomManager::getRoom(const int roomId) // An optional Room& wouldn't work so we return a pointer
{
	std::map<int, Room>::iterator roomsIt = m_rooms.find(roomId);

	if (roomsIt == m_rooms.end())
	{
		return nullptr;
	}

	return &roomsIt->second; // Returning the pointer to the room
}

int RoomManager::generateRoomId() const
{
	if (m_rooms.empty())
		return 1;
	return m_rooms.rbegin()->first + 1;
}

void RoomManager::removeUserFromRoom(int roomId, const LoggedUser& user)
{
	auto it = m_rooms.find(roomId);
	if (it != m_rooms.end())
	{
		it->second.removeUser(user);
	}
}
