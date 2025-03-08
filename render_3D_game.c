#include "raycasting.h"
void draw_ceiling(t_game *game, int start_y, int end_y, int ray_index, int color)
{
    int wall_strip_width = SCREEN_WIDTH / NUM_RAYS;
    for (int y = start_y; y < end_y; y++)
    {
        int pixel = (y * game->size_line) + (ray_index * wall_strip_width * (game->bpp / 8));
        game->img_data[pixel] = color & 0xFF;              // Blue
        game->img_data[pixel + 1] = (color >> 8) & 0xFF;       // Green
        game->img_data[pixel + 2] = (color >> 16) & 0xFF;      // Red
    }
}

void draw_wall_strip(t_game *game, int wall_top, int wall_bottom, int ray_index)
{
    int wall_strip_width = SCREEN_WIDTH / NUM_RAYS;
    t_ray ray = game->rays[ray_index];
    int color;

    if (ray.was_hit_vertical)
        color = 0xFF0000; 
    else
        color = 0x00FF00;

    for (int y = wall_top; y < wall_bottom; y++)
    {
        int pixel = (y * game->size_line) + (ray_index * wall_strip_width * (game->bpp / 8));
        game->img_data[pixel] = color & 0xFF;             
        game->img_data[pixel + 1] = (color >> 8) & 0xFF;
        game->img_data[pixel + 2] = (color >> 16) & 0xFF;
    }
}

void draw_floor(t_game *game, int start_y, int end_y, int ray_index, int color)
{
    int wall_strip_width = SCREEN_WIDTH / NUM_RAYS;
    for (int y = start_y; y < end_y; y++)
    {
        int pixel = (y * game->size_line) + (ray_index * wall_strip_width * (game->bpp / 8));
        game->img_data[pixel] = color & 0xFF;
        game->img_data[pixel + 1] = (color >> 8) & 0xFF;
        game->img_data[pixel + 2] = (color >> 16) & 0xFF; 
    }
}

void render_game_in_3D(t_game *game)
{
    t_ray ray;
    float perp_distance;
    int wall_strip_height;
    int wall_top_pixel;
    int wall_bottom_pixel;
    int ceiling_color = 0x87CEEB;
    int floor_color = 0x8B4513;   

    for (int i = 0; i < NUM_RAYS; i++)
    {
        ray = game->rays[i];
        perp_distance = ray.distance * cos(ray.ray_angle - game->player.rotation_angle);
        wall_strip_height = (int)((TILE_SIZE / perp_distance) * DIST_PROJ_PLANE);
        wall_top_pixel = (SCREEN_HEIGHT / 2) - (wall_strip_height / 2);
        if (wall_top_pixel < 0)
            wall_top_pixel = 0;
        wall_bottom_pixel = (SCREEN_HEIGHT / 2) + (wall_strip_height / 2);
        if (wall_bottom_pixel > SCREEN_HEIGHT)
            wall_bottom_pixel = SCREEN_HEIGHT;
        draw_ceiling(game, 0, wall_top_pixel, i, ceiling_color);
        draw_wall_strip(game, wall_top_pixel, wall_bottom_pixel, i);
        draw_floor(game, wall_bottom_pixel, SCREEN_HEIGHT, i, floor_color);
    }
}
