#pragma once
#include "Data.h"
#include <string>
#include <vector>

/**
 * @brief Предоставляет защищенные паролем операции редактирования вопросов викторины.
 */
class QuestionEditor
{
  public:
    /**
     * @brief Создает редактор с заданным паролем.
     * @param password Пароль, который должен быть передан в login().
     */
    explicit QuestionEditor(const std::string& password);

    /**
     * @brief Проверяет корректность пароля, введенного пользователем.
     * @param input Пароль, введенный пользователем.
     * @return true, если input совпадает с паролем редактора; иначе false.
     */
    bool login(const std::string& input) const;

    /**
     * @brief Заменяет редактируемую коллекцию вопросов.
     * @param questions Вопросы, которые нужно сохранить в редакторе.
     */
    void setQuestions(std::vector<Question> questions);

    /**
     * @brief Возвращает все редактируемые вопросы.
     * @return Константная ссылка на коллекцию вопросов редактора.
     */
    const std::vector<Question>& questions() const;

    /**
     * @brief Возвращает текущий вопрос для изменения.
     * @return Ссылка на вопрос с индексом currentIndex_.
     * @throws std::out_of_range Если список вопросов пуст.
     */
    Question& currentQuestion();

    /**
     * @brief Возвращает текущий вопрос без возможности изменения.
     * @return Константная ссылка на вопрос с индексом currentIndex_.
     * @throws std::out_of_range Если список вопросов пуст.
     */
    const Question& currentQuestion() const;

    /**
     * @brief Сохраняет текущую коллекцию вопросов в файл.
     * @param path Путь к файлу назначения.
     * @return true, если сохранение прошло успешно; иначе false.
     */
    bool save(const std::string& path) const;

    /**
     * @brief Добавляет новый вопрос по умолчанию и делает его текущим.
     */
    void addQuestion();

    /**
     * @brief Удаляет вопрос по индексу, если индекс корректен.
     * @param index Индекс удаляемого вопроса, начиная с нуля.
     */
    void deleteQuestion(int index);

    /**
     * @brief Обновляет текст вопроса, если индекс вопроса корректен.
     * @param index Индекс вопроса, начиная с нуля.
     * @param text Новый текст вопроса.
     */
    void updateQuestionText(int index, const std::string& text);

    /**
     * @brief Обновляет один ответ, если оба индекса корректны.
     * @param index Индекс вопроса, начиная с нуля.
     * @param answerIndex Индекс ответа внутри выбранного вопроса, начиная с нуля.
     * @param answer Новый текст ответа.
     */
    void updateAnswer(int index, int answerIndex, const std::string& answer);

    /**
     * @brief Обновляет ограничение времени вопроса, если индекс вопроса корректен.
     * @param index Индекс вопроса, начиная с нуля.
     * @param time Новое ограничение времени в секундах.
     */
    void setTime(int index, int time);

    /**
     * @brief Обновляет тему вопроса, если индекс вопроса корректен.
     * @param index Индекс вопроса, начиная с нуля.
     * @param topic Новая строка темы.
     */
    void setTopic(int index, const std::string& topic);

    /**
     * @brief Обновляет сложность вопроса, если индекс вопроса корректен.
     * @param index Индекс вопроса, начиная с нуля.
     * @param difficulty Новая строка сложности.
     */
    void setDifficulty(int index, const std::string& difficulty);

    /**
     * @brief Обновляет индекс правильного ответа, если индекс вопроса корректен.
     * @param index Индекс вопроса, начиная с нуля.
     * @param correctAnswer Индекс ответа, который нужно отметить правильным, начиная с нуля.
     */
    void setCorrectAnswer(int index, int correctAnswer);

    /**
     * @brief Возвращает индекс текущего вопроса.
     * @return Индекс текущего вопроса, начиная с нуля.
     */
    int currentIndex() const
    {
        return currentIndex_;
    }

    /**
     * @brief Изменяет индекс текущего вопроса, если переданный индекс корректен.
     * @param index Индекс вопроса для выбора, начиная с нуля.
     */
    void setCurrentIndex(int index);

  private:
    /** @brief Пароль, необходимый для доступа к редактированию из интерфейса. */
    std::string password_;

    /** @brief Редактируемая коллекция вопросов. */
    std::vector<Question> questions_;

    /** @brief Индекс текущего вопроса в questions_, начиная с нуля. */
    int currentIndex_ = 0;
};
