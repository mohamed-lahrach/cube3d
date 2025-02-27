#include "raycasting.h"

int main()
{
    t_game game;
    init_game(&game);

    mlx_hook(game.win, 2, 1L << 0, key_press, &game);
    mlx_hook(game.win, 3, 1L << 1, key_release, &game);
    mlx_hook(game.win, 17, 0, close_window, NULL);


    // the main loop especially the game_loop function is the most important part
    // where the game raycasting logic is implemented
    // and you also impelement your part (2): draw the 3D walls and the floor and ceiling
    // do not change anything of part 1 all data you need is stored in the game structure
    // do not worry about normenete i will update the code
    // this is the all data that you need to draw the 3D walls and the floor and ceiling
    // There are some samll errors in Part 1, but do not worry about it, it will not affect your part
    // read the code and understand it, if you have any question ask me
    mlx_loop_hook(game.mlx, game_loop, &game);
    mlx_loop(game.mlx);
    return 0;
}
