#include "SelbaWard/Spline.hpp"

#include "DisplayElement.h"
#include "Editor/DataManagement.h"
#include "Constants.h"

void ShortText::Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) {
    text_.setPosition(position_.x, position_.y + pixelShift);
    window->draw(text_);
}

ShortText::ShortText(const sf::String &content, const sf::Font &font, unsigned int size, sf::Vector2f position,
                     sf::Color color) : position_(position) {
    text_.setString(content);
    text_.setFont(font);
    text_.setCharacterSize(size);
    text_.setFillColor(color);
}

ButtonWithText::ButtonWithText(sf::Text text, float tolerance, std::function<void()> onClick, sf::Color hoverColor) :
        Button({static_cast<int>(text.getPosition().x - tolerance / 2), static_cast<int>(text.getPosition().y)},
               {static_cast<int>(text.getLocalBounds().width + tolerance),
                static_cast<int>(text.getLocalBounds().height + tolerance)},
               std::move(onClick)), text_(std::move(text)) {

    defaultColor_ = text.getFillColor();
    if (hoverColor == sf::Color::Transparent) hoverColor_ = defaultColor_;
    else hoverColor_ = hoverColor;

    position_ = {text_.getPosition()};
}

bool Button::MouseCheck(sf::Vector2i mousePos) const {
    return (mousePos.x >= x && mousePos.x <= x + width && mousePos.y >= y && mousePos.y <= y + height);
}

void ButtonWithText::Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) {
    if (!IsMouseOver) text_.setFillColor(defaultColor_);
    else text_.setFillColor(hoverColor_);

    text_.setPosition(position_.x, position_.y + pixelShift);
    window->draw(text_);
}

sf::Text& ButtonWithText::GetText() {
    return text_;
}

void ButtonWithText::SetText(const std::string& text) {
    text_.setString(text);
}

void ButtonWithText::SetColor(sf::Color color) {
    defaultColor_ = color;
}

DualSwitchBind::DualSwitchBind(std::unique_ptr<ButtonWithText> left, std::unique_ptr<ButtonWithText> right,
                               sf::Color SelectedColor) : selectedColor_(SelectedColor), left_(std::move(left)), right_(std::move(right)) {

    defaultColor_ = left_->GetText().getFillColor();

    left_->OnClick = [this]() { currentState_ = State::Left;};
    right_->OnClick = [this]() {currentState_ = State::Right;};
}

void DualSwitchBind::Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) {

    if (currentState_ == State::Left) {
        left_->SetColor(selectedColor_);
        right_->SetColor(defaultColor_);
    } else {
        left_->SetColor(defaultColor_);
        right_->SetColor(selectedColor_);
    }

    left_->Draw(window, pixelShift);
    right_->Draw(window, pixelShift);
}

bool DualSwitchBind::MouseCheck(sf::Vector2i mousePos) {
    if (left_->MouseCheck(mousePos)) { currentState_ = State::Left; return true; }
    else if (right_->MouseCheck(mousePos)) { currentState_ = State::Right; return true; }

    return false;
}

bool DualSwitchBind::IsRightSelected() {
    return currentState_ == State::Right;
}

EditorButton::EditorButton(DataManagement &dataManagement, sf::Vector2f positions, sf::Vector2f size) : dataManagement(dataManagement), Button(static_cast<sf::Vector2i>(positions), static_cast<sf::Vector2i>(size), [](){}) {
    x = (int)positions.x;
    y = (int)positions.y;

    width = (int)size.x;
    height = (int)size.y;
}

RectangleButton::RectangleButton(sf::Vector2f positions, sf::Vector2f size, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, DataManagement &dataManagement) : EditorButton(dataManagement, positions, size) {
    this->fillColor = fillColor;
    this->outlineColor = outlineColor;
    this->outline = outlineThickness;
}

SpriteButton::SpriteButton(sf::Vector2f positions, const sf::Image& image, DataManagement &dataManagement) : EditorButton(dataManagement, positions, sf::Vector2f( (float) image.getSize().x, (float)image.getSize().y)) {
    texture.loadFromImage(image);
    sprite.setTexture(texture);
}

SpriteButton::SpriteButton(sf::Vector2f positions, const sf::Image &image, DataManagement &dataManagement,sf::Color color) : SpriteButton(positions, image, dataManagement) {
    sprite.setColor(color);
}

void SpriteButton::Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) {
    sprite.setPosition((float) x,(float) y + pixelShift);
    window->draw(sprite);
}

void RectangleButton::Draw(std::shared_ptr<sf::RenderWindow> window, float pixelShift) {
    sf::RectangleShape square;

    square.setPosition((float) x, (float) y + pixelShift);

    square.setSize(sf::Vector2f((float) width, (float) height));
    square.setFillColor(fillColor);
    square.setOutlineColor(outlineColor);
    square.setOutlineThickness(outline);

    window->draw(square);
}

BrushSizeOrColorButton::BrushSizeOrColorButton(sf::Vector2f positions, sf::Vector2f size, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, DataManagement &dataManagement, float brushSize): RectangleButton(positions, size, fillColor, outlineColor, outlineThickness, dataManagement) {
    this->brushSize = brushSize;
    this->brushColor = sf::Color::Transparent;
}

BrushSizeOrColorButton::BrushSizeOrColorButton(sf::Vector2f positions, sf::Vector2f size, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, DataManagement &dataManagement, sf::Color brushColor): RectangleButton(positions, size, fillColor, outlineColor, outlineThickness, dataManagement) {
    this->brushColor = brushColor;
    this->brushSize = 0;
}

void BrushSizeOrColorButton::OnClick() {
    dataManagement.graphics.UnselectAll();
    dataManagement.graphics.currentTool = Graphics::Tool::Brush;

    if (brushColor != sf::Color::Transparent) dataManagement.graphics.brushColor = brushColor;
    else dataManagement.graphics.brushSize = brushSize;

    dataManagement.graphics.brushChanged = true;
}

void EraseButton::OnClick() {
    dataManagement.graphics.UnselectAll();
    dataManagement.graphics.currentTool = Graphics::Tool::Eraser;

    dataManagement.graphics.brushChanged = true;
}

void SelectButton::OnClick() {
    dataManagement.graphics.UnselectAll();
    dataManagement.graphics.currentTool = Graphics::Tool::Selector;

    dataManagement.graphics.brushChanged = true;
}

void MoveButton::OnClick() {
    dataManagement.graphics.currentTool = Graphics::Tool::Move;

    dataManagement.graphics.brushChanged = true;
}