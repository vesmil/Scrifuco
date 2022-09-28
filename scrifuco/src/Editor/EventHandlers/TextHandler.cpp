#include "Editor/EventHandlers.h"
#include "Editor/Editor.h"

Editor::ModeHandler *Editor::TextHandler::instance_ = nullptr;

Editor::ModeHandler *Editor::TextHandler::GetInstance(Editor &editor) {
    if (instance_ == nullptr || &(*(instance_->context)) != &(editor)) {
        instance_ = new TextHandler(editor);
    }
    return instance_;
}

[[maybe_unused]] void Editor::TextHandler::SetMode(Editor &editor) {
    editor.currentMode_ = GetInstance(editor);
    editor.currentModeString_ = GetName();
    editor.display_->showBrushSettings = false;
    editor.display_->ThinTextCursor();

    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Text)) {
        editor.display_->SetCursor(cursor);
    }
}

void Editor::TextHandler::ProcessEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed)
        context->data_->textCursor.Set(context->display_->CalculateTextCursorPos());

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2i cursorPosition;

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedPreviously) {
            cursorPosition = getCorrectPosition();

            selectionStartPosition_  = cursorPosition;
            mousePressedPreviously = true;
        }
        else if(event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            cursorPosition = getCorrectPosition();
            context->data_->Select(selectionStartPosition_, cursorPosition);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) mousePressedPreviously = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)){
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::C) {
            sf::Clipboard::setString(context->data_->GetSelectedText());
        }
    }
    context->InsertTextEvent(event);
}

sf::Vector2i Editor::TextHandler::getCorrectPosition() {
    sf::Vector2i cursorPosition = context->display_->CalculateTextCursorPos();

    if (0 > cursorPosition.y) { cursorPosition.y = 0; cursorPosition.x = 0; }
    if (0 > cursorPosition.x) { cursorPosition.x = 0; }
    if (context->data_->text.rows.size() <= cursorPosition.y) cursorPosition.y = (int) context->data_->text.rows.size() - 1;
    if (context->data_->text.rows[cursorPosition.y].getSize() <= cursorPosition.x) cursorPosition.x = (int) context->data_->text.rows[cursorPosition.y].getSize();

    if (cursorPosition.x == -1) {
        if (cursorPosition.y == 0) cursorPosition.x = 0;
        else {
            cursorPosition.x = (int) context->data_->text.rows[cursorPosition.y - 1].getSize() - 1;
            cursorPosition.y--;
        }
    }

    return cursorPosition;
}
