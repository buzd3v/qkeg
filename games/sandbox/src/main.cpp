#include <Game.h>

int main()
{

    Game game{};
    game.init({});

    game.run();
    game.cleanup();
    return 0;
}