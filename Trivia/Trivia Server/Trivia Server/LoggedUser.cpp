#include "LoggedUser.h"
#include <iostream>

LoggedUser::LoggedUser(const std::string username, const SOCKET socket)
{
    this->m_username = username;
    this->m_socket = socket;
}

std::string LoggedUser::getUsername() const
{
    return this->m_username;
}

SOCKET LoggedUser::getSocket() const
{
    return this->m_socket;
}

bool LoggedUser::operator==(const LoggedUser& other) const
{
    return m_username == other.m_username;
}
