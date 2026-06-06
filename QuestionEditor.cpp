#include "QuestionEditor.h"
#include <fstream>
#include <iostream>

QuestionEditor::QuestionEditor(const std::string& password) : password_(password) {}

bool QuestionEditor::login(const std::string& input) const
{
    return input == password_;
}

void QuestionEditor::setQuestions(std::vector<Question> questions)
{
    questions_ = std::move(questions);
    if (!questions_.empty() && currentIndex_ >= static_cast<int>(questions_.size()))
    {
        currentIndex_ = static_cast<int>(questions_.size()) - 1;
    }
    if (questions_.empty())
    {
        currentIndex_ = 0;
    }
}

const std::vector<Question>& QuestionEditor::questions() const
{
    return questions_;
}

Question& QuestionEditor::currentQuestion()
{
    return questions_.at(currentIndex_);
}

const Question& QuestionEditor::currentQuestion() const
{
    return questions_.at(currentIndex_);
}

bool QuestionEditor::save(const std::string& path) const
{
    return DataStore::saveQuestions(path, questions_);
}

void QuestionEditor::addQuestion()
{
    Question newQ;
    newQ.text = "Новый вопрос";
    newQ.answers = {"Ответ 1", "Ответ 2", "Ответ 3", "Ответ 4"};
    newQ.correctIndex = 0;
    newQ.timeLimitSec = 30;
    newQ.topic = "Общее";
    newQ.difficulty = "easy";
    questions_.push_back(newQ);
    currentIndex_ = static_cast<int>(questions_.size()) - 1;
}

void QuestionEditor::deleteQuestion(int index)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        questions_.erase(questions_.begin() + index);
        if (questions_.empty())
        {
            currentIndex_ = 0;
        }
        else if (currentIndex_ >= static_cast<int>(questions_.size()))
        {
            currentIndex_ = static_cast<int>(questions_.size()) - 1;
        }
    }
}

void QuestionEditor::updateQuestionText(int index, const std::string& text)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        questions_[index].text = text;
    }
}

void QuestionEditor::updateAnswer(int index, int answerIndex, const std::string& answer)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        auto& q = questions_[index];
        if (answerIndex >= 0 && answerIndex < static_cast<int>(q.answers.size()))
        {
            q.answers[answerIndex] = answer;
        }
    }
}

void QuestionEditor::setTime(int index, int time)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        questions_[index].timeLimitSec = time;
    }
}

void QuestionEditor::setTopic(int index, const std::string& topic)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        questions_[index].topic = topic;
    }
}

void QuestionEditor::setDifficulty(int index, const std::string& difficulty)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        questions_[index].difficulty = difficulty;
    }
}

void QuestionEditor::setCorrectAnswer(int index, int correctAnswer)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        questions_[index].correctIndex = correctAnswer;
    }
}

void QuestionEditor::setCurrentIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(questions_.size()))
    {
        currentIndex_ = index;
    }
}