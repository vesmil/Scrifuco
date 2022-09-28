#include <iostream>
#include <utility>
#include "Editor/Editor.h"
#include "Editor/EventHandlers.h"

Editor::Editor() : currentMode_(nullptr), display_() {
    data_ = std::make_unique<DataManagement>();
    display_ = std::make_unique<EditorWindow>(*data_, currentModeString_, data_->commandBuffer, *this);

    InsertHandler::SetMode(*this);
    Initialized = true;
}

Editor::Editor(const std::string& windowTitle) : Editor() {
    display_->SetWindowTitle(windowTitle);
}

Editor::Editor(const std::string& windowTitle, const std::string& filePath) : Editor() {
    display_->SetWindowTitle(windowTitle);
    OpenFile(filePath);
}

bool Editor::OpenFile(std::string fileName) {
    try{
        data_->file.filename = std::move(fileName);
        if (!data_->file.OpenFile()) return false;
        return true;
    }
    catch (...) {
        display_->Close();
        return false;
    }
}

void Editor::StartLoop(bool advanced) {
    display_->SetAdvanced(advanced);

    while (display_->IsOpen()) {
        sf::Event event; // NOLINT(cppcoreguidelines-pro-type-member-init)

        while (display_->GetNextEvent(event)) {
            if (event.type == sf::Event::Closed) {
                display_->Close();
                return;
            }

            if (!data_->commandBuffer.isEmpty() && !data_->inCommand && data_->commandClock.getElapsedTime() >= commandHideDelay_ ) {
                data_->commandBuffer = "";
            }

            try {
                ProcessEvent(event);
            }
            catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
        display_->Update();
    }
}


void Editor::ProcessEvent(const sf::Event &event) {
    if (event.type == sf::Event::KeyPressed) {
        // File management
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
            && FileOperationSuccessful(event.key.code))
            return;

        // Change mode to command or move cursor
        if (AnyModeKeyboardOperations(event.key.code)) return;
    }

    // Click buttons
    else if (event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        for (auto &button : display_->buttons) {
            if (button->MouseCheck(static_cast<sf::Vector2i>(display_->GetMousePos() - sf::Vector2f(0,display_->pixelShift)))) {
                button->OnClick();
                return;
            }
        }
    }

    // Scroll
    if (event.type == sf::Event::MouseWheelMoved) {
        data_->textCursor.Set({data_->textCursor.x, data_->textCursor.y - event.mouseWheel.delta});
    }

    // Resize window
    else if (event.type == sf::Event::Resized) {
        display_->SetResolution(event.size.width, event.size.height);
    }

    else if (display_->CheckEditorEvents(event)) {
        return;
    }

    // Mode-dependant events
    else currentMode_->ProcessEvent(event);
}

void Editor::InsertTextEvent(const sf::Event &event) {
    // Clipboard
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)){
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::V) {
            if (data_->inSelection) data_->DeleteSelection();
            for (auto &it: std::string(sf::Clipboard::getString())) {
                if (it != '\n')
                    data_->InsertChar(it);
                else
                    data_->InsertNewLine();
            }
        }

        data_->saved = false;
        return;
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Delete) {
        if (data_->inSelection) data_->DeleteSelection();
        else data_->DeleteNextChar();
        data_->saved = false;
        return;
    }

    // Insert text
    if (event.type == sf::Event::TextEntered) {

        switch (event.text.unicode) {
            case '\b':
                if (data_->inSelection) data_->DeleteSelection();
                else data_->DeleteChar();
                break;
            case '\r':
                if (data_->inSelection) data_->DeleteSelection();
                data_->InsertNewLine();
                break;
            case '\t':
                if (data_->inSelection) data_->DeleteSelection();
                for (int i = 0; i < spaceForTab_; ++i) data_->InsertChar(' ');
                break;
            default:
                if (data_->inSelection) data_->DeleteSelection();
                data_->InsertChar(event.text.unicode);
        }

        data_->saved = false;
    }
}

bool Editor::FileOperationSuccessful(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::N:
            data_->file.NewFile();
            display_->DiscardNextEvent();
            return true;
        case sf::Keyboard::Key::O:
            data_->file.OpenFile();
            return true;
        case sf::Keyboard::Key::S:
            data_->file.SaveFile();
            return true;
        default:
            return false;
    }
}

bool Editor::AnyModeKeyboardOperations(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Escape:
            display_->ThickTextCursor();
            switchMode<CommandHandler>();
            return true;

        case sf::Keyboard::Key::Left:
            data_->textCursor.MoveLeft();
            return true;
        case sf::Keyboard::Key::Right:
            data_->textCursor.MoveRight();
            return true;
        case sf::Keyboard::Key::Up:
            data_->textCursor.MoveUp();
            return true;
        case sf::Keyboard::Key::Down:
            data_->textCursor.MoveDown();
            return true;

        default:
            return false;
    }
}