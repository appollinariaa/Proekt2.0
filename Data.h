#pragma once

#include <optional>
#include <string>
#include <vector>

/**
 * @brief Хранит один вопрос викторины и данные, необходимые для его проверки.
 */
struct Question
{
    /** @brief Текст вопроса, который показывается игроку. */
    std::string text;

    /** @brief Список вариантов ответа; приложение ожидает ровно четыре элемента. */
    std::vector<std::string> answers;

    /** @brief Индекс правильного ответа в answers, начиная с нуля. */
    std::size_t correctIndex{0};

    /** @brief Тема или категория, используемая для фильтрации вопросов. */
    std::string topic;

    /** @brief Метка сложности, используемая для фильтрации и сохранения результата. */
    std::string difficulty;

    /** @brief Ограничение времени на вопрос в секундах. */
    int timeLimitSec{30};

    /** @brief Количество баллов за правильный ответ. */
    int points{1};
};

/**
 * @brief Необязательные критерии для выбора части вопросов.
 */
struct QuestionFilters
{
    /** @brief Тема, которую нужно оставить; std::nullopt означает любую тему. */
    std::optional<std::string> topic;

    /** @brief Сложность, которую нужно оставить; std::nullopt означает любую сложность. */
    std::optional<std::string> difficulty;
};

/**
 * @brief Один сохраненный результат викторины, отображаемый в рейтинге.
 */
struct PlayerResult
{
    /** @brief Имя игрока, записанное в рейтинг. */
    std::string playerName;

    /** @brief Количество баллов, набранных игроком. */
    int score{0};

    /** @brief Максимально возможное количество баллов в пройденной викторине. */
    int maxScore{0};

    /** @brief Дата или строка времени, связанная с результатом. */
    std::string timestamp;

    /** @brief Метка сложности, выбранная для игры. */
    std::string difficulty;

    /** @brief Метка режима игры, например "solo" или "duo". */
    std::string mode;
};

/**
 * @brief Вспомогательные функции для работы с файлами, вопросами, фильтрами и результатами.
 */
namespace DataStore
{
/**
 * @brief Создает встроенный набор вопросов по умолчанию.
 * @return Вектор с вопросами по умолчанию.
 */
std::vector<Question> defaultQuestions();

/**
 * @brief Загружает вопросы из текстового файла.
 * @param filename Путь к файлу с сохраненными вопросами.
 * @param questions Выходной вектор, который заполняется загруженными вопросами при успешном чтении.
 * @return true, если загружен хотя бы один корректный вопрос; false, если файл отсутствует или
 * некорректен.
 */
bool loadQuestions(const std::string& filename, std::vector<Question>& questions);

/**
 * @brief Сохраняет вопросы в текстовый файл.
 * @param filename Путь к файлу назначения.
 * @param questions Вопросы, которые нужно записать.
 * @return true, если файл удалось открыть и записать; иначе false.
 */
bool saveQuestions(const std::string& filename, const std::vector<Question>& questions);

/**
 * @brief Возвращает вопросы, подходящие под заданные фильтры.
 * @param questions Исходная коллекция вопросов.
 * @param filters Необязательные фильтры по теме и сложности.
 * @return Вектор со всеми подходящими вопросами в исходном порядке.
 */
std::vector<Question> filterQuestions(const std::vector<Question>& questions,
                                      const QuestionFilters& filters);

/**
 * @brief Извлекает отсортированные уникальные названия тем из вопросов.
 * @param questions Исходная коллекция вопросов.
 * @return Отсортированный вектор уникальных строк с темами.
 */
std::vector<std::string> getUniqueTopics(const std::vector<Question>& questions);

/**
 * @brief Извлекает уникальные названия сложностей из вопросов.
 * @param questions Исходная коллекция вопросов.
 * @return Вектор уникальных строк со сложностями в порядке первого появления.
 */
std::vector<std::string> getUniqueDifficulties(const std::vector<Question>& questions);

/**
 * @brief Добавляет один результат игрока в файл рейтинга.
 * @param filename Путь к файлу рейтинга.
 * @param result Запись результата, которую нужно добавить.
 * @return true, если файл удалось открыть и записать; иначе false.
 */
bool saveResult(const std::string& filename, const PlayerResult& result);

/**
 * @brief Загружает и сортирует результаты игроков из файла рейтинга.
 * @param filename Путь к файлу рейтинга.
 * @return Распознанные результаты, отсортированные по убыванию score, затем по убыванию maxScore.
 */
std::vector<PlayerResult> loadRanking(const std::string& filename);
} // namespace DataStore
