#ifndef SCRUFICO_TEXTCURSOR_H
#define SCRUFICO_TEXTCURSOR_H

#include "SFML/Graphics.hpp"

class DataManagement;

struct TextCursor {
    TextCursor(sf::Vector2<int> positions, DataManagement &data) : x(positions.x), y(positions.y), data_(&data) {};

    void Set(sf::Vector2<int> positions);
    void MoveRight();
    void MoveLeft();
    void MoveUp();
    void MoveDown();

    int x;
    int y;

private:
    int tempX_ = 0;

    DataManagement *data_;
};

#endif
