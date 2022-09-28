#ifndef SCRIFUCO_DISPLAYELEMENT_H
#define SCRIFUCO_DISPLAYELEMENT_H

#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

struct DisplayElement {
    virtual void Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) = 0;
};

struct ShortText : public DisplayElement {
    ShortText(const sf::String &content, const sf::Font &font, unsigned int size, sf::Vector2f position,
              sf::Color color);

    void Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) override;

private:
    sf::Text text_;
    sf::Vector2f position_;
};

struct Button : DisplayElement {
    [[nodiscard]] bool MouseCheck(sf::Vector2i mousePos) const;
    std::function<void()> OnClick;

    int x;
    int y;

    int width;
    int height;

    bool IsMouseOver = false;

protected:
    Button(sf::Vector2i positions, sf::Vector2i size, std::function<void()> onClick)
            : x(positions.x), y(positions.y), width(size.x), height(size.y), OnClick(std::move(onClick)) {}
};

struct ButtonWithText : public Button {
    ButtonWithText(sf::Text text, float tolerance, std::function<void()> onClick, sf::Color hoverColor = sf::Color::Transparent);
    void Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) override;

    sf::Text& GetText();
    void SetText(const std::string& text);
    void SetColor(sf::Color color);

private:
    sf::Text text_;
    sf::Vector2f position_;

    sf::Color defaultColor_;
    sf::Color hoverColor_;
};

struct DualSwitchBind : public DisplayElement {
    explicit DualSwitchBind(std::unique_ptr<ButtonWithText> left, std::unique_ptr<ButtonWithText> right,sf::Color SelectedColor);
    void Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) override;
    bool MouseCheck(sf::Vector2i mousePos);

    bool IsRightSelected();

private:
    enum class State { Left, Right } currentState_ = State::Left;

    std::unique_ptr<ButtonWithText> left_;
    std::unique_ptr<ButtonWithText> right_;

    sf::Color defaultColor_;
    sf::Color selectedColor_;
};

class DataManagement;

struct EditorButton : Button {
    virtual void OnClick() = 0;

protected:
    DataManagement &dataManagement;
    EditorButton(DataManagement &dataManagement, sf::Vector2f positions, sf::Vector2f size);
};

struct SpriteButton : public EditorButton {
    SpriteButton(sf::Vector2f positions, const sf::Image& image, DataManagement &dataManagement);
    SpriteButton(sf::Vector2f positions, const sf::Image& image, DataManagement &dataManagement, sf::Color color);

    void Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) override;

protected:
    sf::Color color;
    sf::Texture texture;
    sf::Sprite sprite;
};

struct RectangleButton : public EditorButton {
    RectangleButton(sf::Vector2f positions, sf::Vector2f size, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, DataManagement &dataManagement);
    void Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) override;

    sf::Color fillColor;
    sf::Color outlineColor;

    float outline;
};

struct BrushSizeOrColorButton : public RectangleButton {
    BrushSizeOrColorButton(sf::Vector2f positions, sf::Vector2f size, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, DataManagement &dataManagement, float brushSize);
    BrushSizeOrColorButton(sf::Vector2f positions, sf::Vector2f size, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, DataManagement &dataManagement, sf::Color brushColor);

    void OnClick() override;

private:
    float brushSize;
    sf::Color brushColor;
};

struct EraseButton : public SpriteButton {
    using SpriteButton::SpriteButton;
    void OnClick() override;
};

struct SelectButton : public RectangleButton {
    using RectangleButton::RectangleButton;
    void OnClick() override;
};

struct MoveButton : public SpriteButton {
    using SpriteButton::SpriteButton;
    void OnClick() override;
};

#endif //SCRIFUCO_DISPLAYELEMENT_H
