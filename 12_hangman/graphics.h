#pragma once

#include "asciiArt.h"

#include <curses.h>
#include <string.h>
#include <string>
#include <wctype.h>

class Graphics {
    Graphics();

public:
    Graphics(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics& operator=(Graphics&&) = delete;
    ~Graphics() noexcept;

    static Graphics& instance() {
        static Graphics i;
        return i;
    }

    void resetScreen();
    void drawHangedMan(size_t state, size_t numTries);
    void drawDisplayWord(const std::wstring& letters);
    void drawWrongLetters(const std::wstring& letters);
    void drawEndScreen(bool won);
    void drawPause();

private:
    void drawChoiceBase(const std::string& title, bool isPause);
};
