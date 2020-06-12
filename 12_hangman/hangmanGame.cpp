#include "hangmanGame.h"

#include <iostream>
#include <vector>

// static size_t i = 0;
// static void log(const char* msg, int J) {
//     mvprintw(i % LINES, 40, "%2i %s %d", i, msg, J);
//     ++i;
// }

void HangmanGame::initWord(const std::wstring& word) {
    wrong.clear();
    this->word = word;
    std::fill(display.begin(), display.end(), '_');
    display.resize(word.size(), '_');
}

void HangmanGame::checkLetter(wchar_t l) {
    if (wrong.find(l) != std::string::npos) return;
    if (display.find(l) != std::string::npos) return;

    auto pos = word.find(l, 0);
    if (pos == std::string::npos) {
        wrong.push_back(l);
        checkEndOfGame();
        return;
    }
    do {
        display[pos] = l;
        pos = word.find(l, pos+1);

    } while (pos != std::string::npos);
    checkWin();
}
void HangmanGame::checkEndOfGame() {
    if (wrong.size() >= numTries) {
        display = word;
        gameover(false);
    }
}
void HangmanGame::checkWin() {
    if (display.find('_') == std::string::npos) {
        gameover(true);
    }
}
void HangmanGame::gameover(bool won) {
    hasWon_ = won;
    running_ = false;
}

void HangmanGame::input() {
    int c;
    switch ((c = getch())) {
    case KEY_F(1):
    case 27:
        pause();
    break;
    default: {
        wchar_t chr = c;
        auto readExtra = [] { return getch() & 0b0011'1111; };
        if (c & 0x80) {
            if ((c & 0b1110'0000) == 0b1100'0000) { //110x'xxxx - 2 bytes
                auto b1 = c & 0b0001'1111;
                auto b2 = readExtra();
                chr = (b1 << 6) | b2;
            }
            else if ((c & 0b1111'0000) == 0b1110'0000) { //1110'xxxx - 3 bytes
                auto b1 = c & 0b0000'1111;
                auto b2 = readExtra();
                auto b3 = readExtra();
                chr = (b1 << 6*2) | (b2 << 6) | b3;
            }
            else if ((c & 0b1111'1000) == 0b1111'0000) { //1111'0xxx - 4 bytes
                auto b1 = c & 0b0000'0111;
                auto b2 = readExtra();
                auto b3 = readExtra();
                auto b4 = readExtra();
                chr = (b1 << 6*3) | (b2 << 6*2) | (b3 <<6) | b4;
            }
        }
        if (iswalpha(chr)) {
            checkLetter(chr);
        }
    } break;
    }
}
void HangmanGame::updateGraphics() {
    g.drawHangedMan(wrong.size(), numTries);
    g.drawWrongLetters(wrong);
    g.drawDisplayWord(display);
}

void HangmanGame::initWord() {
    initWord(gen.genWord());
}
void HangmanGame::drawEndScreen() {
    updateGraphics();
    g.drawEndScreen(hasWon_);
    keyChoice('q', [] { exit(0); },
              'r', [this] { reset(); });
}
void HangmanGame::reset() {
    running_ = true;
    g.resetScreen();
    initWord();
}
void HangmanGame::pause() {
    g.drawPause();
    keyChoice('q', [] { exit(0); },
              'r', [this] { g.resetScreen(); });
}

void HangmanGame::keyChoice(int a, auto cbA, int b, auto cbB) {
    for (;;) {
        auto c = tolower(getch());
        if (c == a) {
            cbA();
            return;
        }
        if (c == b) {
            cbB();
            return;
        }
        // waitAFrame();
    }
}

void HangmanGame::readFile(const char* path) {
    gen.readFile(path, numTries);
}

