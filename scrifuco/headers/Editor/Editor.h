#ifndef SCRUFICO_EDITOR_H
#define SCRUFICO_EDITOR_H

#include "SFML/Window/Event.hpp"
#include "EditorWindow.h"

/// <summary>
/// Editor class with main loop, it also holds EditorWindow, DataManagement and current ModeHandler.
/// </summary>
class Editor{
public:
    Editor();
    explicit Editor(const std::string& windowTitle);
    Editor(const std::string& windowTitle, const std::string& filePath);

    void StartLoop(bool advanced);
    static void StaticStartLoop(const std::string& windowTitle, bool advanced) {
        Editor editor(windowTitle);
        editor.StartLoop(advanced);
    }

    bool OpenFile(std::string fileName);
    bool Initialized = false;

private:
    void ProcessEvent(const sf::Event& event);
    void InsertTextEvent(const sf::Event& event);

    bool FileOperationSuccessful(sf::Keyboard::Key key);
    bool AnyModeKeyboardOperations(sf::Keyboard::Key key);

    template <class mode>
    void switchMode(){
        mode::SetMode(*this);
    }

    std::unique_ptr<DataManagement> data_;
    std::unique_ptr<EditorWindow> display_;

    class ModeHandler {
    public:
        virtual void ProcessEvent(const sf::Event& event) = 0;
        Editor *context;

    protected:
        explicit ModeHandler(Editor &editor) : context(&editor) { }
    };

    friend class EditorWindow;

    class CommandHandler;
    class InsertHandler;
    class TextHandler;

    ModeHandler *currentMode_;
    sf::String currentModeString_;

    const sf::Time commandHideDelay_ = sf::milliseconds(500);
    const int spaceForTab_ = 4;
};

#endif