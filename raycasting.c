/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 05:39:05 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/09 01:22:19 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

void	perform_dda(t_game *game, t_intercept_data *data, int direction)
{
	float	check_x;
	float	check_y;

	while (data->next_touch_x >= 0 && data->next_touch_x <= SCREEN_WIDTH
		&& data->next_touch_y >= 0 && data->next_touch_y <= SCREEN_HEIGHT)
	{
		calculate_check_coordinates(direction, data, &check_x, &check_y);
		if (has_wall_at(check_x, check_y, &game->map))
		{
			data->hit_x = data->next_touch_x;
			data->hit_y = data->next_touch_y;
			data->found_wall = 1;
			break ;
		}
		data->next_touch_x += data->xstep;
		data->next_touch_y += data->ystep;
	}
}

void	cast_horizontal_ray(t_game *game, t_ray ray, t_intercept_data *data)
{
	data->hit_x = 0;
	data->hit_y = 0;
	data->found_wall = 0;
	data->yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
	if (ray.is_facing_down)
		data->yintercept += TILE_SIZE;
	data->xintercept = game->player.x + (data->yintercept - game->player.y)
		/ tan(ray.ray_angle);
	data->ystep = TILE_SIZE;
	if (ray.is_facing_up)
		data->ystep = -data->ystep;
	data->xstep = TILE_SIZE / tan(ray.ray_angle);
	if (ray.is_facing_left && data->xstep > 0)
		data->xstep = -data->xstep;
	if (ray.is_facing_right && data->xstep < 0)
		data->xstep = -data->xstep;
	data->next_touch_x = data->xintercept;
	data->next_touch_y = data->yintercept;
	if (ray.is_facing_up)
		data->subtract_one = 1;
	else
		data->subtract_one = 0;
	perform_dda(game, data, HORIZONTAL);
}

void	cast_vertical_ray(t_game *game, t_ray ray, t_intercept_data *data)
{
	data->hit_x = 0;
	data->hit_y = 0;
	data->found_wall = 0;
	data->xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
	if (ray.is_facing_right)
		data->xintercept += TILE_SIZE;
	data->yintercept = game->player.y + (data->xintercept - game->player.x)
		* tan(ray.ray_angle);
	data->xstep = TILE_SIZE;
	if (ray.is_facing_left)
		data->xstep *= -1;
	data->ystep = TILE_SIZE * tan(ray.ray_angle);
	if (ray.is_facing_up && data->ystep > 0)
		data->ystep *= -1;
	if (ray.is_facing_down && data->ystep < 0)
		data->ystep *= -1;
	data->next_touch_x = data->xintercept;
	data->next_touch_y = data->yintercept;
	if (ray.is_facing_left)
		data->subtract_one = 1;
	else
		data->subtract_one = 0;
	perform_dda(game, data, VERTICAL);
}

t_ray	cast_ray(t_game *game, float ray_angle)
{
	t_ray				ray;
	float				horz_distance;
	float				vert_distance;
	t_intercept_data	horz_intercept_data;
	t_intercept_data	vert_intercept_data;

	ray.ray_angle = normalize_angle(ray_angle);
	set_ray_directions(&ray);
	cast_horizontal_ray(game, ray, &horz_intercept_data);
	cast_vertical_ray(game, ray, &vert_intercept_data);
	horz_distance = get_distance(horz_intercept_data, game);
	vert_distance = get_distance(vert_intercept_data, game);
	game->vert_intercept_data = vert_intercept_data;
	game->horz_intercept_data = horz_intercept_data;
	select_closest_hit(&ray, game,vert_distance, horz_distance);
	return (ray);
}

void	cast_all_rays(t_game *game)
{
	float	ray_angle;
	int		i;

	ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
	i = 0;
	while (i < NUM_RAYS)
	{
		game->rays[i] = cast_ray(game, ray_angle);
		ray_angle += FOV_ANGLE / NUM_RAYS;
		i++;
	}
}
