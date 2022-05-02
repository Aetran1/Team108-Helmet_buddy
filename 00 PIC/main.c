/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16LF1566
        Driver Version    :  2.00
*/

#include "mcc_generated_files/mcc.h"
#include <xc.h>
#include <stdio.h>
//#include "config.h"
//#include "UART.h"
#include "MPU6050.h"

uint8_t L; 
uint8_t R; 
uint8_t turn = 3;
uint16_t convertedValue;

/*
                         Main application
 */

uint8_t rxData;
//uint8_t txData = 'c';



void on(void)
{
    oe_SetLow();
    srclr_SetHigh(); //when SRCLR is high register is not cleared
    rclk_SetLow(); //when RCLK is low data is not stored in storage register
    SPI2_WriteByte(0b11111111);// writes data to shift register
    __delay_ms(10); // allows data to be written
    rclk_SetHigh(); // rising edge, data is stored in the storage register
    __delay_ms(500);
    srclr_SetLow();// when SRCLR low, shift register cleared for next task
}

//void off(void)
//{
//    oe_SetHigh();
//}

void blink(void)
{
    oe_SetLow();
    srclr_SetHigh(); //when SRCLR is high register is not cleared
    rclk_SetLow(); //when RCLK is low data is not stored in storage register
    SPI2_WriteByte(0b11111111);// writes data to shift register
    __delay_ms(1); // allows data to be written
    rclk_SetHigh(); // rising edge, data is stored in the storage register
    __delay_ms(500);
    srclr_SetLow();// when SRCLR low, shift register cleared for next task
    oe_SetHigh();
    __delay_ms(100);
}

void left(void)
{
    oe_SetLow();
    L = 0b00000001; 
    for(uint8_t j = turn; j > 0; j--)
    {
        for(uint8_t k = 8; k > 0; k--)
        {   
            srclr_SetHigh(); //when SRCLR is high register is not cleared
            rclk_SetLow(); //when RCLK is low data is not stored in storage register
            SPI2_WriteByte(L);// writes data to shift register
            __delay_ms(1); // allows data to be written
            rclk_SetHigh(); // rising edge, data is stored in the storage register
            __delay_ms(100);
            srclr_SetLow();// when SRCLR low, shift register cleared for next task

            if(L == 0b10000000) //restarts left moving pattern 
            {
                L = 0b00000001;
            }
            else
            {
            L = L << 1; // turn the light on one space over to the left
            }
        }
    }
}

void right(void)
{
    oe_SetLow();
    R = 0b10000000; 
    for(uint8_t i = turn; i > 0; i--)
    {
        for(uint8_t h = 8; h > 0; h--)
        {
        
            srclr_SetHigh(); //when SRCLR is high register is not cleared
            rclk_SetLow(); //when RCLK is low data is not stored in storage register
            SPI2_WriteByte(R);// writes data to shift register
            __delay_ms(1); // allows data to be written
            rclk_SetHigh(); // rising edge, data is stored in the storage register
            __delay_ms(100);
            srclr_SetLow();// when SRCLR low, shift register cleared for next task

            if(R == 0b00000001) //restarts right moving pattern 
            {
                R = 0b10000000;
            }
            else
            {
            R = R >> 1; // turn the light on one space over to the right
            }
        }
    }
}

void EUSART_RX_isr(void)
{
    EUSART_Receive_ISR(); 
    
    if(EUSART_is_rx_ready())
    { 
        rxData = EUSART_Read();
        if (rxData == 'f'){
            while(1){
                blink();
            }
        }
    }
}

void main(void)
{
    __delay_ms(150);
    // Initialize the device
    SYSTEM_Initialize();
    __delay_ms(150);
    //printf("\r\nSystem Initialized\r\n");
    
    MPU6050_Init();
    
    EUSART_SetRxInterruptHandler(EUSART_RX_isr);
    //printf("MPU6050 Initialized\r\n");
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    // declares analog channel
    typedef enum
    {
        channel_FVR_adc1 =  0x3f,
        channel_VREFH_adc1 =  0x3b,
        channel_Temp_adc1 =  0x3d,
        AN17_adc1 =  0x11
    } adc1_channel_t;
    
    uint16_t convertedValue;
    ADC_Initialize();
    
    SPI2_Open(SPI2_DEFAULT); // sets default configuration
    
    char event;
    
    EUSART_Write('K');
    EUSART_Write('\r');
    EUSART_Write('\n');
    EUSART_Write('\r');
    EUSART_Write('\n');
    
    while (1)
    {
        //Front LED
        ADC1_StartConversion(AN17_adc1);
        convertedValue = ADC1_GetConversionResult();
        //convertedValue = convertedValue>>2;
        //adcValue = (uint8_t)convertedValue;


        if (convertedValue>25){
            FtLed_SetLow();        
        }
        else
        {
            FtLed_SetHigh();
        }
        
        
	    event = MPU6050_Read();
        
        if(!ExBtn_GetValue()){
            event = 'c';
        }

        //printf("Event %c\r\n", event);
        switch(event){
//            case 'b': {
//                on();
//            }
//            break;
            case 'c': {
                EUSART_Write('C'); //display character "C" in MQTT and Thonny
                EUSART_Write('\r');
                blink();
                blink();
                blink();
                event = 'x';
            }
            break;
            case 'l': {
                //EUSART_Write('L'); //display character "L" in MQTT and Thonny
                //EUSART_Write('\r');
                left();
                on();

            }
            break;
            case 'r': {
                //EUSART_Write('R'); //display character "R" in MQTT and Thonny
                //EUSART_Write('\r');
                right();
                on();
            }
            break;
            case 'x': {
                //on();
            }
            break;
        }
        
        __delay_ms(100);
    }
}
/**
 End of File
*/