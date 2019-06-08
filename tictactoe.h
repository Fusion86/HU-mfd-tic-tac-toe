#pragma once

#include "hwlib.hpp"

#include "utility.h"

// #define DONT_PRINT_INTRO // Uncomment to disable

namespace target = hwlib::target;

enum PLAYER : uint8_t {
    PLAYER_RED = 1 << 0,
    PLAYER_BLUE = 1 << 1,
};

enum GAMEBOARD_STATE : uint8_t {
    GAMEBOARD_STATE_UNSET = 1 << 2, // We start at 2 because this is used in the same array (gameboard) as PLAYER
};

// You could theoretically AND multiple colors together (to mix colors), however
// the render_screen() function doesn't currently support that.
enum LED_STATE : uint8_t {
    LED_OFF = 1 << 0,
    LED_RED = 1 << 1,
    LED_GREEN = 1 << 2,
    LED_BLUE = 1 << 3,

    // Composite colors
    LED_WHITE = LED_RED | LED_GREEN | LED_BLUE,
};

enum RESULT : uint8_t {
    RESULT_FALSE = 0,
    RESULT_TRUE = 1,
    RESULT_NEWGAME = 2,
};

class tictactoe {
  private:
    target::board board;

    // Contains player's marks
    uint8_t gameboard[9];

    // Cursor position
    int cursor_pos = 0;

    // LEDs sate buffer
    uint8_t led_buffer[9] = {};

    hwlib::pin_in& newgame_sw = board.sw1;
    hwlib::pin_in& whoami_sw = board.sw3;

    // These are in an array so that we can iterate over them
    hwlib::pin_out* leds[9];

  public:
    tictactoe();

    // Main loop
    void run();

  private:
    // Play a full round, including color select etc
    void play_game();

    //
    RESULT player_set_position(PLAYER player);

    // Returns selected LEDs index (0-8) using the potentiometer's position
    int get_position();

    // Set players mark at selected position
    // Returns true on success, and false when position couldn't be set
    bool set_mark(PLAYER player, int position);

    // Resets gameboard
    void reset_gameboard();

    // Cursor is a white LED to indicate what the currently selection position is (using the potentiometer)
    void set_cursor(int position);

    // Remove cursor
    void unset_cursor();

    // Check if there is a winner, and if ther is then flash the screen in the winner's coll
    bool check_winner();

    // Return winner, or -1 if there is no winner
    int get_winner();

    // Helper function, returns true if row is not GAMEBOARD_STATE_UNSET and all three positions are the same value
    bool row_has_winner(uint8_t a, uint8_t b, uint8_t c);

    // Update frame/screen/LEDs position and flush
    void render_screen();
};

static const char* player_to_string(PLAYER player) {
    switch (player) {
    case PLAYER_RED: return "RED";
    case PLAYER_BLUE: return "BLUE";
    default: return "UNKOWN";
    }
}

static const char* ledstate_to_string(LED_STATE state) {
    switch (state) {
    case LED_OFF: return "LED_OFF";
    case LED_RED: return "LED_RED";
    case LED_GREEN: return "LED_GREEN";
    case LED_BLUE: return "LED_BLUE";
    case LED_WHITE: return "LED_WHITE";
    default: return "UNKOWN";
    }
}
