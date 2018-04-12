#include <stdio.h>
#include "engine.h"

struct enemies {
	PLAYER e;
	int px;
	int py;
	struct enemies *next;
};

const char *enemy_tex()
{
	const char *tex_p[6] = {"img/InvaderA1.png",
		"img/InvaderA2.png",
		"img/InvaderB1.png",
		"img/InvaderB2.png",
		"img/InvaderC1.png",
		"img/InvaderC2.png"
	};

	return tex_p[rand() % 6];
}

void enemies_add(struct enemies **e, int h, int x, int y, const char *str)
{
	struct enemies *new;

	if(h < 1)
		h = rand() % 25 + 10;
	new = malloc(sizeof *new);
	if(str == NULL)
		new->e = player_new(h, enemy_tex());
	else
		new->e = player_new(h, str);
	new->px = x;
	new->py = y;
	player_set_pos(new->e, x, y);
	new->next = *e;
	*e = new;
}

void enemies_rm(struct enemies **e, struct enemies *rm)
{
	struct enemies **it;

	for(it = e; *it != rm; it = &(*it)->next);

	*it = (*it)->next;
	player_del(rm->e);
	free(rm);
}

struct enemies *shoot_collision(struct enemies *shoot, struct enemies *ae)
{
	struct enemies *it;

	for(it = ae; it != NULL; it = it->next) {
		if(player_check_collision(it->e, shoot->e))
			return it;
	}

	return NULL;
}

int main()
{
	engine_init("Space invader", 640, 480);

	int quit;
	int i;
	SDL_Event e;
	struct enemies *ae;
	struct enemies *shoot;
	struct enemies *it, *it2; //used for iteration
	PLAYER play;
	int pp_x, pp_y, pvel;

	ae = NULL;
	shoot = NULL;
	quit = 0;

	play = player_new(25, "img/Ship.png");
	pp_x = 640 / 2; //half of window width
	pp_y = 480 - 32; //height - height of image
	pvel = 0;
	for(i = 0; i < 15; i++)
		enemies_add(&ae, 0, i * 42, 0, NULL);

	while(!quit && play) { //TODO: Restructure it.........
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
				switch(e.key.keysym.sym) {
				case SDLK_LEFT:
					pvel -= 5;
					break;
				case SDLK_RIGHT:
					pvel += 5;
					break;
				case SDLK_UP:
					enemies_add(&shoot, 7, pp_x + 30, pp_y - 32, "img/Bullet.png");
					break;
				}
			}
			else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
				switch(e.key.keysym.sym) {
				case SDLK_LEFT:
					pvel += 5;
					break;
				case SDLK_RIGHT:
					pvel -= 5;
					break;
				}
			}
			else if(e.type == SDL_QUIT) {
				quit = 1;
			}
		}

		pp_x += pvel;

		player_set_pos(play, pp_x, pp_y);

		render_beg();
		player_render(play);
		for(it = ae; it != NULL; it = it->next)
			player_render(it->e);
		for(it = shoot; it != NULL; it = it->next)
			player_render(it->e);

		for(it = shoot; it != NULL; it = it->next) {
			if((it2 = shoot_collision(it, ae)) != NULL) {
				player_dec_health(it2->e, it->e);
				enemies_rm(&shoot, it);
			}
			else if(it->py <= 0) {
				enemies_rm(&shoot, it);
			}
			else {
				it->py -= 5;
				player_set_pos(it->e, it->px, it->py);
			}
		}

		for(it = ae; it != NULL; it = it->next) {
			if(player_check_collision(it->e, play)) {
				player_dec_health(play, it->e);
				enemies_rm(&ae, it);
			}
			else if(it->py <= -480) {
				enemies_rm(&ae, it);
			}
			else {
				it->py += 1;
				player_set_pos(it->e, it->px, it->py);
			}
		}

		for(it = ae; it != NULL; it = it->next) {
			if(player_get_health(it->e) <= 0)
				enemies_rm(&ae, it);
		}

		render_end();

		if(player_get_health(play) <= 0) {
			player_del(play);
			play = NULL;
		}
	}

	while(ae != NULL)
		enemies_rm(&ae, ae);
	while(shoot != NULL)
		enemies_rm(&shoot, shoot);

	if(play != NULL)
		player_del(play);

	engine_quit();
}
