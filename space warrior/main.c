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

int math_mod(int val)
{
	if(val < 0)
		return -val;
	else
		return val;
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
	Uint32 new_em;
	unsigned long long score;
	TEXT score_disp;
	char score_disp_str[12];
	int em_pos_cache;
	int health;
	TEXT health_disp;
	char health_disp_str[12];

	ae = NULL;
	shoot = NULL;
	quit = 0;

	play = player_new(25, "img/Ship.png");
	pp_x = 640 / 2; //half of window width
	pp_y = 480 - 32; //height - height of image
	pvel = 0;
	score = 0;
	em_pos_cache = -1;
	score_disp = text_new("Score: 0", 640 - 100, 5, 100, 30, 0xff, 0xff, 0xff,
			"arial.ttf", 25);
	health_disp = text_new("Health: 25", 0, 5, 100, 30, 0xff, 0xff, 0xff,
			"arial.ttf", 25);
	for(i = 0; i < 5; i++)
		enemies_add(&ae, 0, rand() % 598, 0, NULL); //598: width - width of image
	new_em = SDL_GetTicks();

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

		if((SDL_GetTicks() - new_em) / 1000 >= 2) {
			for(i = 0; i < 3; i++) {
				int rnd = rand() % 598;
				if(math_mod(rnd - em_pos_cache) <= 42) {
					if(rnd - em_pos_cache >= 0)
						rnd += 44;
					else
						rnd -= 44;
				}
				enemies_add(&ae, 0, rnd, 0, NULL);
				em_pos_cache = rnd;
			}
			new_em = SDL_GetTicks();
		}

		pp_x += pvel;

		player_set_pos(play, pp_x, pp_y);

		render_beg();
		player_render(play);
		for(it = ae; it != NULL; it = it->next)
			player_render(it->e);
		for(it = shoot; it != NULL; it = it->next)
			player_render(it->e);

		for(it = shoot; it != NULL; ) {
			struct enemies *rm;
			if((it2 = shoot_collision(it, ae)) != NULL) {
				player_dec_health(it2->e, it->e);
				rm = it;
				it = it->next;
				enemies_rm(&shoot, rm);
			}
			else if(it->py <= 0) {
				rm = it;
				it = it->next;
				enemies_rm(&shoot, rm);
			}
			else {
				it->py -= 5;
				player_set_pos(it->e, it->px, it->py);
				it = it->next;
			}
		}

		for(it = ae; it != NULL; ) {
			struct enemies *rm;
			if(player_check_collision(it->e, play)) {
				player_dec_health(play, it->e);
				rm = it;
				it = it->next;
				enemies_rm(&ae, rm);
			}
			else if(it->py >= 480) {
				rm = it;
				it = it->next;
				enemies_rm(&ae, rm);
				if(score != 0)
					score -= 1;
			}
			else {
				it->py += 1;
				player_set_pos(it->e, it->px, it->py);
				it = it->next;
			}
		}

		for(it = ae; it != NULL;) {
			struct enemies *rm;
			if(player_get_health(it->e) <= 0) {
				rm = it;
				it = it->next;
				enemies_rm(&ae, rm);
				score++;
			}
			else {
				it = it->next;
			}
		}

		sprintf(score_disp_str, "Score: %lld", score);
		text_mod_str(score_disp, score_disp_str);
		text_render(score_disp);
		
		sprintf(health_disp_str, "Health: %d", player_get_health(play));
		text_mod_str(health_disp, health_disp_str);
		text_render(health_disp);
		
		render_end();

		if(player_get_health(play) <= 0) {
			player_del(play);
			play = NULL;
		}
	}

	if(play == NULL)
		printf("Score: %ld\n", score);

	while(ae != NULL)
		enemies_rm(&ae, ae);
	while(shoot != NULL)
		enemies_rm(&shoot, shoot);

	if(play != NULL)
		player_del(play);
	if(score_disp != NULL)
		text_rm(score_disp);
	if(health_disp != NULL)
		text_rm(health_disp);

	engine_quit();
}
