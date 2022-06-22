/*
 * Common.h
 *
 *  Created on: Jun 15, 2022
 *      Author: ASUS
 */

#ifndef MAIN_COMMON_H_
#define MAIN_COMMON_H_

#include "../main/jsonUser/json_user.h"
#include "../main/SPIFFS/spiffs_user.h"

typedef struct _dvinfor
{
	char id[50];
	char token[50];
}Device;
void get_device_infor(Device * _device);





#endif /* MAIN_COMMON_H_ */
