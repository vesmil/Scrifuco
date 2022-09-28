#include "Library.h"
#include "Editor/DataManagement.h"
#include "Editor/Editor.h"

Library::Library(Settings &settings, std::string &filePath, bool advanced, const sf::String& WindowTitle) : settings_(settings), filePath_(filePath), advanced_(advanced) {
    sf::ContextSettings context;
    context.antialiasingLevel = settings_.Antialiasing;

    window_ = std::make_shared<sf::RenderWindow>(
            sf::VideoMode(settings_.ScreenWidth, settings_.ScreenHeight),
            WindowTitle, MENU_STYLE, context);
    window_->setFramerateLimit(settings_.Framerate);

    UpdateButtons();
}

void Library::UpdateButtons() {
    buttons_.clear();

    sf::Text createDocumentText(CREATE_DOCUMENT_STRING, settings_.Font, MENU_FONT_SIZE);
    createDocumentText.setPosition(CREATE_DOCUMENT_POSITION);
    createDocumentText.setFillColor(FONT_COLOR);

    buttons_.emplace_back(std::make_unique<ButtonWithText>(createDocumentText, MENU_BUTTON_TOLERANCE, [this]() {
        std::string file_name = FileManagement::SaveFileDialog(DOCUMENT_FORMAT);
        if (file_name.empty()) return;

        std::ofstream libFile(filePath_, std::ios_base::app);
        libFile << file_name << std::endl;
        libFile.close();

        UpdateButtons();
    }, MENU_HOVER_COLOR));

    buttonPosition_ = CREATE_DOCUMENT_POSITION;
    buttonPosition_.y = buttonPosition_.y * 3;

    std::ifstream file(filePath_);

    if (file.is_open()) {
        std::string row;

        while (std::getline(file, row)) {
            sf::Text documentButtonText(row, settings_.Font, MENU_FONT_SIZE);
            documentButtonText.setPosition(buttonPosition_);
            documentButtonText.setFillColor(FONT_COLOR);

            buttons_.emplace_back(std::make_unique<ButtonWithText>(documentButtonText, MENU_BUTTON_TOLERANCE, [this, row]() {
                threads_.emplace_back(std::thread([this, row](){
                    Editor editor{row, row};
                    if (!editor.Initialized) return;
                    editor.StartLoop(advanced_);
                }));
            }, MENU_HOVER_COLOR));

            sf::Text deleteButton(DELETE_BUTTON_TEXT, settings_.Font, MENU_FONT_SIZE);
            deleteButton.setPosition({buttonPosition_.x + documentButtonText.getLocalBounds().width + OFFSET_X, buttonPosition_.y});
            deleteButton.setFillColor(DELETE_BUTTON_COLOR);

            buttons_.emplace_back(std::make_unique<ButtonWithText>(deleteButton, MENU_BUTTON_TOLERANCE, [this, row]() {
                std::ofstream tempFile("temp.txt");
                std::ifstream file(filePath_);

                std::string line;
                while (getline(file, line)) {
                    if (line != row) tempFile << line << std::endl;
                }

                tempFile.close();
                file.close();

                const char * path = filePath_.c_str();
                remove(path);
                rename("temp.txt", path);

                UpdateButtons();
            }, MENU_HOVER_COLOR));

            buttonPosition_.y += documentButtonText.getLocalBounds().height * LINE_SPACE;
        }
    }

    file.close();
}


void Library::StartLoop() {
    while (window_->isOpen()) Update();
}

void Library::Update() {
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

    window_->display();
}

bool Library::CreateLibraryDialog(std::string& filePath) {
    filePath = FileManagement::SaveFileDialog(LIBRARY_FORMAT);
    return TestPath(filePath);
}

bool Library::OpenLibraryDialog(std::string& filePath) {
    filePath = FileManagement::OpenFileDialog(SELECT_LIBRARY_DIALOG,LIBRARY_FILTER);
    return TestPath(filePath);
}

bool Library::TestPath(std::string &filePath) {
    if (filePath.empty()) return false;

    std::ofstream file(filePath, std::ios_base::app);
    if (!file.is_open()) return false;
    file.close();
    return true;
}

Library::~Library() {
    for (auto &thread: threads_) thread.join();
}
