#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "game.h"

void game_draw(Game *g) {
  GameConfig *config = &g->config;
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      int number = g->board[i][j];

      // Draw the cell rectangle.
      Color background = config->display_cell_background_color;
      if (number == 0) {
        background.a = 50;
      }
      int x = config->display_window_padding +
        (j + 1) * config->display_cell_gap +
        j * config->display_cell_size;
      int y = config->display_window_padding +
        (i + 1) * config->display_cell_gap +
        i * config->display_cell_size;
      DrawRectangle(x, y, config->display_cell_size, config->display_cell_size, background);

      // When the number is 0, we don't want do display it.
      if (number == 0) continue;

      // Draw the number on the cell.
      char text[MAX_NUMBER_LENGTH + 1];
      snprintf(text, MAX_NUMBER_LENGTH + 1, "%d", number);
      Vector2 text_size = MeasureTextEx(config->display_font, text, config->display_font_size, 0);
      Vector2 text_pos = {
        (x + config->display_cell_size / 2) - (text_size.x / 2),
        (y + config->display_cell_size / 2) - (text_size.y / 2)
      };
      Color text_color = config->display_font_colors[((int) log2(number) - 1) % DISPLAY_FONT_COLORS_COUNT];
      DrawTextEx(config->display_font, text, text_pos, config->display_font_size, 0, text_color);
    }
  }

  // Draw points.
  const int text_len = MAX_POINT_LENGTH + strlen("Points: ");
  char text[text_len + 1];
  snprintf(text, text_len + 1, "Points: %d", g->point);
  Vector2 text_size = MeasureTextEx(config->display_font, text, config->display_font_size, 0);
  Vector2 text_pos = {
    config->display_window_padding,
    WIN_HEIGHT - config->display_window_padding - text_size.y
  };
  DrawTextEx(config->display_font, text, text_pos, config->display_font_size, 0, config->display_point_color);
}

// ==============================
// ===> Main
// ==============================

int main() {
  srand(time(NULL));

  InitWindow(WIN_WIDTH, WIN_HEIGHT, "2048");
  SetTargetFPS(60);

  Game g = {0};
  game_init(&g);
  
  // Generate 2 random numbers on startup.
  game_spawn_random(&g);
  game_spawn_random(&g);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(BLACK);
    game_draw(&g);

    if (IsKeyPressed(KEY_Q)) {
      // Quit.
      break;
    }

    char dir = '\0';
    if (IsKeyPressed(KEY_W)) {
      dir = 'U';
    } else if (IsKeyPressed(KEY_S)) {
      dir = 'D';
    } else if (IsKeyPressed(KEY_A)) {
      dir = 'L';
    } else if (IsKeyPressed(KEY_D)) {
      dir = 'R';
    }

    if (dir && game_handle_move(&g, dir)) {
      game_spawn_random(&g);
    }

    if (game_check_game_over(&g)) {
      break;
    }


    EndDrawing();
  }

  game_draw(&g);

  // Pause the program.
  //getchar();

  CloseWindow();
}
