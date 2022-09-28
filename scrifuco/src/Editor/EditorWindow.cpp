#include <cmath>
#include <fstream>

#include "Editor/EditorWindow.h"
#include "Editor/EventHandlers.h"

#include "Settings.h"

EditorWindow::EditorWindow(DataManagement &data, sf::String &mode, sf::String &command, Editor& context, bool advanced)
: data_(data), currentMode_(mode), currentCommand_(command), settings_(Settings::GetInstance()), advanced_(advanced), context_(context) {
    // Create window
    sf::ContextSettings settings;
    settings.antialiasingLevel = settings_.Antialiasing;
    window_ = std::make_shared<sf::RenderWindow>(sf::VideoMode(settings_.ScreenWidth, settings_.ScreenHeight), APP_NAME, WINDOW_STYLE, settings);

    window_->setFramerateLimit(settings_.Framerate);

    // Initialize properties
    InitButtons();
    RefreshFont();
    ThickTextCursor();}

void EditorWindow::InitButtons(){
    colors = {settings_.FontColor, FIRST_COLOR, SECOND_COLOR, THIRD_COLOR };

    buttons.clear();

    // Color buttons
    for (int i = 0; i < colors.size(); ++i) {
        buttons.emplace_back(std::make_unique<BrushSizeOrColorButton>(sf::Vector2f((float) (window_->getSize().x) - settings_.buttonSquareSize - settings_.buttonMargin,
                                                                               (float) i * ( settings_.buttonSquareSize +  settings_.buttonSpacing) + settings_.buttonMargin + pixelShift),
                                                                      sf::Vector2f( settings_.buttonSquareSize,  settings_.buttonSquareSize),
                                                                      colors[i],
                                                                      settings_.FontColor,
                                                                      i == 0 ? BUTTON_OUTLINE : 0,
                                                                      data_,
                                                                      colors[i]));
    }

    // Erase button
    sf::Image eraserButtonImage;
    eraserButtonImage.loadFromFile(ERASER_PATH);

    buttons.emplace_back(std::make_unique<EraseButton>(sf::Vector2f((float) (window_->getSize().x) - settings_.buttonSquareSize - settings_.buttonMargin,
                                                                    (float) buttons.size() * ( settings_.buttonSquareSize +  settings_.buttonSpacing) + settings_.buttonMargin + pixelShift),
                                                       eraserButtonImage, data_, settings_.FontColor));

    size_t buttonOffset = buttons.size() + 1;
    size_t brushButtonsCount = 3;

    // Brush size buttons
    for (int i = 0; i < brushButtonsCount; ++i) {
        buttons.emplace_back(std::make_unique<BrushSizeOrColorButton>(sf::Vector2f((float) (window_->getSize().x) - (float) (1 - i) * settings_.buttonSizeDiff - settings_.buttonSquareSize - settings_.buttonMargin ,
                                                                                   (float) buttonOffset++ * ( settings_.buttonSquareSize +  settings_.buttonSpacing) + (i > 0 ?  settings_.buttonSizeDiff : 0) + settings_.buttonMargin + pixelShift),
                                                                      sf::Vector2f((float) (1-i) *  settings_.buttonSizeDiff +  settings_.buttonSquareSize, (float) (1 - i) *  settings_.buttonSizeDiff +  settings_.buttonSquareSize),
                                                                      sf::Color::Transparent,
                                                                      settings_.FontColor,
                                                                      (brushButtonsCount + 1 - i), data_,
                                                                      (brushButtonsCount + 1 - i)));
    }

    buttonOffset++;

    // Move button
    sf::Image moveButtonImage;
    moveButtonImage.loadFromFile(MOVE_PATH);

    buttons.emplace_back(std::make_unique<MoveButton>(sf::Vector2f((float) (window_->getSize().x) - settings_.buttonSquareSize - settings_.buttonMargin,
                                                                   (float) buttonOffset++ * ( settings_.buttonSquareSize +  settings_.buttonSpacing) + settings_.buttonMargin + pixelShift),
                                                      moveButtonImage, data_, settings_.FontColor));

    // Select button
    buttons.emplace_back(std::make_unique<SelectButton>(sf::Vector2f((float) (window_->getSize().x) - settings_.buttonSquareSize - settings_.buttonMargin,
                                                                    (float) buttonOffset++ * ( settings_.buttonSquareSize +  settings_.buttonSpacing) + settings_.buttonMargin + pixelShift),
                                                       sf::Vector2f( settings_.buttonSquareSize,  settings_.buttonSquareSize),
                                                       sf::Color::Transparent,
                                                       Graphics::selectColor,
                                                       SELECT_BUTTON_OUTLINE,
                                                       data_));
}

void EditorWindow::Update() {
    if (data_.graphics.brushChanged || shouldRefreshCursor) {
        SetCurrentToolCursor();
        data_.graphics.brushChanged = false;
        shouldRefreshCursor = false;
    }

    // Empty command buffer after inactivity
    if (!data_.commandBuffer.isEmpty() && !data_.inCommand && data_.commandClock.getElapsedTime() >= data_.showCommandDelay_) {
        data_.commandBuffer = "";
    }

    // Calculate shift due to scrolling
    int scrollThreshold = (int) std::round(((float) window_->getSize().y / (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing)) * settings_.scrollThresholdPercentage);

    shift_ = data_.textCursor.y - scrollThreshold > 0 ? data_.textCursor.y - scrollThreshold : 0;
    pixelShift = (float) shift_ * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing);

    // Draw everything
    window_->clear(settings_.BackgroundColor);
    ResetView();

    DrawSelection();
    DrawTextCursor();
    DrawText();
    DrawGraphics();

    window_->display();
}

bool EditorWindow::IsOpen() {
    return  window_->isOpen();
}

void EditorWindow::DrawText() {
    // Main text
    for (size_t i = 0; i < data_.text.rows.size() && i + shift_ < data_.text.rows.size(); ++i) {
        if (data_.text.rows[i + shift_].isEmpty()) continue;

        SetStringAndDraw(linesBuffer_[i],data_.text.rows[i + shift_],
                         sf::Vector2f(settings_.OffsetY, settings_.OffsetX + (float) i * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing) + pixelShift));
        }

    // Current mode
    if (advanced_) SetStringAndDraw(modeText_, currentMode_,sf::Vector2f(
            (float) window_->getSize().x - modeText_.getLocalBounds().width - CURRENT_MODE_SUB.x,
            (float) window_->getSize().y - modeText_.getLocalBounds().height - CURRENT_MODE_SUB.y + pixelShift));
    else {
        if (!easyModeInitialized_) InitializeEasyMode();
        dualSwitch_->Draw(window_, pixelShift);
        saveButton_->SetText(data_.saved ? SAVE_TEXT : UNSAVED_SAVE_TEXT);
        saveButton_->Draw(window_, pixelShift);
    }

    SetStringAndDraw(commandText_,currentCommand_,sf::Vector2f(COMMAND_OFFSET_X,((float) window_->getSize().y - settings_.pixelFontSizeHeight * SMALLER_FONT_FACTOR - COMMAND_OFFSET_Y + pixelShift)));
}

void EditorWindow::InitializeEasyMode() {
    sf::Text switchTextLeft(GRAPHICS_MODE_TEXT, settings_.Font, SMALLER_FONT_SIZE);
    switchTextLeft.setPosition({(float) settings_.ScreenWidth - CURRENT_MODE_RIGHT_SUB.x, (float) settings_.ScreenHeight - CURRENT_MODE_RIGHT_SUB.y});
    switchTextLeft.setFillColor(SWITCH_COLOR);

    sf::Text switchTextRight(TEXT_MODE_TEXT, settings_.Font, SMALLER_FONT_SIZE);
    switchTextRight.setPosition({(float) settings_.ScreenWidth - CURRENT_MODE_LEFT_SUB.x, (float) settings_.ScreenHeight - CURRENT_MODE_LEFT_SUB.y});
    switchTextRight.setFillColor(SWITCH_COLOR);

    dualSwitch_ = std::make_unique<DualSwitchBind>(
            std::make_unique<ButtonWithText>(switchTextLeft, CURRENT_MODE_BUTTON_TOLERANCE, [&]() {}),
            std::make_unique<ButtonWithText>(switchTextRight, CURRENT_MODE_BUTTON_TOLERANCE, [&]() {}),
            SWITCH_SELECT_COLOR);

    // Initialize save button
    sf::Text saveText(SAVE_TEXT, settings_.Font, SMALLER_FONT_SIZE);
    saveText.setPosition({(float) settings_.ScreenWidth / 2 - saveText.getLocalBounds().width / 2, (float) settings_.ScreenHeight - CURRENT_MODE_LEFT_SUB.y });
    saveButton_ = std::make_unique<ButtonWithText>(saveText, CURRENT_MODE_BUTTON_TOLERANCE, [&]() {data_.file.SaveFile();}, MENU_HOVER_COLOR);

    easyModeInitialized_ = true;
}


void EditorWindow::SetStringAndDraw(sf::Text &text, sf::String &str, sf::Vector2f pos) {
    text.setPosition(pos);
    text.setString(str);
    window_->draw(text);
}

void EditorWindow::DrawGraphics() {
    data_.graphics.DrawAll(*window_);

    if (showBrushSettings) {
        for (auto &button: buttons) {
            button->Draw(window_, pixelShift);
        }
    }
}

void EditorWindow::SetCursor(sf::Cursor& cursor){
    window_->setMouseCursor(cursor);
}

void EditorWindow::SetCurrentToolCursor() {
    sf::Cursor cursor;
    sf::Image cursorImage;

    switch(data_.graphics.currentTool) {
        case Graphics::Brush:
            cursorImage.create((unsigned int) data_.graphics.brushSize * 2, (unsigned int) data_.graphics.brushSize * 2,
                               data_.graphics.brushColor);

            if (cursor.loadFromPixels(cursorImage.getPixelsPtr(),
                                      sf::Vector2u((unsigned int) data_.graphics.brushSize * 2,
                                                   (unsigned int) data_.graphics.brushSize * 2),
                                      sf::Vector2u((unsigned int) data_.graphics.brushSize,
                                                   (unsigned int) data_.graphics.brushSize))) {
                SetCursor(cursor);
            }
            break;

        case Graphics::Selector:
            cursorImage.create(SELECTOR_SIZE, SELECTOR_SIZE, Graphics::selectColor);
            if (cursor.loadFromPixels(cursorImage.getPixelsPtr(), SMALLER_CURSOR_ICON_SIZE, SELECTOR_HOTSPOT)) {
                SetCursor(cursor);
            }
            break;

        case Graphics::Eraser:
        {
            cursorImage.loadFromFile(ERASER_PATH);
            if (cursor.loadFromPixels(cursorImage.getPixelsPtr(),CURSOR_ICON_SIZE,ERASER_HOTSPOT)) {
                SetCursor(cursor);
            }

            break;
        }
        case Graphics::Move:
            cursorImage.loadFromFile(MOVE_PATH);
            if (cursor.loadFromPixels(cursorImage.getPixelsPtr(),CURSOR_ICON_SIZE,ERASER_HOTSPOT)) {
                SetCursor(cursor);
            }
            break;
    }
}

void EditorWindow::RefreshFont() {
    SetFont(modeText_, settings_.Font, SMALLER_FONT_SIZE, settings_.FontColor);
    SetFont(commandText_, settings_.Font, SMALLER_FONT_SIZE, settings_.FontColor);
    InitializeEasyMode();

    for (auto &&row: linesBuffer_) {
        SetFont(row, settings_.Font, settings_.FontSize, settings_.FontColor);
    }
}

void EditorWindow::ResetView() {
    view_ = sf::View(sf::FloatRect(0, pixelShift, (float) window_->getSize().x, (float) window_->getSize().y));
    window_->setView(view_);
}

void EditorWindow::ExportSettings(const std::string &file) {
    settings_.ExportSettings(file);
}

void EditorWindow::Close() {
    window_->close();
}

bool EditorWindow::GetNextEvent(sf::Event &event) {
    bool success = window_->pollEvent(event);
    return success;
}

void EditorWindow::DiscardNextEvent() {
    sf::Event event{};
    window_->pollEvent(event);
}

sf::Vector2<float> EditorWindow::GetMousePos() {
    return window_->mapPixelToCoords(sf::Mouse::getPosition(*window_));
}

sf::Vector2<int> EditorWindow::CalculateTextCursorPos() {
    auto mousePosition = GetMousePos();

    int lineNum = (int) std::round((mousePosition.y - settings_.OffsetX - settings_.CursorOffsetY) / (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing));
    int charNum = (int) std::round((mousePosition.x - settings_.OffsetY) / settings_.pixelFontSizeWidth);

    return {charNum, lineNum};
}

void EditorWindow::DrawTextCursor() {
    // Cursor blink
    if (clock_.getElapsedTime() >= settings_.CursorDelay) {
        showTextCursor_ = !showTextCursor_;
        clock_.restart();
    }

    // Setting cursor to correct position
    if (showTextCursor_) {
        textCursorRectangle_.setPosition(settings_.OffsetY + (float) data_.textCursor.x * settings_.pixelFontSizeWidth,
                                         settings_.OffsetX + settings_.CursorOffsetY +
                                         (float) data_.textCursor.y * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing));

        window_->draw(textCursorRectangle_);
    }
}

// Insert mode cursor
void EditorWindow::ThinTextCursor() {

    textCursorRectangle_.setSize({THIN_CURSOR_WIDTH, settings_.pixelFontSizeHeight});
    textCursorRectangle_.setFillColor(THIN_CURSOR_COLOR);
}

// Command mode cursor
void EditorWindow::ThickTextCursor() {
    float cursorOverlap = settings_.pixelFontSpacing / 2.0f;
    textCursorRectangle_.setSize({settings_.pixelFontSizeWidth, settings_.pixelFontSizeHeight + cursorOverlap});
    textCursorRectangle_.setFillColor(THICK_CURSOR_COLOR);
}

void EditorWindow::SetWindowTitle(const sf::String& title) {
    window_->setTitle(title);
}

bool EditorWindow::CheckEditorEvents(const sf::Event &event) {
    // Deselect
    if (event.type == sf::Event::MouseButtonPressed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape))
        data_.inSelection = false;

    // Refresh mouse cursor
    if (event.type == sf::Event::MouseMoved) {
        if (event.mouseMove.x < CHANGE_MOUSE_CURSOR_TOLERANCE || event.mouseMove.x > window_->getSize().x - CHANGE_MOUSE_CURSOR_TOLERANCE ||
            event.mouseMove.y < CHANGE_MOUSE_CURSOR_TOLERANCE || event.mouseMove.y > window_->getSize().y - CHANGE_MOUSE_CURSOR_TOLERANCE) {
            shouldRefreshCursor = true;
            return false;
        }
    }

    // Button check
    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Button::Left)
        return false;

    if (saveButton_->MouseCheck({event.mouseButton.x, event.mouseButton.y})) {
        saveButton_->OnClick();
        return true;
    }

    if (easyModeInitialized_ && dualSwitch_->MouseCheck({event.mouseButton.x, event.mouseButton.y})) {
        if (dualSwitch_->IsRightSelected()) Editor::TextHandler::SetMode(context_);
        else Editor::InsertHandler::SetMode(context_);
        return true;
    }
    return false;
}

void EditorWindow::SetAdvanced(bool advanced) {
    advanced_ = advanced;
}

void EditorWindow::DrawSelection() {
    if (data_.inSelection) {
        if (data_.selectionStart.y == data_.selectionEnd.y) {
            sf::RectangleShape selectionRectangle;
            selectionRectangle.setSize({settings_.pixelFontSizeWidth * (float) (data_.selectionEnd.x - data_.selectionStart.x),settings_.pixelFontSizeHeight + settings_.pixelFontSpacing});
            selectionRectangle.setFillColor(SELECTION_COLOR);
            selectionRectangle.setPosition(OFFSET_X + (float) data_.selectionStart.x * settings_.pixelFontSizeWidth,OFFSET_Y + (float) data_.selectionStart.y * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing));
            window_->draw(selectionRectangle);
        }
        else {
            sf::RectangleShape selectionRectangle;
            selectionRectangle.setSize({settings_.pixelFontSizeWidth * (float) (context_.data_->text.rows[data_.selectionStart.y].getSize() - data_.selectionStart.x),settings_.pixelFontSizeHeight + settings_.pixelFontSpacing});
            selectionRectangle.setFillColor(SELECTION_COLOR);
            selectionRectangle.setPosition(OFFSET_X + (float) data_.selectionStart.x * settings_.pixelFontSizeWidth,OFFSET_Y + (float) data_.selectionStart.y * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing));

            window_->draw(selectionRectangle);

            int inBetweenLines = data_.selectionEnd.y - data_.selectionStart.y - 1;

            float charCount;

            for(int i = 0; i < inBetweenLines; i++) {
                charCount = (float) context_.data_->text.rows[data_.selectionStart.y + i + 1].getSize();
                charCount = charCount > 0 ? charCount : 1;
                selectionRectangle.setSize({settings_.pixelFontSizeWidth * charCount,settings_.pixelFontSizeHeight + settings_.pixelFontSpacing});
                selectionRectangle.setPosition(OFFSET_X,OFFSET_Y + (float) (data_.selectionStart.y + i + 1) * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing));
                selectionRectangle.setFillColor(SELECTION_COLOR);
                window_->draw(selectionRectangle);
            }

            charCount = (float) (data_.selectionEnd.x == 0? 1 : data_.selectionEnd.x);
            selectionRectangle.setSize({settings_.pixelFontSizeWidth * charCount, settings_.pixelFontSizeHeight + settings_.pixelFontSpacing});
            selectionRectangle.setFillColor(SELECTION_COLOR);
            selectionRectangle.setPosition(OFFSET_X, OFFSET_Y + (float) data_.selectionEnd.y * (settings_.pixelFontSizeHeight + settings_.pixelFontSpacing));

            window_->draw(selectionRectangle);
        }
    }
}
