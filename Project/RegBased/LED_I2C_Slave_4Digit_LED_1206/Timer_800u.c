#include "MS51_16K.h"

unsigned long data TIMER0CT,TIMER1CT,TIMER2CT,TIMER3CT;
unsigned char data TH0TMP,TL0TMP,TH1TMP,TL1TMP;
extern unsigned int  count;
extern bit toggle ;
/**
  * @brief Timer0 delay interrupt initial setting and timer 0 interrupt vector 
  * @param[in] u32SYSCLK define Fsys clock value. for example 8, use the real Fsys value.
  *                       - \ref 24 (use HIRC)
  *                       - \ref 8  (use MIRC)
  * @param[in] u6DLYUnit define delay time base is us. the minimum delay value not less than 100us, the maxima value base on u32SYSCLK
  *                       - \ref for 24MHz (use HIRC) value range is 100 ~ 32768 us.
  *                       - \ref for 8MHz  (use MIRC) value range is 100 ~ 65536 us.
  * @return  None 
  * @note    This initial subroutine must with following Timer 0 interrupt vector.
  * @exmaple :Timer0_AutoReload_Interrupt_Initial(8,450);
*/
void Timer0_ISR (void) interrupt 1        // Vector @  0x0B
{
_push_(SFRS);
      SFRS = 0;
/* following setting for reload Timer 0 counter */
      TH0 = TH0TMP;
      TL0 = TL0TMP;
/* following clear flag for next interrupt */
      clr_TCON_TF0;
      count++;
			toggle = 1;
_pop_(SFRS);
}
void Timer0_AutoReload_Interrupt_Initial(unsigned char u8SYSCLK, unsigned long u32DLYUnit)
{
    TIMER0_FSYS_DIV12;                                          /* T0M=0, Timer0 Clock = Fsys/12   */
    ENABLE_TIMER0_MODE1;                                        /* Timer0 as 16-bit mode           */
    TIMER0CT = 65535ul-(u8SYSCLK*u32DLYUnit/12ul);
    TH0TMP = HIBYTE(TIMER0CT);
    TL0TMP = LOBYTE(TIMER0CT);
    TH0 = TH0TMP;
    TL0 = TL0TMP;
    clr_TCON_TF0;
    set_TCON_TR0;                                    //Start Timer0
    ENABLE_TIMER0_INTERRUPT;
}