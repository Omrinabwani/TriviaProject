#include "StatisticsManager.h"

std::vector<std::pair<std::string, int>> StatisticsManager::getHighScore() const
{
    
    return m_database->getHighScores();
}

UserStatistics StatisticsManager::getUserStatistics(const std::string& username) const
{
    return m_database->getUserStatistics(username);
}
