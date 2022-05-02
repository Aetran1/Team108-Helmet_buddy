
#include <xc.h>
#include "I2C.h"
#include "MPU6050.h"
#include "mcc_generated_files/mcc.h"
//#include "UART.h"  // for debugging serial terminal
#include <stdio.h>
#include <stdlib.h>

float crash = 0.5;
float turnG = 235.0;
float turnA = 0.5;
float brake = 1.5;
float maxAx, maxAy, maxAz, maxGx, maxGy, maxGz, maxT;

//-------------[ MPU6050 Routines ]------------------
//---------------------------------------------------

void MPU6050_Init()
{
  // Power-Up Delay & I2C_Init
  __delay_ms(100);
  I2C_Master_Init();
 
  // Setting The Sample (Data) Rate
  I2C_Start(0xD0);
  I2C_Master_Write(SMPLRT_DIV);
  I2C_Master_Write(0x07);
  I2C_Master_Stop();
 
  // Setting The Clock Source
  I2C_Start(0xD0);
  I2C_Master_Write(PWR_MGMT_1);
  I2C_Master_Write(0x01);
  I2C_Master_Stop();
 
  // Configure The DLPF
  I2C_Start(0xD0);
  I2C_Master_Write(CONFIG);
  I2C_Master_Write(0x00);
  I2C_Master_Stop();
 
  // Configure The ACCEL (FSR= +-2g)
  I2C_Start(0xD0);
  I2C_Master_Write(ACCEL_CONFIG);
  I2C_Master_Write(0x00);
  I2C_Master_Stop();
 
  // Configure The GYRO (FSR= +-2000d/s)
  I2C_Start(0xD0);
  I2C_Master_Write(GYRO_CONFIG);
  I2C_Master_Write(0x18);
  I2C_Master_Stop();
 
  // Enable Data Ready Interrupts
  I2C_Start(0xD0);
  I2C_Master_Write(INT_ENABLE);
  I2C_Master_Write(0x01);
  I2C_Master_Stop();
}

char MPU6050_Read()
{
  //char buffer[40];
  //char event = 0; // Left=l, Right=r, Brake=b, Crash=c
  int Ax,Ay,Az,T,Gx,Gy,Gz;
  float AX, AY, AZ, t, GX, GY, GZ;
  // Prepare For Reading, Starting From ACCEL_XOUT_H
  I2C_Start(0xD0);
  I2C_Master_Write(ACCEL_XOUT_H);
  I2C_Master_Stop();
  I2C_Start(0xD1);
  I2C_Read(0); // dummy
  Ax = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Ay = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Az = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  T  = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gx = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gy = ((int)I2C_Read(0)<<8) | (int)I2C_Read(0);
  Gz = ((int)I2C_Read(0)<<8) | (int)I2C_Read(1);
  
//  Ax = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(0);
//  Ay = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(0);
//  Az = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(0);
//  T  = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(0);
//  Gx = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(0);
//  Gy = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(0);
//  Gz = ((uint16_t)I2C_Read(0)<<8);
//  (uint16_t)I2C_Read(1);
//  I2C_Master_Stop();
  
    // Convert The Readings
  AX = (float)Ax/16384.0;
  AY = (float)Ay/16384.0;
  AZ = (float)Az/16384.0;
  //int az = (int)AZ;
  GX = (float)Gx/131.0;
  GY = (float)Gy/131.0;
  GZ = (float)Gz/131.0;
  t = ((float)T/340.00)+36.53;
  
//  if (AX > maxAx){
//      maxAx = AX;
//  }
//    if (AY > maxAy){
//      maxAy = AY;
//  }
//    if (AZ > maxAz){
//      maxAz = AZ;
//  }
//    if (GX > maxGx){
//      maxGx = GX;
//  }
//  if (GY > maxGy){
//      maxGy = GY;
//  }
//  if (GZ > maxGz){
//      maxGz = GZ;
//  }
 
//  printf("Ax = %0.2f    ",AX);
//  printf(" Ay = %0.2f    ",AY); 
//  printf(" Az = %0.2f    ",AZ);
//  printf(" Gx = %0.2f    ",GX);
//  printf(" Gy = %0.2f    ",GY);
//  printf(" Gz = %0.2f      \r\n",GZ);
  
//  printf("Ax = %d    ",Ax);
//  printf(" Ay = %d   ",Ay); 
//  printf(" Az = %d    ",Az);
//  printf(" Gx = %d    ",Gx);
//  printf(" Gy = %d    ",Gy);
//  printf(" Gz = %d      \r\n",Gz);
//  EUSART_Write(az);
//  EUSART_Write('\r');
//  EUSART_Write('\n');

  
  //if ((absf(AX) >= crash && absf(AY) >= crash) || (absf(AX) >= crash && absf(AZ)-1 >= crash) || (absf(AZ)-1 >= crash && absf(AY) >= crash)){
//  if ((AZ <= crash) ){
//      return 'c';
//  }
//  else 
  if (GY>turnG && AY>turnA){
      return 'r';
  }
  else if (GY<turnG*-1 && AY<turnA*-1){
      return 'l';
  }
  else {
      return 'x';
  }
  
}

float absf(float input){
    float result = input;
    if (input<0){
        result = input * -1;
    }
    return result;
}