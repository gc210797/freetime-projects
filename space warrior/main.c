#include <stdio.h>
#include "engine.h"

struct enemies {
	PLAYER e;
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

void enemies_add(struct enemies **e, int x, int y)
{
	struct enemies *new;

	new = malloc(sizeof *new);
	new->e = player_new(rand() % 25 + 10, enemy_tex());
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

int main()
{
	engine_init("Space invader", 640, 480);

	int quit;
	int i;
	SDL_Event e;
	struct enemies *ae, *it;
	PLAYER play;
	int p_x, p_y, vel;

	ae = NULL;
	quit = 0;

	play = player_new(25, "img/Ship.png");
	p_x = 640 / 2; //half of window width
	p_y = 480 - 32; //height - height of image
	vel = 0;
	for(i = 0; i < 15; i++)
		enemies_add(&ae, i * 42, 0);

	while(!quit) { //TODO: Restructure it.........
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
				switch(e.key.keysym.sym) {
				case SDLK_LEFT:
					vel -= 5;
					break;
				case SDLK_RIGHT:
					vel += 5;
					break;
				}
			}
			else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
				switch(e.key.keysym.sym) {
				case SDLK_LEFT:
					vel += 5;
					break;
				case SDLK_RIGHT:
					vel -= 5;
					break;
				}
			}
			else if(e.type == SDL_QUIT) {
				quit = 1;
			}
		}

		p_x += vel;

		player_set_pos(play, p_x, p_y);

		render_beg();
		player_render(play);
		for(it = ae; it != NULL; it = it->next)
			player_render(it->e);
		render_end();
	}

	while(ae != NULL)
		enemies_rm(&ae, ae);
	player_del(play);

	engine_quit();
}
