#include "Editor/EventHandlers.h"

Editor::ModeHandler *Editor::CommandHandler::instance_ = nullptr;

Editor::ModeHandler *Editor::CommandHandler::GetInstance(Editor &editor) {
    if (instance_ == nullptr || &(*(instance_->context)) != &(editor)) {
        instance_ = new CommandHandler(editor);
    }

    return instance_;
}

[[maybe_unused]] void Editor::CommandHandler::SetMode(Editor &editor) {
    editor.currentMode_ = GetInstance(editor);
    editor.currentModeString_ = GetName();
    editor.display_->showBrushSettings = false;

    editor.data_->graphics.UnselectAll();

    sf::Image cursorImage;
    cursorImage.create(COMMAND_MODE_CURSOR_SIZE,COMMAND_MODE_CURSOR_SIZE, sf::Color::White);

    sf::Cursor cursor;
    if (cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(COMMAND_MODE_CURSOR_SIZE, COMMAND_MODE_CURSOR_SIZE), sf::Vector2u(0, 0))) {
        editor.display_->SetCursor(cursor);
    }
}

void Editor::CommandHandler::ProcessEvent(const sf::Event &event) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        context->data_->commandBuffer = "";
        context->data_->inCommand = false;
    }

    else if (context->data_->inCommand) {
        HandleCommand(event);
    }

    else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::SemiColon
            && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
        context->data_->commandBuffer = "";
        context->data_->inCommand = true;
    }

    else if (event.type == sf::Event::KeyPressed) {
        ChangeModes(event.key.code);
        MoveCursor(event.key.code);

        switch (event.key.code) {
            case sf::Keyboard::Key::X:
                context->data_->DeleteNextChar();
                break;

            default:
                break;
        }

        context->display_->DiscardNextEvent();
    }
}

void Editor::CommandHandler::HandleCommand(const sf::Event &event) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') context->data_->commandBuffer.erase(context->data_->commandBuffer.getSize() - 1);
        else context->data_->commandBuffer.insert(context->data_->commandBuffer.getSize(), event.text.unicode);

    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Enter) {
        if (!RunCurrentCommand()) context->data_->commandBuffer = "Invalid command";
        context->data_->commandClock.restart();
        context->data_->inCommand = false;
    }

    if(context->data_->commandBuffer.isEmpty() || context->data_->commandBuffer[0] != ':'){
        context->data_->inCommand = false;
    }
}

bool Editor::CommandHandler::RunCurrentCommand() {
    std::string command = context->data_->commandBuffer.substring(1);

    std::string commandName = command.substr(0, command.find(' '));

    auto it = commands.find(commandName);
    if (it == commands.end()) return false;

    std::string commandArgs;
    if (command != commandName) commandArgs = command.substr(commandName.size() + 1);

    it->second(commandArgs, *context);

    return true;
}

void Editor::CommandHandler::ChangeModes(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::A:
            context->data_->textCursor.MoveRight();
        case sf::Keyboard::Key::I:
            context->display_->ThinTextCursor();
            context->switchMode<InsertHandler>();
            break;
        case sf::Keyboard::Key::T:
            context->display_->ThinTextCursor();
            context->switchMode<TextHandler>();
            break;

        default:
            break;
    }
}

void Editor::CommandHandler::MoveCursor(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::H:
            context->data_->textCursor.MoveLeft();
            break;
        case sf::Keyboard::Key::L:
            context->data_->textCursor.MoveRight();
            break;
        case sf::Keyboard::Key::K:
            context->data_->textCursor.MoveUp();
            break;
        case sf::Keyboard::Key::J:
            context->data_->textCursor.MoveDown();
            break;

        default:
            break;
    }
}

const std::map<std::string, std::function<void(std::string, Editor&)>> Editor::CommandHandler::commands = { // NOLINT(cert-err58-cpp)
    {"q",[](const std::string& args, Editor &editor){
        if(editor.data_->saved) {
            editor.display_->Close();
        }
        else
            editor.data_->CommandNotification("There are unsaved changes, use :q! or :wq instead");
    }},
    {"q!",[](const std::string& args, Editor &editor) {
        editor.display_->Close();
    }},
    {"o",[](const std::string& args, Editor &editor) {
        editor.data_->file.OpenFile();
    }},
    {"w",[](const std::string& args, Editor &editor) {
        editor.data_->file.SaveFile();
    }},
    {"wq",[](const std::string& args, Editor &editor) {
        editor.data_->file.SaveFile();
        if(editor.data_->saved) editor.display_->Close();
    }},
    {"fs",[](const std::string& args, Editor &editor) {
        editor.display_->SetFullscreen();
    }},
    {"titlebar",[](const std::string& args, Editor &editor) {
        editor.display_->SwitchWindowMode();
    }},
    {"res",[](const std::string& args, Editor &editor) {
        auto space = std::find(args.begin(), args.end(), ' ');

        auto x = std::stoi(std::string(args.begin(), space));
        auto y = std::stoi(std::string(space + 1, args.end()));
        editor.display_->SetResolution(x, y, true);
    }},
    {"font",[](const std::string& args, Editor &editor) {
        editor.display_->SetFont(std::stoi(args));
    }},
    {"dark",[](const std::string& args, Editor &editor) {
        editor.display_->SetColor(FONT_COLOR, BACKGROUND_COLOR);
    }},
    {"light",[](const std::string& args, Editor &editor) {
        editor.display_->SetColor(LIGHT_FONT_COLOR, LIGHT_BACKGROUND_COLOR);
    }},
    {"spacing",[](const std::string& args, Editor &editor) {
        editor.display_->SetSpacing(std::stof(args));
    }},
    {"margin",[](const std::string& args, Editor &editor) {
        editor.display_->SetMargin(std::stof(args));
    }},
    {"name",[](const std::string& args, Editor &editor) {
        editor.data_->file.filename = args;
    }},
    {"settings",[](const std::string& args, Editor &editor) {
        editor.display_->ExportSettings(SETTINGS_FILE_PATH);
    }},
};
