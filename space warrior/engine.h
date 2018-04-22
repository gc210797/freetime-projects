#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

typedef void * PLAYER; //don't ever do this!!!!!!!!
typedef struct text * TEXT; //better

int engine_init(const char *title, int win_w, int win_h);
void engine_quit();
void *player_new(int health, const char *t_path);
void player_del(PLAYER ref);
int player_dec_health(PLAYER ref, PLAYER obj);
int player_get_health(PLAYER ref);
TEXT text_new(const char *str, int x, int y, int w, int h,
	Uint8 r, Uint8 g, Uint8 b, const char *font, int size);
int text_mod_str(TEXT, const char *str);
void text_mod_pos(TEXT, int x, int y, int w, int h);
void text_render(TEXT);
void text_rm(TEXT t);
void render_beg();
void render_end();
void player_render(PLAYER ref);
void player_set_pos(PLAYER ref, int x, int y);
int player_check_collision(PLAYER play, PLAYER obj);

#endif
