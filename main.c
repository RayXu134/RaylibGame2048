#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <math.h>

#define BOARD_SIZE 4

// Window size.
#define WIN_WIDTH  500
#define WIN_HEIGHT 600

#define MAX_NUMBER_LENGTH 10
#define GENERATE_2_CHANCE_PERCENT 90
#define DISPLAY_FONT_COLORS_COUNT 11

// ==============================
// ===> GameConfig
// ==============================

typedef struct {
  char *display_font_name;
  int display_font_size;
  Color display_font_colors[DISPLAY_FONT_COLORS_COUNT];

  Color display_cell_background_color;
  
  float display_board_size;
  float display_cell_gap;

  // Auto set in game_init(), we don't need to specify these values.
  float display_window_padding;
  float display_cell_size;
  Font display_font;

} GameConfig;

static GameConfig game_conf = {
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

  .display_cell_background_color = GRAY,

  .display_board_size = (float) 450,
  .display_cell_gap   = (float) 10,
};

// ==============================
// ===> Game
// ==============================

typedef struct {
  int board[BOARD_SIZE][BOARD_SIZE];
  int point;
  GameConfig config;
} Game;

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
      snprintf(text, MAX_NUMBER_LENGTH, "%d", number);
      Vector2 text_size = MeasureTextEx(config->display_font, text, config->display_font_size, 0);
      Vector2 text_pos = {
        (x + config->display_cell_size / 2) - (text_size.x / 2),
        (y + config->display_cell_size / 2) - (text_size.y / 2)
      };
      Color text_color = config->display_font_colors[((int) log2(number) - 1) % DISPLAY_FONT_COLORS_COUNT];
      DrawTextEx(config->display_font, text, text_pos, config->display_font_size, 0, text_color);
    }
  }
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

void game_merge_row(int row[BOARD_SIZE]) {
  int temp[BOARD_SIZE] = {0};
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
      j++;
    } else {
      row[idx++] = temp[j];
    }
  }
}

void game_merge_left(Game *g) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    game_merge_row(g->board[i]);
  }
}

// Return true if the user made a move and changed the board.
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
