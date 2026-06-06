#pragma once
#include <string>
#include <vector>

/**
 * @brief Хранит данные участника многопользовательской игры.
 */
struct Player
{
    /** @brief Отображаемое имя игрока. */
    std::string name;

    /** @brief Накопленный счет игрока. */
    int totalScore = 0;
};

/**
 * @brief Управляет упорядоченным списком игроков и указателем на текущего игрока.
 */
class PlayerManager
{
  public:
    /**
     * @brief Добавляет нового игрока с нулевым счетом.
     * @param name Имя, назначаемое новому игроку.
     */
    void addPlayer(const std::string& name);

    /**
     * @brief Удаляет всех игроков и сбрасывает указатель текущего игрока.
     */
    void clear();

    /**
     * @brief Возвращает всех управляемых игроков.
     * @return Константная ссылка на внутренний список игроков.
     */
    const std::vector<Player>& players() const;

    /**
     * @brief Возвращает текущего активного игрока.
     * @return Ссылка на текущего игрока.
     * @throws std::out_of_range Если список игроков пуст.
     */
    Player& current();

    /**
     * @brief Перемещает указатель текущего игрока вперед.
     * @return true, если указатель перешел к следующему игроку; false, если он уже был в конце.
     */
    bool next();

    /**
     * @brief Проверяет, что список игроков пуст.
     * @return true, если список игроков пуст; иначе false.
     */
    bool empty() const;

  private:
    /** @brief Упорядоченный список игроков. */
    std::vector<Player> players_;

    /** @brief Индекс активного игрока, начиная с нуля. */
    std::size_t currentIndex_ = 0;
};
