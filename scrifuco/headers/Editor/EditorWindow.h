#ifndef SCRUFICO_DISPLAY_H
#define SCRUFICO_DISPLAY_H

#include "SFML/Graphics.hpp"

#include "DataManagement.h"
#include "Settings.h"
#include "DisplayElement.h"

class Editor;

/// <summary>
/// Draws to the screen (with update) and is used to receive events. It also contains the DisplaySettings.
/// </summary>
class EditorWindow {
public:
    EditorWindow(DataManagement &data, sf::String &mode, sf::String &command, Editor &context, bool advanced = false);

    bool IsOpen();
    void Update();
    void Close();

    void ThinTextCursor();
    void ThickTextCursor();

    void RefreshFont();
    void ResetView();

    bool CheckEditorEvents(const sf::Event &event);

    sf::Vector2<float> GetMousePos();
    sf::Vector2<int> CalculateTextCursorPos();

    bool GetNextEvent(sf::Event &event);
    void DiscardNextEvent();

    void ExportSettings(const std::string &file);

    std::vector<std::unique_ptr<EditorButton>> buttons;
    float pixelShift = 0;
    bool showBrushSettings = false;

    void SetColor(sf::Color font, sf::Color background);
    void SetFont(int size);
    void SetSpacing(float size);
    void SetMargin(float size);
    void SetResolution(unsigned int width, unsigned int height, bool moveWindow = false);
    void SetCurrentToolCursor();
    void SetCursor(sf::Cursor &cursor);
    void SetWindowTitle(const sf::String& title);
    void SetAdvanced(bool advanced);
    void SwitchWindowMode();
    void SetFullscreen();

private:
    void DrawGraphics();
    void DrawText();
    void DrawTextCursor();

    static void SetFont(sf::Text &text, sf::Font &font, unsigned int size, sf::Color color);
    void SetStringAndDraw(sf::Text &text, sf::String &str,  sf::Vector2f pos);

    void InitButtons();

    std::shared_ptr<sf::RenderWindow> window_;
    Settings& settings_;
    Editor& context_;

    sf::View view_;

    DataManagement &data_;

    sf::String &currentCommand_;
    sf::String &currentMode_;

    sf::Text linesBuffer_[100];
    sf::Text modeText_;
    sf::Text commandText_;

    sf::RectangleShape textCursorRectangle_{};

    sf::Clock clock_;

    size_t shift_ = 0;

    bool showTextCursor_ = true;
    bool isBorderless = true;
    bool advanced_;

    bool shouldRefreshCursor = false;

    bool easyModeInitialized_ = false;
    std::unique_ptr<DualSwitchBind> dualSwitch_;
    std::unique_ptr<ButtonWithText> saveButton_;

    std::vector<sf::Color> colors;

    void InitializeEasyMode();
    void DrawSelection();
};

#endif