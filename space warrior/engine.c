#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "engine.h"

#define ERROR_LOG(...) fprintf(stderr, __VA_ARGS__)

typedef enum {
	false,
	true
} bool;

struct player {
	int health;
	SDL_Rect pos;
	SDL_Texture *tex;
};

static SDL_Window *win;
static SDL_Renderer *ren;

int engine_init(const char *win_title, int win_w, int win_h)
{
	bool status;

	status = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		ERROR_LOG("Unable to init SDL: %s\n", SDL_GetError());
		status = false;
	}
	else {
		win = SDL_CreateWindow(win_title, SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, win_w, win_h, SDL_WINDOW_SHOWN);
		if(win == NULL) {
			ERROR_LOG("Unable to create window: %s\n", SDL_GetError());
			status = false;
		}

		ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(ren == NULL) {
			ERROR_LOG("Unable to create renderer: %s\n", SDL_GetError());
			status = false;
		}

		if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
			ERROR_LOG("Unable to init SDL IMAGE: %s\n", IMG_GetError());
			status = false;
		}
	}

	return status;
}

void engine_quit()
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	IMG_Quit();
	SDL_Quit();
}

static SDL_Texture *image_load(const char *path, SDL_Rect *s)
{
	SDL_Texture *ret;
	SDL_Surface *surf;

	surf = IMG_Load(path);
	if(surf == NULL) {
		ERROR_LOG("Unable to load %s: %s\n", path, IMG_GetError());
		ret = NULL;
	}
	else {
		s->w = surf->w;
		s->h = surf->h;
		ret = SDL_CreateTextureFromSurface(ren, surf);
		if(ret == NULL) {
			ERROR_LOG("Unable to create texutre from sufrace for %s: %s\n",
					path, SDL_GetError());
			ret = NULL;
		}
		SDL_FreeSurface(surf);
	}

	return ret;
}

void *player_new(int health, const char *t_path)
{
	struct player *ret;

	ret = malloc(sizeof *ret);
	ret->tex = image_load(t_path, &ret->pos);
	ret->health = health;

	return ret;
}

int player_get_health(void *ref)
{
	return ((struct player *)ref)->health;
}

int player_dec_health(void *ref)
{
	return --((struct player *)ref)->health;
}

void player_set_tex(void *ref, const char *t_path)
{
	struct player *p;
	SDL_Texture *t;

	p = ref;
	t = p->tex;
	p->tex = image_load(t_path, &p->pos);
	SDL_DestroyTexture(t);
}

void player_set_pos(void *ref, int x, int y)
{
	struct player *p;

	p = ref;
	p->pos.x = x;
	p->pos.y = y;
}

int player_check_collision(void *play, void *obj)
{
	SDL_Rect p;
	SDL_Rect o;
	bool coll;

	p = ((struct player *)play)->pos;
	o = ((struct player *)obj)->pos;
	coll = true;

	if(p.x + p.w < o.x)
		coll = false;
	if(p.x > o.x + o.w)
		coll = false;
	if(p.y + p.h < o.y)
		coll = false;
	if(p.y > o.y + o.w)
		coll = false;
	return coll;
}

void render_beg()
{
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(ren);
}

void render_end()
{
	SDL_RenderPresent(ren);
}

void player_render(void *ref)
{
	SDL_RenderCopy(ren, ((struct player *)ref)->tex, NULL, &((struct player *)ref)->pos);
}

void player_del(void *ref)
{
	struct player *p = ref;
	SDL_DestroyTexture(p->tex);
	free(p);
}
