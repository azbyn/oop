#pragma once

#include "graphics.h"
#include "fileWordGenerator.h"

#include <fstream>
#include <vector>
#include <random>
#include <sstream>

class HangmanGame {
    // the word being guessed (eg potato)
    std::wstring word;
    // the current word being displayed (eg _ot_t_o)
    std::wstring display;

    // the wrong letter guesses (eg jw)
    std::wstring wrong;

    size_t numTries = 8;

    FileWordGenerator gen;

    void initWord(const std::wstring& word);

    HangmanGame(const HangmanGame&) = delete;
    HangmanGame(HangmanGame&&) = delete;
    HangmanGame& operator=(const HangmanGame&) = delete;
    HangmanGame& operator=(HangmanGame&&) = delete;

    HangmanGame() {}

    bool running_ = true;
    bool hasWon_ = true;
    Graphics& g = Graphics::instance();

    void checkLetter(wchar_t l);
    void checkEndOfGame();
    void checkWin();
    void gameover(bool won);

public:
    static HangmanGame& instance() {
        static HangmanGame i;
        return i;
    }
    constexpr bool running() const { return running_; }
    constexpr bool hasWon() const { return hasWon_; }

    void input();
    void updateGraphics();
    void initWord();
    void drawEndScreen();
    void reset();
    void pause();

    void keyChoice(int a, auto cbA, int b, auto cbB);

public:
    void readFile(const char* path);
};
