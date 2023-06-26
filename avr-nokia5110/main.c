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
}inimigo;

typedef struct{
    int xPlayer;
    int yPlayer;
    int xTiro;
    int yTiro;
    int bTiro;
    inimigo oponentes[6];
    int nOponentes;
}objetosTela;

void desenhaTela(objetosTela tela){
    nokia_lcd_clear();
    //Desenha a nave
    nokia_lcd_set_cursor(tela.xPlayer, tela.yPlayer);
    nokia_lcd_write_char(2, 2);

    //Desenha o tiro do jogador
    if (tela.bTiro == 1){
        nokia_lcd_set_cursor(tela.xTiro, tela.yTiro);
        nokia_lcd_write_char(3, 2);  
    }

    //Teste: Desenha oponentes na tela inteira
    for (int i = 0; i < 4; i++){
        inimigo inimigoAtual = tela.oponentes[i];
        nokia_lcd_set_cursor(68, i*12);
        nokia_lcd_set_cursor(44, i*12);
        nokia_lcd_write_char(4, 2); 
    }
    /*
    //Desenha oponentes
    for (int i = 0; i < tela.nOponentes; i++){
        inimigo inimigoAtual = tela.oponentes[i];
        nokia_lcd_set_cursor(inimigoAtual.x, inimigoAtual.y);
        nokia_lcd_write_char(4, 1); 
    }
    */

    nokia_lcd_render();
}

objetosTela invocaInimigo(objetosTela tela){
    srand(time(NULL));
    int indexInimigo = tela.nOponentes;

    if (indexInimigo >= 6)
    return tela;

    inimigo inimigoAtual;
    inimigoAtual.bTiro = 0;
    inimigoAtual.x = 68;
    inimigoAtual.y = (rand() % 9) * 4;
    //Se for igual ao Y de um outro inimigo já existente, faz dnv, n pode ter dois em um mesmo lugar
    for (int i = 0; i < tela.nOponentes; i++){
        if (inimigoAtual.y == tela.oponentes[i].y){
            i = 0;
            inimigoAtual.y = (rand() % 9) * 4;
        }
    }
    tela.oponentes[indexInimigo] = inimigoAtual;
    tela.nOponentes++;
    return tela;
}

objetosTela tiroPlayer(objetosTela tela){
    tela = invocaInimigo(tela);
    tela.bTiro = 1;
    tela.xTiro = 8;
    tela.yTiro = tela.yPlayer;
    for (int i = 0; i<16; i++){
        desenhaTela(tela);
        _delay_ms(30);
        tela.xTiro += 4;
    }
    tela.bTiro = 0;
    desenhaTela(tela);
    return tela;
}

void moveNave(int x, int y){
    nokia_lcd_clear();
    nokia_lcd_set_cursor(x, y);
    nokia_lcd_write_char(2, 2);
    nokia_lcd_render();
    _delay_ms(100);
}

ISR(TIMER1_COMPA_vect) {
    // Aqui vai a função de animação dos disparos 
}


int main(void)
{
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
    
    uint8_t nave[5] = {
        0B1011101,
        0B1110111,
        0B0010100,
        0B0001000,
        0B0001000,
    };
    nokia_lcd_custom(2, nave);
    
    uint8_t tiro[5] = {
        0B0001000,
        0B0001000,
        0B0001000,
        0B0001000,
        0B0001000,
    };
    nokia_lcd_custom(3, tiro);

    uint8_t invasor[5] = {
        0B0010001,
        0B0010101,
        0B0001010,
        0B0001110,
        0B0010001,
    };
    nokia_lcd_custom(4, invasor);

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


    objetosTela tela;

    tela.xPlayer = 0;
    tela.yPlayer = 0;
    tela.xTiro = 0;
    tela.yTiro = 0;
    tela.bTiro = 0;
    tela.nOponentes = 0;


    //lcd write p1: qual objeto | p2: qual o tamanho

    while (1) {
        if (PIND & (1 << PD7)){
            if (tela.yPlayer!=32) tela.yPlayer+=4;
            desenhaTela(tela);
            _delay_ms(100);
        }

        if (PIND & (1 << PD6)){
            if (tela.yPlayer!=0) tela.yPlayer-=4;
            desenhaTela(tela);
            _delay_ms(100);
        }
        
        if (PINB & (1 << PB0)){
            tela = tiroPlayer(tela);
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
