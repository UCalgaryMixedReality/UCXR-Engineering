/*
 * File:   I2C.c
 * Author: Dominic Nutt
 *
 * Created on January 06, 2025, 9:55 AM
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_H
#define	I2C_H

#define I2C_BRG 70      // Calculation from microchip I2C document (DS70000195G).
                        // I2CxBRG = (((1/Fscl) - delay)* FCY) - 2
                        // Fscl = 400KHz, delay is 120ns, FCY = 60.0064MHz
                        // (1/400000) - 0.000000120)* 60006400) - 2) = 140.815232;
                        // The BRG counter takes two cycles for one clock cycle (Tbrg = Tscl/2)
                        // So 140.815232/ 2 = 70.407616. Reg only takes whole numbers so round down to 70

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Restart(void);
void I2C_WriteByte(uint8_t uc_byte);
void I2C_ReadByte(uint8_t* byte);

#endif	/* I2C_H */

