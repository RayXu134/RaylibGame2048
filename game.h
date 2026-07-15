#ifndef GAME_H_
#define GAME_H_

#include <raylib.h>

// ==============================
// ===> Definitions
// ==============================

#define BOARD_SIZE 4

// Window size.
#define WIN_WIDTH  500
#define WIN_HEIGHT 600

#define MAX_NUMBER_LENGTH 10
#define MAX_POINT_LENGTH 10
#define GENERATE_2_CHANCE_PERCENT 90
#define DISPLAY_FONT_COLORS_COUNT 11

// ==============================
// ===> GameConfig
// ==============================

typedef struct {
  char *display_font_name;
  int display_font_size;
  Color display_font_colors[DISPLAY_FONT_COLORS_COUNT];

  Color display_point_color;

  Color display_cell_background_color;
  
  float display_board_size;
  float display_cell_gap;

  // Auto set in game_init(), we don't need to specify these values.
  float display_window_padding;
  float display_cell_size;
  Font display_font;
} GameConfig;

extern GameConfig game_conf;

// ==============================
// ===> Game
// ==============================

typedef struct {
  int board[BOARD_SIZE][BOARD_SIZE];
  int point;
  GameConfig config;
} Game;

void game_init(Game *g);
void game_spawn_random(Game *g);
bool game_check_game_over(Game *g);
void game_transpose(Game *g);
void game_reverse_rows(Game *g);
// Return points that the user got in this row.
int game_merge_row(int row[BOARD_SIZE]);
void game_merge_left(Game *g);
// Return true if the user made a move and changed the board.
bool game_handle_move(Game *g, char dir);

#endif  // GAME_H_
