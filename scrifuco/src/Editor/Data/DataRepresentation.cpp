#include "Editor/DataRepresentation.h"

void Text::Clear() {
    rows.clear();
    rows.emplace_back();
}

void Graphics::AddPoint(sf::Vector2<float> point) {
    if(curves.empty()) curves.emplace_back(SplinePoints{{}, brushSize, brushColor});
    curves[curves.size() - 1].points.emplace_back(point);
}

void Graphics::StartCurve() {
    curves.emplace_back(SplinePoints{{}, brushSize, brushColor});
}

void Graphics::StartCurve(float size, sf::Color color) {
    curves.emplace_back(SplinePoints{{}, size, color});
}

void Graphics::RemoveLast() {
    if (curves.empty()) return;
    curves.pop_back();
}

void Graphics::Clear() {
    curves.clear();
}

void Graphics::Erase(sf::Vector2<float> pointCoordinates) {
    float tolerance = 10;
    int step = 1;

    for (int i = 0; i < (int) curves.size(); i++) {
        auto curve = curves[i];
        for (int j = 0; j < (int) curve.points.size(); j += step) {
            if (std::abs(curve.points[j].x - pointCoordinates.x) <= tolerance && std::abs(curve.points[j].y - pointCoordinates.y) <= tolerance) {
                curves.erase(curves.begin() + i);
                return;
            }
        }
    }
}

sf::Color Graphics::selectColor = SPLINE_SELECT_COLOR;

// Remember references to the selected curves - laterTODO
void Graphics::SetSelected(sf::Vector2<float> pointCoordinates, bool value) {
    float tolerance = SELECT_TOLERANCE;
    int step = 1;

    for (auto & curve : curves) {
        for (int j = 0; j < (int) curve.points.size(); j += step) {
            if (std::abs(curve.points[j].x - pointCoordinates.x) <= tolerance && std::abs(curve.points[j].y - pointCoordinates.y) <= tolerance) {
                curve.selected = value;
                selectedCount += value ? 1 : -1;
                break;
            }
        }
    }
}

void Graphics::TempSelect(sf::Vector2f position) {
    float tolerance = SELECT_TOLERANCE;
    int step = 1;

    for (auto && curve : curves) {
        for (int j = 0; j < (int) curve.points.size(); j += step) {
            if (std::abs(curve.points[j].x - position.x) <= tolerance && std::abs(curve.points[j].y - position.y) <= tolerance) {
                tempSelected = &curve;
                break;
            }
        }
    }
}

void Graphics::MoveSelected(sf::Vector2f delta) {
    std::for_each(curves.begin(), curves.end(), [delta](auto & curve) { if (curve.selected) MoveCurve(curve, delta);});
}

void Graphics::MoveOnlyOne(sf::Vector2<float> delta) { // NOLINT(readability-make-member-function-const)
    if (tempSelected != nullptr)
        MoveCurve(*tempSelected, delta);
}

void Graphics::MoveCurve(Graphics::SplinePoints &curve, sf::Vector2f distance) {
    for (auto && point : curve.points) {
        point.x += distance.x;
        point.y += distance.y;
    }
}

void Graphics::UnselectAll() {
    selectedCount = 0;
    for (auto & curve : curves) {
        curve.selected = false;
    }
}

void Graphics::DeleteSelected() {
    selectedCount = 0;
    for (int i = 0; i < (int) curves.size(); i++) {
        if (curves[i].selected) {
            curves.erase(curves.begin() + i);
            i--;
        }
    }
}

void Graphics::DrawAll(sf::RenderWindow &window) {
    for (auto &Curve: curves) {
        Curve.Draw(window);
    }
}

void Graphics::SplinePoints::Draw(sf::RenderWindow &window) {
    // Creating my own spline would be better - laterTODO
    if (points.size() < 5) {
        DrawCircle(window);
        return;
    }

    sw::Spline spline(1, points[0]);
    spline.addVertices(points);
    spline.setColor(color);
    spline.setThickness(size);

    if(selected) {
        sw::Spline splineOutline(1, points[0]);
        splineOutline.addVertices(points);
        splineOutline.setColor(selectColor);
        splineOutline.setThickness(size + SELECTED_THICKNESS);
        splineOutline.update();
        window.draw(splineOutline);
    }

    spline.update();
    window.draw(spline);
}

void Graphics::SplinePoints::DrawCircle(sf::RenderWindow &window) {
    sf::CircleShape circle;
    circle.setPosition(points[0].x - size / 2, points[0].y - size / 2);
    circle.setRadius(size);
    circle.setFillColor(color);

    if(selected) {
        sf::CircleShape circleOutline;
        circleOutline.setPosition(points[0].x - size / 2 - SINGLE_DOT_SIZE_INCREASE,
                                  points[0].y - size / 2  - SINGLE_DOT_SIZE_INCREASE);
        circleOutline.setRadius(size + SINGLE_DOT_SIZE_INCREASE);
        circleOutline.setFillColor(selectColor);
        window.draw(circleOutline);
    }

    window.draw(circle);
}
