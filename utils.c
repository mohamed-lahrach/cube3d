/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 00:20:10 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/09 00:51:49 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

void	initialize_player_position(t_game *game,
		char initial_map[MAP_HEIGHT][MAP_WIDTH])
{
	int	i;
	int	j;

	i = 0;
	while (i < MAP_HEIGHT)
	{
		j = 0;
		while (j < MAP_WIDTH)
		{
			if (initial_map[i][j] == 'P')
			{
				game->player.x = j * TILE_SIZE + TILE_SIZE / 2;
				game->player.y = i * TILE_SIZE + TILE_SIZE / 2;
				initial_map[i][j] = 0;
			}
			j++;
		}
		i++;
	}
}

void	initialize_map_grid(t_game *game,
		char initial_map[MAP_HEIGHT][MAP_WIDTH])
{
	int	i;
	int	j;

	i = 0;
	while (i < MAP_HEIGHT)
	{
		j = 0;
		while (j < MAP_WIDTH)
		{
			game->map.grid[i][j] = initial_map[i][j];
			j++;
		}
		i++;
	}
}

int	has_wall_at(float x, float y, t_map *map)
{
	int	map_x;
	int	map_y;

	map_x = (int)(x / TILE_SIZE);
	map_y = (int)(y / TILE_SIZE);
	if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT)
		return (1);
	return (map->grid[map_y][map_x] == '1');
}

int	can_move_to(float newX, float newY, t_game *game)
{
	if (has_wall_at(newX + game->player.radius, newY + game->player.radius,
			&game->map))
		return (0);
	if (has_wall_at(newX - game->player.radius, newY + game->player.radius,
			&game->map))
		return (0);
	if (has_wall_at(newX + game->player.radius, newY - game->player.radius,
			&game->map))
		return (0);
	if (has_wall_at(newX - game->player.radius, newY - game->player.radius,
			&game->map))
		return (0);
	return (1);
}
