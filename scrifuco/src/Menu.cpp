#include "Menu.h"

Menu::Menu(Settings &settings) : settings_(settings), dualSwitch_(nullptr) {
    sf::ContextSettings context;
    context.antialiasingLevel = settings_.Antialiasing;

    window_ = std::make_shared<sf::RenderWindow>(
            sf::VideoMode(settings_.ScreenWidth, settings_.ScreenHeight),
            settings_.Title, MENU_STYLE, context);
    window_->setFramerateLimit(settings_.Framerate);

    // Logo
    elements_.emplace_back(std::make_unique<ShortText>(APP_NAME, settings_.Font, LOGO_SIZE, LOGO_POSITION, LOGO_COLOR));

    // Switch
    sf::Text switchTextLeft(STANDARD_OPTION_STRING, settings_.Font, SWITCH_FONT_SIZE);
    switchTextLeft.setPosition(SWITCH_POSITION_LEFT);
    switchTextLeft.setFillColor(SWITCH_COLOR);

    sf::Text switchTextRight(ADVANCED_OPTION_STRING, settings_.Font, SWITCH_FONT_SIZE);
    switchTextRight.setPosition(SWITCH_POSITION_RIGHT);
    switchTextRight.setFillColor(SWITCH_COLOR);

    dualSwitch_ = std::make_unique<DualSwitchBind>(
            std::make_unique<ButtonWithText>(switchTextLeft, MENU_BUTTON_TOLERANCE, [&]() {}),
            std::make_unique<ButtonWithText>(switchTextRight, MENU_BUTTON_TOLERANCE, [&]() {}),
            SWITCH_SELECT_COLOR);

    // Buttons
    std::vector<sf::Text> menuButtonTexts{{QUICK_MODE_STRING,     settings_.Font, MENU_FONT_SIZE},
                                          {OPEN_DOCUMENT_STRING,  settings_.Font, MENU_FONT_SIZE},
                                          {CREATE_LIBRARY_STRING, settings_.Font, MENU_FONT_SIZE},
                                          {OPEN_LIBRARY_STRING,   settings_.Font, MENU_FONT_SIZE},
                                          {EXIT_STRING,           settings_.Font, MENU_FONT_SIZE}};

    std::vector<std::function<void()>> menuButtonFunctions{
        {[this]() {
            window_->close();
            Editor::StaticStartLoop(QUICK_MODE_STRING, this->dualSwitch_->IsRightSelected());
        }},
        {[this]() {
            threads_.emplace_back(std::thread([this](){
                std::string file_name = FileManagement::OpenFileDialog(SELECT_DOCUMENT_DIALOG, DOCUMENT_FILTER);
                if (file_name.empty()) return;

                Editor editor{file_name, file_name};
                if (!editor.Initialized) return;

                editor.StartLoop(this->dualSwitch_->IsRightSelected());
            }));
        }},
        {[this](){
            threads_.emplace_back(std::thread([this](){
                std::string filePath;
                if (!Library::CreateLibraryDialog(filePath)) return;
                Library library{settings_ , filePath};
                library.StartLoop();
            }));
        } },
        {[this](){
            threads_.emplace_back(std::thread([this](){
                std::string filePath;
                if (!Library::OpenLibraryDialog(filePath)) return;
                Library library{ settings_, filePath};
                library.StartLoop();
            }));
        } },
        {[&window = *window_]() {
            window.close();
        }}
    };

    for (unsigned int i = 0; i < menuButtonTexts.size(); i++) {
        menuButtonTexts[i].setPosition({MENU_ELEMENT_OFFSET_X, MENU_ELEMENT_OFFSET_Y + static_cast<float>(i) * MENU_ELEMENT_LINESPACE});
        menuButtonTexts[i].setFillColor(MENU_ELEMENT_COLOR);

        buttons_.emplace_back(std::make_unique<ButtonWithText>(menuButtonTexts[i], MENU_BUTTON_TOLERANCE, menuButtonFunctions[i], MENU_HOVER_COLOR));
    }
}

void Menu::StartLoop() {
    while (window_->isOpen())
        UpdateMenu();
}

void Menu::UpdateMenu() {
    sf::Event event{};

    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::Closed) window_->close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            for (auto &button: buttons_) {
                if (button->MouseCheck({event.mouseButton.x, event.mouseButton.y})) {
                    button->OnClick();
                    break;
                }
            }
            dualSwitch_->MouseCheck({event.mouseButton.x, event.mouseButton.y});
        }

        if (event.type == sf::Event::MouseMoved) {
            for (auto &button: buttons_) {
                button->IsMouseOver = button->MouseCheck(
                        {sf::Mouse::getPosition(*window_).x, sf::Mouse::getPosition(*window_).y});
            }
        }
    }

    window_->clear(settings_.BackgroundColor);

    for (auto &element: elements_) element->Draw(window_, 0);
    for (auto &button: buttons_) button->Draw(window_, 0);
    dualSwitch_->Draw(window_, 0);

    window_->display();
}

Menu::~Menu() {
    for (auto &thread: threads_) thread.join();
}
