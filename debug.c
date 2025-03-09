/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahrach <mlahrach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 01:10:40 by mlahrach          #+#    #+#             */
/*   Updated: 2025/03/09 01:10:41 by mlahrach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "raycasting.h"

void show_data_of_ray(int i, t_game *game)
{
    printf("Ray %d", i);
    printf(" angle: %f rad", game->rays[i].ray_angle);
    printf(", %f deg", game->rays[i].ray_angle * (180 / M_PI));
    printf(", tan(angel) = %f\n", tan(game->rays[i].ray_angle));
}

void show_data_of_rays(t_game *game)
{
    int i;

    i = 0;
    while (i < NUM_RAYS)
    {
        show_data_of_ray(i, game);
        i++;
    }
}