/*
 * TODO: REMOVE void * arg taking
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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

struct text {
	SDL_Rect pos;
	TTF_Font *f;
	SDL_Color c;
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

		if(TTF_Init() == -1) {
			ERROR_LOG("Unable to init SDL TTF: %s\n", TTF_GetError());
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
	TTF_Quit();
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

struct text *text_new(const char *str, int x, int y, int w, int h,
		Uint8 r, Uint8 g, Uint8 b, const char *font, int size)
{
	struct text *ret;

	ret = malloc(sizeof *ret);
	ret->f = TTF_OpenFont(font, size);
	if(ret->f == NULL) {
		ERROR_LOG("Unable to load font: %s\n", TTF_GetError());
		free(ret);
		ret = NULL;
	}
	else {
		SDL_Surface *s;

		ret->c.r = r;
		ret->c.g = g;
		ret->c.b = b;
		ret->c.a = 0xff;

		ret->pos.x = x;
		ret->pos.y = y;
		ret->pos.w = w;
		ret->pos.h = h;

		s = TTF_RenderText_Solid(ret->f, str, ret->c);
		if(s == NULL) {
			ERROR_LOG("Unable to create text surface: %s\n", TTF_GetError());
			free(ret);
			ret = NULL;
		}
		else {
			ret->tex = SDL_CreateTextureFromSurface(ren, s);
			if(ret->tex == NULL) {
				ERROR_LOG("Unable to create texture: %s\n", SDL_GetError());
				free(ret);
				ret = NULL;
			}

			SDL_FreeSurface(s);
		}
	}

	return ret;
}

bool text_mod_str(struct text *t, const char *str)
{
	SDL_Surface *s;
	bool ret;

	s = TTF_RenderText_Solid(t->f, str, t->c);
	ret = true;
	if(s == NULL) {
		ERROR_LOG("Unable to create surface: %s\n", TTF_GetError());
		ret = false;
	}
	else {
		SDL_DestroyTexture(t->tex);
		t->tex = NULL;

		t->tex = SDL_CreateTextureFromSurface(ren, s);
		if(t->tex == NULL) {
			ERROR_LOG("Unable to create texture: %s\n", SDL_GetError());
			ret = false;
		}

		SDL_FreeSurface(s);
	}

	return ret;
}

void text_mod_pos(struct text *t, int x, int y, int w, int h)
{
	SDL_Rect pos;

	pos.x = x;
	pos.y = y;
	pos.w = w;
	pos.h = h;

	t->pos = pos;
}

void text_render(struct text *t)
{
	SDL_RenderCopy(ren, t->tex, NULL, &t->pos);
}

void text_rm(struct text *t)
{
	SDL_DestroyTexture(t->tex);
	TTF_CloseFont(t->f);
	free(t);
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

int player_dec_health(void *ref, void *obj)
{
	return ((struct player *)ref)->health -= ((struct player *)obj)->health;
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
