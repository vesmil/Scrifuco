#define NOMINMAX

#include <fstream>
#include <windows.h>
#include <commdlg.h>
#include <iostream>

#include "Constants.h"
#include "Editor/DataManagement.h"

void FileManagement::NewFile() {
    data_->graphics.Clear();
    data_->text.Clear();
    data_->textCursor.Set({0, 0});

    filename = "";
}

bool FileManagement::SaveFile() {
    if (filename.empty()) filename = SaveFileDialog(DOCUMENT_FORMAT);
    if (filename.empty()) return false;

    data_->saved = true;

    std::ofstream output(filename, std::ofstream::out);

    if (output.is_open()) {
        for (auto &&Row: data_->text.rows) {
            output << Row.toAnsiString() << '\n';
        }

        output << "\n_.__.__.__.__._\n";

        for (auto &&Curve: data_->graphics.curves) {
            output << Curve.color.toInteger() << ' ' << Curve.size << ' ';

            for (auto &&point: Curve.points) {
                output << point.x << ' ' << point.y << ' ';
            }

            output << '|' << '\n';
        }

        output.close();
        return true;
    }

    return false;
}

bool FileManagement::OpenFile() {
    data_->graphics.Clear();
    data_->text.rows.clear();
    data_->textCursor.Set({0, 0});

    if (filename.empty()) filename = OpenFileDialog();
    if (filename.empty()) return false;

    std::ifstream input(filename, std::ifstream::in);

    if (input.is_open()) {
        std::string row;

        while (std::getline(input, row)) {
            if (row == "_.__.__.__.__._") break;
            else data_->text.rows.emplace_back(sf::String(row));
        }

        try {
            std::string word;
            while (input >> word) {
                sf::Color color{std::stoul(word)};

                input >> word;
                float size = std::stof(word);

                data_->graphics.StartCurve(size, color);

                while (true) {
                    input >> word;
                    if (word == "|") break;

                    float x = std::stof(word);
                    input >> word;
                    float y = std::stof(word);

                    data_->graphics.AddPoint({x, y});
                }
            }
            return true;
        }
        catch (...) {
            data_->text.rows.emplace_back("[Error while opening file]");
            std::cerr << "Error while opening file" << std::endl;
            return false;
        }
    }

    return false;
}

std::string FileManagement::OpenFileDialog(const CHAR *title, const CHAR *filter) {
    char filename[ MAX_PATH ];

    OPENFILENAME ofn;
    ZeroMemory( &filename, sizeof( filename ) );
    ZeroMemory( &ofn,      sizeof( ofn ) );
    ofn.lStructSize  = sizeof( ofn );
    ofn.hwndOwner    = nullptr;
    ofn.lpstrFilter  = filter;
    ofn.lpstrTitle   = title;
    ofn.lpstrFile    = filename;
    ofn.nMaxFile     = MAX_PATH;
    ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA( &ofn )) return filename;

    return "";
}

std::string FileManagement::SaveFileDialog(const char *format){
    OPENFILENAME ofn = { sizeof(OPENFILENAME)};

    char filename[_MAX_PATH] = "";
    const char *szExt = format;

    ofn.hwndOwner = GetConsoleWindow();
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename) / sizeof(filename[0]);
    ofn.lpstrFilter = ofn.lpstrDefExt = szExt;
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) return ofn.lpstrFile;

    return "";
}
