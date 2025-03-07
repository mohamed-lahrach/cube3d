/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 05:39:10 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/07 05:39:33 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

int main()
{
    t_game game;
    init_game(&game);

    mlx_hook(game.win, 2, 1L << 0, key_press, &game);
    mlx_hook(game.win, 3, 1L << 1, key_release, &game);
    mlx_hook(game.win, 17, 0, close_window, NULL);


    //  the game_loop function is the most important part
    // where the game raycasting logic is implemented
    // and you also will impelement your part (2)
    //I have drawn the walls ceiling and floor with 
    //colors but you will put an image on the walls and the floor and ceiling using texture mapping
    // do not change anything of part 1 all data you need is stored in the game structure
    // do not worry about normenete i will update the code
    // this is the all data that you need to implement your part
    // There are some samll errors in Part 1, but do not worry about it, it will not affect your part
    // read the code and understand it, if you have any question ask me
    mlx_loop_hook(game.mlx, game_loop, &game);
    mlx_loop(game.mlx);
    return 0;
}
