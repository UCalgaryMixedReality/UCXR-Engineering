# Setup for GY-521 with the ESP32-S3-DevKitC-1U

This project demonstrates how to interface an **MPU6050 (GY-521)** accelerometer and gyroscope module with the **ESP32-S3 DevKitC-1** using the ESP-IDF framework and I2C communication.

## Features

- Reads acceleration, gyroscope, and temperature data from MPU6050 via I2C
- Switch-controlled start/stop logging over UART
- Uses ESP-IDF native APIs for I2C and GPIO
- ESP32 logs data from GY-521 onto Excel

## Hardware Setup

| Device         | ESP32-S3 Pin | Notes                                                             |
|----------------|--------------|-------------------------------------------------------------------|
| MPU6050 SDA    | GPIO 9       | Connect to MPU6050 SDA                                            |
| MPU6050 SCL    | GPIO 8       | Connect to MPU6050 SCL                                            |
| MPU6050 VCC    | 5V           | Check your MPU6050 module spec                                    |
| MPU6050 GND    | GND          |                                                                   |
| Switch         | GPIO 42      | Connect switch between VCC (5V) and GND with middle pin to GPIO42 |

> **Note:** The switch uses the internal pull-up resistor and is active low.

## Software Setup

### 1. Install PlatformIO  
   Use Visual Studio Code and install the PlatformIO IDE extension. Click [here](https://docs.platformio.org/en/latest/integration/ide/vscode.html#installation) and follow the setup instructions. 
   > **Note:** When creating a new project, select the board **Espressif ESP32-S3-DevKitC-1-N8** and ensure the Framework is **Espidf**.

### 2. Clone or copy this project
   Ensure the project structure is as follows:
  
    
    project-root/
    |── include/
    ├── lib/
    ├── src/
    │   └── main.c        ← Main code file
    ├── platformio.ini    ← PlatformIO configuration
    └── README.md         ← You're here!
   > **Note:** When pulling project from GitHub, only **main.c** is getting pulled
    

### 3. Configuration
   Make sure the configuration in platformio.ini is as follows:
    
    [env:esp32-s3-devkitc-1]

    platform = espressif32
    board = esp32-s3-devkitc-1
    framework = espidf
    monitor_speed = 115200
    build_flags = 
        -D I2C_MASTER_SCL_IO=8
        -D I2C_MASTER_SDA_IO=9
        -D SWITCH_GPIO=42
        -D CONFIG_LWIP_SNTP_ENABLE=1
    

### 4. Included Modules
   | Module                                              | Purpose                                                               |
   | --------------------------------------------------- | --------------------------------------------------------------------- |
   | **I2C (driver/i2c.h)**                              | Initializes I2C bus and reads/writes MPU6050 registers                |
   | **MPU6050 driver functions**                        | Sets configuration (range, filter, power), reads 14-byte sensor block |
   | **GPIO (driver/gpio.h)**                            | Configures switch input, reads its state                              |
   | **Wi-Fi (esp\_wifi.h, esp\_netif.h, esp\_event.h)** | Connects to local Wi-Fi network for NTP access                        |
   | **NTP (esp\_sntp.h)**                               | Syncs system clock using internet NTP servers                         |
   | **Logging (esp\_log.h)**                            | Prints logs to serial monitor (info, warnings)                        |
   | **Time (time.h, sys/time.h)**                       | Gets current time, formats it as HH\:MM\:SS string                    |
   | **FreeRTOS (freertos/FreeRTOS.h, freertos/task.h)** | Provides delay functions (vTaskDelay) for timing and polling          |

### 5. Define Variables

| Macro                       | Purpose                                         | How to set up                                           |
| --------------------------- | ------------------------------- | --------------------------------------------------------|
| `WIFI_SSID`                 | Your Wi-Fi network SSID.        | `#define WIFI_SSID 'your_SSID'`.                        |
| `WIFI_PASS`                 | Your Wi-Fi network password.    | `#define WIFI_PASS 'your_password`.                     |
| `I2C_MASTER_SCL_IO`         | GPIO pin for I2C SCL (clock).   | Set to the pin you wired for SCL (usually pin 8 or 18). |
| `I2C_MASTER_SDA_IO`         | GPIO pin for I2C SDA (data).    | Set to the pin you wired for SDA (usually pin 9 or 17). |
| `I2C_MASTER_FREQ_HZ`        | I2C clock frequency.            | Usually 400000 (400 kHz) for fast mode I2C.             |
| `I2C_MASTER_PORT`           | I2C port number.                | Typically set to `I2C_NUM_0`.                           |
| `I2C_MASTER_TX_BUF_DISABLE` | Disables I2C TX buffer.         | Set to 0.                                               |
| `I2C_MASTER_RX_BUF_DISABLE` | Disables I2C RX buffer.         | Set to 0.                                               |
| `SWITCH_GPIO`               | GPIO pin used for switch input. | Set to the GPIO where your switch is wired.             |


### 6. Block Functions

 ---

   ### `void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)`

   Handles Wi-Fi and IP events:

   - Connects on startup and reconnects on disconnect.
   - Sets a flag when IP is obtained.

  ---

   ### `void wifi_init_sta(void)`

   Initializes Wi-Fi in station mode:

   - Creates network interface.
   - Configures Wi-Fi credentials.
   - Registers event handlers.
   - Waits until Wi-Fi connects.

 ---

   ### `void i2c_master_init(void)`

   Initializes I2C master:

   - Sets up SDA and SCL pins, enables pull-ups.
   - Sets clock speed (400 kHz).
   - Installs I2C driver.

 ---

   ### `esp_err_t mpu6050_write_byte(uint8_t reg_addr, uint8_t data)`

   Writes one byte to a register on MPU6050 over I2C.

 ---

   ### `esp_err_t mpu6050_read_bytes(uint8_t reg_addr, uint8_t *data, size_t length)`

   Reads multiple bytes starting at a register address from MPU6050 over I2C.

 ---

   ### `void mpu6050_init(void)`

   Configures the MPU6050:

   - Wakes it up from sleep mode.
   - Sets accelerometer to ±8g.
   - Sets gyroscope to ±500°/s.
   - Applies a 5 Hz low-pass filter.

 ---

   ### `void initialize_sntp(void)`

   Sets up SNTP:

   - Defines NTP server.
   - Starts polling for time updates.

 ---

   ### `void wait_for_time_sync(void)`

   Waits until SNTP updates the time or timeout occurs:

   - Polls system time.
   - Logs status and outcome.

 ---

   ### `void read_mpu6050(void)`

   Reads sensor data:

   - Gets accel, gyro, and temp values.
   - Converts raw values to physical units.
   - Gets current time.
   - Prints a CSV line with timestamp and sensor data.

 ---

   ### `void app_main(void)`

   Main application:

   - Initializes NVS for Wi-Fi.
   - Starts Wi-Fi and SNTP.
   - Initializes MPU6050 and I2C.
   - Configures switch GPIO.
   - Loops:
       - Reads switch state.
       - Starts/stops logging based on switch.
       - Reads and logs sensor data when active.
