#include "game.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

GameConfig game_conf = {
  .display_font_name  = "/usr/share/fonts/Poppins/Poppins-Bold.ttf",
  .display_font_size  = 40,
  .display_font_colors = {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    SKYBLUE,
    BLUE,
    DARKBLUE,
    PURPLE,
    LIGHTGRAY,
    WHITE,
    GOLD,
  },

  .display_point_color = SKYBLUE,

  .display_cell_background_color = GRAY,

  .display_board_size = (float) 450,
  .display_cell_gap   = (float) 10,
};

void game_init(Game *g) {
  g->config = game_conf;

  // Set values.
  GameConfig *config = &g->config;
  config->display_window_padding = (WIN_WIDTH - config->display_board_size) / 2.0;
  config->display_cell_size = (config->display_board_size - (config->display_cell_gap * 5.0)) / 4.0;
  config->display_font = LoadFontEx(config->display_font_name, config->display_font_size, NULL, 0);
}

void game_spawn_random(Game *g) {
  // Find a place.
  int rand_x, rand_y;
  do {
    rand_x = rand() % BOARD_SIZE;
    rand_y = rand() % BOARD_SIZE;
  } while (g->board[rand_y][rand_x] != 0);

  // Generate a random number on that place.
  int number;
  if (rand() % 100 < GENERATE_2_CHANCE_PERCENT) {
    // 90 % chance of generating 2.
    number = 2;
  } else {
    // 10 % chance of generating 4.
    number = 4;
  }
  g->board[rand_y][rand_x] = number;
}

bool game_check_game_over(Game *g) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if (g->board[i][j] == 0) return false;
    }
  }

  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      // If one of the cases equals to board[i][j], that means this cell can be merged,
      // the game is not over yet.
      if ((i + 1 < BOARD_SIZE &&
           g->board[i][j] == g->board[i + 1][j]) ||
          
          (i - 1 >= 0 &&
           g->board[i][j] == g->board[i - 1][j]) ||
          
          (j + 1 < BOARD_SIZE &&
           g->board[i][j] == g->board[i][j + 1]) ||
          
          (j - 1 >= 0 &&
           g->board[i][j] == g->board[i][j - 1])) {
        return false;
      }
    }
  }

  return true;
}

void game_transpose(Game *g) {
  int result[BOARD_SIZE][BOARD_SIZE];
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      result[i][j] = g->board[j][i];
    }
  }

  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      g->board[i][j] = result[i][j];
    }
  }
}

void game_reverse_rows(Game *g) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE / 2; j++) {
      int temp = g->board[i][j];
      g->board[i][j] = g->board[i][BOARD_SIZE - j - 1];
      g->board[i][BOARD_SIZE - j - 1] = temp;
    }
  }
}

int game_merge_row(int row[BOARD_SIZE]) {
  int temp[BOARD_SIZE] = {0};
  int points = 0;
  int idx = 0;

  for (int j = 0; j < BOARD_SIZE; j++) {
    if (row[j]) {
      temp[idx++] = row[j];
    }
  }

  memset(row, 0, sizeof(int) * BOARD_SIZE);

  idx = 0;
  for (int j = 0; j < BOARD_SIZE; j++) {
    if (!temp[j]) break;

    if (j + 1 < BOARD_SIZE && temp[j] == temp[j + 1]) {
      row[idx++] = temp[j] + temp[j + 1];
      points += temp[j] + temp[j + 1];
      j++;
    } else {
      row[idx++] = temp[j];
    }
  }

  return points;
}

void game_merge_left(Game *g) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    g->point += game_merge_row(g->board[i]);
  }
}

bool game_handle_move(Game *g, char dir) {
  int back[BOARD_SIZE][BOARD_SIZE];
  memmove(back, g->board, sizeof(int) * BOARD_SIZE * BOARD_SIZE);

  switch (dir) {
    case 'U': {
      game_transpose(g);

      game_merge_left(g);

      game_transpose(g);
    } break;
    case 'D': {
      game_transpose(g);
      game_reverse_rows(g);

      game_merge_left(g);

      game_reverse_rows(g);
      game_transpose(g);
    } break;
    case 'L': {
      game_merge_left(g);
    } break;
    case 'R': {
      game_reverse_rows(g);

      game_merge_left(g);

      game_reverse_rows(g);
    } break;
    default: return false;
  }

  if (memcmp(g->board, back, sizeof(int) * BOARD_SIZE * BOARD_SIZE) != 0) {
    return true;
  } else {
    return false;
  }
}
