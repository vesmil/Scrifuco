#include <iostream>

#include <Menu.h>
#include <Constants.h>
#include <Settings.h>

int main(int argc, char *argv[]) {

    // Load settings
    Settings& settings = Settings::GetInstance();

    std::fstream settingsFile{SETTINGS_FILE_PATH, std::ios::in};

    if (settingsFile.is_open()) {
        auto result = settings.LoadSettings(settingsFile);
        if (!result) {
            std::cerr << "Failed to load settings file." << std::endl;
            return 1;
        }
    }

    // Create menu
    Menu menu(settings);
    menu.StartLoop();
}