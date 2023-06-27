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

long tempo = 0;
long inicioTiro = 0;

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
    inimigo oponentes[8];
    int nOponentes;
    int matrizOponentes[2][4];
    int tiroFim;
}objetosTela;

objetosTela desenhaTela(objetosTela tela){
    nokia_lcd_clear();
    //Desenha a nave
    nokia_lcd_set_cursor(tela.xPlayer, tela.yPlayer);
    nokia_lcd_write_char(2, 2);

    //Desenha o tiro do jogador
    if (tela.bTiro == 1){
        nokia_lcd_set_cursor(tela.xTiro, tela.yTiro);
        nokia_lcd_write_char(3, 2);
        int xTiroMatriz = -1;
        int yTiroMatriz = -1;

        if (tela.yTiro < 8 ) yTiroMatriz = 0;
        else
            if (tela.yTiro < 20 ) yTiroMatriz = 1;
            else
                if (tela.yTiro < 32 ) yTiroMatriz = 2;
                else
                    if (tela.yTiro == 32 ) yTiroMatriz = 3;
        
        if (tela.xTiro == 68) xTiroMatriz = 0;
        if (tela.xTiro == 56) xTiroMatriz = 1;
        
        for (int i = 0; i < tela.nOponentes; i++){
            if ((xTiroMatriz == tela.oponentes[i].matrizX) && (yTiroMatriz == tela.oponentes[i].matrizY)){
                tela.nOponentes--;
                for(int j = i; j < tela.nOponentes; j++){
                    tela.oponentes[j] = tela.oponentes[j+1];
                }
                tela.bTiro = 0;
                tela.tiroFim = 0;
                inicioTiro = 0;
                tempo = 0;
                //tela.oponentes[tela.nOponentes] = NULL;
            }
        }
    }

    /*
    //Teste: Desenha oponentes na tela inteira
    for (int i = 0; i < 4; i++){
        inimigo inimigoAtual = tela.oponentes[i];
        nokia_lcd_set_cursor(68, i*12);
        nokia_lcd_write_char(4, 2);
        nokia_lcd_set_cursor(56, i*12);
        nokia_lcd_write_char(4, 2); 
    }
    */
    

    //Desenha oponentes
    for (int i = 0; i < tela.nOponentes; i++){
        inimigo inimigoAtual = tela.oponentes[i];
        nokia_lcd_set_cursor(inimigoAtual.x, inimigoAtual.y);
        nokia_lcd_write_char(4, 2); 
    }
    

    nokia_lcd_render();
    return tela;
}

objetosTela invocaInimigo(objetosTela tela){
    srand(time(NULL));
    int indexInimigo = tela.nOponentes;

    if (indexInimigo >= 8)
    return tela;

    inimigo inimigoAtual;
    inimigoAtual.bTiro = 0;

    inimigoAtual.matrizX = (rand() % 2);
    inimigoAtual.x = 68 - (inimigoAtual.matrizX * 12);
    inimigoAtual.matrizY = rand() % 4;
    inimigoAtual.y = inimigoAtual.matrizY * 12;

    //Se for igual ao Y e X de um outro inimigo já existente, faz dnv, n pode ter dois em um mesmo lugar
    for (int i = 0; i < tela.nOponentes; i++){
        if (inimigoAtual.y == tela.oponentes[i].y && inimigoAtual.x == tela.oponentes[i].x){
            i = 0;
            inimigoAtual.matrizX = (rand() % 2);
            inimigoAtual.x = 68 - (inimigoAtual.matrizX * 12);
            inimigoAtual.matrizY = rand() % 4;
            inimigoAtual.y = inimigoAtual.matrizY * 12;
        }
    }
    tela.oponentes[indexInimigo] = inimigoAtual;
    tela.matrizOponentes[inimigoAtual.matrizX][inimigoAtual.matrizY] = 1;
    tela.nOponentes++;
    return tela;
}

objetosTela tiroPlayer(objetosTela tela){
    tela.bTiro = 1;
    tela.xTiro = 8;
    tela.yTiro = tela.yPlayer;
    //for (int i = 0; i<16; i++){
    //}
    //tela.bTiro = 0;
    tela = desenhaTela(tela);
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
    tempo++;

    if (tempo % 15000 == 0)
    tela = invocaInimigo(tela); 
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

    DDRC |= (1 << PC0); // Cima
    
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
    inicioTiro = 0;
    long marcaTiro;
    //marcaTiro = clock();
    //double tempoTiro = 0;
    tela.tiroFim = 0;
    //lcd write p1: qual objeto | p2: qual o tamanho
    /*nokia_lcd_write_string("Defenda a Terra!",1);
    nokia_lcd_set_cursor(0, 12);
    nokia_lcd_write_string("Aperte para Começar", 2);
    nokia_lcd_render();*/

    //if(PIND & (1 << PD7) || PIND & (1 << PD6) || PINB & (1 << PB0)) {
        while (1) {
            if (PIND & (1 << PD7)){
                if (tela.yPlayer!=32) tela.yPlayer+=4;
                tela = desenhaTela(tela);
                _delay_ms(100);
            }

            if (PIND & (1 << PD6)){
                if (tela.yPlayer!=0) tela.yPlayer-=4;
                tela = desenhaTela(tela);
                _delay_ms(100);
            }
        
            if (PINB & (1 << PB0) && tela.tiroFim == 0){
                tela = tiroPlayer(tela);
                _delay_ms(100);
                inicioTiro = 1;
            }
            
            if(inicioTiro > 0) {
                PORTC |= (1 << PC0);
                inicioTiro += tempo;
            }
            //tempoTiro = (double)(marcaTiro - inicioTiro) / CLOCKS_PER_SEC;
            if (inicioTiro >= 3000 && tela.bTiro == 1){ 
                tela = desenhaTela(tela);
                tela.xTiro += 4;
                inicioTiro = 1;
                tela.tiroFim++;
                if (tela.tiroFim == 16){
                    tela.bTiro = 0;
                    tela.tiroFim = 0;
                    inicioTiro = 0;
                    tempo = 0;
                }
            }
        }
    //}
}
