#include "tictactoe.h"

#include "cout.hpp"
#include "utility.h"

tictactoe::tictactoe() {
    leds[0] = &board.led1;
    leds[1] = &board.led2;
    leds[2] = &board.led3;
    leds[3] = &board.led4;
    leds[4] = &board.led5;
    leds[5] = &board.led6;
    leds[6] = &board.led7;
    leds[7] = &board.led8;
    leds[8] = &board.led9;
}

void tictactoe::run() {
#ifndef DONT_PRINT_INTRO
    // This wll take a second or 3 to print, also rip our flash memory space
    cout
        << "Welcome to Tic-Tac-Toe\n"
        << "\n"
        << "How this game works:\n"
        << "1. Decide which player plays with blue and red (alternative to cross and circle)\n"
        << "2. Press the yellow button to start a new game\n"
        << "3. The frame will light up either red or blue, indicating which player can go first\n" // TODO: Implement this, currently you have to press the green button to see who goes first
        << "4. After 5 seconds the main game loop will start\n"                                    // TODO: Implement this
        << "\n"
        << "The current player can select where he wants to place his cross/circle with the potentiometer.\n"
        << "Confirm the selected position by pressing the button with your color (red/blue)\n"
        << "As soon a player wins the LEDs will blink the winning row alternating\n" // TODO: Currently the screen won't flash, it'll just light up in the winning color
        << "between the winning color and white to indicate which row won the game.\n"
        << "\n"
        << "- After the game has ended you can press the yellow button to start a new game\n"
        << "- You can press the green button to display which player's turn it is\n"
        << "\n"
        << "With that said, lets get started!\n\n";
#endif

    while (true) {
        reset_gameboard();
        play_game();
    }
}

void tictactoe::play_game() {
    cout << "Deciding which player should go first...\n";
    PLAYER player1 = (PLAYER)random_in_range(PLAYER_RED, PLAYER_BLUE);
    PLAYER player2 = (player1 == PLAYER_RED) ? PLAYER_BLUE : PLAYER_RED;
    cout << "Alright, " << player_to_string(player1) << " should go first.\n";

    RESULT res;

    while (true) {
        do {
            res = player_set_position(player1);
            if (res == RESULT_NEWGAME) return;
        } while (res != RESULT_TRUE);

        if (check_winner()) return;

        do {
            res = player_set_position(player2);
            if (res == RESULT_NEWGAME) return;
        } while (res != RESULT_TRUE);

        if (check_winner()) return;
    }
}

RESULT tictactoe::player_set_position(PLAYER player) {
    int pos = -1;
    int newpos;

    auto& confirm_sw = player == PLAYER_RED ? board.sw2 : board.sw4;

    cout
        << "Player " << player_to_string(player)
        << " please use the potentiometer to select where you want to place your mark"
        << " and confirm your selection by pressing the " << player_to_string(player) << " button.\n";

    while (true) {
        // Reset led buffer
        for (int i = 0; i < 9; i++)
            led_buffer[i] = LED_OFF;

        newpos = get_position();

        if (newpos != pos) {
            pos = newpos;
            cout << "Selected position: " << pos << "\n";
            set_cursor(pos);
        }

        // If player's button pressed, to confirm position
        if (confirm_sw.read()) {
            if (set_mark(player, pos)) {
                unset_cursor();
                return RESULT_TRUE;
            }
        }

        if (newgame_sw.read()) {
            return RESULT_NEWGAME;
        }

        // If green button is pressed, to display which player's turn it is
        if (whoami_sw.read()) {
            // Render all leds in the same color to display which player's turn it is
            auto current_player_color = (player == PLAYER_RED) ? LED_RED : LED_BLUE;
            for (int i = 0; i < 9; i++)
                led_buffer[i] = current_player_color;
        } else {
            // Render gameboard to buffer
            for (int i = 0; i < 9; i++) {
                switch (gameboard[i]) {
                case PLAYER_RED: led_buffer[i] = LED_RED; break;
                case PLAYER_BLUE: led_buffer[i] = LED_BLUE; break;
                }
            }

            // Render cursor
            if (cursor_pos != -1)
                led_buffer[cursor_pos] = LED_WHITE;
        }

        render_screen();
    }
}

int tictactoe::get_position() {
    // const auto RAW_MIN = 0; // Unused because it's zero for me
    const auto RAW_MAX = 1000; // Slightly below 1024 because we want 0-8, so excluding the upper range (9)
    const auto STEPS = 9;
    const auto STEP_SIZE = 1024 / STEPS; // 1024 is the range of the ADC

    auto raw = board.adc.read();

    if (raw > RAW_MAX)
        raw = RAW_MAX;

    return raw / STEP_SIZE;
}

bool tictactoe::set_mark(PLAYER player, int position) {
    if (gameboard[position] != GAMEBOARD_STATE_UNSET) {
        cout << "Can't place a mark on an already set position!\n";
        return false;
    }

    gameboard[position] = player;
    cout
        << "Player " << player_to_string(player) << " set his mark on position "
        << position
        << " (" << (position / 3) << ", " << (position % 3) << ")\n";
    return true;
}

void tictactoe::reset_gameboard() {
    for (int i = 0; i < 9; i++)
        gameboard[i] = GAMEBOARD_STATE_UNSET;
}

void tictactoe::set_cursor(int position) {
    if (position > 8)
        fatal_error("set_cursor: Out of range!");

    cursor_pos = position;
}

void tictactoe::unset_cursor() {
    cursor_pos = -1;
}

bool tictactoe::check_winner() {
    int winner = get_winner();

    // If there is no winner yet
    if (winner == -1) {
        cout << "No winner yet\n";
        return false;
    }

    cout << "Gratz, " << player_to_string((PLAYER)winner) << " has won this round!\n";

    uint8_t winning_color = (winner == PLAYER_RED) ? LED_RED : LED_BLUE;
    for (int i = 0; i < 9; i++)
        led_buffer[i] = winning_color;

    while (true) {
        render_screen();

        if (newgame_sw.read())
            return true;
    }
}

int tictactoe::get_winner() {
    // Horizontal
    if (row_has_winner(gameboard[0], gameboard[1], gameboard[2]))
        return gameboard[0];
    if (row_has_winner(gameboard[3], gameboard[4], gameboard[5]))
        return gameboard[3];
    if (row_has_winner(gameboard[6], gameboard[7], gameboard[8]))
        return gameboard[6];

    // Vertical
    if (row_has_winner(gameboard[0], gameboard[3], gameboard[6]))
        return gameboard[0];
    if (row_has_winner(gameboard[1], gameboard[4], gameboard[7]))
        return gameboard[1];
    if (row_has_winner(gameboard[2], gameboard[5], gameboard[8]))
        return gameboard[2];

    // Diagonal
    if (row_has_winner(gameboard[0], gameboard[4], gameboard[8]))
        return gameboard[0];
    if (row_has_winner(gameboard[2], gameboard[4], gameboard[6]))
        return gameboard[2];

    return -1;
}

bool tictactoe::row_has_winner(uint8_t a, uint8_t b, uint8_t c) {
    return a != GAMEBOARD_STATE_UNSET && a == b && b == c;
}

void tictactoe::render_screen() {
    // cout << "gameboard\n";
    // print_matrix(gameboard);
    // cout << "led_buffer\n";
    // print_matrix(led_buffer);

    // Multiplexing stuff
    uint8_t used_colors[9] = {0};
    uint8_t color_count = select_distinct(led_buffer, used_colors, 9);

    // cout << "Used colors: ";
    // for (int i = 0; i < color_count; i++)
    //     cout << ledstate_to_string((LED_STATE)used_colors[i]) << ", ";
    // cout << "\n";

    for (int i = 0; i < color_count; i++) {
        // We don't want to print the color LED_OFF (since it isn't a color)
        if (used_colors[i] == LED_OFF) continue;

        // cout << "Currently multiplexing: " << ledstate_to_string((LED_STATE)used_colors[i]) << "\n";

        // Select which LEDs we want to turn on with the selected color
        for (int j = 0; j < 9; j++) {
            // If LED at position has the same color as the currently being printed color
            if ((led_buffer[j] == used_colors[i])) {
                leds[j]->write(1);
            } else {
                leds[j]->write(0); // If color does NOT match then disable the led
            }
        }

        pin_pulse(board.red, used_colors[i] & LED_RED, 100);
        pin_pulse(board.green, used_colors[i] & LED_GREEN, 50);
        pin_pulse(board.blue, used_colors[i] & LED_BLUE, 15);

        // Do we even need these?
        board.sw1.refresh();
        board.sw2.refresh();
        board.sw3.refresh();
        board.sw4.refresh();
    }
}
