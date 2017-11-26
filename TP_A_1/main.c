/******************************************************************************
 *                 TRABALHO PRATICO PARTE A - INTERRUPCOES                    *
 *   Nesta parte voce devera desenvolver um pisca-pisca utilizando o LED      *
 *   vermelho do MSP430. O LED devera ficar 1 segundo aceso e 2 segundos      *
 *   apagado. E importante mencionar que voces deverao calcular o tempo       *
 *   corretamente a partir da interrupcao de tempo.                           *
 *                                                                            *
 *   -> LED VERMELHO = 0x01                                                   *
 *   -> time = variavel para calcular estado de tempo                         *
 *   -> 1 segundo e CCR0 =  62500 (2 * 62500 * 8 us = 1 segundo)              *
 ******************************************************************************/

#include "msp430g2553.h"  
#include <stdio.h>
#define uint8_t unsigned short
volatile uint8_t  time;

void main(void) {
 
  WDTCTL = WDTPW + WDTHOLD;   // Desliga Watchdog timer
  P1DIR = 0x01;               // Pino 1.0 como saida (00000001=1[DEC]=1[HEX])

  time = 2;                   // Contador de segundos para acender (2 segundos)
  CCTL0 = CCIE;               // Habilita interrupcao de comparacao do timer A           
  TACTL = TASSEL_2+MC_3+ID_3; // SMCLK = 1 MHz, SMCLK/8 = 125 KHz (8 us)      
  CCR0 =  62500;              /* Modo up/down: chega no valor e depois volta
                                 para zero, portanto cada interrupcao acontece
                                 2 * 62500 * 8 us = 1 segundo    */

  _BIS_SR(CPUOFF + GIE);      // Modo de baixo consumo com interrupcoes
  while(1);                   // Loop infinito
} 

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {   
    if (time == 0){             // Se faltam 0 segundos para acender...
        P1OUT = P1OUT ^ 0x01;   // Acende LED vermelho 0x01
        time = 2;               // Muda o tempo para 2 segundos para acender
    } else {                    // Se ainda falta tempo para acender...
        P1OUT = P1OUT & 0xfe;   // LED vermelho permanece desligado
        time --;                // Diminui o tempo para acender
    }
} 
