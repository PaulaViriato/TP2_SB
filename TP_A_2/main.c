/******************************************************************************
 *                 TRABALHO PRATICO PARTE A - SEMAFORO                        *
 *  Nesta parte, sera utilizado o conceito de interrupcoes para o             *
 *  desenvolvimento de um simulador de semaforos utilizando o MSP430.         *
 *                                                                            *
 *  O semaforo utilizara os dois LEDs contidos no launchpad do MSP430. O LED  *
 *  0 e a luz vermelha, o LED 1 a verde, e ao acender os dois LED             *
 *  simultaneamente sera a luz amarela. Alem dos LEDs o semaforo tambem ira   *
 *  utilizar o botao do kit de desenvolvimento como entrada, simulando um     *
 *  pedestre esperando a luz vermelha para atravessar.                        *
 *                                                                            *
 *  O semaforo funcionara da seguinte forma:                                  *
 *  Sem o pedestre apertar o botao: a luz vermelha devera ficar acesa 2       *
 *  segundos, a verde 8, e a amarela 1.                                       *
 *                                                                            *
 *  Com o pedestre apertando o botao: se as luzes vermelha ou amarela         *
 *  estiverem acesas, os tempos definidos anteriormente devem ser mantidos.   *
 *  Caso a luz esteja em verde, o tempo para a luz verde sera decrementado em *
 *  5 segundos, entretanto caso o tempo restante para a luz verde seja menor  *
 *  que 3 segundos, o tempo restante sera mantido.                            *
 *                                                                            *
 *   -> LED VERMELHO = 0x01                                                   *
 *   -> LED VERDE = 0x40                                                      *
 *   -> LED VERMELHO E LED VERDE = 0x01 + 0x40                                *
 *   -> BOTAO UTILIZADO E PINO 1.3 (0x08)                                     *
 *   -> id = definir estado atual (vermelho, verde ou amarelo)                *
 *   -> time = variavel para calcular tempo restante do estado atual          *
 *   -> 1 segundo e CCR0 =  62500 (2 * 62500 * 8 us = 1 segundo)              *
 ******************************************************************************/

#include "msp430g2553.h"  
#include <stdio.h>
#define uint8_t unsigned short
volatile uint8_t  id;
volatile uint8_t  time;

void main(void) {
 
  WDTCTL = WDTPW + WDTHOLD;   // Desliga Watchdog timer
  P1SEL = 0x00;
  P1DIR = 0x01 + 0x40;        // Pinos 1.0 e 1.6 como saida (01000001=0x01+0x40)
  P1REN = 0x08;               // Habilita pullup/pulldown do pino 1.3 (00001000)
  P1OUT = 0x08;               // Define pullup para o pino 1.3 (00001000=0x08)

  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
   
  id = 2;                     // Definicao de estado inicial (verde)
  time = 0;                   // Tempo restante para o proximo estado (0 seg)
  CCTL0 = CCIE;               // Habilita interrupcao de comparacao do timer A
  TACTL = TASSEL_2+MC_3+ID_3; // SMCLK = 1 MHz, SMCLK/8 = 125 KHz (8 us)      
  CCR0 =  62500;              /* Modo up/down: chega no valor e depois volta
                                 para zero, portanto cada interrupcao acontece
                                 2 * 62500 * 8 us = 1 segundo */   

  _BIS_SR(CPUOFF + GIE);      // Modo de baixo consumo com interrupcoes
  while(1);                   // Loop infinito
} 

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {   
    if (id == 0){
        if (time == 0){
            id = 1;
            P1OUT = P1OUT & 0x00;      // Inverte saida no pino 1.0 com ou-exclusivo                 
            P1OUT = P1OUT ^ (0x01 + 0x40);      // Inverte saida no pino 1.0 com ou-exclusivo                 
            time = 1;
        } else { time--; }
    } else {
        if (id == 1){
            if (time == 0){
                id = 2;
                P1OUT = P1OUT & 0x00;      // Inverte saida no pino 1.0 com ou-exclusivo                 
                P1OUT = P1OUT ^ 0x40;      // Inverte saida no pino 1.0 com ou-exclusivo                 
                time = 8;
            } else { time--; }
        } else {
            if (id == 2){
                if ((P1IN & 0x08) == 1){
                    P1OUT = P1OUT & 0x00;      // Inverte saida no pino 1.0 com ou-exclusivo                 
                }
                if (time == 0){
                    id = 0;
                    P1OUT = P1OUT & 0x00;      // Inverte saida no pino 1.0 com ou-exclusivo                 
                    P1OUT = P1OUT ^ 0x01;      // Inverte saida no pino 1.0 com ou-exclusivo                 
                    time = 2;
                } else { time--; }
            }
        }
    }
} 
