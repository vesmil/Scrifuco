#ifndef SCRUFICO_DATAMANAGEMENT_H
#define SCRUFICO_DATAMANAGEMENT_H

#include "DisplayElement.h"
#include "DataRepresentation.h"
#include "TextCursor.h"

class DataManagement;

/// <summary>
/// Opens, creates and saves files.
/// </summary>
struct FileManagement {
    static std::string OpenFileDialog(const char *title = "Select a file", const char *filter = "Any File\0*.*\0");
    static std::string SaveFileDialog(const char *format = "");

    explicit FileManagement(DataManagement &data) : data_(&data) {};

    void NewFile();
    bool SaveFile();
    bool OpenFile();

    std::string filename;

private:
    DataManagement *data_;
};

/// <summary>
/// Responsible for storing and managing temporary data from the user.
/// </summary>
class DataManagement {
public:
    DataManagement() { text.rows.emplace_back(); }

    void CommandNotification(const std::string &notification);

    void InsertChar(uint32_t character);
    void InsertNewLine();
    void DeleteChar();
    void DeleteNextChar();

    void Select(sf::Vector2i start, sf::Vector2i end);
    void Deselect();
    void DeleteSelection();
    std::string GetSelectedText();

    FileManagement file{*this};

    Graphics graphics;
    Text text;

    TextCursor textCursor{{0, 0}, *this};

    bool saved = true;
    sf::String commandBuffer;
    bool inCommand = false;
    sf::Clock commandClock;

    bool inSelection = false;
    sf::Vector2i selectionStart;
    sf::Vector2i selectionEnd;

    const sf::Time showCommandDelay_ = sf::milliseconds(2000);
};

#endif