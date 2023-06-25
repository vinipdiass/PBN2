/* Nokia 5110 LCD AVR Library example
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 * Custom char code by Marcelo Cohen - 2021
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "nokia5110.h"

uint8_t glyph[] = {0b00010000, 0b00100100, 0b11100000, 0b00100100, 0b00010000};

void moveNave(int x, int y){
    nokia_lcd_clear();
    nokia_lcd_set_cursor(x, y);
    nokia_lcd_write_char(2, 2);
    nokia_lcd_render();
}

int main(void)
{
    int x = 0; //Horizontal
    int y = 0; //Vertical

    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1, glyph);
    // nokia_lcd_write_string("IT'S WORKING!",1);
    // nokia_lcd_set_cursor(0, 12);
    // nokia_lcd_write_string("Nice!\001", 2);
    // nokia_lcd_drawcircle(20,20,20);
    // nokia_lcd_render();
    // while(1);

       // Entradas
    DDRD &= ~(1 << PD6); // Cima
    DDRD &= ~(1 << PD7); // Baixo
    DDRB &= ~(1 << PB0); // Atira

    uint8_t nave[8] = {
        0B00000,
        0B11011,
        0B01010,
        0B11111,
        0B10001,
        0B11011,
        0B10101,
        0B11111,
    };

    nokia_lcd_custom(2, nave);
    //lcd write p1: qual objeto | p2: qual o tamanho

    while (1) {
        if (PIND & (1 << PD6)){
            y+=4;
            moveNave(x, y);
        }

        if (PIND & (1 << PD7)){
            y-=4;
            moveNave(x, y);
        }
        /*
        if (usuario apertar ^){
            x+=4;
            moveNave(x, y);
        }

        if (usuario apertar v){
            x-=4;
            moveNave(x, y);
        }

        if (usuario apertar >){
            y+=4;
            moveNave(x, y);
        }

        if (usuario apertar <){
            y-=4;
            moveNave(x, y);
        }
        */
    }
}
