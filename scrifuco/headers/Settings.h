#ifndef SCRIFUCO_SETTINGS_H
#define SCRIFUCO_SETTINGS_H

#include <fstream>
#include "Constants.h"

class EditorWindow;

struct Settings {
    static Settings& GetInstance();
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    bool LoadSettings(std::fstream &file);
    void ExportSettings(const std::string& fileName) const;

    void RecalculateSizes();

    std::string Title = APP_NAME;

    unsigned int ScreenWidth = SCREEN_WIDTH;
    unsigned int ScreenHeight = SCREEN_HEIGHT;

    unsigned int FontSize = FONT_SIZE;
    float LineSpace = LINE_SPACE;

    unsigned int Framerate = FRAME_RATE;
    unsigned int Antialiasing = ANTIALIASING;

    sf::Color BackgroundColor = BACKGROUND_COLOR;
    sf::Color FontColor = FONT_COLOR;
    sf::Color SelectionColor = SELECTION_COLOR;

    std::string FontPath = FONT_PATH;
    sf::Font Font;

    float OffsetX = OFFSET_X;
    float OffsetY = OFFSET_Y;

    float CursorOffsetY = (float) FontSize * CURSOR_OFFSET;

    float scrollThresholdPercentage = SCROLL_THRESHOLD;

    const sf::Time CursorDelay = CURSOR_DELAY;

    float buttonSquareSize = BUTTON_SIZE;
    float buttonSpacing = BUTTON_SPACING;
    float buttonMargin = BUTTON_MARGIN;
    float buttonSizeDiff = BUTTON_DIFF;

    // Will be calculated
    float pixelFontSizeHeight = 0;
    float pixelFontSizeWidth = 0;
    float pixelFontSpacing = 0;

private:
    Settings();

    float CalculateFontWidth() const;
    float CalculateFontHeight() const;
    float CalculateSpacing() const;
};

#endif
