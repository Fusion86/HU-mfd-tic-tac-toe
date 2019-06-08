#pragma once

#include "hwlib.hpp"

#define VOID_COUT // Uncomment this to disable all printing, which is pretty slow

class ostream_void : public hwlib::ostream {
  public:
    void putc(char c) override {}
    void flush() override {}
};

#ifndef VOID_COUT
hwlib::cout_using_uart_putc HWLIB_WEAK cout;
#else
ostream_void HWLIB_WEAK cout;
#endif
