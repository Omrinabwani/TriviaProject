#pragma once
#include <string>

class LoginException : public std::exception 
{
public:
    LoginException(const std::string& message);

    virtual char const* what() const noexcept;
private:
    std::string m_message;
};

class RoomException : public std::exception
{
public:
    RoomException(const std::string& message);

    virtual char const* what() const noexcept;
private:
    std::string m_message;
};

class SqliteDatabaseException : public std::exception
{
public:
    SqliteDatabaseException(const std::string message);

    virtual char const* what() const noexcept;
private:
    std::string m_message;
};
