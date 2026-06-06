#include "Data.h"
#include <algorithm>
#include <fstream>
#include <sstream>

/**
 * @brief Разделяет строку по символу-разделителю.
 * @param s Исходная строка для разделения.

 * * @param delim Символ-разделитель.
 * @return Вектор со всеми найденными частями в исходном
 * порядке.
 */
static std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        parts.push_back(item);
    }
    return parts;
}

/**
 * @brief Объединяет строки с использованием символа-разделителя.
 * @param parts Строки,
 * которые нужно объединить.
 * @param delim Разделитель, вставляемый между соседними строками.
 *
 * @return Объединенная строка.
 */
static std::string join(const std::vector<std::string>& parts, char delim)
{
    std::string out;
    for (std::size_t i = 0; i < parts.size(); ++i)
    {
        out += parts[i];
        if (i + 1 < parts.size())
            out += delim;
    }
    return out;
}

std::vector<Question> DataStore::defaultQuestions()
{
    return {
        {"Столица Франции?", {"Париж", "Берлин", "Рим", "Мадрид"}, 0, "География", "easy", 20, 1},
        {"2 + 2 * 2 = ?", {"4", "6", "8", "10"}, 1, "Математика", "easy", 15, 1},
        {"Какая планета известна как Красная?",
         {"Венера", "Марс", "Юпитер", "Меркурий"},
         1,
         "Астрономия",
         "normal",
         20,
         1},
        {"Кто написал 'Войну и мир'?",
         {"Пушкин", "Толстой", "Достоевский", "Гоголь"},
         1,
         "Литература",
         "normal",
         25,
         1},
        {"Что такое SFML?",
         {"Язык", "Библиотека", "ОС", "Компилятор"},
         1,
         "Программирование",
         "easy",
         20,
         1},
        {"Какой тип памяти используется для локальных переменных?",
         {"Heap", "Stack", "ROM", "Cache"},
         1,
         "Программирование",
         "hard",
         25,
         2}};
}

bool DataStore::saveQuestions(const std::string& filename, const std::vector<Question>& questions)
{
    std::ofstream out(filename);
    if (!out.is_open())
        return false;

    for (const auto& q : questions)
    {
        out << q.text << '\n';
        out << join(q.answers, '|') << '\n';
        out << q.correctIndex << '\n';
        out << q.topic << '\n';
        out << q.difficulty << '\n';
        out << q.timeLimitSec << '\n';
        out << q.points << '\n';
        out << "---\n";
    }

    return true;
}

bool DataStore::loadQuestions(const std::string& filename, std::vector<Question>& questions)
{
    std::ifstream in(filename);
    if (!in.is_open())
        return false;

    questions.clear();
    std::string line;

    while (std::getline(in, line))
    {
        if (line.empty() || line == "---")
            continue;

        Question q;
        q.text = line;

        if (!std::getline(in, line))
            break;
        q.answers = split(line, '|');

        if (!std::getline(in, line))
            break;
        try
        {
            q.correctIndex = static_cast<std::size_t>(std::stoul(line));
        }
        catch (...)
        {
            std::getline(in, line);
            continue;
        }

        if (!std::getline(in, line))
            break;
        q.topic = line;

        if (!std::getline(in, line))
            break;
        q.difficulty = line;

        if (!std::getline(in, line))
            break;
        try
        {
            q.timeLimitSec = std::stoi(line);
        }
        catch (...)
        {
            std::getline(in, line);
            continue;
        }

        if (!std::getline(in, line))
            break;
        try
        {
            q.points = std::stoi(line);
        }
        catch (...)
        {
            std::getline(in, line);
            continue;
        }

        std::getline(in, line);

        if (q.answers.size() == 4)
        {
            questions.push_back(q);
        }
    }

    return !questions.empty();
}

std::vector<Question> DataStore::filterQuestions(const std::vector<Question>& questions,
                                                 const QuestionFilters& filters)
{
    std::vector<Question> result;
    for (const auto& q : questions)
    {
        if (filters.topic.has_value() && q.topic != filters.topic.value())
            continue;
        if (filters.difficulty.has_value() && q.difficulty != filters.difficulty.value())
            continue;
        result.push_back(q);
    }
    return result;
}

std::vector<std::string> DataStore::getUniqueTopics(const std::vector<Question>& questions)
{
    std::vector<std::string> topics;
    for (const auto& q : questions)
    {
        if (std::find(topics.begin(), topics.end(), q.topic) == topics.end())
        {
            topics.push_back(q.topic);
        }
    }
    std::sort(topics.begin(), topics.end());
    return topics;
}

std::vector<std::string> DataStore::getUniqueDifficulties(const std::vector<Question>& questions)
{
    std::vector<std::string> diffs;
    for (const auto& q : questions)
    {
        if (std::find(diffs.begin(), diffs.end(), q.difficulty) == diffs.end())
        {
            diffs.push_back(q.difficulty);
        }
    }
    return diffs;
}

bool DataStore::saveResult(const std::string& filename, const PlayerResult& result)
{
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open())
        return false;

    out << result.playerName << '|' << result.score << '|' << result.maxScore << '|'
        << result.timestamp << '|' << result.difficulty << '|' << result.mode << '\n';

    return true;
}

std::vector<PlayerResult> DataStore::loadRanking(const std::string& filename)
{
    std::vector<PlayerResult> results;
    std::ifstream in(filename);
    if (!in.is_open())
        return results;

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        auto parts = split(line, '|');
        if (parts.size() < 6)
            continue;

        PlayerResult r;
        r.playerName = parts[0];

        try
        {
            r.score = std::stoi(parts[1]);
            r.maxScore = std::stoi(parts[2]);
        }
        catch (...)
        {
            continue;
        }

        r.timestamp = parts[3];
        r.difficulty = parts[4];
        r.mode = parts[5];
        results.push_back(r);
    }

    std::sort(results.begin(), results.end(),
              [](const PlayerResult& a, const PlayerResult& b)
              {
                  if (a.score != b.score)
                      return a.score > b.score;
                  return a.maxScore > b.maxScore;
              });

    return results;
}
