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
    return 0;
}
int key_release(int keycode, t_game *game)
{
    if (keycode == KEY_W || keycode == KEY_S)
        game->player.walk_direction = 0;
    else if (keycode == KEY_A || keycode == KEY_D)
        game->player.strafe_direction = 0;
    else if (keycode == KEY_LEFT || keycode == KEY_RIGHT)
        game->player.turn_direction = 0;
    return 0;
}

int has_wall_at(float x, float y, t_map *map)
{
    int mapX = (int)(x / TILE_SIZE);
    int mapY = (int)(y / TILE_SIZE);
    return (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) ? 1 : (map->grid[mapY][mapX] == '1');
}

int can_move_to(float newX, float newY, t_game *game)
{
    // Check all corners of the player's bounding box
    return !has_wall_at(newX + game->player.radius, newY + game->player.radius, &game->map) &&
           !has_wall_at(newX - game->player.radius, newY + game->player.radius, &game->map) &&
           !has_wall_at(newX + game->player.radius, newY - game->player.radius, &game->map) &&
           !has_wall_at(newX - game->player.radius, newY - game->player.radius, &game->map);
}
float normalize_angle(float angle)
{
    angle = fmod(angle, 2 * M_PI);
    if (angle < 0)
        angle = (2 * M_PI) + angle;
    return angle;
}
void update_player(t_game *game)
{
    float moveStep = game->player.walk_direction * game->player.move_speed;
    float strafeStep = game->player.strafe_direction * game->player.move_speed;

    float newPlayerX = game->player.x + cos(game->player.rotation_angle) * moveStep - sin(game->player.rotation_angle) * strafeStep;

    float newPlayerY = game->player.y + sin(game->player.rotation_angle) * moveStep + cos(game->player.rotation_angle) * strafeStep;

    // Update the player's position only if the full new position is free of collision.
    if (can_move_to(newPlayerX, newPlayerY, game))
    {
        game->player.x = newPlayerX;
        game->player.y = newPlayerY;
    }
    game->player.rotation_angle += game->player.turn_direction * game->player.rotation_speed;
    game->player.rotation_angle = fmod(game->player.rotation_angle, 2 * M_PI);
}
t_ray cast_ray(t_game *game, float ray_angle)
{
    t_ray ray;
    int is_ray_facing_down;
    int is_ray_facing_up;
    int is_ray_facing_right;
    int is_ray_facing_left;
    int found_horz_wall;
    float horz_hit_x;
    float horz_hit_y;
    float yintercept;
    float xintercept;
    float ystep_h;
    float xstep_h;
    float next_horz_touch_x;
    float next_horz_touch_y;

    ray.ray_angle = normalize_angle(ray_angle);

    // Determine ray directions
    is_ray_facing_down = ray.ray_angle > 0 && ray.ray_angle < M_PI;
    is_ray_facing_up = !is_ray_facing_down;
    is_ray_facing_right = ray.ray_angle < 0.5 * M_PI || ray.ray_angle > 1.5 * M_PI;
    is_ray_facing_left = !is_ray_facing_right;

    /*
    #############################################
    # Horizontal Intersection                   #
    #############################################
    */

    horz_hit_x = 0;
    horz_hit_y = 0;
    found_horz_wall = 0;

    // 1. Find y-coordinate of first horizontal grid intersection
    yintercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += is_ray_facing_down ? TILE_SIZE : 0;

    // 2. Find x-coordinate
    xintercept = game->player.x + (yintercept - game->player.y) / tan(ray.ray_angle);

    // 3. Calculate step increments
    ystep_h = TILE_SIZE;
    ystep_h *= is_ray_facing_up ? -1 : 1;

    xstep_h = TILE_SIZE / tan(ray.ray_angle);
    xstep_h *= (is_ray_facing_left && xstep_h > 0) ? -1 : 1;
    xstep_h *= (is_ray_facing_right && xstep_h < 0) ? -1 : 1;

    // 4. Temporary position variables
    next_horz_touch_x = xintercept;
    next_horz_touch_y = yintercept;

    // 5. Adjust for exact grid lines
    int subtract_one_h = is_ray_facing_up ? 1 : 0;
    // 5. Adjust for exact grid lines

    // 6. DDA algorithm for horizontal check
    while (next_horz_touch_x >= 0 && next_horz_touch_x <= SCREEN_WIDTH &&
           next_horz_touch_y >= 0 && next_horz_touch_y <= SCREEN_HEIGHT)
    {
        if (has_wall_at(next_horz_touch_x, next_horz_touch_y - subtract_one_h, &game->map))
        {
            horz_hit_x = next_horz_touch_x;
            horz_hit_y = next_horz_touch_y;
            found_horz_wall = 1;
            break;
        }
        next_horz_touch_x += xstep_h;
        next_horz_touch_y += ystep_h;
    }

    /*
    #############################################
    # Vertical Intersection                     #
    #############################################
    */
    float vert_hit_x = 0;
    float vert_hit_y = 0;
    int found_vert_wall = 0;

    // 1. Find x-coordinate of first vertical grid intersection
    float xintercept_v = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
    xintercept_v += is_ray_facing_right ? TILE_SIZE : 0;

    // 2. Find y-coordinate
    float yintercept_v = game->player.y + (xintercept_v - game->player.x) * tan(ray.ray_angle);

    // 3. Calculate step increments
    float xstep_v = TILE_SIZE;
    xstep_v *= is_ray_facing_left ? -1 : 1;

    float ystep_v = TILE_SIZE * tan(ray.ray_angle);
    ystep_v *= (is_ray_facing_up && ystep_v > 0) ? -1 : 1;
    ystep_v *= (is_ray_facing_down && ystep_v < 0) ? -1 : 1;

    // 4. Temporary position variables
    float next_vert_touch_x = xintercept_v;
    float next_vert_touch_y = yintercept_v;

    // 5. Adjust for exact grid lines
    int subtract_one_v = is_ray_facing_left ? 1 : 0;

    // 6. DDA algorithm for vertical check
    while (next_vert_touch_x >= 0 && next_vert_touch_x <= SCREEN_WIDTH &&
           next_vert_touch_y >= 0 && next_vert_touch_y <= SCREEN_HEIGHT)
    {
        if (has_wall_at(next_vert_touch_x - subtract_one_v, next_vert_touch_y, &game->map))
        {
            vert_hit_x = next_vert_touch_x;
            vert_hit_y = next_vert_touch_y;
            found_vert_wall = 1;
            break;
        }
        next_vert_touch_x += xstep_v;
        next_vert_touch_y += ystep_v;
    }

    // Calculate distances
    float horz_distance = found_horz_wall ? sqrt(pow(horz_hit_x - game->player.x, 2) + pow(horz_hit_y - game->player.y, 2)) : FLT_MAX;

    float vert_distance = found_vert_wall ? sqrt(pow(vert_hit_x - game->player.x, 2) + pow(vert_hit_y - game->player.y, 2)) : FLT_MAX;

    // Store results
    if (vert_distance < horz_distance)
    {
        ray.wall_hit_x = vert_hit_x;
        ray.wall_hit_y = vert_hit_y;
        ray.distance = vert_distance;
        ray.was_hit_vertical = 1;
    }
    else
    {
        ray.wall_hit_x = horz_hit_x;
        ray.wall_hit_y = horz_hit_y;
        ray.distance = horz_distance;
        ray.was_hit_vertical = 0;
    }

    return ray;
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
    // rotation angle = 90 degrees ; fov = 60 degrees then ray angle = 90 - 60/2 = 60 degrees
    // 60 degrees in randian = 60 * (M_PI / 180) = 1.0472

    float ray_angle = game->player.rotation_angle - (FOV_ANGLE / 2);

    for (int i = 0; i < NUM_RAYS; i++)
    {
        game->rays[i] = cast_ray(game, ray_angle);
        ray_angle += FOV_ANGLE / NUM_RAYS;
    }
}

void render_game_in_3D(t_game *game)
{
    int wall_strip_width = SCREEN_WIDTH / NUM_RAYS;
    int ceiling_color = 0x87CEEB; // Sky blue color
    int floor_color = 0x8B4513;   // Brown color for floor

    for (int i = 0; i < NUM_RAYS; i++)
    {
        t_ray ray = game->rays[i];

        // Calculate the distance to the projection plane
        float perp_distance = ray.distance * cos(ray.ray_angle - game->player.rotation_angle);

        // Calculate the height of the wall strip
        int wall_strip_height = (int)((TILE_SIZE / perp_distance) * DIST_PROJ_PLANE);

        // Calculate the top and bottom pixel of the wall strip
        int wall_top_pixel = (SCREEN_HEIGHT / 2) - (wall_strip_height / 2);
        wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;

        int wall_bottom_pixel = (SCREEN_HEIGHT / 2) + (wall_strip_height / 2);
        wall_bottom_pixel = wall_bottom_pixel > SCREEN_HEIGHT ? SCREEN_HEIGHT : wall_bottom_pixel;

        // Draw ceiling (from top of screen to top of wall)
        for (int y = 0; y < wall_top_pixel; y++)
        {
            int pixel = (y * game->size_line) + (i * wall_strip_width * (game->bpp / 8));
            game->img_data[pixel] = ceiling_color & 0xFF;             // Blue
            game->img_data[pixel + 1] = (ceiling_color >> 8) & 0xFF;  // Green
            game->img_data[pixel + 2] = (ceiling_color >> 16) & 0xFF; // Red
        }

        // Draw the wall strip
        for (int y = wall_top_pixel; y < wall_bottom_pixel; y++)
        {
            int color = ray.was_hit_vertical ? 0xFF0000 : 0x00FF00; // Red for vertical hits, green for horizontal hits
            int pixel = (y * game->size_line) + (i * wall_strip_width * (game->bpp / 8));
            game->img_data[pixel] = color & 0xFF;             // Blue
            game->img_data[pixel + 1] = (color >> 8) & 0xFF;  // Green
            game->img_data[pixel + 2] = (color >> 16) & 0xFF; // Red
        }

        // Draw floor (from bottom of wall to bottom of screen)
        for (int y = wall_bottom_pixel; y < SCREEN_HEIGHT; y++)
        {
            int pixel = (y * game->size_line) + (i * wall_strip_width * (game->bpp / 8));
            game->img_data[pixel] = floor_color & 0xFF;             // Blue
            game->img_data[pixel + 1] = (floor_color >> 8) & 0xFF;  // Green
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

    return 0;
}

void init_game(t_game *game)
{
    game->mlx = mlx_init();
    game->win = mlx_new_window(game->mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster");

    // Initialize player
    game->player = (t_player){
        .radius = 3,
        .move_speed = 1,
        .rotation_angle = M_PI / 2,
        .turn_direction = 0,
        .walk_direction = 0,
        .strafe_direction = 0,
        .rotation_speed = 0.4 * (M_PI / 180)};

    // Initialize map for testing
    char initial_map[MAP_HEIGHT][MAP_WIDTH] = {
        {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
        {'1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1'},
        {'1', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1'},
        {'1', '1', '1', '0', '1', '0', '0', '0', '0', '0', '1', '0', '1', '0', '1'},
        {'1', '0', '0', '0', '0', '1', '0', '0', '0', '0', '1', '0', '1', '0', '1'},
        {'1', '0', '1', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1', '0', '1'},
        {'1', '0', '0', 'P', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'},
        {'1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'},
        {'1', '1', '1', '1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '0', '1'},
        {'1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'},
        {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'}};

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
    game->img_data = mlx_get_data_addr(game->img, &game->bpp, &game->size_line, &game->endian);
}
