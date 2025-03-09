/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 01:10:43 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/09 01:10:44 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

int	close_window(void *param)
{
	(void)param;
	exit(0);
}

int	key_press(int keycode, t_game *game)
{
	if (keycode == KEY_W)
		game->player.walk_direction = 1;
	else if (keycode == KEY_S)
		game->player.walk_direction = -1;
	else if (keycode == KEY_A)
		game->player.strafe_direction = -1;
	else if (keycode == KEY_D)
		game->player.strafe_direction = 1;
	else if (keycode == KEY_LEFT)
		game->player.turn_direction = -1;
	else if (keycode == KEY_RIGHT)
		game->player.turn_direction = 1;
	else if (keycode == KEY_ESC)
		exit(0);
	return (0);
}
int	key_release(int keycode, t_game *game)
{
	if (keycode == KEY_W || keycode == KEY_S)
		game->player.walk_direction = 0;
	else if (keycode == KEY_A || keycode == KEY_D)
		game->player.strafe_direction = 0;
	else if (keycode == KEY_LEFT || keycode == KEY_RIGHT)
		game->player.turn_direction = 0;
	return (0);
}