#include "Settings.h"
#include "Editor/EditorWindow.h"

#include <fstream>
#include <sstream>

Settings& Settings::GetInstance() {
    static Settings instance;
    return instance;
}

Settings::Settings() {
    Font.loadFromFile(FONT_PATH);
    RecalculateSizes();
}

void Settings::ExportSettings(const std::string& fileName) const {
    std::ofstream fileStream(fileName);

    if (fileStream.is_open()) {
        fileStream << "WindowSize: " << ScreenWidth << ' ' << ScreenHeight << '\n';
        fileStream << "MarginXY:" << OffsetX << ' ' << OffsetY << '\n';
        fileStream << "FontPath: " << FontPath << '\n';
        fileStream << "LineSpace: " << LineSpace << '\n';
        fileStream << "FontSize: " << FontSize << '\n';
        fileStream << "FontColor: " << FontColor.r << ' ' << FontColor.g << ' ' << FontColor.b << '\n';
        fileStream << "BackgroundColor: " << BackgroundColor.r << ' ' << BackgroundColor.g << ' ' << BackgroundColor.b << '\n';
        fileStream << "ScrollThreshold: " << scrollThresholdPercentage << '\n';
    }
}

// Get exact font dimensions in pixels
void Settings::RecalculateSizes() {
    pixelFontSizeWidth = CalculateFontWidth();
    pixelFontSizeHeight = CalculateFontHeight();
    pixelFontSpacing = CalculateSpacing();
}

float Settings::CalculateFontWidth() const {
    sf::Text sample;
    sample.setFont(Font);
    sample.setCharacterSize(FontSize);
    sample.setString("_");
    auto temp = sample.getLocalBounds().width;
    sample.setString("__");

    return sample.getLocalBounds().width - temp;
}

float Settings::CalculateFontHeight() const {
    sf::Text sample;
    sample.setFont(Font);
    sample.setCharacterSize(FontSize);
    sample.setString("|");

    return sample.getLocalBounds().height;
}

float Settings::CalculateSpacing() const {
    sf::Text sample;
    sample.setFont(Font);
    sample.setCharacterSize(FontSize);
    sample.setString("|");
    sample.setLineSpacing(LineSpace);
    auto temp = sample.getLocalBounds().height;
    sample.setString("|\n|");

    return sample.getLocalBounds().height - 2 * temp;
}

bool Settings::LoadSettings(std::fstream &file) {
    try {
        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string name;
            ss >> name;

            if (name == "WindowSize:") {
                ss >> ScreenWidth >> ScreenHeight;
            } else if (name == "MarginXY:") {
                ss >> OffsetX >> OffsetY;
            } else if (name == "LineSpace:") {
                ss >> LineSpace;
            } else if (name == "FontSize:") {
                ss >> FontSize;
            } else if (name == "FontColor:") {
                ss >> FontColor.r >> FontColor.g >> FontColor.b;
            } else if (name == "BackgroundColor:") {
                ss >> BackgroundColor.r >> BackgroundColor.g >> BackgroundColor.b;
            } else if (name == "ScrollThreshold:") {
                ss >> scrollThresholdPercentage;
            } else if (name == "FontPath:") {
                ss >> FontPath;
            }
        }

        Font.loadFromFile(FontPath);

        return true;
    }
    catch(...) {
        return false;
    }
}

void EditorWindow::SetResolution(unsigned int width, unsigned int height, bool moveWindow) {
    auto position = window_->getPosition();

    window_->setSize({width, height});
    settings_.ScreenWidth = width;
    settings_.ScreenHeight = height;

    if (moveWindow) {
        window_->setPosition({(int) (sf::VideoMode::getDesktopMode().width - width) / 2,(int) (sf::VideoMode::getDesktopMode().height - height) / 2,});
    } else {
        window_->setPosition(position);
    }

    InitializeEasyMode();
    InitButtons();
}

void EditorWindow::SetFont(int size) {
    settings_.FontSize = size;
    settings_.RecalculateSizes();

    RefreshFont();
    ThickTextCursor();
}

void EditorWindow::SetColor(sf::Color font, sf::Color background) {
    settings_.BackgroundColor = background;
    settings_.FontColor = font;

    colors[0] = font;
    InitButtons();

    RefreshFont();
}

void EditorWindow::SetMargin(float size) {
    settings_.OffsetX = size;
    settings_.OffsetY = size;
}

void EditorWindow::SetSpacing(float size) {
    settings_.LineSpace = size;
    settings_.RecalculateSizes();

    RefreshFont();
}

void EditorWindow::SwitchWindowMode() {
    auto position = window_->getPosition();

    if (isBorderless) {
        window_ = std::make_shared<sf::RenderWindow>(sf::VideoMode(window_->getSize().x, window_->getSize().y),
                                                     settings_.Title, sf::Style::Default, window_->getSettings());
    } else {
        window_ = std::make_shared<sf::RenderWindow>(sf::VideoMode(window_->getSize().x,  window_->getSize().y),
                                                     settings_.Title, sf::Style::None, window_->getSettings());
    }

    isBorderless = !isBorderless;
    window_->setPosition(position);
}

void EditorWindow::SetFullscreen() {
    SetResolution(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height, true);

    if (!isBorderless) {
        SwitchWindowMode();
    }
}

void EditorWindow::SetFont(sf::Text &text, sf::Font &font, unsigned int size, sf::Color color) {
    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(color);
}