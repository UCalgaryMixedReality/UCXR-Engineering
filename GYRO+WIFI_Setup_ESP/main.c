#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

#include <time.h>                   // Gives access to standard time functions
#include <sys/time.h>               //                    V
#include "esp_sntp.h"               // Enables SNTP (Simple Network Time Protocol)
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define I2C_MASTER_SCL_IO           8
#define I2C_MASTER_SDA_IO           9
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0

#define MPU6050_ADDR               0x68
#define MPU6050_PWR_MGMT_1         0x6B
#define MPU6050_ACCEL_CONFIG       0x1C
#define MPU6050_GYRO_CONFIG        0x1B
#define MPU6050_CONFIG             0x1A
#define MPU6050_ACCEL_XOUT_H       0x3B

#define SWITCH_GPIO                 42
bool isRunning = false;
bool lastSwitchState = true;

#define WIFI_SSID "aiya"
#define WIFI_PASS "aiya-maya"
static const char *TAG = "TIME_SYNC";
static EventGroupHandle_t s_wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Disconnected. Reconnecting...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Got IP Address");
    }
}

static void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
                                        &wifi_event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Connecting to Wi-Fi...");

    // Wait until connected
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        false, true, portMAX_DELAY);
}

esp_err_t mpu6050_write_byte (uint8_t reg_addr, uint8_t data){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t mpu6050_read_bytes(uint8_t reg_addr, uint8_t *data, size_t length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);

    if (length > 1) {
        i2c_master_read(cmd, data, length - 1, I2C_MASTER_ACK);
    }
    
    i2c_master_read_byte(cmd, data + length - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void mpu6050_init() {
    mpu6050_write_byte(MPU6050_PWR_MGMT_1, 0x00);        // Wake up
    mpu6050_write_byte(MPU6050_ACCEL_CONFIG, 0x10);      // +/- 8g
    mpu6050_write_byte(MPU6050_GYRO_CONFIG, 0x08);       // 500 deg/s
    mpu6050_write_byte(MPU6050_CONFIG, 0x06);            // 5 Hz filter
    ESP_LOGI(TAG, "MPU6050 initialized");
}

void initialize_sntp(void) {
    ESP_LOGI("NTP", "Initializing SNTP");       // Prints an info log under the tag NTP.
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);    // Tells SNTP to continuously request time updates.
    esp_sntp_setservername(0, "pool.ntp.org");      // Uses a public NTP server of choice to get current time.
    esp_sntp_init();
}

void wait_for_time_sync(void) {
    time_t now;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 20;

    time(&now);
    localtime_r(&now, &timeinfo);

    while (timeinfo.tm_year < (2025 - 1900) && ++retry < retry_count) {
        ESP_LOGI("NTP", "Waiting for time sync... (%d)", retry);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }


    if (timeinfo.tm_year >= (2025 - 1900)) {
        ESP_LOGI("NTP", "Time synced successfully!");
    } else {
        ESP_LOGW("NTP", "Time sync failed.");
    }
}

void read_mpu6050() {
    uint8_t raw[14];
    mpu6050_read_bytes(MPU6050_ACCEL_XOUT_H, raw, 14);

    int16_t ax = (raw[0] << 8) | raw[1];
    int16_t ay = (raw[2] << 8) | raw[3];
    int16_t az = (raw[4] << 8) | raw[5];
    int16_t temp_raw = (raw[6] << 8) | raw[7];
    int16_t gx = (raw[8] << 8) | raw[9];
    int16_t gy = (raw[10] << 8) | raw[11];
    int16_t gz = (raw[12] << 8) | raw[13];

    float temperature = temp_raw / 340.0 + 36.53;
    float ax_ms = ax / 4096.0 * 9.80665;
    float ay_ms = ay / 4096.0 * 9.80665;
    float az_ms = az / 4096.0 * 9.80665;
    float gx_dps = gx / 65.5;
    float gy_dps = gy / 65.5;
    float gz_dps = gz / 65.5;

    time_t now;
    struct tm timeinfo;
    char time_str[64];

    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
    printf("%s UTC,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", 
       time_str, ax_ms, ay_ms, az_ms, gx_dps, gy_dps, gz_dps, temperature);

}

void app_main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Setup Network
    wifi_init_sta();

    // Start NTP
    initialize_sntp();
    wait_for_time_sync();
    
    // Start MPU6050 and I2C
    mpu6050_init();
    i2c_master_init();

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SWITCH_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);

    ESP_LOGI(TAG, "Flip switch to start/stop logging");

    while (1) {
        bool currentSwitchState = (gpio_get_level(SWITCH_GPIO) == 1);
        if (currentSwitchState != lastSwitchState) {
            isRunning = currentSwitchState;
            ESP_LOGI(TAG, "%s", isRunning ? "START" : "STOP");
            lastSwitchState = currentSwitchState;
        }  
        

        if (isRunning) {
            read_mpu6050();
            vTaskDelay(pdMS_TO_TICKS(500));
            
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}