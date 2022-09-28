#ifndef SCRIFUCO_DATAREPRESENTATION_H
#define SCRIFUCO_DATAREPRESENTATION_H

#include "TextCursor.h"
#include "Constants.h"

#include <SelbaWard/Spline.hpp>

/// <summary>
/// Class containing the main text.
/// </summary>
struct Text {
    void Clear();
    std::vector<sf::String> rows;
};

/// <summary>
/// Class containing all the graphics.
/// Also responsible for drawing all the curves to the screen.
/// </summary>
class Graphics {
private:
    struct SplinePoints {
        SplinePoints(std::vector<sf::Vector2<float>> points, float size, sf::Color color) : points(std::move(points)), color(color), size(size) {}
        void Draw(sf::RenderWindow &window);

        std::vector<sf::Vector2<float>> points;
        sf::Color color;
        float size;

        bool selected = false;

        void DrawCircle(sf::RenderWindow &window);
    };

public:
    void DrawAll(sf::RenderWindow &window);

    void StartCurve();
    void StartCurve(float size, sf::Color color);

    void AddPoint(sf::Vector2<float> point);
    void Erase(sf::Vector2<float> pointCoordinates);

    void SetSelected(sf::Vector2<float> pointCoordinates, bool value);
    void UnselectAll();

    void TempSelect(sf::Vector2f TempSelect);

    void MoveSelected(sf::Vector2f delta);
    void MoveOnlyOne(sf::Vector2<float> delta);
    static void MoveCurve(SplinePoints &curve, sf::Vector2f distance);
    void DeleteSelected();

    void RemoveLast();
    void Clear();

    std::vector<SplinePoints> curves{};

    enum Tool { Brush, Selector, Eraser, Move } currentTool = Tool::Brush;

    float brushSize = 2.0f;
    sf::Color brushColor = sf::Color::White;

    bool brushChanged = false;

    static sf::Color selectColor;

    unsigned int selectedCount = 0;
    SplinePoints* tempSelected = nullptr;
};


#endif //SCRIFUCO_DATAREPRESENTATION_H
