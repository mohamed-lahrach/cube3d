#include "minilibx-linux/mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define TILE_SIZE 64
#define MAP_WIDTH 15
#define MAP_HEIGHT 11
#define SCREEN_WIDTH (TILE_SIZE * MAP_WIDTH)
#define SCREEN_HEIGHT (TILE_SIZE * MAP_HEIGHT)
#define GRAY_COLOR 0xCCCCCC
#define BLACK_COLOR 0x000000
#define RED_COLOR 0xFF0000
#define YELLOW_COLOR 0xFFFF00
#define KEY_ESC 65307
#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define M_PI 3.14159265358979323846
#define FOV_ANGLE (60 * (M_PI / 180))
#define WALL_STRIP_WIDTH 1
#define NUM_RAYS (SCREEN_WIDTH / WALL_STRIP_WIDTH)
#define FOV_ANGLE (60 * (M_PI / 180)) // 60 degrees field of view in radians
#define DIST_PROJ_PLANE ((SCREEN_WIDTH / 2) / tan(FOV_ANGLE / 2))
#define MINIMAP_SCALE_FACTOR 0.3


typedef struct s_map
{
    int grid[MAP_HEIGHT][MAP_WIDTH];
} t_map;

typedef struct s_player
{
    float x;
    float y;
    float radius;
    int turn_direction;
    int walk_direction;
    int strafe_direction;
    float rotation_angle;
    float move_speed;
    float rotation_speed;
} t_player;

typedef struct s_ray
{
    float ray_angle;
    float wall_hit_x;
    float wall_hit_y;
    float distance;
    int was_hit_vertical;
} t_ray;

typedef struct s_game
{
    void *mlx;
    void *win;
    void *img;
    char *img_data;
    int bpp;
    int size_line;
    int endian;
    t_map map;
    t_player player;
    t_ray rays[NUM_RAYS];
} t_game;

int close_window(void *param);
void draw_square(char *img_data, int x, int y, int size, int color, int size_line, int bpp);
void draw_line(char *img_data, int x0, int y0, int x1, int y1, int color, int size_line, int bpp, float alpha);
void draw_circle(char *img_data, int x0, int y0, int radius, int color, int size_line, int bpp);
void render_player(t_game *game);
int key_press(int keycode, t_game *game);
int key_release(int keycode, t_game *game);
int has_wall_at(float x, float y, t_map *map);
float normalize_angle(float angle);
void update_player(t_game *game);
int can_move_to(float newX, float newY, t_game *game);
t_ray cast_ray(t_game *game, float ray_angle);
void show_data_of_ray(int i, t_game *game);
void cast_all_rays(t_game *game);
void draw_rays(t_game *game, float factor);
void render_game_in_3D(t_game *game);
int game_loop(t_game *game);
void init_game(t_game *game);
void render_minimap(t_game *game);
