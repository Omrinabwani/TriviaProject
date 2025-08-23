#include "Question.h"

Question::Question()
{
}

Question::Question(const std::string& question, const std::vector<std::string>& possibleAnswers, const int correctAnswerId):
    m_question(question), m_possibleAnswers(possibleAnswers), m_correctAnswerId(correctAnswerId)
{
}

std::string Question::getQuestion() const
{
    return m_question;
}

std::vector<std::string> Question::getPossibleAnswers() const
{
    return m_possibleAnswers;
}

int Question::getCorrectAnswerId() const
{
    return m_correctAnswerId;
}

bool Question::operator==(const Question& other) const
{
    return m_question == other.m_question && m_possibleAnswers == other.m_possibleAnswers && m_possibleAnswers == other.m_possibleAnswers;
}
