#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "Data.h"
#include "PlayerManager.h"
#include "QuestionEditor.h"
#include "QuizEngine.h"
#include "RankingView.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace
{

namespace fs = std::filesystem;

Question makeQuestion(std::string text, std::string topic, std::string difficulty,
                      std::size_t correctIndex = 0, int timeLimitSec = 30, int points = 1)
{
    return {std::move(text),
            {"answer 0", "answer 1", "answer 2", "answer 3"},
            correctIndex,
            std::move(topic),
            std::move(difficulty),
            timeLimitSec,
            points};
}

std::vector<Question> sampleQuestions()
{
    return {makeQuestion("math easy", "math", "easy", 1, 5, 2),
            makeQuestion("math hard", "math", "hard", 2, 7, 3),
            makeQuestion("history normal", "history", "normal", 0, 9, 4)};
}

fs::path uniquePath(const std::string& filename)
{
    static int counter = 0;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    return fs::temp_directory_path() / ("quiz_sfml_tests_" + std::to_string(ticks) + "_" +
                                        std::to_string(counter++) + "_" + filename);
}

fs::path pathInsideMissingDirectory(const std::string& filename)
{
    return uniquePath("missing_directory") / filename;
}

bool contains(const std::string& text, const std::string& fragment)
{
    return text.find(fragment) != std::string::npos;
}

PlayerResult result(std::string name, int score, int maxScore, std::string mode = "solo")
{
    return {std::move(name), score, maxScore, "2026-06-05", "normal", std::move(mode)};
}

} // namespace

TEST_CASE("DataStore::defaultQuestions returns valid built-in questions")
{
    const auto questions = DataStore::defaultQuestions();

    REQUIRE_FALSE(questions.empty());
    for (const auto& question : questions)
    {
        CHECK_FALSE(question.text.empty());
        CHECK(question.answers.size() == 4);
        CHECK(question.correctIndex < question.answers.size());
        CHECK_FALSE(question.topic.empty());
        CHECK_FALSE(question.difficulty.empty());
        CHECK(question.timeLimitSec > 0);
        CHECK(question.points > 0);
    }
}

TEST_CASE("DataStore saves and loads questions, and rejects invalid files")
{
    const auto path = uniquePath("questions.txt");
    const auto invalidPath = pathInsideMissingDirectory("questions.txt");
    const auto invalidFile = uniquePath("invalid_questions.txt");
    const auto questions = sampleQuestions();

    CHECK(DataStore::saveQuestions(path.string(), questions));
    CHECK_FALSE(DataStore::saveQuestions(invalidPath.string(), questions));

    std::vector<Question> loaded;
    CHECK(DataStore::loadQuestions(path.string(), loaded));
    REQUIRE(loaded.size() == questions.size());
    CHECK(loaded[0].text == "math easy");
    CHECK(loaded[0].answers == questions[0].answers);
    CHECK(loaded[1].correctIndex == 2);
    CHECK(loaded[2].points == 4);

    std::vector<Question> missingLoaded = questions;
    CHECK_FALSE(DataStore::loadQuestions(pathInsideMissingDirectory("missing.txt").string(),
                                         missingLoaded));

    {
        std::ofstream out(invalidFile);
        out << "broken question\n";
        out << "only|three|answers\n";
        out << "not-a-number\n";
    }

    std::vector<Question> invalidLoaded;
    CHECK_FALSE(DataStore::loadQuestions(invalidFile.string(), invalidLoaded));

    std::error_code ec;
    fs::remove(path, ec);
    fs::remove(invalidFile, ec);
}

TEST_CASE("DataStore filters questions and collects unique values")
{
    const auto questions = sampleQuestions();

    QuestionFilters mathOnly;
    mathOnly.topic = "math";
    auto filtered = DataStore::filterQuestions(questions, mathOnly);
    REQUIRE(filtered.size() == 2);
    CHECK(filtered[0].topic == "math");
    CHECK(filtered[1].topic == "math");

    QuestionFilters mathHard;
    mathHard.topic = "math";
    mathHard.difficulty = "hard";
    filtered = DataStore::filterQuestions(questions, mathHard);
    REQUIRE(filtered.size() == 1);
    CHECK(filtered[0].text == "math hard");

    QuestionFilters noMatches;
    noMatches.topic = "physics";
    CHECK(DataStore::filterQuestions(questions, noMatches).empty());

    auto topics = DataStore::getUniqueTopics(questions);
    REQUIRE(topics.size() == 2);
    CHECK(topics[0] == "history");
    CHECK(topics[1] == "math");
    CHECK(DataStore::getUniqueTopics({}).empty());

    auto difficulties = DataStore::getUniqueDifficulties(questions);
    REQUIRE(difficulties.size() == 3);
    CHECK(difficulties[0] == "easy");
    CHECK(difficulties[1] == "hard");
    CHECK(difficulties[2] == "normal");
    CHECK(DataStore::getUniqueDifficulties({}).empty());
}

TEST_CASE("DataStore saves results and loads sorted ranking, ignoring bad lines")
{
    const auto path = uniquePath("ranking.txt");
    const auto invalidPath = pathInsideMissingDirectory("ranking.txt");

    CHECK(DataStore::saveResult(path.string(), result("Alice", 7, 10)));
    CHECK(DataStore::saveResult(path.string(), result("Bob", 9, 10)));
    CHECK(DataStore::saveResult(path.string(), result("Charlie", 7, 9, "duo")));
    CHECK_FALSE(DataStore::saveResult(invalidPath.string(), result("Nobody", 1, 1)));

    {
        std::ofstream out(path, std::ios::app);
        out << "bad|line\n";
        out << "Invalid|score|max|2026-06-05|normal|solo\n";
    }

    const auto ranking = DataStore::loadRanking(path.string());
    REQUIRE(ranking.size() == 3);
    CHECK(ranking[0].playerName == "Bob");
    CHECK(ranking[1].playerName == "Alice");
    CHECK(ranking[2].playerName == "Charlie");
    CHECK(
        DataStore::loadRanking(pathInsideMissingDirectory("missing_ranking.txt").string()).empty());

    std::error_code ec;
    fs::remove(path, ec);
}

TEST_CASE("QuizEngine loads questions, navigates them, and reports finished state")
{
    QuizEngine engine;
    CHECK(engine.finished());
    CHECK(engine.currentQuestion() == nullptr);
    CHECK(engine.totalQuestions() == 0);
    CHECK(engine.timeLeft() == 0);

    const auto questions = sampleQuestions();
    engine.setQuestions(questions);

    CHECK_FALSE(engine.finished());
    REQUIRE(engine.currentQuestion() != nullptr);
    CHECK(engine.currentQuestion()->text == "math easy");
    CHECK(engine.totalQuestions() == questions.size());
    CHECK(engine.timeLeft() == 5);

    CHECK(engine.nextQuestion());
    REQUIRE(engine.currentQuestion() != nullptr);
    CHECK(engine.currentQuestion()->text == "math hard");
    CHECK(engine.timeLeft() == 7);

    CHECK(engine.nextQuestion());
    CHECK_FALSE(engine.nextQuestion());
    CHECK(engine.finished());
    CHECK(engine.currentQuestion() == nullptr);
}

TEST_CASE("QuizEngine answers correctly, incorrectly, and refuses answers after finish")
{
    QuizEngine engine;
    engine.setQuestions({makeQuestion("scored", "math", "easy", 1, 5, 3)});

    CHECK(engine.answer(1));
    CHECK(engine.score() == 3);
    CHECK(engine.maxScore() == 3);
    CHECK_FALSE(engine.nextQuestion());
    CHECK_FALSE(engine.answer(1));
    CHECK(engine.score() == 3);
    CHECK(engine.maxScore() == 3);

    engine.setQuestions({makeQuestion("wrong", "math", "easy", 0, 5, 2)});
    CHECK_FALSE(engine.answer(3));
    CHECK(engine.score() == 0);
    CHECK(engine.maxScore() == 2);
}

TEST_CASE("QuizEngine filters questions and builds player results")
{
    QuizEngine engine;
    QuestionFilters filters;
    filters.topic = "math";
    filters.difficulty = "hard";

    engine.setQuestionsWithFilter(sampleQuestions(), filters);
    CHECK(engine.totalQuestions() == 1);
    REQUIRE(engine.currentQuestion() != nullptr);
    CHECK(engine.currentQuestion()->text == "math hard");

    engine.setDifficulty("hard");
    CHECK(engine.answer(2));
    const auto playerResult = engine.buildResult("Dana");
    CHECK(playerResult.playerName == "Dana");
    CHECK(playerResult.score == 3);
    CHECK(playerResult.maxScore == 3);
    CHECK(playerResult.difficulty == "hard");
    CHECK(playerResult.timestamp == "2026-06-03");

    filters.topic = "missing";
    engine.setQuestionsWithFilter(sampleQuestions(), filters);
    CHECK(engine.finished());
    CHECK(engine.currentQuestion() == nullptr);
}

TEST_CASE("QuizEngine::tick decreases remaining time and clamps it to zero")
{
    QuizEngine engine;
    engine.setQuestions({makeQuestion("timed", "math", "easy", 0, 1, 1)});

    REQUIRE(engine.timeLeft() == 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    engine.tick();
    CHECK(engine.timeLeft() == 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    engine.tick();
    CHECK(engine.timeLeft() == 0);
}

TEST_CASE("QuestionEditor checks password and stores current questions")
{
    QuestionEditor editor("secret");

    CHECK(editor.login("secret"));
    CHECK_FALSE(editor.login("wrong"));

    editor.setQuestions(sampleQuestions());
    CHECK(editor.questions().size() == 3);
    CHECK(editor.currentIndex() == 0);
    CHECK(editor.currentQuestion().text == "math easy");

    editor.setCurrentIndex(2);
    CHECK(editor.currentQuestion().text == "history normal");
    editor.setCurrentIndex(99);
    CHECK(editor.currentIndex() == 2);

    editor.setQuestions({makeQuestion("single", "topic", "easy")});
    CHECK(editor.currentIndex() == 0);
    CHECK(editor.currentQuestion().text == "single");

    editor.setQuestions({});
    CHECK(editor.questions().empty());
    CHECK(editor.currentIndex() == 0);
    CHECK_THROWS_AS(editor.currentQuestion(), std::out_of_range);
}

TEST_CASE("QuestionEditor adds, deletes, updates, and ignores invalid indexes")
{
    QuestionEditor editor("secret");
    editor.setQuestions(sampleQuestions());

    editor.addQuestion();
    REQUIRE(editor.questions().size() == 4);
    CHECK(editor.currentIndex() == 3);
    CHECK(editor.currentQuestion().answers.size() == 4);

    editor.updateQuestionText(0, "updated text");
    editor.updateAnswer(0, 2, "updated answer");
    editor.setTime(0, 42);
    editor.setTopic(0, "updated topic");
    editor.setDifficulty(0, "updated difficulty");
    editor.setCorrectAnswer(0, 2);

    CHECK(editor.questions()[0].text == "updated text");
    CHECK(editor.questions()[0].answers[2] == "updated answer");
    CHECK(editor.questions()[0].timeLimitSec == 42);
    CHECK(editor.questions()[0].topic == "updated topic");
    CHECK(editor.questions()[0].difficulty == "updated difficulty");
    CHECK(editor.questions()[0].correctIndex == 2);

    const auto beforeInvalidUpdates = editor.questions()[0];
    editor.updateQuestionText(-1, "bad");
    editor.updateAnswer(0, 99, "bad");
    editor.setTime(99, 1);
    editor.setTopic(99, "bad");
    editor.setDifficulty(99, "bad");
    editor.setCorrectAnswer(99, 0);
    CHECK(editor.questions()[0].text == beforeInvalidUpdates.text);
    CHECK(editor.questions()[0].answers == beforeInvalidUpdates.answers);
    CHECK(editor.questions()[0].timeLimitSec == beforeInvalidUpdates.timeLimitSec);
    CHECK(editor.questions()[0].topic == beforeInvalidUpdates.topic);
    CHECK(editor.questions()[0].difficulty == beforeInvalidUpdates.difficulty);
    CHECK(editor.questions()[0].correctIndex == beforeInvalidUpdates.correctIndex);

    editor.deleteQuestion(3);
    CHECK(editor.questions().size() == 3);
    CHECK(editor.currentIndex() == 2);
    editor.deleteQuestion(99);
    CHECK(editor.questions().size() == 3);
}

TEST_CASE("QuestionEditor saves valid data and rejects invalid save paths")
{
    const auto path = uniquePath("editor_questions.txt");
    const auto invalidPath = pathInsideMissingDirectory("editor_questions.txt");

    QuestionEditor editor("secret");
    editor.setQuestions(sampleQuestions());

    CHECK(editor.save(path.string()));
    CHECK_FALSE(editor.save(invalidPath.string()));

    std::vector<Question> loaded;
    CHECK(DataStore::loadQuestions(path.string(), loaded));
    REQUIRE(loaded.size() == editor.questions().size());
    CHECK(loaded[0].text == editor.questions()[0].text);

    std::error_code ec;
    fs::remove(path, ec);
}

TEST_CASE("RankingView builds text for empty, solo, and duo rankings")
{
    RankingView view;

    const auto emptyText = view.buildText();
    CHECK(contains(emptyText, "==="));
    CHECK_FALSE(contains(emptyText, " | "));

    view.setRanking({result("Alice", 7, 10), result("Bob", 8, 10, "duo")});

    const auto text = view.buildText();
    CHECK(contains(text, "Alice | 7/10"));
    CHECK(contains(text, "Bob | 8/10"));
}

TEST_CASE("RankingView scrolls down, up, resets, and handles mixed short groups")
{
    RankingView view;
    std::vector<PlayerResult> manySolo;
    for (int i = 1; i <= 12; ++i)
    {
        manySolo.push_back(result("Player" + std::to_string(i), i, 12));
    }

    view.setRanking(manySolo);
    CHECK(contains(view.buildText(), "Player1 | 1/12"));
    CHECK_FALSE(contains(view.buildText(), "Player11 | 11/12"));

    view.scrollDown();
    CHECK(contains(view.buildText(), "Player11 | 11/12"));
    CHECK_FALSE(contains(view.buildText(), "Player1 | 1/12"));

    view.scrollUp();
    CHECK(contains(view.buildText(), "Player1 | 1/12"));

    view.scrollDown();
    view.resetScroll();
    CHECK(contains(view.buildText(), "Player1 | 1/12"));

    view.setRanking({result("OnlySolo", 1, 1), result("Duo1", 1, 1, "duo"),
                     result("Duo2", 2, 2, "duo"), result("Duo3", 3, 3, "duo"),
                     result("Duo4", 4, 4, "duo"), result("Duo5", 5, 5, "duo"),
                     result("Duo6", 6, 6, "duo"), result("Duo7", 7, 7, "duo"),
                     result("Duo8", 8, 8, "duo"), result("Duo9", 9, 9, "duo"),
                     result("Duo10", 10, 10, "duo"), result("Duo11", 11, 11, "duo")});

    view.scrollDown();
    const auto mixedText = view.buildText();
    CHECK(contains(mixedText, "OnlySolo | 1/1"));
    CHECK(contains(mixedText, "Duo11 | 11/11"));
}

TEST_CASE("PlayerManager manages players and rejects invalid current access")
{
    PlayerManager manager;

    CHECK(manager.empty());
    CHECK(manager.players().empty());
    CHECK_FALSE(manager.next());
    CHECK_THROWS_AS(manager.current(), std::out_of_range);

    manager.addPlayer("Alice");
    manager.addPlayer("Bob");

    CHECK_FALSE(manager.empty());
    REQUIRE(manager.players().size() == 2);
    CHECK(manager.current().name == "Alice");

    manager.current().totalScore = 5;
    CHECK(manager.players()[0].totalScore == 5);

    CHECK(manager.next());
    CHECK(manager.current().name == "Bob");
    CHECK_FALSE(manager.next());

    manager.clear();
    CHECK(manager.empty());
    CHECK(manager.players().empty());
    CHECK_THROWS_AS(manager.current(), std::out_of_range);
}
