#pragma once
#include <string>
#include <vector>
#include "IDatabase.h"
#include "UserStatistics.h"

class StatisticsManager
{
public:
	StatisticsManager(IDatabase* database) : m_database(database) {}

	std::vector<std::pair<std::string, int>> getHighScore() const;
	UserStatistics getUserStatistics(const std::string& username) const;

private:
	IDatabase* m_database{};
};
