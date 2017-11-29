/******************************************************************************
 *                 TRABALHO PRATICO PARTE A - INTERRUPCOES                    *
 *   Nesta parte voce devera desenvolver um pisca-pisca utilizando o LED      *
 *   vermelho do MSP430. O LED devera ficar 1 segundo aceso e 2 segundos      *
 *   apagado. E importante mencionar que voces deverao calcular o tempo       *
 *   corretamente a partir da interrupcao de tempo.                           *
 *                                                                            *
 *   -> LED VERMELHO = 0x01                                                   *
 *   -> 1 segundo e CCR0 =  3000 (TACTL = TASSEL_1 + MC_1 + ID_2)             *
 ******************************************************************************/

#include "msp430g2553.h"  

void main(void) {
 
  WDTCTL = WDTPW + WDTHOLD;   // Desliga Watchdog timer
  P1DIR = 0x01;               // Pino 1.0 como saida (00000001=1[DEC]=1[HEX])

  BCSCTL1 = CALBC1_1MHZ;      // Define clock como 1 MHz
  DCOCTL = CALDCO_1MHZ;       // Define clock do time como 1 MHz
  
  BCSCTL3 = BCSCTL3 | LFXT1S_2;                // LFXT1 = VLO
  IFG1 = IFG1 & (~OFIFG);                      // Limpa a flag OSCFault
  BCSCTL2 = BCSCTL2 | (SELM_0+DIVM_3+DIVS_3);  // MCLK = DCO/8 , SMCLK = DCO/8

  CCTL0 = CCIE;                // Habilita interrupcao de comparacao do timer A
  TACTL = TASSEL_1+MC_1 +ID_2; // ACLK = 32 KHz, ACLK/4 = 8 KHz (0.5 us)
  CCR0 =  3000;                // Modo Up: chega no valor e chama a interrupcao
                               // 3000 * 0.5 us = 1 segundo

  P1OUT = P1OUT & 0x00;      // Apaga todos os LEDS
  _BIS_SR(CPUOFF + GIE);      // Modo de baixo consumo com interrupcoes
  while(1);                   // Loop infinito
} 

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
    P1OUT = P1OUT ^ 0x01;   // Troca o estado do LED vermelho (0x01)
    P1IFG = 0x00;           // Zera flag de interrupcao (00000000)
} 
