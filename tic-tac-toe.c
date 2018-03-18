#include <SDL2/SDL.h>
#include <stdio.h>

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

struct line {
	int x1;
	int x2;
	int y1;
	int y2;
};

enum shape {
	CIRCLE,
	CROSS
};

struct boxes {
	SDL_Texture *tex;
	SDL_Rect loc;
	enum shape s;
};

SDL_Window *win;
SDL_Renderer *ren;

void system_init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initialze: %s\n", SDL_GetError());
		return;
	}

	win = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
	if(win == NULL) {
		printf("Unable to create window: %s\n", SDL_GetError());
		return;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == NULL) {
		printf("Unable to create renderer: %s\n", SDL_GetError());
		return;
	}
}

void system_close()
{
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

	SDL_Quit();
}

void generate_box(struct line *l)
{
	l[0].x1 = 0;
	l[0].x2 = WIN_WIDTH;
	l[0].y1 = WIN_HEIGHT / 3;
	l[0].y2 = WIN_HEIGHT / 3;

	l[1].x1 = 0;
	l[1].x2 = WIN_WIDTH;
	l[1].y1 = WIN_HEIGHT - WIN_HEIGHT / 3;
	l[1].y2 = WIN_HEIGHT - WIN_HEIGHT / 3;

	l[2].x1 = WIN_WIDTH / 3;
	l[2].x2 = WIN_WIDTH / 3;
	l[2].y1 = 0;
	l[2].y2 = WIN_HEIGHT;

	l[3].x1 = WIN_WIDTH - WIN_WIDTH / 3;
	l[3].x2 = WIN_WIDTH - WIN_WIDTH / 3;
	l[3].y1 = 0;
	l[3].y2 = WIN_HEIGHT;
}

void display_box(struct line *l)
{
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderDrawLine(ren, l[0].x1, l[0].y1, l[0].x2, l[0].y2);
	SDL_RenderDrawLine(ren, l[1].x1, l[1].y1, l[1].x2, l[1].y2);
	SDL_RenderDrawLine(ren, l[2].x1, l[2].y1, l[2].x2, l[2].y2);
	SDL_RenderDrawLine(ren, l[3].x1, l[3].y1, l[3].x2, l[3].y2);
}

void generate_shape(SDL_Texture **ob, enum shape T)
{
	*ob = SDL_CreateTexture(ren, SDL_GetWindowPixelFormat(win),
			SDL_TEXTUREACCESS_TARGET, WIN_WIDTH / 6, WIN_HEIGHT / 6);

	SDL_SetRenderTarget(ren, *ob);

	SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(ren);

	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);

	if(T == CIRCLE) {
		//bresenhams circle drawing algorithm
		int p, q, r, d, x, y;

		p = 0;
		r = WIN_WIDTH / 24;
		q = r;
		d = 3 - 2 * r;
		x = WIN_WIDTH / 12;
		y = WIN_HEIGHT / 12;

		while(p <= q) {
			SDL_RenderDrawPoint(ren, x + p, y + q);
			SDL_RenderDrawPoint(ren, x - p, y + q);
			SDL_RenderDrawPoint(ren, x + p, y - q);
			SDL_RenderDrawPoint(ren, x - p, y - q);
			SDL_RenderDrawPoint(ren, x + q, y + p);
			SDL_RenderDrawPoint(ren, x - q, y + p);
			SDL_RenderDrawPoint(ren, x + q, y - p);
			SDL_RenderDrawPoint(ren, x - q, y - p);

			p++;

			if(d < 0) {
				d += 4 * p + 6;
			}
			else {
				q--;
				d += 4 * (p - q) + 10;
			}

			SDL_RenderDrawPoint(ren, x + p, y + q);
			SDL_RenderDrawPoint(ren, x - p, y + q);
			SDL_RenderDrawPoint(ren, x + p, y - q);
			SDL_RenderDrawPoint(ren, x - p, y - q);
			SDL_RenderDrawPoint(ren, x + q, y + p);
			SDL_RenderDrawPoint(ren, x - q, y + p);
			SDL_RenderDrawPoint(ren, x + q, y - p);
			SDL_RenderDrawPoint(ren, x - q, y - p);
		}
	}
	else if(T == CROSS) {
		struct line l1;
		struct line l2;

		l1.x1 = 0;
		l1.y1 = 0;
		l1.x2 = WIN_WIDTH / 6;
		l1.y2 = WIN_HEIGHT / 6;

		l2.x1 = WIN_WIDTH / 6;
		l2.y1 = 0;
		l2.x2 = 0;
		l2.y2 = WIN_HEIGHT / 6;

		SDL_RenderDrawLine(ren, l1.x1, l1.y1, l1.x2, l1.y2);
		SDL_RenderDrawLine(ren, l2.x1, l2.y1, l2.x2, l2.y2);
	}

	SDL_SetRenderTarget(ren, NULL);

}

int get_sign_loc(struct line *l)
{
	int x;
	int y;
	int ret;

	ret = -1;

	SDL_GetMouseState(&x, &y);

	if(x > 0 && x < l[2].x1 && y < l[0].y1 && y > 0)
		ret = 0;
	else if(x > l[2].x1 && x < l[3].x1 && y < l[0].y1 && y > 0)
		ret = 1;
	else if(x > l[3].x1 && x < WIN_WIDTH && y < l[0].y1 && y > 0)
		ret = 2;
	else if(x > 0 && x < l[2].x1 && y > l[0].y1 && y < l[1].y1)
		ret = 3;
	else if(x > l[2].x1 && x < l[3].x1 && y > l[0].y1 && y < l[1].y1)
		ret = 4;
	else if(x > l[3].x1 && x < WIN_WIDTH && y > l[0].y1 && y < l[1].y1)
		ret = 5;
	else if(x > 0 && x < l[2].x1 && y > l[0].y1 && y < WIN_HEIGHT)
		ret = 6;
	else if(x > l[2].x1 && x < l[3].x1 && y > l[1].y1 && y < WIN_HEIGHT)
		ret = 7;
	else if(x > l[3].x1 && x < WIN_WIDTH && y > l[1].y1 && y < WIN_HEIGHT)
		ret = 8;

	return ret;
}

void place_signs(struct boxes **b, int loc, enum shape sh, SDL_Texture *s)
{
	if(loc == -1 && b[loc] != NULL)
		return;

	SDL_Rect l;
	l.w = WIN_WIDTH / 6;
	l.h = WIN_HEIGHT / 6;
	b[loc] = malloc(sizeof **b);
	b[loc]->tex = s;
	b[loc]->s = sh;

	switch(loc) {
	case 0:
		l.x = WIN_WIDTH / 12;
		l.y = WIN_HEIGHT / 12;
		break;
	case 1:
		l.x = WIN_WIDTH / 3 + WIN_WIDTH / 12;
		l.y = WIN_HEIGHT / 12;
		break;
	case 2:
		l.x = 2 * WIN_WIDTH / 3 + WIN_WIDTH / 12;
		l.y = WIN_HEIGHT / 12;
		break;
	case 3:
		l.x = WIN_WIDTH / 12;
		l.y = WIN_HEIGHT / 3 + WIN_HEIGHT / 12;
		break;
	case 4:
		l.x = WIN_WIDTH / 3 + WIN_WIDTH / 12;
		l.y = WIN_HEIGHT / 3 + WIN_HEIGHT / 12;
		break;
	case 5:
		l.x = 2 * WIN_WIDTH / 3 + WIN_WIDTH / 12;
		l.y = WIN_HEIGHT / 3 + WIN_HEIGHT / 12;
		break;
	case 6:
		l.x = WIN_WIDTH / 12;
		l.y = 2 * WIN_HEIGHT / 3 + WIN_HEIGHT / 12;
		break;
	case 7:
		l.x = WIN_WIDTH / 3 + WIN_WIDTH / 12;
		l.y = 2 * WIN_HEIGHT / 3 + WIN_HEIGHT / 12;
		break;
	case 8:
		l.x = 2 * WIN_WIDTH / 3 + WIN_WIDTH / 12;
		l.y = 2 * WIN_HEIGHT / 3 + WIN_HEIGHT / 12;
		break;
	}

	b[loc]->loc = l;
}

int check_winner(struct boxes **b)
{
	if((b[0] != NULL && b[4] != NULL && b[8] != NULL) &&
	((b[0]->s == b[4]->s) && (b[4]->s == b[8]->s))) {	
		return b[0]->s;
	}
	else if((b[2] != NULL && b[4] != NULL && b[6] != NULL) && 
	((b[2]->s == b[4]->s) && (b[4]->s == b[6]->s))) {
		return b[2]->s;
	}
	else if((b[0] != NULL && b[1] != NULL && b[2] != NULL) &&
	(b[0]->s == b[1]->s) && (b[1]->s == b[2]->s)) {
		return b[0]->s;
	}
	else if((b[3] != NULL && b[4] != NULL && b[5] != NULL) &&
	((b[3]->s == b[4]->s) && (b[4]->s == b[5]->s))) {
		return b[3]->s;
	}
	else if((b[6] != NULL && b[7] != NULL && b[8] != NULL) &&
	(b[6]->s == b[7]->s) && (b[7]->s == b[8]->s)) {
		return b[6]->s;
	}
	else if((b[0] != NULL && b[3] != NULL && b[6] != NULL) &&
	(b[0]->s == b[3]->s) && (b[3]->s == b[6]->s)) {
		return b[0]->s;
	}
	else if((b[1] != NULL && b[4] != NULL && b[7] != NULL) &&
	(b[1]->s == b[4]->s) && (b[4]->s == b[7]->s)) {
		return b[1]->s;
	}
	else if((b[2] != NULL && b[5] != NULL && b[8] != NULL) && 
	(b[2]->s == b[5]->s) && (b[5]->s == b[8]->s)) {
		return b[2]->s;
	}

	return -1;
}

int place_move(struct boxes **b)
{
	int i;
	int j;
	int k;
	int empty;
	int filled[9];

	for(i = 0, empty = 1; i < 9; i++) {
		if(b[i] != NULL) {
			empty = 0;
			break;
		}
	}

	if(empty) {
		return 4;
	}
	else {
		for(i = 0, j = 0; i < 9; i++) {
			if(b[i] != NULL) {
				if(b[i]->s == CROSS)
					filled[i] = 1;
				else
					filled[i] = 2;
			}
			else {
				filled[i] = 0;
			}
		}

		/*for(i = 1; i < 9; i++) {
			if(filled[i] == 0) {
				if(i == 1 || i == 5 || i == 7) {
					if(filled[(i - 1) % 9] == 1 && filled[(i + 1) % 9] == 1)
						return i;
				}
				else if(filled[(i + 6) % 9] == 1 && filled[(i + 3) % 9] == 1)
					return i;
				else if(filled[(i + 4) % 9] == 1 && filled[(i + 8) % 9] == 1)
					return i;
				else if(filled[(i + 2) % 9] == 1 && filled[(i + 5) % 9] == 1)
					return i;
			}
		}*/

		for(i = 0; i < 9; i++) {
			if(filled[i] == 0) {
				b[i] = malloc(sizeof **b);
				b[i]->s = CROSS;
				//b[i]->loc = i;
				//b[i]->tex = NULL;

				if(check_winner(b) != -1) {
					free(b[i]);
					b[i] = NULL;
					return i;
				}

				free(b[i]);
				b[i] = NULL;
			}
		}

		if(b[4] == NULL)
			return 4;
		else if(b[0] == NULL)
			return 0;
		else if(b[2] == NULL)
			return 2;
		else if(b[6] == NULL)
			return 6;
		else if(b[8] == NULL)
			return 8;
		for(i = 0; i < 9; i++) {
			if(filled[i] == 0)
				return i;
		}
	}
}

int main()
{
	SDL_Event e;
	struct line l[4];
	struct boxes *b[9];
	SDL_Texture *circle;
	SDL_Texture *cross;
	enum shape turn;
	enum shape winner;
	int loc;
	int i;
	int filled;
	int quit;

	system_init();

	generate_shape(&circle, CIRCLE);
	generate_shape(&cross, CROSS);

	generate_box(l);

	for(i = 0; i < 9; i++)
		b[i] = NULL;

	loc = -1;
	turn = CIRCLE;
	filled = -1;
	quit = 0;

	while(!quit) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT)
				quit = 1;
			else if(e.type == SDL_MOUSEBUTTONDOWN)
				if(filled < 9 && turn == CROSS) {
					loc = get_sign_loc(l);
					filled++;
				}
		}

		SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(ren);

		display_box(l);

		if((winner = check_winner(b)) != -1) {
			printf("%d is winner\n", winner);
		}
		else {
			if(filled < 9 && turn == CIRCLE) {
				loc = place_move(b);
				filled++;
			}
		}

		if(loc != -1 && filled < 9) {
			switch(turn) {
			case CROSS:
				place_signs(b, loc, CROSS, cross);
				turn = CIRCLE;
				break;
			case CIRCLE:
				place_signs(b, loc, CIRCLE, circle);
				turn = CROSS;
				break;
			}
		}

		loc = -1;

		for(i = 0; i < 9; i++) {
			if(b[i] != NULL)
				SDL_RenderCopy(ren, b[i]->tex, NULL, &b[i]->loc);
		}

		SDL_RenderPresent(ren);
	}

	for(i = 0; i < 9; i++) {
		if(b[i] != NULL)
			free(b[i]);
	}

	SDL_DestroyTexture(circle);
	SDL_DestroyTexture(cross);
	system_close();
}
