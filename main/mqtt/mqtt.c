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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "../jsonUser/json_user.h"
#include "../OTA/fota.h"
#include "../common.h"
char sub_buf[100];
static const char *TAG = "MQTT_USER";
extern Device Device_Infor;
esp_mqtt_client_config_t mqtt_cfg;
esp_mqtt_client_handle_t client;
RingbufHandle_t buf_handle;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        sprintf(sub_buf, "ont2mqtt/%s/commands/set", Device_Infor.id);
        msg_id = esp_mqtt_client_subscribe(client, sub_buf, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        UBaseType_t res =  xRingbufferSend(buf_handle, event->data, event->data_len, pdMS_TO_TICKS(1000));
		if (res != pdTRUE) {
			printf("Failed to send item\n");
		}
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
void mqtt_handle(void *arg)
{
	char * item = NULL;
	size_t item_size;
	char action[20];
	while(1)
	{
		item = (char *)xRingbufferReceive(buf_handle, &item_size, pdMS_TO_TICKS(1000));
		if(item)
		{
			item[item_size] = NULL;
			printf("SUB_DATA: %s\r\n", item);
			JSON_analyze_SUB(item, action);
			if(strstr(action, "upgrade"))
			{
				  xTaskCreate(&ota_task, "ota_task", 1024 * 8, NULL, 5, NULL);
			}
			vRingbufferReturnItem(buf_handle, (void *)item);
		}
	}
}
void mqtt_app_start(char *broker, char *client_id, char *passowrd)
{
	buf_handle = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);
	if (buf_handle == NULL) {
			printf("Failed to create ring buffer\n");
	}
    mqtt_cfg.uri = broker;
    mqtt_cfg.username = client_id;
    mqtt_cfg.client_id = client_id;
    mqtt_cfg.password = passowrd;
    mqtt_cfg.keepalive = 20;
    client = esp_mqtt_client_init(&mqtt_cfg);
    xTaskCreate(mqtt_handle, "mqtt_handle", 4096, NULL, 3, NULL);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

