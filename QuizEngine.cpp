#include "QuizEngine.h"

void QuizEngine::setQuestions(const std::vector<Question>& questions)
{
    questions_ = questions;
    reset();
}

void QuizEngine::setQuestionsWithFilter(const std::vector<Question>& allQuestions,
                                        const QuestionFilters& filters)
{
    questions_ = DataStore::filterQuestions(allQuestions, filters);
    reset();
}

void QuizEngine::setDifficulty(const std::string& diff)
{
    difficulty_ = diff;
}

void QuizEngine::reset()
{
    currentIndex_ = 0;
    score_ = 0;
    maxScore_ = 0;
    timeLeft_ = questions_.empty() ? 0 : questions_[0].timeLimitSec;
    lastTick_ = std::chrono::steady_clock::now();
}

const Question* QuizEngine::currentQuestion() const
{
    if (finished())
        return nullptr;
    return &questions_[currentIndex_];
}

bool QuizEngine::answer(std::size_t index)
{
    const Question* q = currentQuestion();
    if (!q)
        return false;
    maxScore_ += q->points;
    if (index == q->correctIndex)
    {
        score_ += q->points;
        return true;
    }
    return false;
}

bool QuizEngine::nextQuestion()
{
    if (currentIndex_ + 1 < questions_.size())
    {
        ++currentIndex_;
        timeLeft_ = questions_[currentIndex_].timeLimitSec;
        lastTick_ = std::chrono::steady_clock::now();
        return true;
    }
    currentIndex_ = questions_.size();
    return false;
}

bool QuizEngine::finished() const
{
    return questions_.empty() || currentIndex_ >= questions_.size();
}

int QuizEngine::score() const
{
    return score_;
}
int QuizEngine::maxScore() const
{
    return maxScore_;
}
int QuizEngine::timeLeft() const
{
    return timeLeft_;
}

void QuizEngine::tick()
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto sec = duration_cast<seconds>(now - lastTick_).count();
    if (sec > 0)
    {
        timeLeft_ -= static_cast<int>(sec);
        lastTick_ = now;
        if (timeLeft_ < 0)
            timeLeft_ = 0;
    }
}

PlayerResult QuizEngine::buildResult(const std::string& playerName) const
{
    PlayerResult r;
    r.playerName = playerName;
    r.score = score_;
    r.maxScore = maxScore_;
    r.difficulty = difficulty_;
    r.timestamp = "2026-06-03";
    return r;
}

std::size_t QuizEngine::totalQuestions() const
{
    return questions_.size();
}