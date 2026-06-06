#pragma once
#include "Data.h"
#include <chrono>
#include <string>
#include <vector>

/**
 * @brief Управляет ходом викторины, подсчетом баллов, таймером и созданием результата.
 */
class QuizEngine
{
  public:
    /**
     * @brief Заменяет текущий набор вопросов и сбрасывает прогресс викторины.
     * @param questions Вопросы, которые будут использоваться в викторине.
     */
    void setQuestions(const std::vector<Question>& questions);

    /**
     * @brief Фильтрует полный набор вопросов, сохраняет подходящие вопросы и сбрасывает прогресс.
     * @param allQuestions Исходная коллекция вопросов до фильтрации.
     * @param filters Фильтры по теме и сложности, которые нужно применить.
     */
    void setQuestionsWithFilter(const std::vector<Question>& allQuestions,
                                const QuestionFilters& filters);

    /**
     * @brief Устанавливает метку сложности, которая будет записана в результат игрока.
     * @param diff Метка сложности для сохранения.
     */
    void setDifficulty(const std::string& diff);

    /**
     * @brief Возвращает викторину к первому вопросу и очищает счетчики баллов.
     */
    void reset();

    /**
     * @brief Возвращает текущий активный вопрос.
     * @return Указатель на текущий вопрос или nullptr, если викторина завершена либо пуста.
     */
    const Question* currentQuestion() const;

    /**
     * @brief Проверяет ответ на текущий вопрос и обновляет счетчики баллов.
     * @param index Индекс ответа, выбранного игроком, начиная с нуля.
     * @return true, если ответ правильный; false, если ответ неправильный или активного вопроса
     * нет.
     */
    bool answer(std::size_t index);

    /**
     * @brief Переключает викторину на следующий вопрос.
     * @return true, если следующий вопрос стал активным; false, если викторина завершена.
     */
    bool nextQuestion();

    /**
     * @brief Проверяет, остались ли активные вопросы.
     * @return true, если викторина пуста или currentIndex_ указывает за последний вопрос.
     */
    bool finished() const;

    /**
     * @brief Возвращает количество баллов, набранных на текущий момент.
     * @return Текущий счет игрока.
     */
    int score() const;

    /**
     * @brief Возвращает максимум баллов по уже отвеченным вопросам.
     * @return Максимально доступный счет, накопленный на текущий момент.
     */
    int maxScore() const;

    /**
     * @brief Возвращает оставшееся время для текущего вопроса.
     * @return Оставшееся время в секундах, не меньше нуля.
     */
    int timeLeft() const;

    /**
     * @brief Обновляет оставшееся время с учетом реально прошедшего времени.
     */
    void tick();

    /**
     * @brief Создает запись результата для текущего состояния викторины.
     * @param playerName Имя игрока, которое нужно сохранить в результате.
     * @return PlayerResult с именем игрока, счетом, максимальным счетом, сложностью и датой.
     */
    PlayerResult buildResult(const std::string& playerName) const;

    /**
     * @brief Возвращает количество загруженных вопросов.
     * @return Общее число вопросов в текущей викторине.
     */
    std::size_t totalQuestions() const;

  private:
    /** @brief Вопросы, которые сейчас используются движком. */
    std::vector<Question> questions_;

    /** @brief Индекс активного вопроса, начиная с нуля. */
    std::size_t currentIndex_ = 0;

    /** @brief Баллы, полученные за правильные ответы. */
    int score_ = 0;

    /** @brief Максимально возможные баллы среди вопросов, на которые уже был дан ответ. */
    int maxScore_ = 0;

    /** @brief Оставшиеся секунды для текущего вопроса. */
    int timeLeft_ = 0;

    /** @brief Метка сложности, записываемая в значения PlayerResult. */
    std::string difficulty_ = "normal";

    /** @brief Последний момент времени, используемый tick() для расчета прошедших секунд. */
    std::chrono::steady_clock::time_point lastTick_;
};
