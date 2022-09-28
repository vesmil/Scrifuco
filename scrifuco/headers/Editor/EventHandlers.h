#ifndef SCRUFICO_OTHEREVENTHANDLERS_H
#define SCRUFICO_OTHEREVENTHANDLERS_H

#include "SFML/Window/Event.hpp"

#include "Editor/Editor.h"

class Editor::InsertHandler : public virtual ModeHandler {
public:
    static void SetMode(Editor &editor);
    void ProcessEvent(const sf::Event& event) override;

private:
    using ModeHandler::ModeHandler;
    static ModeHandler *GetInstance(Editor &editor);

    static std::string GetName() { return "Insert mode"; }

    static ModeHandler *instance_;

    bool mousePressedPreviously = false;
    sf::Vector2f startPosition_;

    void BrushMouseEvent(const sf::Event &event);
    void SelectorMouseEvent(const sf::Event &event);
    void EraserMouseEvent(const sf::Event &event);

    void MoveMouseEvent(const sf::Event &event);
};

class Editor::TextHandler : public virtual ModeHandler {
public:
    static void SetMode(Editor &editor);
    void ProcessEvent(const sf::Event& event) override;

private:
    using ModeHandler::ModeHandler;
    static ModeHandler *GetInstance(Editor &editor);

    static std::string GetName() { return "Text mode"; }

    static ModeHandler *instance_;

    bool mousePressedPreviously = false;
    sf::Vector2i selectionStartPosition_;

    sf::Vector2i getCorrectPosition();
};

class Editor::CommandHandler : public virtual ModeHandler {
public:
    static void SetMode(Editor &editor);
    void ProcessEvent(const sf::Event& event) override;

private:
    const static std::map<std::string, std::function<void(std::string, Editor&)>> commands;

    using ModeHandler::ModeHandler;
    static ModeHandler *GetInstance(Editor &editor);

    static std::string GetName() { return "Command mode"; }

    static ModeHandler *instance_;

    void ChangeModes(sf::Keyboard::Key key);
    void MoveCursor(sf::Keyboard::Key key);
    void HandleCommand(const sf::Event &event);
    bool RunCurrentCommand();
};

#endif
