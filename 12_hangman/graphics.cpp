#include "graphics.h"

#include "asciiArt.h"

#include <curses.h>
#include <string.h>
#include <string>
#include <wctype.h>

Graphics::Graphics() {
    setlocale(LC_ALL, "");
    initscr(); /* initialize the curses library */
    keypad(stdscr, true); /* enable keyboard mapping */
    nonl(); /* tell curses not to do NL->CR/NL on output */
    cbreak(); /* take input chars one at a time, no wait for \n */
    noecho();
    // nodelay(stdscr, true);
    meta(stdscr, true);
    curs_set(0);
    //putenv("ESCDELAY=25");
    // if (has_colors())
    //     initColors();
}
Graphics::~Graphics() noexcept {
    endwin();
}
void Graphics::resetScreen() { clear(); }

void Graphics::drawHangedMan(size_t state, size_t numTries) {
    state = ((MaxTries - 1) * state) / (numTries);
    if (state >= MaxTries)
        state = MaxTries - 1;
    auto& which = HangedmanStages[state];
    size_t i = 0;
    for (auto& line : which) {
        mvaddstr(i, 1, line);
        ++i;
    }
    // mvprintw(1,0,"state: %d\n", state);
}
void Graphics::drawDisplayWord(const std::wstring& letters) {
    constexpr size_t paddingLeft = HangedmanWidth/2;
    constexpr size_t paddingTop = 1;
    size_t i = 0;
    wchar_t wstr[] = {0, 0};
    for (auto l : letters) {
        wstr[0] = towupper(l);
        mvaddwstr(HangedmanHeight + paddingTop, paddingLeft+i*2, wstr);
        ++i;
    }
}
void Graphics::drawWrongLetters(const std::wstring& letters) {
    constexpr size_t padding = 5;
    constexpr size_t perLine = 5;
    size_t i = 0;
    for (auto l : letters) {
        auto x = (i%perLine)*3;
        auto y = i/perLine;
        mvaddch(2 + y, HangedmanWidth+padding+ x, toupper(l));
        ++i;
    }
}


static void drawBox(int x, int y, int width, int height) {
    auto drawTerminatedLine = [] (int x, int y, int width) {
        move(y, x);
        addch('+');
        for (int i = 0; i < width-2; ++i)
            addch('-');
        addch('+');
    };
    drawTerminatedLine(x, y, width);
    drawTerminatedLine(x, y+height-1, width);

    mvvline(y+1, x, '|', height-2);
    mvvline(y+1, x+width-1, '|', height-2);
}

void Graphics::drawChoiceBase(const std::string& title, bool isPause) {
    drawWrongLetters(std::wstring(MaxTries, ' '));
    constexpr auto left = HangedmanWidth+5;
    constexpr auto width = 40;
    constexpr auto height = 8;
    constexpr auto drawAtMid = [](size_t y, const char* str) {
        auto x = left + (width- strlen(str))/2;
        mvaddstr(y, x, str);
    };
    drawBox(left, 0, width, height);
    drawAtMid(2, title.c_str());
    drawAtMid(4, isPause ? " Quit       Resume" :
                           " Quit       Retry ");
    drawAtMid(5,           "   Q          R   ");
}

void Graphics::drawEndScreen(bool won) {
    drawChoiceBase(won ? "YOU WON!" : "GAME  OVER", false);
}
void Graphics::drawPause() {
    drawChoiceBase("Paused", true);
}
