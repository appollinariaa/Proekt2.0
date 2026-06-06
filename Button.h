#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Нажимаемая кнопка SFML с подсветкой при наведении и центрированным текстом.
 */
class Button
{
  public:
    /**
     * @brief Удаленный конструктор по умолчанию, потому что кнопке нужны графические ресурсы.
     */
    Button() = delete;

    /**
     * @brief Создает кнопку с подписью, позицией и размером.
     * @param font Шрифт, используемый для подписи кнопки.
     * @param label Текст, отображаемый на кнопке.
     * @param position Позиция левого верхнего угла кнопки в координатах окна.
     * @param size Размер кнопки в пикселях.
     */
    Button(const sf::Font& font, const sf::String& label, sf::Vector2f position, sf::Vector2f size);

    /**
     * @brief Изменяет подпись кнопки и заново центрирует ее.
     * @param label Новый текст, отображаемый на кнопке.
     */
    void setLabel(const sf::String& label);

    /**
     * @brief Устанавливает, находится ли кнопка в состоянии наведения.
     * @param hovered true для цвета наведения; false для обычного цвета.
     */
    void setHovered(bool hovered);

    /**
     * @brief Проверяет, находится ли точка внутри прямоугольника кнопки.
     * @param point Точка в той же системе координат, что и кнопка.
     * @return true, если точка находится внутри границ кнопки; иначе false.
     */
    bool contains(sf::Vector2f point) const;

    /**
     * @brief Рисует форму кнопки и ее текст.
     * @param window Окно отрисовки, используемое для вывода.
     */
    void draw(sf::RenderWindow& window) const;

  private:
    /** @brief Прямоугольный фон и область нажатия. */
    sf::RectangleShape shape_;

    /** @brief Центрированная подпись кнопки. */
    sf::Text text_;

    /** @brief Нужно ли использовать цвет наведения при отрисовке. */
    bool hovered_ = false;
};
