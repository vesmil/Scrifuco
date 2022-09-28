#ifndef SCRIFUCO_CONSTANTS_H
#define SCRIFUCO_CONSTANTS_H

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

const std::string APP_NAME = "Scrifuco";

const std::string SETTINGS_FILE_PATH = "config.ini";
const std::string FONT_PATH = "assets/fonts/UbuntuMono.ttf";

const std::string ERASER_PATH ="assets/images/eraser.png";
const std::string MOVE_PATH = "assets/images/move.png";

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

const unsigned int FRAME_RATE = 144;
const unsigned int ANTIALIASING = 16;

const auto WINDOW_STYLE = sf::Style::Default;
const auto MENU_STYLE = sf::Style::Titlebar | sf::Style::Close;

const unsigned int FONT_SIZE = 28;
const float LINE_SPACE = 1.6f;

const sf::Color BACKGROUND_COLOR{45, 45, 45};
const sf::Color FONT_COLOR{240, 240, 240};
const sf::Color SELECTION_COLOR{80, 190, 240, 180};
const sf::Color TEXT_SELECTION_COLOR{58, 76, 99};

const sf::Color LIGHT_FONT_COLOR{10, 10, 10};
const sf::Color LIGHT_BACKGROUND_COLOR{250, 250, 250};

const sf::Color FIRST_COLOR{106, 179, 234};
const sf::Color SECOND_COLOR{234, 72, 75};
const sf::Color THIRD_COLOR{201, 230, 98};

const float OFFSET_X = 50;
const float OFFSET_Y = 50;

const float SCROLL_THRESHOLD = 0.75f;
const float CURSOR_OFFSET = 0.2f;
const sf::Time CURSOR_DELAY = sf::milliseconds(400);

const float BUTTON_SIZE = 30;
const float BUTTON_SPACING = 10;
const float BUTTON_MARGIN = 20;
const float BUTTON_DIFF = 0.2f * BUTTON_SIZE;

const unsigned int LOGO_SIZE = 88;
const int LOGO_OFFSET_X = 88;
const int LOGO_OFFSET_Y = 88;
const sf::Color LOGO_COLOR{80, 170, 210};
const sf::Vector2f LOGO_POSITION{LOGO_OFFSET_X, LOGO_OFFSET_Y};

const int MENU_FONT_SIZE = 36;
const int MENU_ELEMENT_OFFSET_X = 115;
const int MENU_ELEMENT_OFFSET_Y = 220;
const int MENU_ELEMENT_LINESPACE = static_cast<int>(MENU_FONT_SIZE * 1.6);
const int MENU_BUTTON_TOLERANCE = 14;

const std::string QUICK_MODE_STRING = "Quick mode";
const std::string OPEN_DOCUMENT_STRING = "Open document";
const std::string CREATE_LIBRARY_STRING = "Create library";
const std::string OPEN_LIBRARY_STRING = "Open library";
const std::string SETTINGS_STRING = "Settings";
const std::string EXIT_STRING = "Exit";

const std::string CREATE_DOCUMENT_STRING = "Create document";
const sf::Vector2f CREATE_DOCUMENT_POSITION {OFFSET_X, OFFSET_Y};

const sf::Color MENU_ELEMENT_COLOR{240, 240, 240};
const sf::Color MENU_HOVER_COLOR{189, 191, 8};

const sf::Color SWITCH_COLOR{120, 120, 120};
const sf::Color SWITCH_SELECT_COLOR{240, 240, 240};

const std::string STANDARD_OPTION_STRING = "Standard";
const std::string ADVANCED_OPTION_STRING = "Advanced";

static const char *SELECT_DOCUMENT_DIALOG = "Select a document";
static const char *SELECT_LIBRARY_DIALOG = "Select a library";

static const char *DOCUMENT_FORMAT = "stxt\0";
static const char *LIBRARY_FORMAT = "slib\0";

static const char *DOCUMENT_FILTER = "Scrifuco Document\0*.stxt\0";
static const char *LIBRARY_FILTER = "Scrifuco Document\0*.slib\0";

const int SWITCH_FONT_SIZE = 24;
const sf::Vector2f SWITCH_POSITION_LEFT{250, SCREEN_HEIGHT - 65};
const sf::Vector2f SWITCH_POSITION_RIGHT{360, SCREEN_HEIGHT - 65};

const sf::Color SPLINE_SELECT_COLOR = sf::Color{80,190,240, 180};

const int SELECTED_THICKNESS = 7;
const int SINGLE_DOT_SIZE_INCREASE = 4;

const int SELECT_TOLERANCE = 15;

const int BUTTON_OUTLINE = 1;
const int SELECT_BUTTON_OUTLINE = 2;

const int COMMAND_OFFSET_X = 15;
const int COMMAND_OFFSET_Y = 30;

const float SMALLER_FONT_FACTOR = 0.75f;
const int SMALLER_FONT_SIZE = (int) ((float) FONT_SIZE * SMALLER_FONT_FACTOR);

const sf::Color THIN_CURSOR_COLOR{200, 200, 200};
const sf::Color THICK_CURSOR_COLOR{120, 120, 120};

const int THIN_CURSOR_WIDTH = 2;

const sf::Vector2<unsigned int> SMALLER_CURSOR_ICON_SIZE{15,15};
const sf::Vector2<unsigned int> CURSOR_ICON_SIZE{30,30};

const sf::Vector2<unsigned int> SELECTOR_HOTSPOT{8,8};
const sf::Vector2<unsigned int> ERASER_HOTSPOT{20,10};
const sf::Vector2<unsigned int> MOVE_HOTSPOT{15,15};

const int SELECTOR_SIZE = 25;

const std::string TEXT_MODE_TEXT = "Text mode";
const std::string GRAPHICS_MODE_TEXT = "Hybrid mode";
const std::string SAVE_TEXT = "Save file (saved)";
const std::string UNSAVED_SAVE_TEXT = "Save file (unsaved)";

const int CURRENT_MODE_BUTTON_TOLERANCE = 10;

const sf::Vector2f CURRENT_MODE_SUB{15, 30};
const sf::Vector2f CURRENT_MODE_RIGHT_SUB{140, 50};
const sf::Vector2f CURRENT_MODE_LEFT_SUB{260, 50};

const int COMMAND_MODE_CURSOR_SIZE = 5;

const int CHANGE_MOUSE_CURSOR_TOLERANCE = 5;

const std::string DELETE_BUTTON_TEXT = "X";
const sf::Color DELETE_BUTTON_COLOR{240, 40, 40};

#endif