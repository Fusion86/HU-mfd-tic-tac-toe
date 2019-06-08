#pragma once

#include "cout.hpp"
#include "hwlib.hpp"

// Taken from hwlib/library/graphics/hwlib-graphics-window-demos.hpp
uint_fast32_t HWLIB_WEAK rand() {
    static uint_fast32_t n = 0;
    n = n * 214013L + 2531011L;
    return (n >> 16) & 0x7fff;
}

// Taken from hwlib/library/graphics/hwlib-graphics-window-demos.hpp
uint_fast32_t HWLIB_WEAK random_in_range(
    uint_fast32_t min,
    uint_fast32_t max) {
    auto x = rand();
    x = x % (max - min + 1);
    return min + x;
}

static void fatal_error(const char* desc) {
    cout << "FATAL: " << desc << "\n";

    // Block program
    while (true) {};
}

// Only works for matrices with 9 items
static void print_matrix(uint8_t mat[]) {
    cout << "----------\n";
    for (int i = 0; i < 9; i++) {
        if (i != 0 && i % 3 == 0)
            cout << "\n";

        cout << mat[i] << " ";
    }
    cout << "\n----------\n\n";
}

// Count distinct elements in array
// Based on http://www.techcrashcourse.com/2016/08/program-to-print-all-distinct-elements-array.html
static int select_distinct(uint8_t array[], uint8_t distinct[], int size) {
    int i, j;
    int idx = 0;

    for (i = 0; i < size; i++) {
        for (j = i + 1; j < size; j++) {
            if (array[i] == array[j]) {
                /* Duplicate element found */
                break;
            }
        }

        if (j == size)
            distinct[idx++] = array[i];
    }

    return idx;
}

// Taken from hwlib demo code
static void pin_pulse(hwlib::pin_out& pin, bool x, uint16_t us) {
    pin.write(x);
    pin.flush();
    hwlib::wait_us(us);
    pin.write(0);
    pin.flush();
}
