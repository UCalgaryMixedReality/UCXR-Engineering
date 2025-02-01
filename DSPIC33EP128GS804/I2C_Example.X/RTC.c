/*
 * File:   I2C.c
 * Author: Dominic Nutt
 *
 * Created on December 19, 2024, 1:17 PM
 */

#include <p33EP128GS804.h>

#include "RTC.h"
#include "I2C.h"

/*******************************************************************************
    FUNCTION: RTC_SetDateTime
    SUMMARY: Sets the Date and Time of the RTC module
    DESCRIPTION: Takes a DateTime structure and writes it over I2C
                 to the clock page of the RTC module.
    REQUIRES: None
    ARGS: t_DateTime DateTime
    RETURN: None
*******************************************************************************/
void RTC_SetDateTime(t_DateTime DateTime)
{
    I2C_Start();                            // Assert Start on bus
    I2C_WriteByte(RTC_WRITE_ADDRESS);       // Send write address of RTC module
    I2C_WriteByte(RTC_CLOCK_PAGE_ADDRESS);  // Send address of clock page, This starts the write on the seconds register
    I2C_WriteByte(DateTime.uc_Seconds);     // Send Seconds
                                            // Slave automatic increments address by 1 after ACK
    I2C_WriteByte(DateTime.uc_Minutes);     // Send Minutes
    I2C_WriteByte(DateTime.uc_Hours);       // Send Hours
    I2C_WriteByte(DateTime.uc_Days);        // Send Days
    I2C_WriteByte(DateTime.uc_WeekDay);     // Send Weekday
    I2C_WriteByte(DateTime.uc_Month);       // Send Month
    I2C_WriteByte(DateTime.uc_Year);        // Send Year
    I2C_Stop();                             // Assert Stop on bus
}

/*******************************************************************************
    FUNCTION: RTC_GetDateTime
    SUMMARY: Gets the Date and Time from the RTC module
    DESCRIPTION: Reads the whole clock page of the RTC module over I2C.
                 Returns the information as a t_DateTime structure.
                 Note: Although the slave allows a continues read the PIC would 
                 refuse to assert an ACK after a read so this function opts to do 
                 consecutive reads
    REQUIRES: None
    ARGS: None
    RETURN: t_DateTime DateTime
*******************************************************************************/
t_DateTime RTC_GetDateTime()
{
    t_DateTime DateTime;                    // Return variable
    I2C_Start();                            // Assert Start on bus
    I2C_WriteByte(RTC_WRITE_ADDRESS);       // Send write address of RTC module
    I2C_WriteByte(RTC_CLOCK_PAGE_ADDRESS);  // Send address of clock page
    
    I2C_Restart();                          // Assert Restart on bus (Do you need to do a restart when you switch from write to read or vise versa?))
    
    I2C_WriteByte(RTC_READ_ADDRESS);        // Send read address of RTC module
    I2C_ReadByte(&DateTime.uc_Seconds);     // Read seconds
    I2C_Stop();                             // Assert Stop on bus
                                            // For the next read the slave will read from the last address + 1 if it is just an instant read with no given address, In this case the minutes reg
    I2C_Start();                            // Assert Start on bus
    I2C_WriteByte(RTC_READ_ADDRESS);        // Send read address of RTC module
    I2C_ReadByte(&DateTime.uc_Minutes);     // Read minutes
    I2C_Stop();                             // Assert Stop on bus
    
    I2C_Start();                            // This read pattern continues until the whole clock page has been read
    I2C_WriteByte(RTC_READ_ADDRESS);        // Send read address of RTC module
    I2C_ReadByte(&DateTime.uc_Hours);       // Read hours
    I2C_Stop();
    
    I2C_Start();
    I2C_WriteByte(RTC_READ_ADDRESS);
    I2C_ReadByte(&DateTime.uc_Days);
    I2C_Stop();
    
    I2C_Start();
    I2C_WriteByte(RTC_READ_ADDRESS);
    I2C_ReadByte(&DateTime.uc_WeekDay);
    I2C_Stop();
    
    I2C_Start();
    I2C_WriteByte(RTC_READ_ADDRESS);
    I2C_ReadByte(&DateTime.uc_Month);
    I2C_Stop();
            
    I2C_Start();
    I2C_WriteByte(RTC_READ_ADDRESS);
    I2C_ReadByte(&DateTime.uc_Year);
    I2C_Stop();     
    
    return DateTime;                        // Return the Date and Time
}