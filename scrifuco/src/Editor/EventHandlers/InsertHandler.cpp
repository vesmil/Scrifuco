#include "Editor/EventHandlers.h"
#include "Editor/Editor.h"

Editor::ModeHandler *Editor::InsertHandler::instance_ = nullptr;

Editor::ModeHandler *Editor::InsertHandler::GetInstance(Editor &editor) {
    if (instance_ == nullptr || &(*(instance_->context)) != &(editor)) {
        instance_ = new InsertHandler(editor);
    }
    return instance_;
}

[[maybe_unused]] void Editor::InsertHandler::SetMode(Editor &editor) {
    editor.currentMode_ = GetInstance(editor);
    editor.currentModeString_ = GetName();
    editor.display_->showBrushSettings = true;
    editor.display_->ThinTextCursor();

    editor.display_->SetCurrentToolCursor();
}

void Editor::InsertHandler::ProcessEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::MouseMoved) {
        switch (context->data_->graphics.currentTool) {
            case Graphics::Brush:
                BrushMouseEvent(event);
                break;
            case Graphics::Selector:
                SelectorMouseEvent(event);
                break;
            case Graphics::Eraser:
                EraserMouseEvent(event);
                break;
            case Graphics::Move:
                MoveMouseEvent(event);
                break;
        }
    }

    if ((context->data_->graphics.currentTool == Graphics::Move || context->data_->graphics.currentTool == Graphics::Selector) &&
        event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Delete) {
        context->data_->graphics.DeleteSelected();
        context->data_->saved = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Z) {
            context->data_->graphics.RemoveLast();
            context->data_->saved = false;
        }
    }

    // Keyboard events
    context->InsertTextEvent(event);
}

void Editor::InsertHandler::BrushMouseEvent(const sf::Event &event) {
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedPreviously) {
        context->data_->graphics.StartCurve();
        context->data_->graphics.AddPoint({context->display_->GetMousePos()});
        mousePressedPreviously = true;

    } else if(event.type == sf::Event::MouseButtonReleased) {
        mousePressedPreviously = false;

    } else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left) && mousePressedPreviously) {
        context->data_->graphics.AddPoint({context->display_->GetMousePos()});
        context->data_->saved = false;

    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !mousePressedPreviously) {
        context->data_->saved = false;
        context->data_->graphics.Erase({context->display_->GetMousePos()});
    }
}

void Editor::InsertHandler::SelectorMouseEvent(const sf::Event &event) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        context->data_->graphics.SetSelected({context->display_->GetMousePos()}, true);
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        context->data_->graphics.SetSelected({context->display_->GetMousePos()}, false);
    }
}

void Editor::InsertHandler::EraserMouseEvent(const sf::Event &event) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        context->data_->saved = false;
        context->data_->graphics.Erase({context->display_->GetMousePos()});
    }
}

void Editor::InsertHandler::MoveMouseEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        mousePressedPreviously = true;
        startPosition_ = context->display_->GetMousePos();
        if (context->data_->graphics.selectedCount == 0) {
            context->data_->graphics.TempSelect(startPosition_);
        }

        context->data_->saved = false;
    }

    if (event.type == sf::Event::MouseMoved && mousePressedPreviously) {
        if (context->data_->graphics.selectedCount != 0)
            context->data_->graphics.MoveSelected( context->display_->GetMousePos() - startPosition_);
        else {
            context->data_->graphics.MoveOnlyOne(context->display_->GetMousePos() - startPosition_);
        }

        startPosition_ = context->display_->GetMousePos();
        context->data_->saved = false;
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        mousePressedPreviously = false;
        context->data_->graphics.tempSelected = nullptr;
    }
}