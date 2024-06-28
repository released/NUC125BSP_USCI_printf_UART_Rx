/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "misc_config.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
#define PLL_CLOCK       			FREQ_50MHZ

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_UART_RX_RCV_TIMEOUT         			(flag_PROJ_CTL.bit1)
#define FLAG_PROJ_UART_RX_RCV_READY       				(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                              (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;


enum{
	fmt_head ,      // 0
    
 	fmt_data_1 ,    // 1
 	fmt_data_2 ,    // 2
 	fmt_data_3 ,    // 3
 	fmt_data_4 ,    // 4
 	fmt_data_5 ,    // 5
 	fmt_data_6 ,    // 6
 	fmt_data_7 ,    // 7

 	fmt_tail ,      // 8
}cmd_fmt_index;

enum{
	error_head , 
	// error_wrong_code ,

	// error_wrong_data0 ,   
    error_wrong_data1 ,  
    error_wrong_data2 ,  
    error_wrong_data3 ,   
    error_wrong_data4 ,  
    error_wrong_data5 ,  
    error_wrong_data6 ,  
    error_wrong_data7 ,

	// error_cs ,  
	error_tail , 

	error_timeout,  
	error_not_define ,       

}err_fmt_index;


#define _T(x)                                           x

#define UART_RX_RCV_LEN                                 (9)

#define UART_CMD_FMT_HEAD                               (0xA5)
#define UART_CMD_FMT_TAIL                               (0x7D)

unsigned char RXBUFFER[UART_RX_RCV_LEN] = {0};
unsigned char uart_rcv_cnt = 0;
volatile uint32_t u32rcvtick = 0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

// void delay_ms(uint16_t ms)
// {
// 	TIMER_Delay(TIMER0, 1000*ms);
// }


void stop_timer0(void)
{
    TIMER_ClearIntFlag(TIMER0);    
    TIMER_DisableInt(TIMER0); 
    TIMER_Stop(TIMER0);        
}

void start_timer0(void)
{
    TIMER_EnableInt(TIMER0);
    TIMER_Start(TIMER0);
}

void put_rc(uint8_t rc)
{
    #if 1
    const char *p =
            _T("error_head\0")
            _T("error_wrong_data1\0")
            _T("error_wrong_data2\0")
            _T("error_wrong_data3\0")
            _T("error_wrong_data4\0")
            _T("error_wrong_data5\0")
            _T("error_wrong_data6\0")
            _T("error_wrong_data7\0")
            _T("error_tail\0")
            _T("error_timeout\0");
    #else
    const char *p =
            _T("error_head\0")
            _T("error_wrong_code\0")
            _T("error_wrong_data0\0")
            _T("error_wrong_data1\0")
            _T("error_wrong_data2\0")
            _T("error_wrong_data3\0")
            _T("error_wrong_data4\0")
            _T("error_wrong_data5\0")
            _T("error_wrong_data6\0")
            _T("error_wrong_data7\0")
            _T("error_cs\0")
            _T("error_tail\0")
            _T("error_timeout\0")
            _T("error_not_define\0");
    #endif

    uint32_t i;
    for (i = 0; (i != (unsigned int)rc) && *p; i++)
    {
        while(*p++) ;
    }
    printf(_T("rc=%u result:%s\r\n"), (unsigned int)rc, p);
}

void fmt_err(uint8_t idx)
{
    uint8_t i = 0;

    printf("cmd parsing error(0x%2X)\r\n",idx);
    put_rc(idx);
    for(i = 0 ; i <= (UART_RX_RCV_LEN-1) ; i++)
    {
        printf("(%d):0x%2X",i , RXBUFFER[i]);
        if (i == idx)
        {
            printf("   ***");
        }
        printf("\r\n");
        // if ((i+1)%8 ==0)
        // {
        //     printf("\r\n");
        // }            
    }
    printf("\r\n\r\n");

    uart_rcv_cnt = 0;
    FLAG_PROJ_UART_RX_RCV_TIMEOUT = 0; 
    FLAG_PROJ_UART_RX_RCV_READY = 0; 
    stop_timer0();

    for(i = 0 ; i <= (UART_RX_RCV_LEN-1) ; i++)
    {
        RXBUFFER[i] = 0x00;
    }
}

void fmt_parsing(void)
{
    // uint8_t cal = 0;

    if (FLAG_PROJ_UART_RX_RCV_READY)                        //Uart receives completion and starts parsing.
    {
        if (RXBUFFER[fmt_head] != UART_CMD_FMT_HEAD )       //Input format parsing. check header
        {
            fmt_err(error_head);
            return;
        }
        if (RXBUFFER[fmt_tail] != UART_CMD_FMT_TAIL)        //Input format parsing. check tail
        {
            fmt_err(error_tail);
            return;
        }
        if (RXBUFFER[fmt_data_2] > 0x09 )
        {
            fmt_err(error_wrong_data2);
            return;
        }
        if (RXBUFFER[fmt_data_3] > 0x09 )
        {
            fmt_err(error_wrong_data3);
            return;
        }
        if (RXBUFFER[fmt_data_4] > 0x09 )
        {
            fmt_err(error_wrong_data4);
            return;
        }
        if (RXBUFFER[fmt_data_5] > 0x09 )
        {
            fmt_err(error_wrong_data5);
            return;
        }
        if (RXBUFFER[fmt_data_6] > 0x09 )
        {
            fmt_err(error_wrong_data6);
            return;
        }
        if (RXBUFFER[fmt_data_7] > 0x09 )
        {
            fmt_err(error_wrong_data7);
            return;
        }
        /*finally parse correct data*/
        FLAG_PROJ_UART_RX_RCV_TIMEOUT = 0;

        printf("\r\n\r\nprocess OK !\r\n");

        /*
            [MSB..LSB] 00RG BRGB
            RXBUFFER[fmt_data_1]

            Bit0: LED_Blue1   --1= LED On / 0= LED Off
            Bit1: LED_Green1 --1= LED On / 0= LED Off
            Bit2: LED_Red1    --1= LED On / 0= LED Off
            Bit3: LED_Blue2   --1= LED On / 0= LED Off
            Bit4: LED_Green2 --1= LED On / 0= LED Off
            Bit5: LED_Red2    --1= LED On / 0= LED Off
            Bit6..Bit7 : Reserved.

            LED_Red2    0x00 to 0x09
            LED_Green2  0x00 to 0x09
            LED_Blue2   0x00 to 0x09
            LED_Red1    0x00 to 0x09
            LED_Green1  0x00 to 0x09
            LED_Blue1   0x00 to 0x09
        */                   

        dump_buffer(RXBUFFER , UART_RX_RCV_LEN);   


        printf("Bit0: LED_Blue1 - ");
        (RXBUFFER[fmt_data_1] & BIT0) ? printf("ON\r\n") : printf("***\r\n") ;
        printf("Bit1: LED_Green1 - ");
        (RXBUFFER[fmt_data_1] & BIT1) ? printf("ON\r\n") : printf("***\r\n") ;
        printf("Bit2: LED_Red1 - ");
        (RXBUFFER[fmt_data_1] & BIT2) ? printf("ON\r\n") : printf("***\r\n") ;

        printf("Bit3: LED_Blue2 - ");
        (RXBUFFER[fmt_data_1] & BIT3) ? printf("ON\r\n") : printf("***\r\n") ;
        printf("Bit4: LED_Green2 - ");
        (RXBUFFER[fmt_data_1] & BIT4) ? printf("ON\r\n") : printf("***\r\n") ;
        printf("Bit5: LED_Red2 - ");
        (RXBUFFER[fmt_data_1] & BIT5) ? printf("ON\r\n") : printf("***\r\n") ;

        printf("\r\n");
        
        printf("LED_Red2:0x%02X\r\n",RXBUFFER[fmt_data_2]);
        printf("LED_Green2:0x%02X\r\n",RXBUFFER[fmt_data_3]);
        printf("LED_Blue2:0x%02X\r\n",RXBUFFER[fmt_data_4]);

        printf("LED_Red1:0x%02X\r\n",RXBUFFER[fmt_data_5]);
        printf("LED_Green1:0x%02X\r\n",RXBUFFER[fmt_data_6]);
        printf("LED_Blue1:0x%02X\r\n",RXBUFFER[fmt_data_7]);
        FLAG_PROJ_UART_RX_RCV_READY = 0; 
    }
}

/*
    xA5\x09\x09\x09\x09\x09\x09\x09\x7D
    xA5\x11\x09\x09\x09\x09\x09\x09\x7D


*/

void rx_rcv_irq(uint8_t c)
{    
    if (c == 0x0D /*|| c == UART_CMD_FMT_TAIL*/)                        //hit enter or tail judged that the input is completed
    {
        u32rcvtick = 0;
        FLAG_PROJ_UART_RX_RCV_TIMEOUT = 1;
        stop_timer0();
    }

    if (uart_rcv_cnt < (UART_RX_RCV_LEN-1))
    {
        RXBUFFER[uart_rcv_cnt] = c;
        uart_rcv_cnt++;

        if (uart_rcv_cnt == 1)
        {
            start_timer0();
        }

    }
    else if (uart_rcv_cnt ==  (UART_RX_RCV_LEN-1))
    {
        RXBUFFER[uart_rcv_cnt] = c;
        FLAG_PROJ_UART_RX_RCV_READY = 1; 
        uart_rcv_cnt = 0;
        stop_timer0();
    }    
}

void TMR0_IRQHandler(void)
{	
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        u32rcvtick++;
        
		if (u32rcvtick == 1000)
		{
            u32rcvtick = 0;
            FLAG_PROJ_UART_RX_RCV_TIMEOUT = 1;
            stop_timer0();
		}   

		FLAG_PROJ_UART_RX_RCV_READY = 0; 
    }
}

void TIMER0_Init(void)
{
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 10);

    NVIC_EnableIRQ(TMR0_IRQn);	
}

void TMR3_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        TIMER_ClearIntFlag(TIMER3);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
            FLAG_PROJ_TIMER_PERIOD_1000MS = 1;//set_flag(flag_timer_period_1000ms ,ENABLE);
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER3_Init(void)
{
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);	
    TIMER_Start(TIMER3);
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)//(is_flag_set(flag_timer_period_1000ms))
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;//set_flag(flag_timer_period_1000ms ,DISABLE);

        // printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
        PB4 ^= 1;        
    }


    if (FLAG_PROJ_UART_RX_RCV_TIMEOUT)
    {
        FLAG_PROJ_UART_RX_RCV_TIMEOUT = 0;
        fmt_err(error_timeout);     
    }

    fmt_parsing();

}



void UARTx_Process(void)
{
	uint8_t res = 0;
	res = UART_READ(UART0);

    rx_rcv_irq(res);
}

void UART0_IRQHandler(void)
{
    if(UART_GET_INT_FLAG(UART0, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
			UARTx_Process();
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
	UART_SetTimeoutCnt(UART0, 20);

	UART0->FIFO &= ~UART_FIFO_RFITL_4BYTES;
	UART0->FIFO |= UART_FIFO_RFITL_8BYTES;
	
	UART_ENABLE_INT(UART0, UART_INTEN_RDAIEN_Msk | UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk);
	
    NVIC_EnableIRQ(UART0_IRQn);

}


void UUARTx_Process(void)
{
	uint8_t res = 0;
	res = UUART_READ(UUART0);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		printf("press : %c\r\n" , res);
		switch(res)
		{
			case '1':
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
				break;
		}
	}
}

void USCI_IRQHandler(void)
{
    if (UUART_GET_PROT_STATUS(UUART0) & UUART_PROTSTS_RXENDIF_Msk)
    {        
        UUART_CLR_PROT_INT_FLAG(UUART0, UUART_PROTSTS_RXENDIF_Msk);

        while(UUART_GET_RX_EMPTY(UUART0) == 0)
        {
			UUARTx_Process();
        }

    }
}

void USCI_Init(void)
{
    SYS_ResetModule(USCI0_RST);

    UUART_Open(UUART0, 115200);

    UUART_ENABLE_TRANS_INT(UUART0, UUART_INTEN_RXENDIEN_Msk);
    NVIC_EnableIRQ(USCI_IRQn);

	#if (_debug_log_UART_ == 1)	//debug	
	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
	#endif	
}

void GPIO_Init(void)
{
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB4MFP_Msk)) | (SYS_GPB_MFPL_PB4MFP_GPIO);	
    GPIO_SetMode(PB, BIT4, GPIO_MODE_OUTPUT);

}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);	

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /*------------------------------------------------------------------------------------------------------*/
    /* Enable Module Clock                                                                                  */
    /*------------------------------------------------------------------------------------------------------*/
    CLK_EnableModuleClock(TMR0_MODULE);
  	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0);

    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_PCLK1, 0);

    /***********************************/	
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_HIRC_DIV2, CLK_CLKDIV0_UART(1));

    /* Set PB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB0MFP_Msk)) | SYS_GPB_MFPL_PB0MFP_UART0_RXD;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_UART0_TXD;
    // SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD2MFP_Msk)) | SYS_GPD_MFPL_PD2MFP_UART0_TXD;
    // SYS->GPD_MFPL = (SYS->GPD_MFPL & (~SYS_GPD_MFPL_PD1MFP_Msk)) | SYS_GPD_MFPL_PD1MFP_UART0_RXD;

    /***********************************/	
    CLK_EnableModuleClock(USCI0_MODULE);

    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB6MFP_Msk)) | SYS_GPB_MFPL_PB6MFP_USCI0_DAT0;  // RX
    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB7MFP_Msk)) | SYS_GPB_MFPL_PB7MFP_USCI0_DAT1;  // TX

    /***********************************/
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();


    /* Lock protected registers */
    SYS_LockReg();
	
} // SYS_Init()


int main()
{
    SYS_Init();

	GPIO_Init();
    
    /*for printf ,PB7 : USCIx_DAT1/TX , PB6 : USCIx_DAT0/RX*/
	USCI_Init();

    /* PD2 : UART0_TX , PD1 : UART0_RX*/
	UART0_Init();

	TIMER0_Init();
	TIMER3_Init();
    // SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif


    /* Got no where to go, just loop forever */
    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
