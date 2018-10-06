#ifndef ENGINE3D_MAIN_H
#define ENGINE3D_MAIN_H

extern const int engine3D_width;
extern const int engine3D_height;
extern const char *const engine3D_title;
extern const double engine3D_frameCap;

typedef void(*game_callback_t)(void);

void engine3D_init(void);

void engine3D_start(void);

void engine3D_stop(void);

void engine3D_setGame_init(game_callback_t fun);

void engine3D_setGame_input(game_callback_t fun);

void engine3D_setGame_update(game_callback_t fun);

void engine3D_setGame_render(game_callback_t fun);

#endif /* ENGINE3D_MAIN_H */