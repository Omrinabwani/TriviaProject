#include <iostream>
#include <thread>
#include "Server.h"
#include "Communicator.h"
#include "SqliteDatabase.h"
#include "Helper.hpp"

Server::Server()
	: m_database(new SqliteDatabase()),
	m_handlerFactory(m_database)
{
	m_database->open();
}
Server::~Server()
{
	m_database->close();
	free(m_database); 
}

void Server::run()
{
	bool doExit = false;
	int option = 0;


	// Create a new thread for handling message, passing the server's m_communicator
	std::thread tr(&Communicator::startHandleRequests, std::ref(Communicator::getInstance(m_handlerFactory)));
	tr.detach();

	std::cout << "Welcome to Magshitrivia admin interface!" << std::endl << std::endl;
	std::cout << "MENU" << std::endl
		<< "1 - add question" << std::endl
		<< "2 - delete question" << std::endl
		<< "99 - exit and turn the server off" << std::endl;

	while (!doExit)
	{
		std::cout << std::endl << "You may now enter a command number" << std::endl;
		try
		{
			std::cin >> option;
			cleanCin();
			if (option == 1)
			{
				std::vector<std::string> possibleAnswers;
				std::string possibleAnswer = "";
				std::string questionString = "";
				int correctAnswerId = 0;

				std::cout << "Enter question: ";
				std::getline(std::cin, questionString);
				for (int i = 0; i < 4; i++)
				{
					std::cout << "Enter possible answer number " + std::to_string(i + 1) + ": ";
					std::getline(std::cin, possibleAnswer);
					possibleAnswers.push_back(possibleAnswer);
				}

				std::cout << "Enter the number of the correct answer: ";
				std::cin >> correctAnswerId;
				cleanCin();

				Question newQuestion(questionString, possibleAnswers, correctAnswerId);
				m_database->addNewQuestion(newQuestion);
			}
			else if (option == 2)
			{
				std::string questionString = "";

				std::cout << "Enter question: ";
				std::getline(std::cin, questionString);

				m_database->deleteQuestion(questionString);
			}
			else if (option == 99)
			{
				doExit = true;
			}
			else
			{
				std::cerr << "No valid option has been entered" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	} 
}
