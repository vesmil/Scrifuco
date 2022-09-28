#include "Editor/DataManagement.h"

void DataManagement::InsertChar(uint32_t character) {
    while (text.rows.size() < textCursor.y + 1) text.rows.emplace_back();
    while (text.rows[textCursor.y].getSize() < textCursor.x) text.rows[textCursor.y].insert(textCursor.x, ' ');

    text.rows[textCursor.y].insert(textCursor.x, character);
    textCursor.MoveRight();
}

void DataManagement::InsertNewLine() {
    // Move the rest of the current line
    text.rows.insert(text.rows.begin() + textCursor.y + 1,
                     text.rows[textCursor.y].substring(textCursor.x,
                                                       text.rows[textCursor.y].getSize() -
                                                       textCursor.x));
    text.rows[textCursor.y].erase(textCursor.x, text.rows[textCursor.y].getSize() - textCursor.x);

    textCursor.y += 1;
    textCursor.x = 0;
}

void DataManagement::DeleteChar() {
    // Remove one char
    if (textCursor.x > 0 && textCursor.y < text.rows.size()) {
        text.rows[textCursor.y].erase(textCursor.x - 1, 1);
        textCursor.MoveLeft();
    }

    // Remove \n and merge lines
    else if (textCursor.y > 0 && textCursor.x == 0) {
        textCursor.x = (int) text.rows[textCursor.y - 1].getSize();
        text.rows[textCursor.y - 1].insert(text.rows[textCursor.y - 1].getSize(),
                                           text.rows[textCursor.y]);
        text.rows.erase(text.rows.begin() + textCursor.y);
        textCursor.MoveUp();
    }
}

void DataManagement::DeleteNextChar() {
    if (text.rows[textCursor.y].getSize() - textCursor.x > 0) {
        text.rows[textCursor.y].erase(textCursor.x, 1);
    } else if (textCursor.y < (int) text.rows.size() - 1) {
        textCursor.y += 1;
        textCursor.x = 0;
        DeleteChar();
    }
}



void DataManagement::CommandNotification(const std::string& notification){
    inCommand = false;
    commandBuffer = notification;
    commandClock.restart();
}

void DataManagement::Select(sf::Vector2i start, sf::Vector2i end) {
    inSelection = true;
    if (start.y < end.y || (start.y == end.y && start.x < end.x)) {
        selectionStart = start;
        selectionEnd = end;
    } else {
        selectionStart = end;
        selectionEnd = start;
    }

    textCursor.Set(selectionEnd);
}

void DataManagement::Deselect() {
    inSelection = false;
}

std::string DataManagement::GetSelectedText() {
    if (inSelection) {
        std::string result;
        if (selectionStart.y == selectionEnd.y) {
            result = text.rows[selectionStart.y].substring(selectionStart.x, selectionEnd.x - selectionStart.x);
        } else {
            result = text.rows[selectionStart.y].substring(selectionStart.x, text.rows[selectionStart.y].getSize() - selectionStart.x) + "\n";
            for (int i = selectionStart.y + 1; i < selectionEnd.y; i++) {
                result += text.rows[i] + "\n";
            }
            result += text.rows[selectionEnd.y].substring(0, selectionEnd.x);
        }
        return result;
    }

    return "";
}

void DataManagement::DeleteSelection() {
    if (inSelection) {
        try {
            if (selectionStart.y == selectionEnd.y) {
                text.rows[selectionStart.y].erase(selectionStart.x, selectionEnd.x - selectionStart.x);
            } else {
                text.rows[selectionStart.y].erase(selectionStart.x, text.rows[selectionStart.y].getSize() - selectionStart.x);
                text.rows[selectionStart.y].insert(text.rows[selectionStart.y].getSize(),text.rows[selectionEnd.y].substring(selectionEnd.x,text.rows[selectionEnd.y].getSize()));

                for (int i = selectionEnd.y; i > selectionStart.y; --i) {
                    text.rows.erase(text.rows.begin() + i);
                }
            }
        } catch (...) {
            // Problems with empty rows - because it's not common, this is good enough
        }

        Deselect();
        textCursor.Set(selectionStart);
    }
}
