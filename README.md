# DMA Sequencing with two slaved ADCs demo

# ADC DMA sequencing

This application shows how to use DMA sequencing feature available in ADC peripheral library.  I ran it on the Microchip SAM E54 Xplained Pro board with a Rev A ATSAME54P20A microcontroller.  It's copied from and adapted from the Microchip reference code, which can be found [here](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sam_d5x_e5x).  It also doesn't work correctly, and I'm posting this code to figure out why..

## Description

DMA sequencing enables you to reconfigure the ADC using DMA. The example application uses the DMA sequencing feature to write to MUXPOS bit field in the INPUTCTRL register to select input pin before an ADC conversion.  Both ADCs on the board are configured this way.

Four DMA channels are used in this application. One channel to copy the results from ADC result register to an array in SRAM, another DMA channel to copy new MUXPOS value to the INPUTCTRL register. ADC DMA sequencing trigger is used as trigger source for the second DMA channel which updates INPUTCTRL. Two channels for ADC0, and two for ADC1.  TC0 acts as a trigger to start ADC conversion.

ADC data is printed on the USART console via EDBG Virtual COM Port in hexadecimal format.

## Setting up the hardware

The following table shows the target hardware for the application projects.

| Project Name| Board|
|:---------|:---------:|
| sam_e54_xpro.X    | [SAM E54 Xplained Pro board](https://www.microchip.com/developmenttools/ProductDetails/atsame54-xpro)|
|||

### Setting up  [SAM E54 Xplained Pro board](https://www.microchip.com/developmenttools/ProductDetails/atsame54-xpro)

- Connect the Debug USB port on the board to the computer using a micro USB cable

## Running the Application

1. Open the Terminal application (Ex.:Tera term) on the computer
2. Connect to the EDBG Virtual COM port and configure the serial settings as follows:
    - Baud : 115200
    - Data : 8 Bits
    - Parity : None
    - Stop : 1 Bit
    - Flow Control : None
3. Build and Program the application using its IDE
4. Observe output message in console 
