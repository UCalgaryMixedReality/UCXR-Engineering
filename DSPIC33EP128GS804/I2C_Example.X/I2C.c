/*
 * File:   I2C.c
 * Author: Dominic Nutt
 *
 * Created on January 06, 2025, 9:55 AM
 */

#include <p33EP128GS804.h>

#include "I2C.h"

/*******************************************************************************
    FUNCTION: I2C_Init
    SUMMARY: Initializes I2C module.
    DESCRIPTION: Sets the baud rate generator to produce an SCL of 400KHz.
              Then enables module.
    REQUIRES: None
    ARGS: None
    RETURN: None
*******************************************************************************/
void I2C_Init(void)
{
    I2C1CONLbits.I2CEN = 0;     // 1 = Enables the I2Cx module and configures the SDAx and SCLx pins as serial port pins
                                // 0 = Disables the I2Cx module; all I2C pins are controlled by port functions       
    
    I2C1BRG = I2C_BRG;               // Set baud rate
    
    I2C1CONLbits.I2CEN = 1;
}

/*******************************************************************************
    FUNCTION: I2C_Start
    SUMMARY: Asserts Start condition on I2C bus
    DESCRIPTION: Asserts Start condition on I2C bus and waits for completion
    REQUIRES: None
    ARGS: None
    RETURN: None
*******************************************************************************/
void I2C_Start(void)
{
    I2C1CONLbits.SEN = 1;       // Start
    while(I2C1CONLbits.SEN);
}

/*******************************************************************************
    FUNCTION: I2C_Stop
    SUMMARY: Asserts Stop condition on I2C bus
    DESCRIPTION: Asserts Stop condition on I2C bus and waits for completion
    REQUIRES: None
    ARGS: None
    RETURN: None
*******************************************************************************/
void I2C_Stop(void)
{
    I2C1CONLbits.PEN = 1;       // Stop
    while(I2C1CONLbits.PEN);      
}

/*******************************************************************************
    FUNCTION: I2C_Restart
    SUMMARY: Asserts Stop condition on I2C bus
    DESCRIPTION: Asserts Stop condition on I2C bus and waits for completion
    REQUIRES: None
    ARGS: None
    RETURN: None
*******************************************************************************/
void I2C_Restart(void)
{
    I2C1CONLbits.RSEN = 1;      // Restart
    while(I2C1CONLbits.RSEN);
}

/*******************************************************************************
    FUNCTION: I2C_WriteByte
    SUMMARY: Transmits a byte
    DESCRIPTION: Loads transmit register with byte. Waits until reg is empty
                 and ACK has been received from slave.
    REQUIRES: None
    ARGS: uint8_t uc_byte
    RETURN: None
*******************************************************************************/
void I2C_WriteByte(uint8_t uc_byte)
{
    I2C1TRN = uc_byte;          // Load transmit register with byte
    while(I2C1STATbits.TRSTAT); // Wait until register has emptied and ack has been received
}

/*******************************************************************************
    FUNCTION: I2C_ReadByte
    SUMMARY: Reads byte from slave
    DESCRIPTION: Sets PIC to receive mode. Waits until receive register is full
                 and then stores byte in given parameter.
    REQUIRES: None
    ARGS: uint8_t* byte
    RETURN: None
*******************************************************************************/
void I2C_ReadByte(uint8_t* byte)
{
    I2C1CONLbits.RCEN = 1;      // Set PIC to receive mode
    while(!I2C1STATbits.RBF);   // Wait until whole byte has been received
    *byte = I2C1RCV;             // Store byte from receive register
    while(I2C1STATbits.RBF);    // Wait until empty
}