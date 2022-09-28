cmake_minimum_required(VERSION 3.16)

project(Scrifuco)
set(CMAKE_CXX_STANDARD 17)

set(Scrifuco_source src/Main.cpp src/Menu.cpp headers/Menu.h headers/Constants.h headers/DisplayElement.h src/Editor/Data/DataManagement.cpp headers/Editor/DataManagement.h src/Editor/EditorWindow.cpp headers/Editor/EditorWindow.h src/Editor/Editor.cpp headers/Editor/Editor.h src/Editor/Data/FileManagement.cpp src/Editor/EventHandlers/CommandHandler.cpp src/Editor/EventHandlers/InsertHandler.cpp src/Editor/EventHandlers/TextHandler.cpp src/Editor/Data/TextCursor.cpp headers/Editor/TextCursor.h headers/Editor/EventHandlers.h src/Settings.cpp src/DisplayElements.cpp src/Menu.cpp headers/Menu.h src/Editor/Data/DataRepresentation.cpp headers/Editor/DataRepresentation.h src/Library.cpp headers/Library.h)

set(SelbaWard_source ext/SelbaWardSplines/SelbaWard/Spline.cpp ext/SelbaWardSplines/SelbaWard/Spline.hpp ext/SelbaWardSplines/SelbaWard/Common.hpp)

if (WIN32)
    add_executable(Scrifuco WIN32 ${Scrifuco_source})
    set(SFML_ROOT "${CMAKE_CURRENT_LIST_DIR}/ext/SFML")
    include_directories(headers "${CMAKE_CURRENT_LIST_DIR}/ext/SFML/include")
else ()
    add_executable(Scrifuco ${Scrifuco_source})
    include_directories(headers /usr/include)
endif ()

add_library(SelbaWard ${SelbaWard_source})
include_directories(headers "${CMAKE_CURRENT_LIST_DIR}/ext/SelbaWardSplines")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED audio system window graphics)

if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(SelbaWard ${SFML_LIBRARIES})
    target_link_libraries(Scrifuco ${SFML_LIBRARIES} SelbaWard)
endif ()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy dlls to build
if (WIN32)
    file(GLOB BINARY_DEP_DLLS "${SFML_INCLUDE_DIR}/../bin/*.dll")
    file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
    file(GLOB MINGW_DEP_DLLS "C:/mingw64/bin/*.dll")
    file(COPY ${MINGW_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
endif ()
