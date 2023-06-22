/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George and 2017, 2018 Rami Ali
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "library.h"
#include "mphalport.h"
#include <emscripten.h>


EM_JS(void, mp_js_sleep_with_intr, (int ms), {
  Asyncify.handleSleep(wakeUp => {
    window.do_sleep_with_intr(ms, (i) => {
        if(i == 1)
            Module.ccall('mp_keyboard_interrupt', 'null', ['null'], ['null']);
        wakeUp();
    });
  });
});

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    mp_js_write(str, len);
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_js_sleep_with_intr(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    /* first sleep for as many ms as possible */
    mp_uint_t ms_to_sleep = (us / 1000);
    if(ms_to_sleep > 0) {
        mp_js_sleep_with_intr(ms_to_sleep);
        us -= (ms_to_sleep * 1000);
    }
    /* busy-wait the remaining time */
    uint32_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
    }
}

mp_uint_t mp_hal_ticks_us(void) {
    return mp_js_ticks_ms() * 1000;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return mp_js_ticks_ms();
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}

extern int mp_interrupt_char;

int mp_hal_get_interrupt_char(void) {
    return mp_interrupt_char;
}

void mp_hal_move_cursor_back(uint pos)
{
	mp_hal_stdout_tx_strn("\b", 1);
}
void mp_hal_erase_line_from_cursor(uint pos)
{
}
