/*
 * json_user.c
 *
 *  Created on: Jun 14, 2022
 *      Author: ASUS
 */

#include "json_user.h"


void JSON_analyze_post(char* my_json_string, char * deviceid, char * devicetoken)
{
	cJSON *root = cJSON_Parse(my_json_string);
	cJSON *current_element = NULL;
	cJSON_ArrayForEach(current_element, root)
	{
		if (current_element->string)
		{
			const char* string = current_element->string;
			if(strcmp(string, "deviceid") == 0)
			{
				memcpy(deviceid, current_element->valuestring, strlen(current_element->valuestring));
			}
			if(strcmp(string, "devicetoken") == 0)
			{
				memcpy(devicetoken, current_element->valuestring, strlen(current_element->valuestring));
			}
		}
	}
	cJSON_Delete(root);
}

void JSON_analyze_SUB(char* my_json_string, char * action)
{
	cJSON *root = cJSON_Parse(my_json_string);
	cJSON *current_element = NULL;
	cJSON_ArrayForEach(current_element, root)
	{
		if (current_element->string)
		{
			const char* string = current_element->string;
			if(strcmp(string, "action") == 0)
			{
				memcpy(action, current_element->valuestring, strlen(current_element->valuestring));
			}
		}
	}
	cJSON_Delete(root);
}
