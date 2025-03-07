/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 05:39:05 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/07 06:11:34 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

int close_window(void *param)
{
	(void)param;
	exit(0);
}

int key_press(int keycode, t_game *game)
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
int key_release(int keycode, t_game *game)
{
	if (keycode == KEY_W || keycode == KEY_S)
		game->player.walk_direction = 0;
	else if (keycode == KEY_A || keycode == KEY_D)
		game->player.strafe_direction = 0;
	else if (keycode == KEY_LEFT || keycode == KEY_RIGHT)
		game->player.turn_direction = 0;
	return (0);
}

int has_wall_at(float x, float y, t_map *map)
{
	int mapX;
	int mapY;

	mapX = (int)(x / TILE_SIZE);
	mapY = (int)(y / TILE_SIZE);
	return (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) ? 1 : (map->grid[mapY][mapX] == '1');
}

int can_move_to(float newX, float newY, t_game *game)
{
	// Check all corners of the player's bounding box
	return (!has_wall_at(newX + game->player.radius, newY + game->player.radius,
						 &game->map) &&
			!has_wall_at(newX - game->player.radius, newY + game->player.radius, &game->map) && !has_wall_at(newX + game->player.radius, newY - game->player.radius, &game->map) && !has_wall_at(newX - game->player.radius, newY - game->player.radius, &game->map));
}
float normalize_angle(float angle)
{
	angle = fmod(angle, 2 * M_PI);
	if (angle < 0)
		angle = (2 * M_PI) + angle;
	return (angle);
}
void update_player(t_game *game)
{
	float moveStep;
	float strafeStep;
	float newPlayerX;
	float newPlayerY;

	moveStep = game->player.walk_direction * game->player.move_speed;
	strafeStep = game->player.strafe_direction * game->player.move_speed;
	newPlayerX = game->player.x + cos(game->player.rotation_angle) * moveStep - sin(game->player.rotation_angle) * strafeStep;
	newPlayerY = game->player.y + sin(game->player.rotation_angle) * moveStep + cos(game->player.rotation_angle) * strafeStep;
	if (can_move_to(newPlayerX, newPlayerY, game))
	{
		game->player.x = newPlayerX;
		game->player.y = newPlayerY;
	}
	game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
	game->player.rotation_angle = fmod(game->player.rotation_angle, 2 * M_PI);
}
void set_ray_directions(t_ray *ray)
{
	ray->is_facing_down = ray->ray_angle > 0 && ray->ray_angle < M_PI;
	ray->is_facing_up = !ray->is_facing_down;
	ray->is_facing_right = ray->ray_angle < 0.5 * M_PI || ray->ray_angle > 1.5 * M_PI;
	ray->is_facing_left = !ray->is_facing_right;
}

float get_distance(t_intercept_data intercept_data, t_game *game)
{
	float player_x;
	float player_y;
	float hit_x;
	float hit_y;
	int found_wall;

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

void calculate_check_coordinates(int direction, t_intercept_data *data, float *check_x, float *check_y)
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
void perform_DDA(t_game *game, t_intercept_data *data, int direction)
{
	float check_x, check_y;

	while (data->next_touch_x >= 0 && data->next_touch_x <= SCREEN_WIDTH &&
		   data->next_touch_y >= 0 && data->next_touch_y <= SCREEN_HEIGHT)
	{
		calculate_check_coordinates(direction, data, &check_x, &check_y);

		if (has_wall_at(check_x, check_y, &game->map))
		{
			data->hit_x = data->next_touch_x;
			data->hit_y = data->next_touch_y;
			data->found_wall = 1;
			break;
		}
		data->next_touch_x += data->xstep;
		data->next_touch_y += data->ystep;
	}
}
void cast_horizontal_ray(t_game *game, t_ray ray, t_intercept_data *data)
{
	data->hit_x = 0;
	data->hit_y = 0;
	data->found_wall = 0;
	data->yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
	if (ray.is_facing_down)
		data->yintercept += TILE_SIZE;
	data->xintercept = game->player.x + (data->yintercept - game->player.y) / tan(ray.ray_angle);
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
	perform_DDA(game, data, HORIZONTAL);
}

void cast_vertical_ray(t_game *game, t_ray ray, t_intercept_data *data)
{
	data->hit_x = 0;
	data->hit_y = 0;
	data->found_wall = 0;
	data->xintercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
	if (ray.is_facing_right)
		data->xintercept += TILE_SIZE;
	data->yintercept = game->player.y + (data->xintercept - game->player.x) * tan(ray.ray_angle);
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
	perform_DDA(game, data, VERTICAL);
}

t_ray cast_ray(t_game *game, float ray_angle)
{
	t_ray ray;
	float horz_distance;
	float vert_distance;
	t_intercept_data horz_intercept_data;
	t_intercept_data vert_intercept_data;

	ray.ray_angle = normalize_angle(ray_angle);
	set_ray_directions(&ray);
	cast_horizontal_ray(game, ray, &horz_intercept_data);
	cast_vertical_ray(game, ray, &vert_intercept_data);
	horz_distance = get_distance(horz_intercept_data, game);
	vert_distance = get_distance(vert_intercept_data, game);
	if (vert_distance < horz_distance)
	{
		ray.wall_hit_x = vert_intercept_data.hit_x;
		ray.wall_hit_y = vert_intercept_data.hit_y;
		ray.distance = vert_distance;
		ray.was_hit_vertical = 1;
	}
	else
	{
		ray.wall_hit_x = horz_intercept_data.hit_x;
		ray.wall_hit_y = horz_intercept_data.hit_y;
		ray.distance = horz_distance;
		ray.was_hit_vertical = 0;
	}
	return (ray);
}
void show_data_of_ray(int i, t_game *game)
{
	printf("Ray %d", i);
	printf(" angle: %f rad", game->rays[i].ray_angle);
	printf(", %f deg", game->rays[i].ray_angle * (180 / M_PI));
	printf(", tan(angel) = %f\n", tan(game->rays[i].ray_angle));
}
// this is for debugging it does not affect the final result
void show_data_of_rays(t_game *game)
{
	for (int i = 0; i < NUM_RAYS; i++)
	{
		show_data_of_ray(i, game);
	}
}

void cast_all_rays(t_game *game)
{
	float ray_angle;

	ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);
	for (int i = 0; i < NUM_RAYS; i++)
	{
		game->rays[i] = cast_ray(game, ray_angle);
		ray_angle += FOV_ANGLE / NUM_RAYS;
	}
}

void render_game_in_3D(t_game *game)
{
	int wall_strip_width;
	t_ray ray;
	float perp_distance;
	int wall_strip_height;
	int wall_top_pixel;
	int wall_bottom_pixel;
	int pixel;
	int color;

	wall_strip_width = SCREEN_WIDTH / NUM_RAYS;
	int ceiling_color = 0x87CEEB; // Sky blue color
	int floor_color = 0x8B4513;	  // Brown color for floor
	for (int i = 0; i < NUM_RAYS; i++)
	{
		ray = game->rays[i];
		// Calculate the distance to the projection plane
		perp_distance = ray.distance * cos(ray.ray_angle - game->player.rotation_angle);
		// Calculate the height of the wall strip
		wall_strip_height = (int)((TILE_SIZE / perp_distance) * DIST_PROJ_PLANE);
		// Calculate the top and bottom pixel of the wall strip
		wall_top_pixel = (SCREEN_HEIGHT / 2) - (wall_strip_height / 2);
		wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;
		wall_bottom_pixel = (SCREEN_HEIGHT / 2) + (wall_strip_height / 2);
		wall_bottom_pixel = wall_bottom_pixel > SCREEN_HEIGHT ? SCREEN_HEIGHT : wall_bottom_pixel;
		// Draw ceiling (from top of screen to top of wall)
		for (int y = 0; y < wall_top_pixel; y++)
		{
			pixel = (y * game->size_line) + (i * wall_strip_width * (game->bpp / 8));
			game->img_data[pixel] = ceiling_color & 0xFF;			  // Blue
			game->img_data[pixel + 1] = (ceiling_color >> 8) & 0xFF;  // Green
			game->img_data[pixel + 2] = (ceiling_color >> 16) & 0xFF; // Red
		}
		// Draw the wall strip
		for (int y = wall_top_pixel; y < wall_bottom_pixel; y++)
		{
			color = ray.was_hit_vertical ? 0xFF0000 : 0x00FF00;
			// Red for vertical hits, green for horizontal hits
			pixel = (y * game->size_line) + (i * wall_strip_width * (game->bpp / 8));
			game->img_data[pixel] = color & 0xFF;			  // Blue
			game->img_data[pixel + 1] = (color >> 8) & 0xFF;  // Green
			game->img_data[pixel + 2] = (color >> 16) & 0xFF; // Red
		}
		// Draw floor (from bottom of wall to bottom of screen)
		for (int y = wall_bottom_pixel; y < SCREEN_HEIGHT; y++)
		{
			pixel = (y * game->size_line) + (i * wall_strip_width * (game->bpp / 8));
			game->img_data[pixel] = floor_color & 0xFF;				// Blue
			game->img_data[pixel + 1] = (floor_color >> 8) & 0xFF;	// Green
			game->img_data[pixel + 2] = (floor_color >> 16) & 0xFF; // Red
		}
	}
}

int game_loop(t_game *game)
{
	mlx_clear_window(game->mlx, game->win);
	// Update and redraw the player
	update_player(game);
	// Cast and draw rays
	cast_all_rays(game);
	show_data_of_rays(game);
	// your part 2: Put the image on walls and the floor and ceiling using testure mapping
	// I think all your code will be in this function
	render_game_in_3D(game);
	render_minimap(game);
	// Put the updated image onto the window
	mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
	return (0);
}

void init_game(t_game *game)
{
	char initial_map[MAP_HEIGHT][MAP_WIDTH] = {{'1', '1', '1', '1', '1', '1',
												'1', '1', '1', '1', '1', '1', '1', '1', '1'},
											   {'1', '0', '0', '0',
												'0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1'},
											   {'1', '0',
												'0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1'},
											   {'1', '1', '1', '0', '1', '0', '0', '0', '0', '0', '1', '0', '1',
												'0', '1'},
											   {'1', '0', '0', '0', '0', '1', '0', '0', '0', '0', '1',
												'0', '1', '0', '1'},
											   {'1', '0', '1', '0', '0', '0', '0', '1', '1',
												'1', '1', '1', '1', '0', '1'},
											   {'1', '0', '0', 'P', '0', '0', '0',
												'0', '0', '0', '0', '0', '0', '0', '1'},
											   {'1', '0', '0', '0', '0',
												'0', '0', '0', '0', '0', '0', '0', '0', '0', '1'},
											   {'1', '1', '1',
												'1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '0', '1'},
											   {'1',
												'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
												'1'},
											   {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
												'1', '1', '1'}};

	game->mlx = mlx_init();
	game->win = mlx_new_window(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT,
							   "Raycaster");
	// Initialize player
	game->player = (t_player){.radius = 3, .move_speed = 1, .rotation_angle = M_PI / 2, .turn_direction = 0, .walk_direction = 0, .strafe_direction = 0, .rotation_speed = 0.4 * (M_PI / 180)};
	// Initialize map for testing
	for (int i = 0; i < MAP_HEIGHT; i++)
		for (int j = 0; j < MAP_WIDTH; j++)
			if (initial_map[i][j] == 'P')
			{
				game->player.x = j * TILE_SIZE + TILE_SIZE / 2;
				game->player.y = i * TILE_SIZE + TILE_SIZE / 2;
				initial_map[i][j] = 0;
			}
	for (int i = 0; i < MAP_HEIGHT; i++)
		for (int j = 0; j < MAP_WIDTH; j++)
			game->map.grid[i][j] = initial_map[i][j];
	// Create a new image
	game->img = mlx_new_image(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	game->img_data = mlx_get_data_addr(game->img, &game->bpp, &game->size_line,
									   &game->endian);
}
