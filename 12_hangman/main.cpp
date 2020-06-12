#include "hangmanGame.h"

int main(int argc, char* argv[]) {
    auto& game = HangmanGame::instance();
    const char* path = argc >= 2 ? argv[1] : "cuvinte.txt";

    game.readFile(path);

    game.initWord();
    for (;;) {
        while (game.running()) {
            game.updateGraphics();
            game.input();
        }
        game.drawEndScreen();
    }

    return 0;
}
