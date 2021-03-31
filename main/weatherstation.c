#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "mqtt_client.h"

#include "weatherstation.h"
#include "bme280.h" 
#include "credentials.h" // Place wifi info here

// Private Function Declarations
void weatherInit(void);
void wifiInit(void);
void mqttInit(void);

// Wind Direction is connected to IO35 --> ADC1_7

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
esp_err_t event_handler(void *, system_event_t *);
// Private Variables
tcpip_adapter_ip_info_t ip;
uint8_t level = 0;

void app_main(void)
{
    // Initialize the things!
    wifiInit();
    weatherInit();
    mqttInit();

    while (true) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        //ESP_LOGI(LOG_TAG, "LED set to: [%d]", level);
        if (tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip) == 0) 
        {
            //ESP_LOGI(LOG_TAG, "~~~~~~~~~~~");
            //ESP_LOGI(LOG_TAG, "IP:"IPSTR, IP2STR(&ip.ip));
            //ESP_LOGI(LOG_TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
            //ESP_LOGI(LOG_TAG, "GW:"IPSTR, IP2STR(&ip.gw));
            //ESP_LOGI(LOG_TAG, "~~~~~~~~~~~");
        }
    }
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

void weatherInit(void)
{
    int ret = 0;
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    ret = bme2800_init();
    if(ret < 0)
        ESP_LOGI(LOG_TAG, "BME280 Init Fail");
    ESP_LOGI(LOG_TAG, "BME280 Init Complete");
}

void mqttInit(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://192.168.1.119:1883", // TODO: Determine which URIs are acceptable
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(LOG_TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(LOG_TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(LOG_TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(LOG_TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(LOG_TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(LOG_TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(LOG_TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(LOG_TAG, "Event dispatched from event loop base= %s, event_id= %d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK; // TODO: Make this more good-er.
}
