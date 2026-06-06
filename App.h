#pragma once

#include "Button.h"
#include "Data.h"
#include "QuestionEditor.h"
#include "QuizEngine.h"
#include "RankingView.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Определяет экран, который сейчас отображается приложением.
 */
enum class ScreenState
{
    /** @brief Экран главного меню. */
    Menu,
    /** @brief Экран ввода имени для одиночной игры. */
    NameInput,
    /** @brief Экран выбора темы для одиночной игры. */
    TopicSelect,
    /** @brief Экран выбора сложности для одиночной игры. */
    DifficultySelect,
    /** @brief Экран одиночной викторины. */
    Quiz,
    /** @brief Экран результата одиночной игры. */
    Result,
    /** @brief Экран таблицы рейтинга. */
    Ranking,
    /** @brief Экран ввода пароля перед открытием редактора. */
    PasswordInput,
    /** @brief Экран редактора вопросов. */
    Editor,
    /** @brief Экран ввода имени первого игрока в многопользовательском режиме. */
    MultiplayerNameInput1,
    /** @brief Экран ввода имени второго игрока в многопользовательском режиме. */
    MultiplayerNameInput2,
    /** @brief Экран выбора темы для многопользовательского режима. */
    MultiplayerTopicSelect,
    /** @brief Экран выбора сложности для многопользовательского режима. */
    MultiplayerDifficultySelect,
    /** @brief Экран многопользовательской викторины. */
    MultiplayerQuiz,
    /** @brief Экран результата многопользовательской игры. */
    MultiplayerResult
};

/**
 * @brief Определяет поле, выбранное для редактирования в редакторе вопросов.
 */
enum class EditorField
{
    /** @brief Поле редактора не выбрано. */
    None,
    /** @brief Поле текста вопроса. */
    Question,
    /** @brief Поле первого ответа. */
    Answer0,
    /** @brief Поле второго ответа. */
    Answer1,
    /** @brief Поле третьего ответа. */
    Answer2,
    /** @brief Поле четвертого ответа. */
    Answer3,
    /** @brief Поле ограничения времени. */
    Time,
    /** @brief Поле темы вопроса. */
    Topic,
    /** @brief Поле сложности вопроса. */
    Difficulty
};

/**
 * @brief Главное SFML-приложение, связывающее интерфейс, логику викторины, редактор и рейтинг.
 */
class App
{
  public:
    /**
     * @brief Создает окно, инициализирует ресурсы, элементы интерфейса и данные.
     */
    App();

    /**
     * @brief Запускает главный цикл приложения до закрытия окна.
     */
    void run();

  private:
    /** @brief Главное окно отрисовки, используемое для ввода и вывода. */
    sf::RenderWindow window;

    /** @brief Шрифт, используемый всеми текстами и кнопками на экране. */
    sf::Font font;

    /** @brief Движок одиночной викторины. */
    QuizEngine engine;

    /** @brief Необязательный экземпляр редактора вопросов, защищенного паролем. */
    std::optional<QuestionEditor> editor;

    /** @brief Объект форматирования рейтинга и хранения состояния прокрутки. */
    RankingView rankingView;

    /** @brief Текущее состояние экрана. */
    ScreenState state{ScreenState::Menu};

    /** @brief Текущие фильтры по теме и сложности, выбранные пользователем. */
    QuestionFilters currentFilters;

    /** @brief Полная коллекция вопросов, загруженная из файла или созданная по умолчанию. */
    std::vector<Question> questions;

    /** @brief Отфильтрованная коллекция вопросов для многопользовательского режима. */
    std::vector<Question> multiplayerQuestions;

    /** @brief Кнопки, отображаемые в главном меню. */
    std::vector<Button> menuButtons;

    /** @brief Кнопки, отображаемые при выборе темы. */
    std::vector<Button> topicButtons;

    /** @brief Кнопки, отображаемые при выборе сложности. */
    std::vector<Button> difficultyButtons;

    /** @brief Кнопки ответов, отображаемые на экранах викторины. */
    std::vector<Button> answerButtons;

    /** @brief Кнопки, отображаемые на экране рейтинга. */
    std::vector<Button> rankingButtons;

    /** @brief Кнопки полей и навигации, отображаемые в редакторе. */
    std::vector<Button> editorButtons;

    /** @brief Необязательная кнопка перехода к следующему вопросу. */
    std::optional<Button> nextButton;

    /** @brief Необязательная кнопка перезапуска викторины или выхода с экрана результатов дуэли. */
    std::optional<Button> restartButton;

    /** @brief Необязательная кнопка возврата на предыдущий экран или в меню. */
    std::optional<Button> backButton;

    /** @brief Необязательная кнопка отправки введенного пароля. */
    std::optional<Button> submitButton;

    /** @brief Необязательная кнопка прокрутки рейтинга вверх. */
    std::optional<Button> scrollUpButton;

    /** @brief Необязательная кнопка прокрутки рейтинга вниз. */
    std::optional<Button> scrollDownButton;

    /** @brief Необязательная кнопка сохранения всех отредактированных вопросов. */
    std::optional<Button> editorSaveButton;

    /** @brief Необязательная кнопка выхода из редактора. */
    std::optional<Button> editorBackButton;

    /** @brief Крупный заголовок, отображаемый на большинстве экранов. */
    std::optional<sf::Text> titleText;

    /** @brief Текст текущего вопроса, рисуемый на экранах викторины. */
    std::optional<sf::Text> questionText;

    /** @brief Контекстная подсказка или текст состояния. */
    std::optional<sf::Text> infoText;

    /** @brief Текст счета для экранов викторины и результата. */
    std::optional<sf::Text> scoreText;

    /** @brief Текст таймера или информации о ходе игрока. */
    std::optional<sf::Text> timerText;

    /** @brief Форматированный текст рейтинга. */
    std::optional<sf::Text> rankingText;

    /** @brief Текст, используемый для полей ввода. */
    std::optional<sf::Text> inputText;

    /** @brief Буфер ввода пароля. */
    sf::String typedInput;

    /** @brief Буфер ввода имени игрока для одиночной игры. */
    sf::String typedName;

    /** @brief Буфер ввода редактора для выбранного поля. */
    sf::String editorInput;

    /** @brief Буфер ввода имени игрока для многопользовательского режима. */
    sf::String multiplayerInput;

    /** @brief Имя текущего игрока одиночной игры, используемое при сохранении результата. */
    std::string currentPlayerName;

    /** @brief Пароль, необходимый для открытия редактора вопросов. */
    std::string password{"1234"};

    /** @brief Текущее выбранное поле редактора. */
    EditorField editorField{EditorField::None};

    /** @brief Имена двух участников многопользовательского режима. */
    std::vector<std::string> playerNames{"Игрок 1", "Игрок 2"};

    /** @brief Счета двух участников многопользовательского режима. */
    std::array<int, 2> playerScores{0, 0};

    /** @brief Индекс игрока, чей ход активен в многопользовательском режиме. */
    std::size_t currentQuizPlayer{0};

    /** @brief Индекс текущего вопроса в многопользовательском режиме. */
    std::size_t multiplayerQuestionIndex{0};

    /** @brief Признак того, что приложение работает в многопользовательском режиме. */
    bool multiplayerMode{false};

    /** @brief Часы для измерения длительности текущего хода в многопользовательском режиме. */
    sf::Clock multiplayerClock;

    /** @brief Оставшиеся секунды для текущего хода в многопользовательском режиме. */
    int multiplayerTimeLeft{0};

    /** @brief Записи рейтинга, загруженные из файла результатов. */
    std::vector<PlayerResult> ranking;

    /**
     * @brief Загружает шрифты и инициализирует текстовые объекты SFML.
     */
    void loadResources();

    /**
     * @brief Создает кнопки, загружает вопросы и обновляет видимое состояние интерфейса.
     */
    void setupUI();

    /**
     * @brief Обрабатывает события окна, клавиатуры, текстового ввода и мыши.
     */
    void processEvents();

    /**
     * @brief Обновляет состояния наведения, таймеры, прогресс викторины и видимые тексты.
     */
    void update();

    /**
     * @brief Отрисовывает текущий экран.
     */
    void render();

    /**
     * @brief Обновляет значения текстовых объектов SFML в соответствии с текущим экраном.
     */
    void refreshTexts();

    /**
     * @brief Обновляет подписи кнопок ответов для активного вопроса викторины.
     */
    void refreshAnswerButtons();

    /**
     * @brief Пересоздает кнопки выбора темы на основе текущего списка вопросов.
     */
    void refreshTopicButtons();

    /**
     * @brief Пересоздает кнопки выбора сложности на основе текущих фильтров.
     */
    void refreshDifficultyButtons();

    /**
     * @brief Пересоздает кнопки редактора вопросов на основе текущего состояния редактора.
     */
    void refreshEditorButtons();

    /**
     * @brief Сбрасывает активную одиночную викторину с учетом текущих фильтров.
     */
    void resetQuiz();

    /**
     * @brief Сохраняет текущий результат одиночной игры и перезагружает данные рейтинга.
     */
    void saveCurrentResult();

    /**
     * @brief Загружает записи рейтинга из файла результатов в rankingView.
     */
    void loadRankingData();

    /**
     * @brief Возвращает приложение в главное меню и очищает временное состояние ввода.
     */
    void switchToMenu();

    /**
     * @brief Открывает редактор, если введенный пароль корректен.
     */
    void openEditorIfAllowed();

    /**
     * @brief Запускает сценарий подготовки многопользовательской игры и сбрасывает ее состояние.
     */
    void startMultiplayer();

    /**
     * @brief Применяет результат ответа в многопользовательском режиме и переключает игрока или
     * вопрос.
     * @param answeredCorrectly true, если текущий игрок ответил правильно; иначе false.
     */
    void advanceMultiplayerTurn(bool answeredCorrectly);

    /**
     * @brief Преобразует UTF-8 std::string в sf::String.
     * @param s Исходная строка в кодировке UTF-8.
     * @return Строка SFML с тем же текстом.
     */
    sf::String toSf(const std::string& s) const;

    /**
     * @brief Преобразует nullable UTF-8 C-строку в sf::String.
     * @param s Исходная строка с завершающим нулем; nullptr считается пустой строкой.
     * @return Строка SFML с тем же текстом или пустая строка.
     */
    sf::String toSfUtf8(const char* s) const;

    /**
     * @brief Считывает значение поля из текущего вопроса редактора.
     * @param field Идентификатор поля, которое нужно прочитать.
     * @return Значение поля как UTF-8 std::string или пустая строка, если значение недоступно.
     */
    std::string getEditorFieldValue(EditorField field) const;

    /**
     * @brief Записывает значение в поле текущего вопроса редактора.
     * @param field Идентификатор поля, которое нужно изменить.
     * @param value Новое значение поля.
     */
    void setEditorFieldValue(EditorField field, const std::string& value);

    /**
     * @brief Отмечает один ответ как правильный в текущем вопросе редактора.
     * @param index Индекс ответа, который нужно отметить правильным, начиная с нуля.
     */
    void setCorrectAnswer(std::size_t index);

    /**
     * @brief Выбирает поле редактора и копирует его текущее значение в буфер ввода.
     * @param field Идентификатор поля, которое нужно выбрать.
     */
    void selectEditorField(EditorField field);

    /**
     * @brief Применяет буфер ввода редактора к выбранному полю.
     */
    void applyEditorInput();

    /**
     * @brief Размещает текст ввода редактора на экране.
     */
    void placeEditorInput();
};
