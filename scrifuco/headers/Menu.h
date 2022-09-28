#ifndef SCRIFUCO_MENU_H
#define SCRIFUCO_MENU_H

#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "Editor/Editor.h"
#include "Editor/DataManagement.h"
#include "Settings.h"
#include "DisplayElement.h"
#include "Library.h"

class Menu {
public:
    explicit Menu(Settings &settings);
    void StartLoop();
    ~Menu();

private:
    std::shared_ptr<sf::RenderWindow> window_;

    std::vector<std::unique_ptr<DisplayElement>> elements_;
    std::vector<std::unique_ptr<Button>> buttons_;
    std::unique_ptr<DualSwitchBind> dualSwitch_;

    Settings &settings_;

    std::vector<std::thread> threads_;

    void UpdateMenu();
};

#endif
