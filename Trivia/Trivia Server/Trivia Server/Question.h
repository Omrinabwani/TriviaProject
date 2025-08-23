#pragma once
#include <string>
#include <vector>

class Question
{
public:
	Question();
	Question(const std::string& question, const std::vector<std::string>& possibleAnswers, const int correctAnswerId);
	std::string getQuestion() const;
	std::vector<std::string> getPossibleAnswers() const;
	int getCorrectAnswerId() const;
	bool operator==(const Question& other) const;

private:
	std::vector<std::string> m_possibleAnswers;
	std::string m_question;
	int m_correctAnswerId;
};
