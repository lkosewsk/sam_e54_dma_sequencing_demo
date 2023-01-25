/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#define ADC_VREF                (3.3f)

uint32_t  adc_seq_regs[3] = {0x1818, 0x1819, 0x181A,};
volatile uint16_t     adc0_res[3]      = {0};
volatile uint16_t     adc1_res[3]      = {0};
volatile bool         adc_dma_done[2]    = {0};
float input_voltage;

void adc_sram_dma_callback(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
	if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        adc_dma_done[contextHandle] = true;
    }
}

bool ever_got_here = false;
void dma2_seq_cb(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        ever_got_here = true;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

static void reset_adc0_dma(void)
{
    DMAC_ChannelTransfer(DMAC_CHANNEL_1,(const void *)&ADC0_REGS->ADC_RESULT,(const void *)adc0_res,sizeof(adc0_res));
    DMAC_ChannelTransfer(DMAC_CHANNEL_0,(const void *)adc_seq_regs,(const void *)&ADC0_REGS->ADC_DSEQDATA,sizeof(adc_seq_regs));
}

static void reset_adc1_dma(void)
{
    DMAC_ChannelTransfer(DMAC_CHANNEL_3,(const void *)&ADC1_REGS->ADC_RESULT,(const void *)adc1_res,sizeof(adc1_res));
    DMAC_ChannelTransfer(DMAC_CHANNEL_2,(const void *)adc_seq_regs,(const void *)&ADC1_REGS->ADC_DSEQDATA,sizeof(adc_seq_regs));
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    printf("\r\n Configuring DMA............. \r\n");
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_1, adc_sram_dma_callback, 0);
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_3, adc_sram_dma_callback, 1);
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_2, dma2_seq_cb, 0);
    
    reset_adc0_dma();
    reset_adc1_dma();
    
    ADC0_Enable();
    // Don't enable ADC1, it's in slave mode
    TC0_TimerStart();

    while ( true )
    {
        if (adc_dma_done[0]) {
			adc_dma_done[0] = false;
			printf("\r\n ADC conversion of 3 inputs done \r\n");
            input_voltage = (float)adc0_res[0] * ADC_VREF / 4095U;
			printf("\r\n 1/4 VCORE: ADC Count: 0x%03x, ADC Input Voltage = %d.%02d V \r\n\r\n", 
                    adc0_res[0], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
			input_voltage = (float)adc0_res[1] * ADC_VREF / 4095U;
            printf("\r\n 1/4 VBAT: ADC Count: 0x%03x, ADC Input Voltage = %d.%02d V \r\n\r\n", 
                    adc0_res[1], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
            input_voltage = (float)adc0_res[2] * ADC_VREF / 4095U;
            printf("\r\n 1/4 VIO: ADC Count: 0x%03x, ADC Input Voltage = %d.%02d V \r\n\r\n", 
                    adc0_res[2], (int)input_voltage, (int)((input_voltage - (int)input_voltage)*100.0));
            
            reset_adc0_dma();
		}
        if (adc_dma_done[1])
        {
            printf("These values should be similar but are not :(: { 0x%03x, 0x%03x, 0x%03x }\r\n",
                    adc1_res[0], adc1_res[1], adc1_res[2]);
            printf("Also, did ADC1_SEQ ever complete? %d\r\n", ever_got_here);
            
            reset_adc1_dma();
        }
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

