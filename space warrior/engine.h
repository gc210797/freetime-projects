#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

typedef void * PLAYER;

int engine_init(const char *title, int win_w, int win_h);
void engine_quit();
void *player_new(int health, const char *t_path);
void player_del(PLAYER ref);
int player_dec_health(PLAYER ref, PLAYER obj);
int player_get_health(PLAYER ref);
void render_beg();
void render_end();
void player_render(PLAYER ref);
void player_set_pos(PLAYER ref, int x, int y);
int player_check_collision(PLAYER play, PLAYER obj);

#endif
