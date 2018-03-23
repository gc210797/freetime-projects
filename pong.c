#include <SDL2/SDL.h>
#include <stdio.h>

struct player {
	SDL_Texture *tex;
	int velx;
	int vely;
	SDL_Rect r;

	void (*control_move)(struct player *p, SDL_Event e);
	void (*move)(void *p);
};

struct bot_d {
	struct player *p1;
	SDL_Rect *ball_loc;
	int *ball_velx;
	int *ball_vely;
};

SDL_Window *win;
SDL_Renderer *ren;

int system_init()
{
	int ret;

	ret = 1;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initalize video: %s\n", SDL_GetError());
		ret = 0;
	}

	win = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480, SDL_WINDOW_SHOWN);
	if(win == NULL) {
		printf("unable to create window: %s\n", SDL_GetError());
		ret = 0;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == NULL) {
		printf("Unable to create renderer: %s\n", SDL_GetError());
		ret = 0;
	}

	return ret;
}

void system_close()
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

SDL_Texture *create_block(int w, int h)
{
	SDL_Texture *ret;
	SDL_Rect targ;

	ret = SDL_CreateTexture(ren, SDL_GetWindowPixelFormat(win), SDL_TEXTUREACCESS_TARGET, w, h);

	SDL_SetRenderTarget(ren, ret);

	targ.x = 0;
	targ.y = 0;
	targ.w = w;
	targ.h = h;

	SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(ren);

	SDL_RenderFillRect(ren, &targ);

	SDL_SetRenderTarget(ren, NULL);

	return ret;
}

void control_man_move(struct player *p, SDL_Event e)
{
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		switch(e.key.keysym.sym) {
		case SDLK_UP:
			p->vely -= 5;
			break;
		case SDLK_DOWN:
			p->vely += 5;
			break;
		}
	}
	else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
		switch(e.key.keysym.sym) {
		case SDLK_UP:
			p->vely += 5;
			break;
		case SDLK_DOWN:
			p->vely -= 5;
			break;
		}
	}
}

void player_mv(void *d)
{
	struct player *p;

	p = d;
	p->r.x += p->velx;
	p->r.y += p->vely;

	if(p->r.y < 0)
		p->r.y = 0;
	else if(p->r.y > 480 - p->r.h)
		p->r.y = 480 - p->r.h;
}

void bot_mv(void *d)
{
	struct bot_d *data;
	struct player *p;
	int d_y;

	data = d;
	p = data->p1;
	p->vely = 4;
	d_y = ((data->p1->r.y - data->ball_loc->y) * (data->p1->r.y - data->ball_loc->y));

	if(*data->ball_velx > 0 && data->ball_loc->x >=  640 / 2 + 640 / 40) {
		if(p->r.y - data->ball_loc->y < 0)
			p->r.y += p->vely;
		else
			p->r.y += -p->vely;
		if(p->r.y < 0)
			p->r.y += p->vely;
		else if(p->r.y >= 480 - p->r.h)
			p->r.y += -p->vely;
	}
}

struct player *create_player(int x, int y, int w, int h)
{
	struct player *ret;
	SDL_Rect r;

	r.w = w;
	r.h = h;
	r.x = x;
	r.y = y;
	ret = malloc(sizeof *ret);

	ret->tex = create_block(w, h);
	ret->velx = 0;
	ret->vely = 0;
	ret->r = r;
	ret->control_move = control_man_move;
	ret->move = player_mv;
}

void set_bot(struct player *p1)
{
	p1->move = bot_mv;
}

int main()
{
	SDL_Event e;
	int quit;
	struct player *p1;
	struct player *p2;
	SDL_Texture *ball;
	SDL_Rect ball_loc;
	int beg;
	int ball_velx;
	int ball_vely;
	struct bot_d bt;

	if(!system_init()) {
		system_close();
		return -1;
	}

	p1 = create_player(640 * 0.10, 480 * 0.50, 10, 60);

	p2 = create_player(640 - 640 * 0.10, 480 - 480 * 0.50, 10, 60);
	set_bot(p2);

	ball = create_block(5, 5);
	ball_loc.w = 5;
	ball_loc.h = 5;
	ball_loc.x = p1->r.x + p1->r.w;
	ball_loc.y = p1->r.y + p1->r.h / 2;

	quit = 0;
	beg = 0;
	ball_velx = 4;
	ball_vely = 4;
	bt.p1 = p2;
	bt.ball_velx = &ball_velx;
	bt.ball_vely = &ball_vely;
	bt.ball_loc = &ball_loc;

	while(!quit) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				quit = 1;
			}

			p1->control_move(p1, e);

			if(e.type == SDL_KEYDOWN) {
				switch(e.key.keysym.sym) {
					case SDLK_RIGHT:
						beg = 1;
						break;
				}
			}
		}

		p1->move(p1);
		if(beg)
			p2->move(&bt);

		if(!beg) {
			ball_loc.x = p1->r.x + p1->r.w;
			ball_loc.y = p1->r.y + p1->r.h / 2;
		}
		else {
			ball_loc.x += ball_velx;
			ball_loc.y += ball_vely;

			if(ball_loc.y >= 480 - ball_loc.h || ball_loc.y <= 0)
				ball_vely = -ball_vely;
			if(ball_loc.x >= p2->r.x - ball_loc.w &&
					(ball_loc.y >= p2->r.y && ball_loc.y <= p2->r.y + p2->r.h))
				ball_velx = -ball_velx;
			else if(ball_loc.x <= p1->r.x + p1->r.w &&
					(ball_loc.y >= p1->r.y && ball_loc.y <= p1->r.y + p1->r.h))
				ball_velx = -ball_velx;
			if(ball_loc.x >= 640 || ball_loc.x <= 0)
				ball_velx = -ball_velx;
		}

		SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);
		SDL_RenderClear(ren);

		SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderDrawLine(ren, 640 / 2, 0, 640 / 2, 480);
		SDL_RenderCopy(ren, p1->tex, NULL, &p1->r);
		SDL_RenderCopy(ren, p2->tex, NULL, &p2->r);
		SDL_RenderCopy(ren, ball, NULL, &ball_loc);

		SDL_RenderPresent(ren);
	}

	SDL_DestroyTexture(ball);
	SDL_DestroyTexture(p1->tex);
	SDL_DestroyTexture(p2->tex);
	free(p1);
	free(p2);
	system_close();
	return 0;
}
