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
#include "../main/jsonUser/json_user.c"
#include "../main/SPIFFS/spiffs_user.c"
#define TAG "MAIN"
#define BUTTON 0
char *base_path = "/spiffs";
char deviceid[50];
char devicetoken[50];
typedef struct _dvinfor
{
	char id[50];
	char token[50];
}Device;
Device Device_Infor;
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
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
}
void button (void * arg)
{
	button_event_t ev;
	QueueHandle_t button_events = button_init(PIN_BIT(BUTTON));
	int press_count = 0;
	while (true) {
	    if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
	        if ((ev.pin == BUTTON) && (ev.event == BUTTON_DOWN)) {
	        	ESP_LOGI(TAG, "press_count: %d", ++press_count);
	            if(press_count == 5)
	            {
	            	press_count = 0;
	            	ESP_LOGI(TAG, "Start Quick Pair");
	            	start_smartconfig();

	            }
	        }
	    }
	}
}
void app_main(void)
{
	esp_log_level_set("BUTTON", ESP_LOG_NONE);
	init_wifi();
	mountSPIFFS();

    xTaskCreate(button, "button", 4096, NULL, 3, NULL);
}

