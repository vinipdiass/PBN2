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
#include <avr/interrupt.h>
#include <stdio.h>
#include <time.h>

uint8_t glyph[] = {0b00010000, 0b00100100, 0b11100000, 0b00100100, 0b00010000};

#define TIMER_CLK		F_CPU / 1024
#define IRQ_FREQ		15625


typedef struct{
    int x;
    int y;
    int xTiro;
    int yTiro;
    int bTiro;
    int matrizX;
    int matrizY;
}inimigo;

typedef struct{
    int xPlayer;
    int yPlayer;
    int xTiro;
    int yTiro;
    int bTiro;
    int nOponentes;
    int matrizOponentes[2][4];
    inimigo oponentes[8];
}objetosTela;

objetosTela tela = {0, 0, 0, 0, 0, 0};

int *xPlayer = &tela.xPlayer;
int *yPlayer = &tela.yPlayer;
int *xTiro = &tela.xTiro;
int *yTiro = &tela.yTiro;
int *bTiro = &tela.bTiro;
int cont = 0;


ISR(TIMER1_COMPA_vect) {
    nokia_lcd_clear();
    //Desenha a nave
    nokia_lcd_set_cursor(tela.xPlayer, tela.yPlayer);
    nokia_lcd_write_char(2, 2);
    //Desenha Tiro
    if (tela.bTiro == 1){
        nokia_lcd_set_cursor(tela.xTiro, tela.yTiro);
        nokia_lcd_write_char(3, 2);
        if(cont == 100) {
            tela.xTiro += 8;
            nokia_lcd_render();
            if(tela.xTiro == 80){
               tela.bTiro = 0;
            }
            cont = 0;
        }
        
    }


    nokia_lcd_render();
    cont++;
}



int main(void)
{
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1, glyph);

    // Entradas
    DDRD &= ~(1 << PD6); // Cima
    DDRD &= ~(1 << PD7); // Baixo
    DDRB &= ~(1 << PB0); // Atira

    // Assets
    uint8_t nave[5] = {
            0B1011101,
            0B1110111,
            0B0010100,
            0B0001000,
            0B0001000,
    };

    uint8_t tiro[5] = {
            0B0001000,
            0B0001000,
            0B0001000,
            0B0001000,
            0B0001000,
    };

    /*uint8_t invasor[5] = {
            0B0010001,
            0B0010101,
            0B0001010,
            0B0001110,
            0B0010001,
    };*/

    // Configuração do TIMER
    cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	OCR1A = (TIMER_CLK / IRQ_FREQ) - 1;
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);  
	TIMSK1 |= (1 << OCIE1A);
    PORTC |= 1 << PC1;
    sei();

    nokia_lcd_custom(2, nave);
    nokia_lcd_custom(3, tiro);
    while (1) {
        
        if (PIND & (1 << PD7)){
            if (tela.yPlayer!=32) tela.yPlayer+=4;
        }

        if (PIND & (1 << PD6)){
            if (tela.yPlayer!=0) tela.yPlayer-=4;
        }
        
        if (PINB & (1 << PB0)){
            tela.bTiro = 1;
            tela.xTiro = 8;
            tela.yTiro = tela.yPlayer;
        }

        /*if (usuario apertar v){
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
