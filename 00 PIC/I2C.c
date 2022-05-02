
#include <xc.h>
#include "I2C.h"
#include "mcc_generated_files/mcc.h"
#define SEN SSP1CON2bits.SEN
#define RSEN SSP1CON2bits.RSEN
#define PEN SSP1CON2bits.PEN
#define ACKDT SSP1CON2bits.ACKDT
#define ACKEN SSP1CON2bits.ACKEN
#define ACKSTAT SSP1CON2bits.ACKSTAT
#define RCEN SSP1CON2bits.RCEN
#define BF SSP1STATbits.BF
#define SSPIF PIR1bits.SSP1IF


//---------------[ I2C Routines ]-------------------
//--------------------------------------------------

void I2C_Master_Init()
{
  SSP1CON1  = 0x28;
  SSP1CON2 = 0x00;
  SSP1STAT = 0x00;
  SSP1ADD = ((_XTAL_FREQ/4)/I2C_BaudRate) - 1;
  SCL_D = 1;
  SDA_D = 1; 
}

void I2C_Master_Wait()
{
    while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F));
}

void I2C_Master_Start()
{
    I2C_Master_Wait();
    SEN = 1;
}

void I2C_Start(char add)
{
    I2C_Master_Wait();
    //printf("Wait finished\r\n");
    SEN = 1;
    //printf("SEN set\r\n");
    I2C_Master_Write(add);
    
}

void I2C_Master_RepeatedStart()
{
    I2C_Master_Wait();
    RSEN = 1;
}

void I2C_Master_Stop()
{
    I2C_Master_Wait();
    PEN = 1;
}

void I2C_ACK(void)
{
	ACKDT = 0;			// 0 -> ACK
    ACKEN = 1;			// Send ACK
    while(ACKEN);
}

void I2C_NACK(void)
{
	ACKDT = 1;			// 1 -> NACK
	ACKEN = 1;			// Send NACK
    while(ACKEN);
}

unsigned char I2C_Master_Write(unsigned char data)
{
    //printf("Entered Write function\r\n");
    I2C_Master_Wait();
    //printf("Wait finished\r\n");
    SSP1BUF = data;
    //printf("Buffer set\r\n");
    while(!SSPIF);  // Wait Until Completion
	SSPIF = 0;
    return ACKSTAT;
}

unsigned char I2C_Read_Byte(void)
{
    //---[ Receive & Return A Byte ]---
    I2C_Master_Wait();
    RCEN = 1;		  // Enable & Start Reception
	while(!SSPIF);	  // Wait Until Completion
	SSPIF = 0;		  // Clear The Interrupt Flag Bit
    I2C_Master_Wait();
    return SSP1BUF;	  // Return The Received Byte
}

unsigned char I2C_Read(unsigned char ACK_NACK)
{  
    //---[ Receive & Return A Byte & Send ACK or NACK ]---
    unsigned char Data;
    RCEN = 1;              
    while(!BF);      
    Data = SSP1BUF;           
    if(ACK_NACK==0)
        I2C_ACK();            
    else
        I2C_NACK();     
    while(!SSPIF);                 
    SSPIF=0;   
    return Data;
}



