/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_2Dmap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 01:10:53 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/09 01:10:54 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

void	draw_rays(t_game *game, float factor)
{
	for (int i = 0; i < NUM_RAYS; i++)
	{
		draw_line(game->img_data, factor * game->player.x, factor
			* game->player.y, factor * game->rays[i].wall_hit_x, factor
			* game->rays[i].wall_hit_y, RED_COLOR, game->size_line, game->bpp,
			0.3);
	}
}
void	draw_square(char *img_data, int x, int y, int size, int color,
		int size_line, int bpp)
{
	int	pixel;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			pixel = (y + i) * size_line + (x + j) * (bpp / 8);
			img_data[pixel] = color & 0xFF;             // Blue
			img_data[pixel + 1] = (color >> 8) & 0xFF;  // Green
			img_data[pixel + 2] = (color >> 16) & 0xFF; // Red
		}
	}
}

void	draw_line(char *img_data, int x0, int y0, int x1, int y1, int color,
		int size_line, int bpp, float alpha)
{
	double			deltaX;
	double			deltaY;
	int				steps;
	double			xIncrement;
	double			yIncrement;
	double			x;
	double			y;
	int				pixel;
	unsigned char	bg_b;
	unsigned char	bg_g;
	unsigned char	bg_r;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	new_r;
	unsigned char	new_g;
	unsigned char	new_b;

	deltaX = x1 - x0;
	deltaY = y1 - y0;
	steps = fabs(deltaX) > fabs(deltaY) ? fabs(deltaX) : fabs(deltaY);
	xIncrement = deltaX / steps;
	yIncrement = deltaY / steps;
	x = x0;
	y = y0;
	for (int i = 0; i <= steps; i++)
	{
		pixel = (int)y * size_line + (int)x * (bpp / 8);
		// Get the background color
		bg_b = img_data[pixel];
		bg_g = img_data[pixel + 1];
		bg_r = img_data[pixel + 2];
		// Extract the color components
		r = (color >> 16) & 0xFF;
		g = (color >> 8) & 0xFF;
		b = color & 0xFF;
		// Blend the colors
		new_r = (unsigned char)((r * alpha) + (bg_r * (1 - alpha)));
		new_g = (unsigned char)((g * alpha) + (bg_g * (1 - alpha)));
		new_b = (unsigned char)((b * alpha) + (bg_b * (1 - alpha)));
		// Set the new color
		img_data[pixel] = new_b;     // Blue
		img_data[pixel + 1] = new_g; // Green
		img_data[pixel + 2] = new_r; // Red
		x += xIncrement;
		y += yIncrement;
	}
}

void	draw_circle(char *img_data, int x0, int y0, int radius, int color,
		int size_line, int bpp)
{
	int	pixel;

	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			if (x * x + y * y <= radius * radius)
			{
				pixel = (y0 + y) * size_line + (x0 + x) * (bpp / 8);
				img_data[pixel] = color & 0xFF;             // Blue
				img_data[pixel + 1] = (color >> 8) & 0xFF;  // Green
				img_data[pixel + 2] = (color >> 16) & 0xFF; // Red
			}
		}
	}
}

void	render_player(t_game *game)
{
	draw_circle(game->img_data, game->player.x, game->player.y,
		game->player.radius, RED_COLOR, game->size_line, game->bpp);
}

void	render_minimap(t_game *game)
{
	// Calculate the size of the mini-map (20% of the original map size)
	int minimap_width = (int)(MAP_WIDTH * TILE_SIZE * MINIMAP_SCALE_FACTOR);
	int minimap_height = (int)(MAP_HEIGHT * TILE_SIZE * MINIMAP_SCALE_FACTOR);

	// Calculate the size of each tile in the mini-map
	int minimap_tile_size = (int)(TILE_SIZE * MINIMAP_SCALE_FACTOR);

	// Draw the mini-map grid
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			// Calculate the position of the tile in the mini-map
			int minimap_x = j * minimap_tile_size;
			int minimap_y = i * minimap_tile_size;

			// Determine the color of the tile
			int color = (game->map.grid[i][j] == '1') ? GRAY_COLOR : BLACK_COLOR;

			// Draw the tile as a small square in the mini-map
			draw_square(game->img_data, minimap_x, minimap_y, minimap_tile_size,
				color, game->size_line, game->bpp);
		}
	}

	// Draw the player in the mini-map
	int player_minimap_x = (int)(game->player.x * MINIMAP_SCALE_FACTOR);
	int player_minimap_y = (int)(game->player.y * MINIMAP_SCALE_FACTOR);
	int player_minimap_radius = (int)(game->player.radius
			* MINIMAP_SCALE_FACTOR);

	draw_circle(game->img_data, player_minimap_x, player_minimap_y,
		player_minimap_radius, RED_COLOR, game->size_line, game->bpp);

	// Draw the rays in the mini-map
	for (int i = 0; i < NUM_RAYS; i++)
	{
		// Scale the ray's start and end points to the mini-map size
		if (1)
		{
			int ray_start_x = player_minimap_x;
			int ray_start_y = player_minimap_y;
			int ray_end_x = (int)(game->rays[i].wall_hit_x
					* MINIMAP_SCALE_FACTOR);
			int ray_end_y = (int)(game->rays[i].wall_hit_y
					* MINIMAP_SCALE_FACTOR);

			// Draw the ray as a line in the mini-map
			draw_line(game->img_data, ray_start_x, ray_start_y, ray_end_x,
				ray_end_y, RED_COLOR, game->size_line, game->bpp, 0.3);
		}
	}
}