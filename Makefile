CC = cc
#CFLAGS = -Wall -Wextra -Werror
MLX_DIR = minilibx-linux
MLX_FLAGS = -L$(MLX_DIR) -lmlx_Linux -lX11 -lXext -lm
SRC = main.c raycasting.c mini_2Dmap.c render_3D_game.c debug.c utils.c input_handling.c raycasting_utils.c
OBJ = $(SRC:.c=.o)
NAME = cube3d

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(MLX_FLAGS) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
