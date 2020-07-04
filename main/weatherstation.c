#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "weatherstation.h"
#include "credentials.h" // Place wifi info here

// Private Function Declarations
void weatherInit(void);
void wifiInit(void);
esp_err_t event_handler(void *, system_event_t *);
// Private Variables
tcpip_adapter_ip_info_t ip;
uint8_t level = 0;

void app_main(void)
{
    // Initialize the things!
    wifiInit();
    weatherInit();

    while (true) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        ESP_LOGI(LOG_TAG, "LED set to: [%d]", level);
        if (tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip) == 0) 
        {
            ESP_LOGI(LOG_TAG, "~~~~~~~~~~~");
            ESP_LOGI(LOG_TAG, "IP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(LOG_TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(LOG_TAG, "GW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(LOG_TAG, "~~~~~~~~~~~");
        }
    }
}

void weatherInit(void)
{
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
}

void wifiInit(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = HOME_WIFI_SSID,
            .password = HOME_WIFI_PASS,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
    memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK; // TODO: Make this more good-er.
}
