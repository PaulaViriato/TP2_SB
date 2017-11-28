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
 *   -> CCR0 = define tempo para chamar a interrupção de timer                *
 *   -> 1 segundo e CCR0 =  3000 (TACTL = TASSEL_1 + MC_1 + ID_2)             *
 ******************************************************************************/

#include "msp430g2553.h"
#define uint8_t unsigned short
volatile uint8_t  id = 2;

void main(void) {

  WDTCTL = WDTPW + WDTHOLD;   // Desliga Watchdog timer
  P1DIR = 0x01 + 0x40;        // Define pinos 1.0 e 1.6 como saída (0100 0001)
  P1IE = 0x08;                // Habilita interrupcao no pino 1.3 (00001000)
  P1IES = 0x08;               // Define borda de subida no pino 1.3 (00001000)

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
  P1OUT = P1OUT ^ 0x01;      // Acende LED vermelho (Estado 2)

  _BIS_SR(CPUOFF + GIE);
  while(1);
} 

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
  if (id == 0){                          // Fim do estado 0 (Luz verde)
      id = 1;                            // Troca para o estado 1 (amarela)
      CCR0 = 3000;                       // Modifica tempo do estado
      P1OUT = P1OUT & 0x00;              // Apaga todos os LEDS
      P1OUT = P1OUT ^ (0x01 + 0x40);     // Acende amarelo (R + G) todos
  } else {
      if (id == 1){                      // Fim do estado 1 (Luz amarela)
          id = 2;                        // Troca para o estado 2 (vermelho)
          CCR0 = 6000;                   // Modifica tempo do estado
          P1OUT = P1OUT & 0x00;          // Apaga todos os LEDS
          P1OUT = P1OUT ^ 0x01;          // Acende LED vermelho (0x01)
      } else {
          if (id == 2){                  // Fim do estado 2 (Luz vermelha)
              id = 0;                    // Troca para o estado 0 (verde)
              CCR0 = 24000;              // Modifica tempo do estado
              P1OUT = P1OUT & 0x00;      // Apaga todos os LEDS
              P1OUT = P1OUT ^ 0x40;      // Acende LED verde (0x40)
          }
      }
  }
  P1IFG = 0x00;                       // Zera flag de interrupcao (00000000)
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
  if (id == 0){  // Se o estado está no sinal verde...
                 // Se o tempo restante está entre 5 e 3 segundo termina
      if ((CCR0 <= 15000)&&(CCR0 > 9000)){ CCR0 = 1000; }
      else {
                 // Se o tempo restante é maior que 5 diminui 5 segundos
          if (CCR0 > 15000){ CCR0 = CCR0 - 14000; }
      }
  }
  P1IFG = 0x00;                   // Zera flag de interrupcao (00000000)
}