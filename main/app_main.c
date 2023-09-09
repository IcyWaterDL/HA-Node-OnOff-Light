/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "led.h"
#include "button.h"
#include "SmartConfig.h"
#include "WiFi_proc.h"
#include "mqtt.h"

#define ONOFF_LIGHT_PIN   	4
#define BROKER 				"mqtt://192.168.0.108"
#define USER_NAME			"nmtam"
#define PASSWORD			"221220"

enum system_state_t STATE = UNKNOW;

__NOINIT_ATTR bool flag_smart_config;

extern esp_mqtt_client_handle_t client;

static const char *TAG = "MAIN";

void app_main(void)
{
    nvs_flash_init();
    init_wifi();
    led_init();

	xTaskCreate(led_status_task, "led_status_task", 1024, NULL, 200, NULL);
	xTaskCreate(button_task, "button_task", 4096, NULL, 200, NULL);

    if( esp_reset_reason() == ESP_RST_UNKNOWN || esp_reset_reason() == ESP_RST_POWERON)
	{
    	flag_smart_config = false;
	}

    if (flag_smart_config) {
		start_smartconfig();
		STATE = SMARTCONFIG_MODE;
		ESP_LOGI(TAG, "STATE = SMARTCONFIG_MODE");
	}
    else if (flag_smart_config == false) {
		wifi_config_t wifi_config = {
			.sta = {
				.threshold.authmode = WIFI_AUTH_WPA2_PSK,
				.pmf_cfg = {
					.capable = true,
					.required = false
				},
			},
		};
		if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config) == ESP_OK)
		{
			ESP_LOGI(TAG, "Wifi configuration already stored in flash partition called NVS");
			ESP_LOGI(TAG, "%s" ,wifi_config.sta.ssid);
			ESP_LOGI(TAG, "%s" ,wifi_config.sta.password);
			wifi_init_sta(wifi_config, WIFI_MODE_STA);
			mqtt_app_start(BROKER, USER_NAME, PASSWORD);
		}
    }

}
