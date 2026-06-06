#include "Button.h"

Button::Button(const sf::Font& font, const sf::String& label, sf::Vector2f position,
               sf::Vector2f size)
    : text_(font, label, 26)
{
    shape_.setPosition(position);
    shape_.setSize(size);
    shape_.setFillColor(sf::Color(24, 36, 68));
    shape_.setOutlineThickness(2.f);
    shape_.setOutlineColor(sf::Color::White);

    text_.setFillColor(sf::Color::White);

    auto bounds = text_.getLocalBounds();
    text_.setOrigin(
        {bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    text_.setPosition({position.x + size.x / 2.f, position.y + size.y / 2.f});
}

void Button::setLabel(const sf::String& label)
{
    text_.setString(label);

    auto bounds = text_.getLocalBounds();
    auto pos = shape_.getPosition();
    auto size = shape_.getSize();

    text_.setOrigin(
        {bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    text_.setPosition({pos.x + size.x / 2.f, pos.y + size.y / 2.f});
}

void Button::setHovered(bool hovered)
{
    hovered_ = hovered;
}

bool Button::contains(sf::Vector2f point) const
{
    return shape_.getGlobalBounds().contains(point);
}

void Button::draw(sf::RenderWindow& window) const
{
    auto color = hovered_ ? sf::Color(45, 78, 140) : sf::Color(24, 36, 68);
    const_cast<sf::RectangleShape&>(shape_).setFillColor(color);
    window.draw(shape_);
    window.draw(text_);
}