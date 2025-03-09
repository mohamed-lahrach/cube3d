/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 00:59:58 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/09 01:08:08 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

float	normalize_angle(float angle)
{
	angle = fmod(angle, 2 * M_PI);
	if (angle < 0)
		angle = (2 * M_PI) + angle;
	return (angle);
}

void	set_ray_directions(t_ray *ray)
{
	ray->is_facing_down = ray->ray_angle > 0 && ray->ray_angle < M_PI;
	ray->is_facing_up = !ray->is_facing_down;
	ray->is_facing_right = ray->ray_angle < 0.5 * M_PI || ray->ray_angle > 1.5
		* M_PI;
	ray->is_facing_left = !ray->is_facing_right;
}

float	get_distance(t_intercept_data intercept_data, t_game *game)
{
	float	player_x;
	float	player_y;
	float	hit_x;
	float	hit_y;
	int		found_wall;

	player_x = game->player.x;
	player_y = game->player.y;
	hit_x = intercept_data.hit_x;
	hit_y = intercept_data.hit_y;
	found_wall = intercept_data.found_wall;
	if (found_wall)
		return (sqrt(pow(hit_x - player_x, 2) + pow(hit_y - player_y, 2)));
	else
		return (FLT_MAX);
}

void	select_closest_hit(t_ray *ray, t_game *game, double vert_distance,
		double horz_distance)
{
	if (vert_distance < horz_distance)
	{
		ray->wall_hit_x = game->vert_intercept_data.hit_x;
		ray->wall_hit_y = game->vert_intercept_data.hit_y;
		ray->distance = vert_distance;
		ray->was_hit_vertical = 1;
	}
	else
	{
		ray->wall_hit_x = game->horz_intercept_data.hit_x;
		ray->wall_hit_y = game->horz_intercept_data.hit_y;
		ray->distance = horz_distance;
		ray->was_hit_vertical = 0;
	}
}

void	calculate_check_coordinates(int direction, t_intercept_data *data,
		float *check_x, float *check_y)
{
	if (direction == HORIZONTAL)
	{
		*check_x = data->next_touch_x;
		*check_y = data->next_touch_y - data->subtract_one;
	}
	else if (direction == VERTICAL)
	{
		*check_x = data->next_touch_x - data->subtract_one;
		*check_y = data->next_touch_y;
	}
}
