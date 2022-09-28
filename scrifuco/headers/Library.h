#ifndef SCRIFUCO_LIBRARY_H
#define SCRIFUCO_LIBRARY_H

#include "Settings.h"
#include "DisplayElement.h"

#include <memory>
#include <thread>

class Library {
public:
    explicit Library(Settings &settings, std::string &filePath, bool advanced = false, const sf::String& WindowTitle = "Library");
    ~Library();

    void StartLoop();

    static bool OpenLibraryDialog(std::string& filePath);
    static bool CreateLibraryDialog(std::string& filePath);

private:
    std::shared_ptr<sf::RenderWindow> window_;

    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<DisplayElement>> elements_;
    std::vector<std::unique_ptr<Button>> buttons_;

    std::string filePath_;
    Settings &settings_;
    bool advanced_;

    sf::Vector2f buttonPosition_;

    void Update();
    void UpdateButtons();

    static bool TestPath(std::string &filePath);
};


#endif //SCRIFUCO_LIBRARY_H
