/*
 * File:   main.c
 * Author: 14036
 *
 * Created on January 1, 2025, 8:07 PM
 */

#include <xc.h>
#include <stdint.h>

#define BNO055_ADDRESS 0x28      // BNO055 I2C address
#define BNO055_CHIP_ID_REG 0x00  // CHIP_ID register

// Function to initialize the LED pin
void LED_Init(void) {
    TRISBbits.TRISB0 = 0;    // Set RB0 as output for LED
    LATBbits.LATB0 = 0;      // Initially turn off the LED
}

// Function to light up LED if chip_id is correct
void CheckAndLightLED(uint8_t chip_id) {
    if (chip_id == 0xA0) {
        LATBbits.LATB0 = 1;  // Turn on LED if chip ID matches
    }
}

// Your I2C read function for BNO055
uint8_t BNO055_ReadRegister(uint8_t reg) {
    uint8_t data;

    // Start condition
    I2C1CONLbits.SEN = 1;
    while (I2C1CONLbits.SEN);

    // Send device address with write bit
    I2C1TRN = (BNO055_ADDRESS << 1) | 0;
    while (I2C1STATbits.TBF);
    while (I2C1STATbits.ACKSTAT);

    // Send register address
    I2C1TRN = reg;
    while (I2C1STATbits.TBF);
    while (I2C1STATbits.ACKSTAT);

    // Restart condition
    I2C1CONLbits.RSEN = 1;
    while (I2C1CONLbits.RSEN);

    // Send device address with read bit
    I2C1TRN = (BNO055_ADDRESS << 1) | 1;
    while (I2C1STATbits.TBF);
    while (I2C1STATbits.ACKSTAT);

    // Read data
    I2C1CONLbits.RCEN = 1;
    while (!I2C1STATbits.RBF);
    data = I2C1RCV;

    // Send NACK
    I2C1CONLbits.ACKDT = 1;
    I2C1CONLbits.ACKEN = 1;
    while (I2C1CONLbits.ACKEN);

    // Stop condition
    I2C1CONLbits.PEN = 1;
    while (I2C1CONLbits.PEN);

    return data;
}

int main(void) {
    LED_Init();   // Initialize LED pin

    // Assuming I2C is already configured here

    uint8_t chip_id = BNO055_ReadRegister(BNO055_CHIP_ID_REG);

    // Check the chip_id and light up LED if valid
    CheckAndLightLED(chip_id);

    while (1) {
        // Main loop
    }

    return 0;
}

