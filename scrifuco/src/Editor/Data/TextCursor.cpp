#include "Editor/TextCursor.h"
#include "Editor/DataManagement.h"

void TextCursor::Set(sf::Vector2<int> positions) {
    if (data_->text.rows.size() == 0) return;

    int numLines = (int) data_->text.rows.size() - 1;
    y = positions.y <= 0 ? 0 : positions.y > numLines ? numLines : positions.y;

    int numChars = (int) data_->text.rows[y].getSize();
    x = positions.x <= 0 ? 0 : positions.x > numChars ? numChars : positions.x;
}

void TextCursor::MoveRight() {
    tempX_ = 0;

    if (x == data_->text.rows[y].getSize() && y < data_->text.rows.size() - 1) Set({0, y + 1});
    else Set({x + 1, y});
}

void TextCursor::MoveLeft() {
    tempX_ = 0;

    if (x == 0 && y != 0) Set({(int) data_->text.rows[y - 1].getSize(), y - 1});
    else Set({x - 1, y});
}

void TextCursor::MoveUp() {
    if (tempX_ == 0) tempX_ = x;

    Set({tempX_, y - 1});
}

void TextCursor::MoveDown() {
    if (tempX_ == 0) tempX_ = x;

    Set({tempX_, y + 1});
}
