/**
 * @file App.cpp
 */
#include "App.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>

static std::string wrapText(const std::string& text, std::size_t maxLineLen = 120)
{
    std::string out;
    std::string line;
    std::istringstream iss(text);
    std::string word;

    while (iss >> word)
    {
        if (line.size() + word.size() + 1 > maxLineLen)
        {
            out += line + '\n';
            line = word;
        }
        else
        {
            if (!line.empty())
                line += ' ';
            line += word;
        }
    }

    if (!line.empty())
        out += line;
    return out;
}

static std::string nowDate()
{
    return "2026-06-06";
}

App::App() : window(sf::VideoMode({1200u, 800u}), "Mini Quiz SFML")
{
    window.setVerticalSyncEnabled(true);
    currentFilters.topic = std::nullopt;
    currentFilters.difficulty = std::nullopt;
    loadResources();
    editor.emplace(password);
    setupUI();
    loadRankingData();
}

sf::String App::toSf(const std::string& s) const
{
    return sf::String::fromUtf8(s.begin(), s.end());
}

sf::String App::toSfUtf8(const char* s) const
{
    const std::string str = s ? std::string(s) : std::string();
    return sf::String::fromUtf8(str.begin(), str.end());
}

void App::loadResources()
{
    if (!font.openFromFile("C:/Windows/Fonts/tahoma.ttf"))
    {
        std::cerr << "[ERROR] Failed to load font\n";
        return;
    }

    titleText.emplace(font, toSf("Мини-викторина"), 66);
    questionText.emplace(font, sf::String(), 32);
    infoText.emplace(font, sf::String(), 28);
    scoreText.emplace(font, sf::String(), 28);
    timerText.emplace(font, sf::String(), 28);
    rankingText.emplace(font, sf::String(), 22);
    inputText.emplace(font, sf::String(), 28);

    titleText->setPosition(sf::Vector2f(40.f, 20.f));
    questionText->setPosition(sf::Vector2f(40.f, 120.f));
    infoText->setPosition(sf::Vector2f(40.f, 650.f));
    scoreText->setPosition(sf::Vector2f(850.f, 20.f));
    timerText->setPosition(sf::Vector2f(850.f, 60.f));
    rankingText->setPosition(sf::Vector2f(40.f, 120.f));
    inputText->setPosition(sf::Vector2f(40.f, 40.f));
}

void App::setupUI()
{
    answerButtons.clear();
    answerButtons.emplace_back(font, sf::String(), sf::Vector2f(50.f, 250.f),
                               sf::Vector2f(1100.f, 60.f));
    answerButtons.emplace_back(font, sf::String(), sf::Vector2f(50.f, 330.f),
                               sf::Vector2f(1100.f, 60.f));
    answerButtons.emplace_back(font, sf::String(), sf::Vector2f(50.f, 410.f),
                               sf::Vector2f(1100.f, 60.f));
    answerButtons.emplace_back(font, sf::String(), sf::Vector2f(50.f, 490.f),
                               sf::Vector2f(1100.f, 60.f));

    menuButtons.clear();
    menuButtons.emplace_back(font, toSf("Начать игру"), sf::Vector2f(475.f, 220.f),
                             sf::Vector2f(250.f, 50.f));
    menuButtons.emplace_back(font, toSf("2 игрока"), sf::Vector2f(475.f, 290.f),
                             sf::Vector2f(250.f, 50.f));
    menuButtons.emplace_back(font, toSf("Рейтинг"), sf::Vector2f(475.f, 360.f),
                             sf::Vector2f(250.f, 50.f));
    menuButtons.emplace_back(font, toSf("Редактор"), sf::Vector2f(475.f, 430.f),
                             sf::Vector2f(250.f, 50.f));
    menuButtons.emplace_back(font, toSf("Выход"), sf::Vector2f(475.f, 500.f),
                             sf::Vector2f(250.f, 50.f));

    rankingButtons.clear();
    rankingButtons.emplace_back(font, toSf("Назад"), sf::Vector2f(1000.f, 700.f),
                                sf::Vector2f(180.f, 50.f));

    scrollUpButton.emplace(font, toSf("Вверх"), sf::Vector2f(950.f, 200.f),
                           sf::Vector2f(180.f, 50.f));
    scrollDownButton.emplace(font, toSf("Вниз"), sf::Vector2f(950.f, 260.f),
                             sf::Vector2f(180.f, 50.f));

    nextButton.emplace(font, toSf("Далее"), sf::Vector2f(950.f, 700.f), sf::Vector2f(180.f, 50.f));
    restartButton.emplace(font, toSf("Сначала"), sf::Vector2f(730.f, 700.f),
                          sf::Vector2f(180.f, 50.f));
    backButton.emplace(font, toSf("Назад"), sf::Vector2f(40.f, 700.f), sf::Vector2f(180.f, 50.f));
    submitButton.emplace(font, toSf("Войти"), sf::Vector2f(320.f, 700.f),
                         sf::Vector2f(180.f, 50.f));

    editorSaveButton.emplace(font, toSf("Сохранить всё"), sf::Vector2f(960.f, 710.f),
                             sf::Vector2f(180.f, 50.f));
    editorBackButton.emplace(font, toSf("Назад в меню"), sf::Vector2f(760.f, 710.f),
                             sf::Vector2f(180.f, 50.f));

    questions = DataStore::defaultQuestions();
    if (!DataStore::loadQuestions("questions.txt", questions))
    {
        DataStore::saveQuestions("questions.txt", questions);
    }

    refreshTopicButtons();
    refreshDifficultyButtons();
    engine.setQuestions(questions);
    if (editor)
        editor->setQuestions(questions);
    refreshAnswerButtons();
    refreshEditorButtons();
    refreshTexts();
}

std::string App::getEditorFieldValue(EditorField field) const
{
    if (!editor || editor->questions().empty())
        return {};
    const Question& q = editor->currentQuestion();

    switch (field)
    {
        case EditorField::Question:
            return q.text;
        case EditorField::Answer0:
            return q.answers[0];
        case EditorField::Answer1:
            return q.answers[1];
        case EditorField::Answer2:
            return q.answers[2];
        case EditorField::Answer3:
            return q.answers[3];
        case EditorField::Time:
            return std::to_string(q.timeLimitSec);
        case EditorField::Topic:
            return q.topic;
        case EditorField::Difficulty:
            return q.difficulty;
        default:
            return {};
    }
}

void App::setEditorFieldValue(EditorField field, const std::string& value)
{
    if (!editor || editor->questions().empty())
        return;
    Question& q = editor->currentQuestion();

    switch (field)
    {
        case EditorField::Question:
            q.text = value;
            break;
        case EditorField::Answer0:
            q.answers[0] = value;
            break;
        case EditorField::Answer1:
            q.answers[1] = value;
            break;
        case EditorField::Answer2:
            q.answers[2] = value;
            break;
        case EditorField::Answer3:
            q.answers[3] = value;
            break;
        case EditorField::Time:
            try
            {
                q.timeLimitSec = std::stoi(value);
            }
            catch (...)
            {
            }
            break;
        case EditorField::Topic:
            q.topic = value;
            break;
        case EditorField::Difficulty:
            q.difficulty = value;
            break;
        default:
            break;
    }

    questions = editor->questions();
    engine.setQuestions(questions);
}

void App::setCorrectAnswer(std::size_t index)
{
    if (!editor || editor->questions().empty())
        return;
    Question& q = editor->currentQuestion();
    if (index >= q.answers.size())
        return;

    q.correctIndex = index;
    questions = editor->questions();
    engine.setQuestions(questions);
    refreshEditorButtons();
}

void App::placeEditorInput()
{
    if (!inputText)
        return;
    inputText->setOrigin({0.f, 0.f});
    inputText->setPosition({600.f, 600.f});
}

void App::selectEditorField(EditorField field)
{
    editorField = field;
    editorInput = toSf(getEditorFieldValue(field));
    if (inputText)
    {
        inputText->setString(editorInput);
        placeEditorInput();
    }
}

void App::applyEditorInput()
{
    if (editorField == EditorField::None)
        return;
    const auto utf8 = editorInput.toUtf8();
    setEditorFieldValue(editorField, std::string(utf8.begin(), utf8.end()));
    refreshEditorButtons();
    editorField = EditorField::None;
    editorInput.clear();
    if (inputText)
        inputText->setString(toSf("Ввод: "));
}

void App::refreshEditorButtons()
{
    editorButtons.clear();

    if (!editor || editor->questions().empty())
    {
        editorButtons.emplace_back(font, toSf("Нет вопросов"), sf::Vector2f(400.f, 300.f),
                                   sf::Vector2f(200.f, 50.f));
        return;
    }

    const Question& q = editor->currentQuestion();

    editorButtons.emplace_back(font, toSf("<Назад"), sf::Vector2f(40.f, 100.f),
                               sf::Vector2f(120.f, 45.f));
    editorButtons.emplace_back(font,
                               sf::String(std::to_string(editor->currentIndex() + 1) + "/" +
                                          std::to_string(editor->questions().size())),
                               sf::Vector2f(170.f, 100.f), sf::Vector2f(160.f, 45.f));
    editorButtons.emplace_back(font, toSf("Вперед>"), sf::Vector2f(340.f, 100.f),
                               sf::Vector2f(120.f, 45.f));
    editorButtons.emplace_back(font, toSf("Добавить"), sf::Vector2f(470.f, 100.f),
                               sf::Vector2f(120.f, 45.f));
    editorButtons.emplace_back(font, toSf("Удалить"), sf::Vector2f(600.f, 100.f),
                               sf::Vector2f(120.f, 45.f));

    editorButtons.emplace_back(font, toSf("Вопрос:"), sf::Vector2f(40.f, 170.f),
                               sf::Vector2f(110.f, 40.f));
    editorButtons.emplace_back(font, toSf(q.text), sf::Vector2f(180.f, 170.f),
                               sf::Vector2f(790.f, 40.f));

    for (int i = 0; i < 4; ++i)
    {
        float y = 230.f + i * 55.f;
        editorButtons.emplace_back(font, toSf("Ответ " + std::to_string(i + 1) + ":"),
                                   sf::Vector2f(40.f, y), sf::Vector2f(110.f, 40.f));
        editorButtons.emplace_back(font, toSf(q.answers[i]), sf::Vector2f(180.f, y),
                                   sf::Vector2f(630.f, 40.f));
        editorButtons.emplace_back(font, toSf(i == static_cast<int>(q.correctIndex) ? "Верно" : ""),
                                   sf::Vector2f(830.f, y), sf::Vector2f(90.f, 40.f));
    }

    editorButtons.emplace_back(font, toSf("Время:"), sf::Vector2f(40.f, 460.f),
                               sf::Vector2f(130.f, 40.f));
    editorButtons.emplace_back(font, toSf(std::to_string(q.timeLimitSec)),
                               sf::Vector2f(180.f, 460.f), sf::Vector2f(130.f, 40.f));

    editorButtons.emplace_back(font, toSf("Предмет:"), sf::Vector2f(40.f, 515.f),
                               sf::Vector2f(130.f, 40.f));
    editorButtons.emplace_back(font, toSf(q.topic), sf::Vector2f(180.f, 515.f),
                               sf::Vector2f(300.f, 40.f));

    editorButtons.emplace_back(font, toSf("Сложность"), sf::Vector2f(40.f, 570.f),
                               sf::Vector2f(130.f, 40.f));
    editorButtons.emplace_back(font, toSf(q.difficulty), sf::Vector2f(180.f, 570.f),
                               sf::Vector2f(300.f, 40.f));
}

void App::refreshTopicButtons()
{
    topicButtons.clear();
    auto topics = DataStore::getUniqueTopics(questions);
    topicButtons.emplace_back(font,
                              toSf("Все темы (" + std::to_string(questions.size()) + " вопросов)"),
                              sf::Vector2f(340.f, 150.f), sf::Vector2f(520.f, 50.f));
    for (std::size_t i = 0; i < topics.size(); ++i)
    {
        QuestionFilters filter;
        filter.topic = topics[i];
        auto filtered = DataStore::filterQuestions(questions, filter);
        topicButtons.emplace_back(
            font, toSf(topics[i] + " (" + std::to_string(filtered.size()) + ")"),
            sf::Vector2f(340.f, 220.f + static_cast<float>(i) * 60.f), sf::Vector2f(520.f, 50.f));
    }
}

void App::refreshDifficultyButtons()
{
    difficultyButtons.clear();

    std::vector<Question> questionsToShow = questions;
    if (currentFilters.topic.has_value())
    {
        QuestionFilters tempFilter;
        tempFilter.topic = currentFilters.topic.value();
        questionsToShow = DataStore::filterQuestions(questions, tempFilter);
    }

    auto difficulties = DataStore::getUniqueDifficulties(questionsToShow);

    std::vector<std::string> sortedDifficulties;
    if (std::find(difficulties.begin(), difficulties.end(), "easy") != difficulties.end())
        sortedDifficulties.push_back("easy");
    if (std::find(difficulties.begin(), difficulties.end(), "normal") != difficulties.end())
        sortedDifficulties.push_back("normal");
    if (std::find(difficulties.begin(), difficulties.end(), "hard") != difficulties.end())
        sortedDifficulties.push_back("hard");

    difficultyButtons.emplace_back(
        font, toSf("Все сложности (" + std::to_string(questionsToShow.size()) + " вопросов)"),
        sf::Vector2f(400.f, 250.f), sf::Vector2f(450.f, 50.f));

    for (std::size_t i = 0; i < sortedDifficulties.size(); ++i)
    {
        QuestionFilters filter;
        filter.topic = currentFilters.topic;
        filter.difficulty = sortedDifficulties[i];
        auto filtered = DataStore::filterQuestions(questions, filter);
        difficultyButtons.emplace_back(
            font, toSf(sortedDifficulties[i] + " (" + std::to_string(filtered.size()) + ")"),
            sf::Vector2f(500.f, 320.f + static_cast<float>(i) * 70.f), sf::Vector2f(250.f, 50.f));
    }
}

void App::resetQuiz()
{
    if (state == ScreenState::Quiz)
    {
        auto filtered = DataStore::filterQuestions(questions, currentFilters);
        engine.setQuestions(filtered);
        engine.reset();
        refreshTexts();
        refreshAnswerButtons();
    }
}

void App::refreshTexts()
{
    titleText->setString(toSf("Мини-викторина"));

    if (state == ScreenState::NameInput)
    {
        infoText->setString(toSf("Нажмите Enter после ввода имени"));
        if (inputText)
        {
            inputText->setString(toSf("Имя: ") + typedName);
            inputText->setOrigin({0.f, 0.f});
            inputText->setPosition({40.f, 200.f});
        }
        return;
    }

    if (state == ScreenState::TopicSelect)
    {
        infoText->setString(toSf("Нажмите на предмет или 'Все предметы'"));
        return;
    }

    if (state == ScreenState::DifficultySelect)
    {
        std::string topicName = currentFilters.topic.value_or("Все предметы");
        int filteredCount =
            static_cast<int>(DataStore::filterQuestions(questions, currentFilters).size());
        infoText->setString(toSf("Предмет: " + topicName +
                                 "\nВопросов после фильтра: " + std::to_string(filteredCount)));
        return;
    }

    if (state == ScreenState::Quiz)
    {
        if (const Question* q = engine.currentQuestion())
        {
            questionText->setString(toSf(wrapText(q->text)));
            scoreText->setString(toSf("Счёт: " + std::to_string(engine.score()) + " / " +
                                      std::to_string(engine.maxScore())));
            timerText->setString(toSf("Время: " + std::to_string(engine.timeLeft())));
        }
        infoText->setString(sf::String());
    }
    else if (state == ScreenState::Result)
    {
        scoreText->setString(toSf("Финальный счёт: " + std::to_string(engine.score()) + " / " +
                                  std::to_string(engine.maxScore())));
        timerText->setString(sf::String());
        infoText->setString(toSf("Игра завершена."));
    }
    else if (state == ScreenState::Ranking)
    {
        rankingText->setString(toSf(rankingView.buildText()));
    }
    else if (state == ScreenState::Editor)
    {
        if (editor && !editor->questions().empty())
        {
            const Question& q = editor->currentQuestion();
            std::string info = "Вопрос " + std::to_string(editor->currentIndex() + 1) + " из " +
                               std::to_string(editor->questions().size()) + "\n";
            info += "Предмет: " + q.topic + "\n";
            info += "Сложность: " + q.difficulty + "\n";
            info += "Кликни по полю и печатай.";
            infoText->setString(toSf(info));
            infoText->setPosition({40.f, 650.f});
        }
        else
        {
            infoText->setString(toSf("Нет вопросов для редактирования."));
        }
    }
    else if (state == ScreenState::PasswordInput)
    {
        infoText->setString(toSf("Введите пароль и нажмите Enter или кнопку Войти."));
        if (inputText)
        {
            inputText->setString(typedInput);
            inputText->setOrigin({0.f, 0.f});
            inputText->setPosition({40.f, 200.f});
        }
    }
    else if (state == ScreenState::MultiplayerNameInput1)
    {
        infoText->setString(toSf("Игрок 1, введите имя"));
        if (inputText)
        {
            inputText->setString(toSf("Игрок 1: ") + multiplayerInput);
            inputText->setOrigin({0.f, 0.f});
            inputText->setPosition({40.f, 200.f});
        }
    }
    else if (state == ScreenState::MultiplayerNameInput2)
    {
        infoText->setString(toSf("Игрок 2, введите имя"));
        if (inputText)
        {
            inputText->setString(toSf("Игрок 2: ") + multiplayerInput);
            inputText->setOrigin({0.f, 0.f});
            inputText->setPosition({40.f, 200.f});
        }
    }
    else if (state == ScreenState::MultiplayerTopicSelect)
    {
        infoText->setString(toSf("Выберите предмет для дуэли"));
    }
    else if (state == ScreenState::MultiplayerDifficultySelect)
    {
        std::string topicName = currentFilters.topic.value_or("Все предметы");
        int filteredCount =
            static_cast<int>(DataStore::filterQuestions(questions, currentFilters).size());
        infoText->setString(toSf("Предмет: " + topicName +
                                 "\nВопросов после фильтра: " + std::to_string(filteredCount)));
    }
    else if (state == ScreenState::MultiplayerQuiz)
    {
        if (!multiplayerQuestions.empty() && multiplayerQuestionIndex < multiplayerQuestions.size())
        {
            const Question& q = multiplayerQuestions[multiplayerQuestionIndex];
            questionText->setString(toSf(q.text));
            scoreText->setString(sf::String());
            timerText->setString(toSf("Ход: " + playerNames[currentQuizPlayer] +
                                      "\nВремя: " + std::to_string(multiplayerTimeLeft)));
            infoText->setString(toSf("Вопрос " + std::to_string(multiplayerQuestionIndex + 1) +
                                     " из " + std::to_string(multiplayerQuestions.size())));
        }
    }
    else if (state == ScreenState::MultiplayerResult)
    {
        infoText->setString(toSf("Результаты игроков"));
        std::string resultLine;
        if (playerScores[0] == playerScores[1])
        {
            resultLine = "Ничья!";
        }
        else if (playerScores[0] > playerScores[1])
        {
            resultLine = "Победил " + playerNames[0] + "!";
        }
        else
        {
            resultLine = "Победил " + playerNames[1] + "!";
        }
        scoreText->setString(toSf(playerNames[0] + ": " + std::to_string(playerScores[0]) + "\n" +
                                  playerNames[1] + ": " + std::to_string(playerScores[1]) + "\n\n" +
                                  resultLine));
        timerText->setString(sf::String());
    }
    else
    {
        infoText->setString(toSf("Выберите действие."));
    }

    if (state == ScreenState::PasswordInput || state == ScreenState::MultiplayerNameInput1 ||
        state == ScreenState::MultiplayerNameInput2)
    {
        return;
    }

    if (state != ScreenState::Editor || editorField == EditorField::None)
    {
        inputText->setString(toSf("Ввод: ") + typedInput);
        auto bounds = inputText->getLocalBounds();
        inputText->setOrigin(
            {bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        inputText->setPosition({40.f, 100.f});
    }
}

void App::refreshAnswerButtons()
{
    const Question* q = nullptr;

    if (state == ScreenState::Quiz)
    {
        q = engine.currentQuestion();
    }
    else if (state == ScreenState::MultiplayerQuiz)
    {
        if (!multiplayerQuestions.empty() && multiplayerQuestionIndex < multiplayerQuestions.size())
        {
            q = &multiplayerQuestions[multiplayerQuestionIndex];
        }
    }

    if (!q)
    {
        for (auto& b : answerButtons)
            b.setLabel(sf::String());
        return;
    }

    for (std::size_t i = 0; i < answerButtons.size(); ++i)
    {
        if (i < q->answers.size())
            answerButtons[i].setLabel(toSf(q->answers[i]));
        else
            answerButtons[i].setLabel(sf::String());
    }
}

void App::saveCurrentResult()
{
    auto result = engine.buildResult(currentPlayerName);
    result.timestamp = nowDate();
    result.mode = "SOLO";
    DataStore::saveResult("results.txt", result);
    loadRankingData();
}

void App::loadRankingData()
{
    ranking = DataStore::loadRanking("results.txt");
    rankingView.setRanking(ranking);
}

void App::switchToMenu()
{
    state = ScreenState::Menu;
    typedInput.clear();
    typedName.clear();
    editorInput.clear();
    multiplayerInput.clear();
    editorField = EditorField::None;
    refreshTexts();
}

void App::openEditorIfAllowed()
{
    if (!editor)
        return;

    const auto utf8 = typedInput.toUtf8();
    if (editor->login(std::string(utf8.begin(), utf8.end())))
    {
        state = ScreenState::Editor;
        infoText->setString(toSf("Пароль верный. Редактор открыт."));
        refreshEditorButtons();
    }
    else
    {
        infoText->setString(toSf("Неверный пароль."));
    }

    typedInput.clear();
    if (inputText)
        inputText->setString(toSf("Ввод: "));
}

void App::startMultiplayer()
{
    multiplayerMode = true;
    playerNames = {"Игрок 1", "Игрок 2"};
    playerScores = {0, 0};
    currentQuizPlayer = 0;
    multiplayerQuestionIndex = 0;
    state = ScreenState::MultiplayerNameInput1;
    multiplayerInput.clear();
    if (inputText)
    {
        inputText->setString(toSf("Игрок 1: "));
        inputText->setOrigin({0.f, 0.f});
        inputText->setPosition({60.f, 100.f});
    }
    titleText->setString(toSf("Мини-викторина"));
}

void App::advanceMultiplayerTurn(bool answeredCorrectly)
{
    if (answeredCorrectly)
    {
        playerScores[currentQuizPlayer]++;
    }

    if (currentQuizPlayer == 0)
    {
        currentQuizPlayer = 1;
    }
    else
    {
        currentQuizPlayer = 0;
        ++multiplayerQuestionIndex;

        if (multiplayerQuestionIndex >= multiplayerQuestions.size())
        {
            state = ScreenState::MultiplayerResult;

            PlayerResult result;
            result.playerName = playerNames[0] + " & " + playerNames[1];
            result.score = playerScores[0] + playerScores[1];
            result.maxScore = static_cast<int>(multiplayerQuestions.size() * 2);
            result.timestamp = nowDate();
            result.difficulty = currentFilters.difficulty.value_or("any");
            result.mode = "DUO";
            DataStore::saveResult("results.txt", result);
            loadRankingData();
            return;
        }

        multiplayerClock.restart();
        multiplayerTimeLeft = multiplayerQuestions[multiplayerQuestionIndex].timeLimitSec;
    }

    refreshTexts();
    refreshAnswerButtons();
}

void App::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void App::processEvents()
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                if (state == ScreenState::Quiz || state == ScreenState::TopicSelect ||
                    state == ScreenState::DifficultySelect || state == ScreenState::Editor ||
                    state == ScreenState::PasswordInput || state == ScreenState::NameInput ||
                    state == ScreenState::MultiplayerNameInput1 ||
                    state == ScreenState::MultiplayerNameInput2 ||
                    state == ScreenState::MultiplayerTopicSelect ||
                    state == ScreenState::MultiplayerDifficultySelect ||
                    state == ScreenState::MultiplayerQuiz ||
                    state == ScreenState::MultiplayerResult)
                {
                    switchToMenu();
                }
                else
                {
                    window.close();
                }
            }

            if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
            {
                if (state == ScreenState::PasswordInput)
                {
                    openEditorIfAllowed();
                }
                else if (state == ScreenState::NameInput)
                {
                    const auto utf8 = typedName.toUtf8();
                    currentPlayerName = std::string(utf8.begin(), utf8.end());
                    if (currentPlayerName.empty())
                        currentPlayerName = "Игрок";
                    state = ScreenState::TopicSelect;
                    refreshTopicButtons();
                    refreshTexts();
                }
                else if (state == ScreenState::Editor && editorField != EditorField::None)
                {
                    applyEditorInput();
                }
                else if (state == ScreenState::MultiplayerNameInput1)
                {
                    const auto utf8 = multiplayerInput.toUtf8();
                    playerNames[0] = std::string(utf8.begin(), utf8.end());
                    if (playerNames[0].empty())
                        playerNames[0] = "Игрок 1";
                    multiplayerInput.clear();
                    state = ScreenState::MultiplayerNameInput2;
                    if (inputText)
                    {
                        inputText->setString(toSf("Игрок 2: "));
                        inputText->setOrigin({0.f, 0.f});
                        inputText->setPosition({60.f, 100.f});
                    }
                }
                else if (state == ScreenState::MultiplayerNameInput2)
                {
                    const auto utf8 = multiplayerInput.toUtf8();
                    playerNames[1] = std::string(utf8.begin(), utf8.end());
                    if (playerNames[1].empty())
                        playerNames[1] = "Игрок 2";
                    state = ScreenState::MultiplayerTopicSelect;
                    currentFilters.topic = std::nullopt;
                    currentFilters.difficulty = std::nullopt;
                    refreshTopicButtons();
                    refreshTexts();
                }
            }

            if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace)
            {
                if (state == ScreenState::PasswordInput && !typedInput.isEmpty())
                {
                    typedInput.erase(typedInput.getSize() - 1, 1);
                }
                if (state == ScreenState::NameInput && !typedName.isEmpty())
                {
                    typedName.erase(typedName.getSize() - 1, 1);
                }
                if (state == ScreenState::Editor && editorField != EditorField::None &&
                    !editorInput.isEmpty())
                {
                    editorInput.erase(editorInput.getSize() - 1, 1);
                    if (inputText)
                    {
                        inputText->setString(editorInput);
                        placeEditorInput();
                    }
                }
                if (state == ScreenState::MultiplayerNameInput1 ||
                    state == ScreenState::MultiplayerNameInput2)
                {
                    if (!multiplayerInput.isEmpty())
                        multiplayerInput.erase(multiplayerInput.getSize() - 1, 1);
                    if (inputText)
                    {
                        if (state == ScreenState::MultiplayerNameInput1)
                            inputText->setString(toSf("Игрок 1: ") + multiplayerInput);
                        else
                            inputText->setString(toSf("Игрок 2: ") + multiplayerInput);
                        inputText->setOrigin({0.f, 0.f});
                        inputText->setPosition({60.f, 100.f});
                    }
                }
            }

            if (state == ScreenState::Editor && editor && !editor->questions().empty())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                {
                    editor->setCurrentIndex(editor->currentIndex() - 1);
                    editorField = EditorField::None;
                    editorInput.clear();
                    refreshEditorButtons();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                {
                    editor->setCurrentIndex(editor->currentIndex() + 1);
                    editorField = EditorField::None;
                    editorInput.clear();
                    refreshEditorButtons();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Equal)
                {
                    editor->addQuestion();
                    questions = editor->questions();
                    engine.setQuestions(questions);
                    refreshEditorButtons();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Hyphen)
                {
                    editor->deleteQuestion(editor->currentIndex());
                    questions = editor->questions();
                    engine.setQuestions(questions);
                    refreshEditorButtons();
                }
            }
        }

        if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
        {
            const char32_t u = textEntered->unicode;
            if (u < 32 || u == 127)
                continue;

            if (state == ScreenState::PasswordInput)
            {
                typedInput += u;
                if (inputText)
                {
                    inputText->setString(typedInput);
                    inputText->setOrigin({0.f, 0.f});
                    inputText->setPosition({40.f, 100.f});
                }
            }

            if (state == ScreenState::NameInput)
            {
                typedName += u;
                if (inputText)
                {
                    inputText->setString(toSf("Имя: ") + typedName);
                    inputText->setOrigin({0.f, 0.f});
                    inputText->setPosition({40.f, 100.f});
                }
            }

            if (state == ScreenState::Editor && editorField != EditorField::None)
            {
                editorInput += u;
                if (inputText)
                {
                    inputText->setString(editorInput);
                    placeEditorInput();
                }
            }

            if (state == ScreenState::MultiplayerNameInput1 ||
                state == ScreenState::MultiplayerNameInput2)
            {
                multiplayerInput += u;
                if (inputText)
                {
                    if (state == ScreenState::MultiplayerNameInput1)
                        inputText->setString(toSf("Игрок 1: ") + multiplayerInput);
                    else
                        inputText->setString(toSf("Игрок 2: ") + multiplayerInput);
                    inputText->setOrigin({0.f, 0.f});
                    inputText->setPosition({60.f, 100.f});
                }
            }
        }

        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button != sf::Mouse::Button::Left)
                continue;

            sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x),
                                  static_cast<float>(mousePressed->position.y));

            if (state == ScreenState::Menu)
            {
                if (menuButtons[0].contains(mousePos))
                {
                    state = ScreenState::NameInput;
                    typedName.clear();
                    if (inputText)
                    {
                        inputText->setString(toSf("Имя: "));
                        inputText->setOrigin({0.f, 0.f});
                        inputText->setPosition({40.f, 70.f});
                    }
                }
                else if (menuButtons[1].contains(mousePos))
                {
                    startMultiplayer();
                }
                else if (menuButtons[2].contains(mousePos))
                {
                    state = ScreenState::Ranking;
                    loadRankingData();
                }
                else if (menuButtons[3].contains(mousePos))
                {
                    state = ScreenState::PasswordInput;
                    typedInput.clear();
                    if (inputText)
                    {
                        inputText->setString(toSf("Ввод: "));
                        inputText->setOrigin({0.f, 0.f});
                        inputText->setPosition({40.f, 70.f});
                    }
                }
                else if (menuButtons[4].contains(mousePos))
                {
                    window.close();
                }
            }
            else if (state == ScreenState::TopicSelect)
            {
                bool found = false;
                for (std::size_t i = 0; i < topicButtons.size(); ++i)
                {
                    if (topicButtons[i].contains(mousePos))
                    {
                        currentFilters.difficulty = std::nullopt;
                        if (i == 0)
                            currentFilters.topic = std::nullopt;
                        else
                            currentFilters.topic = DataStore::getUniqueTopics(questions)[i - 1];
                        refreshDifficultyButtons();
                        state = ScreenState::DifficultySelect;
                        found = true;
                        break;
                    }
                }
                if (!found && backButton && backButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::DifficultySelect)
            {
                bool found = false;
                std::vector<std::string> sortedDifficulties;
                auto difficulties = DataStore::getUniqueDifficulties(questions);
                if (std::find(difficulties.begin(), difficulties.end(), "easy") !=
                    difficulties.end())
                    sortedDifficulties.push_back("easy");
                if (std::find(difficulties.begin(), difficulties.end(), "normal") !=
                    difficulties.end())
                    sortedDifficulties.push_back("normal");
                if (std::find(difficulties.begin(), difficulties.end(), "hard") !=
                    difficulties.end())
                    sortedDifficulties.push_back("hard");

                if (currentFilters.topic.has_value())
                {
                    std::vector<Question> topicQuestions;
                    for (const auto& q : questions)
                        if (q.topic == currentFilters.topic.value())
                            topicQuestions.push_back(q);
                    auto topicDifficulties = DataStore::getUniqueDifficulties(topicQuestions);
                    sortedDifficulties.clear();
                    if (std::find(topicDifficulties.begin(), topicDifficulties.end(), "easy") !=
                        topicDifficulties.end())
                        sortedDifficulties.push_back("easy");
                    if (std::find(topicDifficulties.begin(), topicDifficulties.end(), "normal") !=
                        topicDifficulties.end())
                        sortedDifficulties.push_back("normal");
                    if (std::find(topicDifficulties.begin(), topicDifficulties.end(), "hard") !=
                        topicDifficulties.end())
                        sortedDifficulties.push_back("hard");
                }

                for (std::size_t i = 0; i < difficultyButtons.size(); ++i)
                {
                    if (difficultyButtons[i].contains(mousePos))
                    {
                        if (i == 0)
                            currentFilters.difficulty = std::nullopt;
                        else if (i - 1 < sortedDifficulties.size())
                            currentFilters.difficulty = sortedDifficulties[i - 1];
                        auto filtered = DataStore::filterQuestions(questions, currentFilters);
                        if (filtered.empty())
                        {
                            infoText->setString(toSf("По этому фильтру нет вопросов."));
                            refreshTexts();
                            found = true;
                            break;
                        }
                        engine.setQuestions(filtered);
                        engine.setDifficulty(currentFilters.difficulty.value_or("any"));
                        engine.reset();
                        state = ScreenState::Quiz;
                        refreshTexts();
                        refreshAnswerButtons();
                        found = true;
                        break;
                    }
                }
                if (!found && backButton && backButton->contains(mousePos))
                {
                    currentFilters.difficulty = std::nullopt;
                    state = ScreenState::TopicSelect;
                }
            }
            else if (state == ScreenState::Ranking)
            {
                if (scrollUpButton && scrollUpButton->contains(mousePos))
                {
                    rankingView.scrollUp();
                    refreshTexts();
                }
                else if (scrollDownButton && scrollDownButton->contains(mousePos))
                {
                    rankingView.scrollDown();
                    refreshTexts();
                }
                else if (!rankingButtons.empty() && rankingButtons[0].contains(mousePos))
                {
                    rankingView.resetScroll();
                    switchToMenu();
                }
            }
            else if (state == ScreenState::PasswordInput)
            {
                if (submitButton && submitButton->contains(mousePos))
                    openEditorIfAllowed();
                else if (backButton && backButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::NameInput)
            {
                if (backButton && backButton->contains(mousePos))
                {
                    switchToMenu();
                }
            }
            else if (state == ScreenState::Editor)
            {
                if (!editor || editor->questions().empty())
                {
                    if (editorBackButton && editorBackButton->contains(mousePos))
                        switchToMenu();
                    continue;
                }

                if (editorButtons.size() >= 1 && editorButtons[0].contains(mousePos))
                {
                    if (editor->currentIndex() > 0)
                    {
                        editor->setCurrentIndex(editor->currentIndex() - 1);
                        editorField = EditorField::None;
                        editorInput.clear();
                        refreshEditorButtons();
                    }
                }
                else if (editorButtons.size() >= 3 && editorButtons[2].contains(mousePos))
                {
                    editor->setCurrentIndex(editor->currentIndex() + 1);
                    editorField = EditorField::None;
                    editorInput.clear();
                    refreshEditorButtons();
                }
                else if (editorButtons.size() >= 4 && editorButtons[3].contains(mousePos))
                {
                    editor->addQuestion();
                    questions = editor->questions();
                    engine.setQuestions(questions);
                    refreshEditorButtons();
                }
                else if (editorButtons.size() >= 5 && editorButtons[4].contains(mousePos))
                {
                    editor->deleteQuestion(editor->currentIndex());
                    questions = editor->questions();
                    engine.setQuestions(questions);
                    refreshEditorButtons();
                }

                struct HitField
                {
                    EditorField field;
                    sf::FloatRect rect;
                };
                const std::vector<HitField> fields = {
                    {EditorField::Question, sf::FloatRect({160.f, 170.f}, {760.f, 40.f})},
                    {EditorField::Answer0, sf::FloatRect({160.f, 230.f}, {600.f, 40.f})},
                    {EditorField::Answer1, sf::FloatRect({160.f, 285.f}, {600.f, 40.f})},
                    {EditorField::Answer2, sf::FloatRect({160.f, 340.f}, {600.f, 40.f})},
                    {EditorField::Answer3, sf::FloatRect({160.f, 395.f}, {600.f, 40.f})},
                    {EditorField::Time, sf::FloatRect({160.f, 460.f}, {120.f, 40.f})},
                    {EditorField::Topic, sf::FloatRect({160.f, 515.f}, {220.f, 40.f})},
                    {EditorField::Difficulty, sf::FloatRect({160.f, 570.f}, {220.f, 40.f})}};

                for (const auto& hf : fields)
                {
                    if (hf.rect.contains(mousePos))
                    {
                        selectEditorField(hf.field);
                        break;
                    }
                }

                for (int i = 0; i < 4; ++i)
                {
                    float y = 230.f + i * 55.f;
                    sf::FloatRect markRect({800.f, y}, {80.f, 40.f});
                    if (markRect.contains(mousePos))
                    {
                        setCorrectAnswer(static_cast<std::size_t>(i));
                        refreshEditorButtons();
                        break;
                    }
                }

                if (editorSaveButton && editorSaveButton->contains(mousePos))
                {
                    if (editor->save("questions.txt"))
                    {
                        infoText->setString(toSf("Вопросы сохранены!"));
                        questions = editor->questions();
                        engine.setQuestions(questions);
                    }
                    else
                    {
                        infoText->setString(toSf("Ошибка сохранения!"));
                    }
                }

                if (editorBackButton && editorBackButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::Quiz)
            {
                if (nextButton && nextButton->contains(mousePos) && !engine.finished())
                {
                    if (engine.nextQuestion())
                        refreshAnswerButtons();
                    else
                    {
                        state = ScreenState::Result;
                        saveCurrentResult();
                    }
                }

                if (restartButton && restartButton->contains(mousePos))
                {
                    resetQuiz();
                    state = ScreenState::Quiz;
                }

                if (!engine.finished())
                {
                    for (std::size_t i = 0; i < answerButtons.size(); ++i)
                    {
                        if (answerButtons[i].contains(mousePos))
                        {
                            engine.answer(i);
                            if (engine.nextQuestion())
                                refreshAnswerButtons();
                            else
                            {
                                state = ScreenState::Result;
                                saveCurrentResult();
                            }
                            break;
                        }
                    }
                }
            }
            else if (state == ScreenState::MultiplayerNameInput1)
            {
                if (backButton && backButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::MultiplayerNameInput2)
            {
                if (backButton && backButton->contains(mousePos))
                {
                    state = ScreenState::MultiplayerNameInput1;
                    multiplayerInput = toSf(playerNames[0]);
                    if (inputText)
                    {
                        inputText->setString(toSf("Игрок 1: ") + multiplayerInput);
                        inputText->setOrigin({0.f, 0.f});
                        inputText->setPosition({60.f, 100.f});
                    }
                }
            }
            else if (state == ScreenState::MultiplayerTopicSelect)
            {
                bool found = false;
                for (std::size_t i = 0; i < topicButtons.size(); ++i)
                {
                    if (topicButtons[i].contains(mousePos))
                    {
                        currentFilters.difficulty = std::nullopt;
                        if (i == 0)
                            currentFilters.topic = std::nullopt;
                        else
                            currentFilters.topic = DataStore::getUniqueTopics(questions)[i - 1];
                        refreshDifficultyButtons();
                        state = ScreenState::MultiplayerDifficultySelect;
                        found = true;
                        break;
                    }
                }
                if (!found && backButton && backButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::MultiplayerDifficultySelect)
            {
                bool found = false;
                std::vector<std::string> sortedDifficulties;
                auto difficulties = DataStore::getUniqueDifficulties(questions);
                if (std::find(difficulties.begin(), difficulties.end(), "easy") !=
                    difficulties.end())
                    sortedDifficulties.push_back("easy");
                if (std::find(difficulties.begin(), difficulties.end(), "normal") !=
                    difficulties.end())
                    sortedDifficulties.push_back("normal");
                if (std::find(difficulties.begin(), difficulties.end(), "hard") !=
                    difficulties.end())
                    sortedDifficulties.push_back("hard");

                if (currentFilters.topic.has_value())
                {
                    std::vector<Question> topicQuestions;
                    for (const auto& q : questions)
                        if (q.topic == currentFilters.topic.value())
                            topicQuestions.push_back(q);
                    auto topicDifficulties = DataStore::getUniqueDifficulties(topicQuestions);
                    sortedDifficulties.clear();
                    if (std::find(topicDifficulties.begin(), topicDifficulties.end(), "easy") !=
                        topicDifficulties.end())
                        sortedDifficulties.push_back("easy");
                    if (std::find(topicDifficulties.begin(), topicDifficulties.end(), "normal") !=
                        topicDifficulties.end())
                        sortedDifficulties.push_back("normal");
                    if (std::find(topicDifficulties.begin(), topicDifficulties.end(), "hard") !=
                        topicDifficulties.end())
                        sortedDifficulties.push_back("hard");
                }

                for (std::size_t i = 0; i < difficultyButtons.size(); ++i)
                {
                    if (difficultyButtons[i].contains(mousePos))
                    {
                        if (i == 0)
                            currentFilters.difficulty = std::nullopt;
                        else if (i - 1 < sortedDifficulties.size())
                            currentFilters.difficulty = sortedDifficulties[i - 1];

                        multiplayerQuestions =
                            DataStore::filterQuestions(questions, currentFilters);
                        if (multiplayerQuestions.empty())
                            multiplayerQuestions = questions;
                        multiplayerQuestionIndex = 0;
                        currentQuizPlayer = 0;
                        playerScores = {0, 0};
                        multiplayerClock.restart();
                        multiplayerTimeLeft = multiplayerQuestions[0].timeLimitSec;
                        state = ScreenState::MultiplayerQuiz;
                        refreshAnswerButtons();
                        refreshTexts();
                        found = true;
                        break;
                    }
                }
                if (!found && backButton && backButton->contains(mousePos))
                {
                    state = ScreenState::MultiplayerTopicSelect;
                }
            }
            else if (state == ScreenState::MultiplayerQuiz)
            {
                if (!multiplayerQuestions.empty())
                {
                    for (std::size_t i = 0; i < answerButtons.size(); ++i)
                    {
                        if (answerButtons[i].contains(mousePos))
                        {
                            const Question& q = multiplayerQuestions[multiplayerQuestionIndex];
                            bool correct = (i < q.answers.size() && i == q.correctIndex);
                            advanceMultiplayerTurn(correct);
                            break;
                        }
                    }
                }
                if (backButton && backButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::MultiplayerResult)
            {
                if (restartButton && restartButton->contains(mousePos))
                    switchToMenu();
                if (backButton && backButton->contains(mousePos))
                    switchToMenu();
            }
            else if (state == ScreenState::Result)
            {
                if (restartButton && restartButton->contains(mousePos))
                {
                    resetQuiz();
                    state = ScreenState::Quiz;
                }
                else if (backButton && backButton->contains(mousePos))
                {
                    switchToMenu();
                }
            }
        }
    }
}

void App::update()
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (auto& b : menuButtons)
        b.setHovered(b.contains(mousePos));
    for (auto& b : topicButtons)
        b.setHovered(b.contains(mousePos));
    for (auto& b : difficultyButtons)
        b.setHovered(b.contains(mousePos));
    for (auto& b : answerButtons)
        b.setHovered(b.contains(mousePos));
    for (auto& b : rankingButtons)
        b.setHovered(b.contains(mousePos));
    for (auto& b : editorButtons)
        b.setHovered(b.contains(mousePos));
    if (nextButton)
        nextButton->setHovered(nextButton->contains(mousePos));
    if (restartButton)
        restartButton->setHovered(restartButton->contains(mousePos));
    if (backButton)
        backButton->setHovered(backButton->contains(mousePos));
    if (submitButton)
        submitButton->setHovered(submitButton->contains(mousePos));
    if (scrollUpButton)
        scrollUpButton->setHovered(scrollUpButton->contains(mousePos));
    if (scrollDownButton)
        scrollDownButton->setHovered(scrollDownButton->contains(mousePos));
    if (editorSaveButton)
        editorSaveButton->setHovered(editorSaveButton->contains(mousePos));
    if (editorBackButton)
        editorBackButton->setHovered(editorBackButton->contains(mousePos));
    if (state == ScreenState::Quiz)
    {
        engine.tick();
        if (engine.timeLeft() == 0 && !engine.finished())
        {
            if (!engine.nextQuestion())
            {
                state = ScreenState::Result;
                saveCurrentResult();
            }
            else
            {
                refreshAnswerButtons();
            }
        }
    }

    if (state == ScreenState::MultiplayerQuiz && !multiplayerQuestions.empty())
    {
        int elapsed = static_cast<int>(multiplayerClock.getElapsedTime().asSeconds());
        int limit = multiplayerQuestions[multiplayerQuestionIndex].timeLimitSec;
        multiplayerTimeLeft = std::max(0, limit - elapsed);

        if (multiplayerTimeLeft == 0)
        {
            advanceMultiplayerTurn(false);
        }
    }

    refreshTexts();
}

void App::render()
{
    window.clear(sf::Color(10, 18, 40));

    if (titleText)
        window.draw(*titleText);

    switch (state)
    {
        case ScreenState::Menu:
            for (auto& b : menuButtons)
                b.draw(window);
            break;
        case ScreenState::NameInput:
            if (infoText)
                window.draw(*infoText);
            if (inputText)
                window.draw(*inputText);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::TopicSelect:
            for (auto& b : topicButtons)
                b.draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::DifficultySelect:
            for (auto& b : difficultyButtons)
                b.draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::Quiz:
            if (questionText)
                window.draw(*questionText);
            if (infoText)
                window.draw(*infoText);
            if (scoreText)
                window.draw(*scoreText);
            if (timerText)
                window.draw(*timerText);
            for (auto& b : answerButtons)
                b.draw(window);
            if (nextButton)
                nextButton->draw(window);
            if (restartButton)
                restartButton->draw(window);
            break;
        case ScreenState::Result:
            if (infoText)
                window.draw(*infoText);
            if (scoreText)
                window.draw(*scoreText);
            if (restartButton)
                restartButton->draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::Ranking:
            if (rankingText)
                window.draw(*rankingText);
            if (!rankingButtons.empty())
                rankingButtons[0].draw(window);
            if (scrollUpButton)
                scrollUpButton->draw(window);
            if (scrollDownButton)
                scrollDownButton->draw(window);
            break;
        case ScreenState::Editor:
            if (infoText)
                window.draw(*infoText);
            for (auto& b : editorButtons)
                b.draw(window);
            if (editorSaveButton)
                editorSaveButton->draw(window);
            if (editorBackButton)
                editorBackButton->draw(window);
            if (editorField != EditorField::None && inputText)
                window.draw(*inputText);
            break;
        case ScreenState::PasswordInput:
            if (infoText)
                window.draw(*infoText);
            if (inputText)
                window.draw(*inputText);
            if (submitButton)
                submitButton->draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::MultiplayerNameInput1:
        case ScreenState::MultiplayerNameInput2:
            if (infoText)
                window.draw(*infoText);
            if (inputText)
                window.draw(*inputText);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::MultiplayerTopicSelect:
            for (auto& b : topicButtons)
                b.draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::MultiplayerDifficultySelect:
            for (auto& b : difficultyButtons)
                b.draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::MultiplayerQuiz:
            if (questionText)
                window.draw(*questionText);
            if (infoText)
                window.draw(*infoText);
            if (timerText)
                window.draw(*timerText);
            for (auto& b : answerButtons)
                b.draw(window);
            if (backButton)
                backButton->draw(window);
            break;
        case ScreenState::MultiplayerResult:
            if (infoText)
                window.draw(*infoText);
            if (scoreText)
                window.draw(*scoreText);
            if (restartButton)
                restartButton->draw(window);
            if (backButton)
                backButton->draw(window);
            break;
    }

    window.display();
}
