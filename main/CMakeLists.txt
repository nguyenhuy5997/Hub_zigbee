# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c"
					"../main/Pair/QuickMode/SmartConfig.c"
					"../main/Button/Button.c"
					"../main/Pair/HttpServer/WebServer.c"
					"../main/jsonUser/json_user.c"
					"../main/SPIFFS/spiffs_user.c"
					"../main/WiFi/WiFi_proc.c"
					"../main/Pair/CompatibleMode/AP.c"
					"../main/Mqtt/mqtt.c"
					"../main/OTA/fota.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
