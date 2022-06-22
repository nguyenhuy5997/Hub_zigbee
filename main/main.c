#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "../main/Button/Button.h"
#include "../main/Pair/QuickMode/SmartConfig.h"
#include "../main/Pair/HttpServer/WebServer.h"
#include "../main/jsonUser/json_user.h"
#include "../main/SPIFFS/spiffs_user.h"
#include "../main/WiFi/WiFi_proc.c"
#include "../main/Pair/CompatibleMode/AP.h"
#include "../main/Mqtt/mqtt.h"
#include "OTA/fota.h"
#include "common.h"
#define TAG "MAIN"
#define BUTTON 0

Device Device_Infor;
__NOINIT_ATTR bool Flag_quick_pair;
__NOINIT_ATTR bool Flag_compatible_pair;
void get_device_infor(Device * _device)
{
	char buff[513];
	readfromfile("deviceinfor", buff);
	JSON_analyze_post(buff, _device->id, _device->token);
}
void init_wifi()
{
    nvs_flash_init();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
	esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
//    esp_wifi_start();
}
void button (void * arg)
{
	button_event_t ev;
	QueueHandle_t button_events = button_init(PIN_BIT(BUTTON));
	int press_count = 0;
	uint32_t capture = 0;
	while (true) {
	    if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
	        if ((ev.pin == BUTTON) && (ev.event == BUTTON_DOWN)) {
	        	if((esp_timer_get_time() / 1000) > (capture + 1000))
	        	{
	        		press_count = 0;
	        	}
	        	ESP_LOGI(TAG, "press_count: %d", ++press_count);
	        	capture = esp_timer_get_time() / 1000;
	            if(press_count == 5)
	            {
	            	if(Flag_quick_pair == false)
	            	{
	            		Flag_compatible_pair = false;
	            		Flag_quick_pair = true;
						ESP_LOGI(TAG, "Start Quick Pair");
//						start_smartconfig();
	            	}
	            	else if (Flag_compatible_pair == false)
	            	{
	            		Flag_quick_pair = false;
	            		Flag_compatible_pair = true;
	            		ESP_LOGI(TAG, "Start Quick Pair");
//	            		wifi_init_softap();
	            	}
	            	esp_restart();
	            	press_count = 0;
	            }
	        }
	    }
	}
}
void app_main(void)
{
	esp_log_level_set("BUTTON", ESP_LOG_NONE);
	xTaskCreate(button, "button", 4096, NULL, 3, NULL);
	init_wifi();
	mountSPIFFS();
	get_device_infor(&Device_Infor);
	ESP_LOGI(TAG, "ID: %s, TOK: %s", Device_Infor.id, Device_Infor.token);
	if( esp_reset_reason() == ESP_RST_UNKNOWN || esp_reset_reason() == ESP_RST_POWERON)
	{
		Flag_quick_pair = false;
		Flag_compatible_pair = false;
	}
	if (Flag_quick_pair)
	{
		start_smartconfig();
	}
	else if (Flag_compatible_pair)
	{
		wifi_init_softap();
	}
	else if (Flag_quick_pair == false && Flag_compatible_pair == false)
	{
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
		  if(wifi_init_sta(wifi_config))
		  {
			  mqtt_app_start("mqtt://mqtt.innoway.vn:1883", Device_Infor.id, Device_Infor.token);
		  }
		}
	}
}

