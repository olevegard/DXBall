SOURCES += ../main.cpp
SOURCES += ../structs/game_objects/GamePiece.cpp
SOURCES += ../structs/game_objects/Tile.cpp
SOURCES += ../structs/game_objects/Ball.cpp
SOURCES += ../structs/game_objects/Paddle.cpp
SOURCES += ../structs/game_objects/BonusBox.cpp
SOURCES += ../structs/game_objects/Bullet.cpp
SOURCES += ../structs/board/Board.cpp
SOURCES += ../structs/board/TilePosition.cpp
SOURCES += ../structs/net/TCPMessage.cpp
SOURCES += ../structs/net/TCPConnection.cpp
SOURCES += ../tools/RenderTools.cpp
SOURCES += ../math/Vector2f.cpp
SOURCES += ../math/Rect.cpp
SOURCES += ../Timer.cpp
SOURCES += ../Renderer.cpp
SOURCES += ../GameManager.cpp
SOURCES += ../BoardLoader.cpp
SOURCES += ../MenuManager.cpp
SOURCES += ../NetManager.cpp
SOURCES += ../ColorConfigLoader.cpp
SOURCES += ../ConfigLoader.cpp
SOURCES += ../MenuList.cpp

DIST =
TARGET = ../DXBall_exe

CONFIG -= qt

LIBS = -lSDL2
LIBS += -lSDL2_net
LIBS += -lSDL2_ttf
LIBS += -lSDL2_image

QMAKE_CXXFLAGS = -std=c++11

QMAKE_CXXFLAGS += -Weverything
QMAKE_CXXFLAGS += -Wno-c++98-compat
QMAKE_CXXFLAGS += -Wno-c++98-compat-pedantic
QMAKE_CXXFLAGS += -Wno-padded
QMAKE_CXXFLAGS += -Wno-switch-enum
QMAKE_CXXFLAGS += -Wno-float-equal
QMAKE_CXXFLAGS += -Wno-unused-function
QMAKE_CXXFLAGS += -Wno-vla
QMAKE_CXXFLAGS += -Wno-vla-extension
QMAKE_CXXFLAGS += -Wno-sign-conversion
QMAKE_CXXFLAGS += -Wno-exit-time-destructors

QMAKE_CXXFLAGS += -Wextra
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -pedantic

QMAKE_CXXFLAGS += -Werror

QMAKE_CXXFLAGS += -g

QMAKE_C = clang
QMAKE_CXX = clang++
QMAKE_LINK = clang++

OBJECTS_DIR = build/.obj/
