#include "bsp.h"

/// SPL
#include "MDR32F9Qx_port.h"

/// IPS
#include "tcpip_api.h"

/// board includes
#include "clock.h"
#include "ports.h"

/// application includes
#include "utils.h"



/*
 *  Initialize MCU clock, internal peripheral and drivers, external peripheral and
 *  drivers, software drivers, TCP/IP stack, system utils.
 */
void prvSetupHardware (void *pvParameters)
{
    System_ClockInit();         /// use PLL to multiply 8 MHz to 5 (40 MHz)

    Utils_Init();               /// set system utils settings
    PortsInit();                /// turn on all the neccessary ports

    TCPIP_Init();               /// initialize TCP/IP stack
}


/*
 *  Start/enable initialized MCU peripheral
 */
void prvStartHardware (void *pvParams)
{
    //
}


/// Exception handlers


void HardFaultServiceRoutine (unsigned long *args)
{
    unsigned long stacked_r0;
    unsigned long stacked_r1;
    unsigned long stacked_r2;
    unsigned long stacked_r3;
    unsigned long stacked_r12;
    unsigned long stacked_lr;
    unsigned long stacked_pc;
    unsigned long stacked_psr;

    stacked_r0 = ((unsigned long) args[0]);
    stacked_r1 = ((unsigned long) args[1]);
    stacked_r2 = ((unsigned long) args[2]);
    stacked_r3 = ((unsigned long) args[3]);

    stacked_r12 = ((unsigned long) args[4]);
    stacked_lr = ((unsigned long) args[5]);
    stacked_pc = ((unsigned long) args[6]);
    stacked_psr = ((unsigned long) args[7]);

    (void) stacked_r0;
    (void) stacked_r1;
    (void) stacked_r2;
    (void) stacked_r3;
    (void) stacked_r12;
    (void) stacked_lr;
    (void) stacked_pc;
    (void) stacked_psr;

	/* Go to infinite loop when Hard Fault exception occurs */
    while (1)   { };
}

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
    //
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
    unsigned int contr_reg;
	contr_reg = __get_CONTROL();
	if(contr_reg&2)
	{
#if defined ( __CMCARM__ )
		__ASM MRS R0, PSP;
#else
		__ASM("MRS R0, PSP");
#endif
	}
	else{
#if defined ( __CMCARM__ )
		__ASM MRS R0, MSP;
#else
		__ASM("MRS R0, MSP");
#endif
	}
	//top of stack is in R0. It is passed to C-function.
#if defined ( __CMCARM__ )
	__ASM BL (HardFaultServiceRoutine);
#else
	__ASM("BL (HardFaultServiceRoutine)");
#endif

	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)   { };
}
