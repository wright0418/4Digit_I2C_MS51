/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2021 nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/


//***********************************************************************************************************
//  File Function: MUG51 I2C Slave demo code
//***********************************************************************************************************

#include "MS51_16K.h"
#include "timer_800u.h"

#define I2C_SLAVE_ADDRESS         0x7A //0x3D for 7bit
#define LOOP_SIZE                 5 


//unsigned char data_received[12], data_num = 0 ;
unsigned char xdata data_received[15], data_num = 0 ;      /*15 data with command code */
unsigned char xdata LED_DATA[12];
unsigned int  count=0;
unsigned char DD_count=0;

bit toggle=0 ;
bit DotShowEn=0;

unsigned char number_table[11] = {0x7F,0x0E,0xB7,0x9F,0xCE,0xDD,0xFD,0x0F,0xFF,0xDF,0x04}; //0,1,2,3,4,5,6,7,8,9

bit I2CWOVERFLAG = 0;

//========================================================================================================
void I2C0_ISR(void) interrupt 6
{
  _push_(SFRS);
  SFRS = 0;
  switch (I2STAT)
    {
        case 0x00:
            set_I2CON_STO;
        break;

        case 0x80:
            data_received[data_num] = I2DAT;
            data_num++;
            if (data_num>LOOP_SIZE) 
            {
                clr_I2CON_AA;
            }
            else
                set_I2CON_AA;
        break;

        case 0xA0:
            I2CWOVERFLAG = 1 ;
            data_num =0;
            set_I2CON_AA;
        break;

    }

    I2C0_SI_Check();
_pop_(SFRS);
}

//========================================================================================================
void Init_I2C_Slave_Interrupt(void)
{
    P13_OPENDRAIN_MODE;          /* External pull high resister in circuit */
    P14_OPENDRAIN_MODE;          /* External pull high resister in circuit */
    P13_QUASI_MODE;          /* External pull high resister in circuit */
    P14_QUASI_MODE;          /* External pull high resister in circuit */
    set_P1S_3;                   /* Setting schmit tigger mode */
    set_P1S_4;                   /* Setting schmit tigger mode */
  
    SDA = 1;                     /* set SDA and SCL pins high */
    SCL = 1;
  
    I2C_Slave_Open(I2C_SLAVE_ADDRESS);
    I2C_Interrupt(ENABLE);
}

void Init_ICE_I2C_Slave_Interrupt(void)
{
    P16_OPENDRAIN_MODE;          /* External pull high resister in circuit */
    P02_OPENDRAIN_MODE;          /* External pull high resister in circuit */

    set_P1S_6;                   /* Setting schmit tigger mode */
    set_P0S_2;                   /* Setting schmit tigger mode */
  
    P16 = 1;                     /* set SDA and SCL pins high */
    P02 = 1;
  
    I2C_Slave_Open(I2C_SLAVE_ADDRESS);
    set_I2CON_I2CPX;
    I2C_Interrupt(ENABLE);
}


void ScanShow4Digit()
{
		unsigned Digit ;
		Digit = count % 5; // 300us *25 = 20ms 
		if (Digit == 0)  // enable Digit1
		{
				P0 = number_table[LED_DATA[0]];
				P17 = 0 ; //Point
			
				P11=P13=P14=P12 =0;
				P10 = 1 ; //Scan Digit1
		}
		else if (Digit ==1) // enable Digit2
		{
				P0 = number_table[LED_DATA[1]];
				P10=P13=P14=P12 =0;	
				P17 = 0 ; //Point
				
				P11 = 1 ; //Scan Digit2
		}
		else if (Digit ==2) // enable Digit3
		{
				P0 = number_table[LED_DATA[2]];
				P17 =0;
				P10=P11=P14=P12 =0;	
				P13 = 1 ; //Scan Digit3
		}
		else if (Digit ==3)  // enable Digit4
		{
				P0 = number_table[LED_DATA[3]];
				P17 =0;
				P10=P11=P13=P12 =0;
				P14 = 1 ; //Scan Digit4
		}
		else if (Digit ==4)  // enable :
		{
				P17 =0;
				DD_count++;
			
				if (DotShowEn == 1)
				{
					if (DD_count > 50) // 20ms *50 =1sec
					{
						P15 = ~P15; // :
						DD_count =0;
					}
				}
				else  P15 =0 ;
				
				P10=P11=P13=P14 =0;
				P12 = 1 ; //Scan :
		}
}


//========================================================================================================
void main(void)
{
    unsigned ct_tp=0, temp=0 , TimerCount=0; 
		unsigned char TempBuf[4];
		bit SatrtState = 1;
		
		

    //P12_QUASI_MODE;
		//P12_PUSHPULL_MODE;
		ALL_GPIO_PUSHPULL_MODE;
		
	
    Timer0_AutoReload_Interrupt_Initial(16,800); //3000us = 3 ms , 3ms

    /* Initial I2C function */
    //Init_I2C_Slave_Interrupt();                                 //initial I2C circuit
		 Init_ICE_I2C_Slave_Interrupt();                           //For ICE pin I2C initial
		 EA =1;
	
		LED_DATA[0] = 8 ;
		LED_DATA[1] = 8 ;
		LED_DATA[2] = 8 ;
		LED_DATA[3] = 8 ;

/* =================== */


    while (1)
    {
        if (I2CWOVERFLAG )
        {
           /* After receive storage in dataflash */
           if (data_received[0]==1) //CMD = 0x01
           {
              for (ct_tp=0;ct_tp<4;ct_tp++)
              {
                LED_DATA[ct_tp] = data_received[ct_tp+1];
              }
              if (data_received[5]&SET_BIT7) //0x80 display on
              {
                /* Display */
								
							
              }
              else
              {
                 /* close */
								LED_DATA[0]=10;LED_DATA[1]=10;LED_DATA[2]=10;LED_DATA[3]=10;
								
              }
              if (data_received[5]&SET_BIT2)
              {
                /* Flash Display */
              }
              else
              {
                 /*  Flash Display close */
              }
              switch (data_received[4]&0x0F)
              {
                case 0:  break;
                case 1:  break;
                case 2:  break;
                case 3:  break;
              }
            }

           if (data_received[0]==2) //CMD =0x02
           {
						 for (ct_tp=0;ct_tp<2;ct_tp++)
              {
                TempBuf[ct_tp] = data_received[ct_tp+1];		
              }
							if (TempBuf[0] <= 99)
							{
								LED_DATA[0] = (int)TempBuf[0] /10;
								LED_DATA[1] = TempBuf[0] %10;
							}
							else {LED_DATA[0] = 10;	LED_DATA[1] = 10;}
							if (TempBuf[1] <= 99)
							{
								LED_DATA[2] = (int) TempBuf[1]/10;
								LED_DATA[3] = TempBuf[1]%10;
							}
							else {LED_DATA[2] = 10;	LED_DATA[3] = 10;	}
															
             if (data_received[7]==0)
             {
                /* ON */
             }
             else 
             {
                /* OFF */
             }
             /* data_received[8]  lighting status */ 
           }


           if (data_received[0]==3) //CMD =0x03
           {
						  TempBuf[0] = data_received[0+1];		
       
							if (TempBuf[0] <= 9999)
							{
								LED_DATA[0] = (int)TempBuf[0] /1000;
								LED_DATA[1] = (int) (TempBuf[0]%1000 )/100;
								LED_DATA[2] = (int) (TempBuf[0]%100 )/10;
								LED_DATA[3] = (int) TempBuf[0]%10 ;
							}
							else {LED_DATA[0] = 10;	LED_DATA[1] = 10;}
							
						}

				}
				
				if (toggle == 1)
				{
					if (SatrtState == 1) // show 8888 init test 
					{ 
						temp ++;
						if (temp >= 300)
						{
							LED_DATA[0] =0;LED_DATA[1] =0;LED_DATA[2] =0;LED_DATA[3] =0;
							SatrtState =0;
							temp =0 ;
						}	
						
					}	
					ScanShow4Digit();
					toggle = 0;
				}
     }

}

