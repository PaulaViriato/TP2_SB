#include <msp430G2553.h>
#include <stdio.h>

#define uint32_t unsigned long
#define uint16_t unsigned int
#define uint8_t unsigned short


#define BACKUP_REGS 12 /* r4 to r15*/

#define STACK_SIZE  50
#define STACK_TOP   STACK_SIZE - 1   
#define TOTAL_TASKS 3

/*Enable GIE in SR so that the WDT never stops when we go to user task*/
/*Enable SCG0 for 25MHZ CPU execution*/
#define DEFAULT_SR  ((uint16_t)0x0048) 

#define SAVE_CONTEXT()           \
  asm ( "push r4  \n\t" \
        "push r5  \n\t" \
        "push r6  \n\t" \
        "push r7  \n\t" \
        "push r8  \n\t" \
        "push r9  \n\t" \
        "push r10 \n\t" \
        "push r11 \n\t" \
        "push r12 \n\t" \
        "push r13 \n\t" \
        "push r14 \n\t" \
        "push r15 \n\t" \
      );

#define RESTORE_CONTEXT()       \
  asm ( "pop r15 \n\t" \
        "pop r14 \n\t" \
        "pop r13 \n\t" \
        "pop r12 \n\t" \
        "pop r11 \n\t" \
        "pop r10 \n\t" \
        "pop r9  \n\t" \
        "pop r8  \n\t" \
        "pop r7  \n\t" \
        "pop r6  \n\t" \
        "pop r5  \n\t" \
        "pop r4  \n\t" \
        "reti    \n\t" \
      );

/*stack for each task*/
uint16_t task1ram[STACK_SIZE];
uint16_t task2ram[STACK_SIZE];
uint16_t task3ram[STACK_SIZE];

volatile uint8_t  task_id; /*has the current running task*/
volatile uint16_t *stack_pointer[TOTAL_TASKS]; /*address of stack pointer for each task*/

/*****************************************************/
volatile uint8_t button1 = 0x1, button2=0x1; /*volatile since its a shared resource between tasks*/
/*****************************************************/

void task1(void)
{ 
  asm ("mov.w  #0x00, r4    \n\t");
  asm ("mov.w  #0x1770, r5    \n\t");
  asm ("mov.w  #0x01, r6    \n\t");
  asm ("mov.w  #0x00, r7    \n\t");

  while (1) {
      asm ("mov.w r4, TA1R \n\t");
      asm ("mov.w r5, TA1CCR0 \n\t");
  //    asm ("mov.w r6, P1OUT \n\t");
      asm ("mov.w r7, P1IFG \n\t");
      if (TA1R == TA1CCR0){
          P1OUT = P1OUT ^ 0x01;   // Troca o estado do LED vermelho (0x01)
          P1IFG = 0x00;           // Zera flag de interrupcao (00000000)
      } else { TA1R++; }
      asm ("mov.w  TA1R, r4    \n\t");
      asm ("mov.w  TA1CCR0, r5    \n\t");
  //    asm ("mov.w  P1OUT, r6    \n\t");
      asm ("mov.w  P1IFG, r7    \n\t");
  }
}

void task2(void)
{
  uint16_t i;
  asm ("mov.w  #0x00, r4    \n\t");
  asm ("mov.w  #0x1770, r5    \n\t");
  asm ("mov.w  #0x40, r6    \n\t");
  asm ("mov.w  #0x00, r7    \n\t");

  while (1) {
      asm ("mov.w r4, TA1R \n\t");
      asm ("mov.w r4, TA1R \n\t");
      asm ("mov.w r6, P1OUT \n\t");
      asm ("mov.w r7, P1IFG \n\t");
      if (TA1R == TA1CCR0){
          P1OUT = P1OUT ^ 0x40;   // Troca o estado do LED verde (0x40)
          P1IFG = 0x00;           // Zera flag de interrupcao (00000000)
      } else { TA1R++; }
      asm ("mov.w  TA1R, r4    \n\t");
      asm ("mov.w  TA1CCR0, r5    \n\t");
      asm ("mov.w  P1OUT, r6    \n\t");
      asm ("mov.w  P1IFG, r7    \n\t");
  }
}

volatile uint16_t i;
void task3(void)
{
  uint16_t i;

  asm ("mov.w  #0x00, r4    \n\t");
  asm ("mov.w  #0x00, r5    \n\t");
  asm ("mov.w  #0x00, r6    \n\t");

  while (1) {
      asm ("mov.w r4, P1IN \n\t");
      asm ("mov.w r5, P1IFG \n\t");
      asm ("mov.w r6, i \n\t");

//      P1OUT = P1OUT & 0x00;   // Troca o estado do LED verde (0x40)
      if((P1IN & 0x08) == 1){
//          P1OUT = P1OUT & 0x00;   // Troca o estado do LED verde (0x40)
          for(i= 0; i< 8; i++) { asm ("pop r4 \n\t"); }
          asm ("pop r5 \n\t"); 
          asm ("pop r6 \n\t"); 
          asm ("pop r7 \n\t"); 
          asm ("pop r8 \n\t"); 

          for(i= 0; i< 8; i++) { asm ("pop r4 \n\t"); }
          asm ("pop r9 \n\t"); 
          asm ("pop r10 \n\t"); 
          asm ("pop r11 \n\t"); 
          asm ("pop r12 \n\t"); 

          asm ("mov.w  r9, r13    \n\t");
          asm ("mov.w  r5, r9    \n\t");
          asm ("mov.w  r13, r5    \n\t");

          asm ("push r12  \n\t");
          asm ("push r11  \n\t");
          asm ("push r10  \n\t");
          asm ("push r9  \n\t");
          for(i= 0; i< 8; i++) { asm ("push r4 \n\t"); }

          asm ("push r8  \n\t");
          asm ("push r7  \n\t");
          asm ("push r6  \n\t");
          asm ("push r5  \n\t");
          for(i= 0; i< 8; i++) { asm ("push r4 \n\t"); }

          P1IFG = 0x00;           // Zera flag de interrupcao (00000000)
      }
      asm (  "mov.w  P1IN, r4    \n\t");
      asm (  "mov.w  P1IFG, r5    \n\t");
      asm (  "mov.w  i, r6    \n\t");
  }
}


/*****************************************************/

/*
 * This function will initialise stack for each task. Following are filled into the stack
 * 1) Store the PC first since it has to be poped back and loaded at end 
 * 2) then store SR register. This determines the CPU condition at which your task should execute
 * 3) leave the space for the registers r4 to r15 since the scheduler will pop these when a task switching occurs
 * 3) return the address of stack which will contain all informations for the task to execute after task switching
 * 4) TODO: fill the end of stack with known values to detect buffer overflows.
 */
uint16_t *initialise_stack(void (* func)(void), uint16_t *stack_location)
{
  uint8_t i;
  
  *stack_location = (uint16_t)func;
  stack_location--;

  *stack_location = (((uint16_t)((uint32_t)(0xf0000 & (uint32_t)func) >> 4))| DEFAULT_SR); 
  
  for(i= 0; i< BACKUP_REGS; i++) { stack_location--; }
  
  return stack_location;
}


  
volatile uint16_t *temp;
void main(void)
{
  
  //Stop the watchdog timer until we configure our scheduler
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR = 0x01 + 0x40;               // Pino 1.0 como saida (00000001=1[DEC]=1[HEX])
  P1IE = 0x08;                // Habilita interrupcao no pino 1.3 (00001000)
  P1IES = 0x08;               // Define borda de subida no pino 1.3 (00001000)

  BCSCTL1 = CALBC1_1MHZ;      // Define clock como 1 MHz
  DCOCTL = CALDCO_1MHZ;       // Define clock do time como 1 MHz
  
  BCSCTL3 = BCSCTL3 | LFXT1S_2;                // LFXT1 = VLO
  IFG1 = IFG1 & (~OFIFG);                      // Limpa a flag OSCFault
  BCSCTL2 = BCSCTL2 | (SELM_0+DIVM_3+DIVS_3);  // MCLK = DCO/8 , SMCLK = DCO/8
 
  
  /*initialise stack for each task*/
  stack_pointer[0] = initialise_stack(task1, &task1ram[STACK_TOP]);
  stack_pointer[1] = initialise_stack(task2, &task2ram[STACK_TOP]);
  stack_pointer[2] = initialise_stack(task3, &task3ram[STACK_TOP]);

  TA0CCTL0 = CCIE;               // Habilita interrupção de comparação do timer A           
  TA0CTL = TASSEL_1+MC_1 +ID_2; // ACLK = 32 KHz, ACLK/4 = 8 KHz (0.5 us)
  TA1CCTL0 = CCIE;               // Habilita interrupção de comparação do timer A           
  TA1CTL = TASSEL_1+MC_1 +ID_2; // ACLK = 32 KHz, ACLK/4 = 8 KHz (0.5 us)
  TA0CCR0 =  3000;              // Modo up/down: chega no valor e depois volta
                              // para zero, portanto cada interrupção acontece
                              // 2 * 62500 * SMCLK/8 = 1 segundo 

  /*initialise to first task*/
  task_id = 0;
  temp = stack_pointer[task_id];


  asm ("mov.w  temp, r1 \n\t");
  _BIS_SR(GIE);  
  // lets pop our first task out!

  RESTORE_CONTEXT();
}



#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
  //0 -Save Context 
  SAVE_CONTEXT();

  //1-Load Stack Pointer
  asm ("mov.w r1, temp \n\t");
  stack_pointer[task_id] = temp;
  
  //2-update the task id
  if(task_id < (TOTAL_TASKS-1)){ task_id++; }
  else{ task_id = 0; }

  //3-Save Stack pointer
  temp = stack_pointer[task_id];
  asm (  "mov.w  temp, r1    \n\t");

  //4 Load Context 
  RESTORE_CONTEXT();
}