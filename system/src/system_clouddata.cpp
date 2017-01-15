/**
 ******************************************************************************
  Copyright (c) 2013-2014 IntoRobot Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
*/

#include "intorobot_config.h"

#ifndef configNO_CLOUD

#include <stdlib.h>
#include <stdio.h>
#include "wiring_string.h"
#include "system_cloud_def.h"
#include "system_cloud.h"
#include "ajson.h"

/*debug switch*/
#define SYSTEM_CLOUDDATA_DEBUG

#ifdef SYSTEM_CLOUDDATA_DEBUG
#define SCLOUDDATA_DEBUG(...)  do {DEBUG(__VA_ARGS__);}while(0)
#define SCLOUDDATA_DEBUG_D(...)  do {DEBUG_D(__VA_ARGS__);}while(0)
#else
#define SCLOUDDATA_DEBUG(...)
#define SCLOUDDATA_DEBUG_D(...)
#endif


property_conf* properties[PROPERTIES_MAX];
int properties_count = 0;

int intorobotDiscoverProperty(const uint16_t dpID)
{
    for (int i = 0; i < properties_count; i++)
    {
        if (properties[i]->dpID == dpID)
        {
            return i;
        }
    }
    return -1;
}

void intorobotAddDataPointBool(const uint16_t dpID, const char *permission, bool value, const char *policy, int lapse)
{
    if (-1 == intorobotDiscoverProperty(dpID))
    {
        // Create property structure
        property_conf* prop = new property_conf {dpID, DATA_TYPE_BOOL, permission, policy, (long)lapse*1000, 0, RESULT_DATAPOINT_OLD};
        prop->value = String(value);
        properties[properties_count] = prop; // Save pointer to scructure
        properties_count++; // count the number of properties
    }
}

void intorobotAddDataPointNumber(const uint16_t dpID, const char *permission, const double minValue, const double maxValue, const double resolution, double value, const char *policy, int lapse)
{
    if (-1 == intorobotDiscoverProperty(dpID))
    {
        property_conf* prop;
        // Create property structure
        if(resolution == int(resolution))
        {
            prop = new property_conf {dpID, DATA_TYPE_INT, permission, policy, (long)lapse*1000, 0, RESULT_DATAPOINT_OLD};
            prop->intProperty.minValue = (int)minValue;
            prop->intProperty.maxValue = (int)maxValue;
            prop->intProperty.resolution = (int)resolution;
            prop->value = String((int)value);
        }
        else
        {
            prop = new property_conf {dpID, DATA_TYPE_FLOAT, permission, policy, (long)lapse*1000, 0, RESULT_DATAPOINT_OLD};
            prop->floatProperty.minValue = minValue;
            prop->floatProperty.maxValue = maxValue;
            prop->floatProperty.resolution = resolution;
            prop->value = String(value);
        }
        properties[properties_count] = prop; // Save pointer to scructure
        properties_count++; // count the number of properties
    }
}

void intorobotAddDataPointEnum(const uint16_t dpID, const char *permission, int value, const char *policy, int lapse)
{
    if (-1 == intorobotDiscoverProperty(dpID))
    {
        // Create property structure
        property_conf* prop = new property_conf {dpID, DATA_TYPE_ENUM, permission, policy, (long)lapse*1000, 0, RESULT_DATAPOINT_OLD};
        prop->value = String(value);
        properties[properties_count] = prop; // Save pointer to scructure
        properties_count++; // count the number of properties
    }
}

void intorobotAddDataPointString(const uint16_t dpID, const char *permission, char *value, const char *policy, int lapse)
{
    if (-1 == intorobotDiscoverProperty(dpID))
    {
        // Create property structure
        property_conf* prop = new property_conf {dpID, DATA_TYPE_STRING, permission, policy, (long)lapse*1000, 0, RESULT_DATAPOINT_OLD};
        prop->value = value;
        properties[properties_count] = prop; // Save pointer to scructure
        properties_count++; // count the number of properties
    }
}

void intorobotAddDataPointBinary(const uint16_t dpID, const char *permission, uint8_t *value, uint16_t len, const char *policy, int lapse)
{
    if (-1 != intorobotDiscoverProperty(dpID))
    {
        // Create property structure
        property_conf* prop = new property_conf {dpID, DATA_TYPE_BINARY, permission, policy, (long)lapse*1000, 0, RESULT_DATAPOINT_OLD};
        prop->binaryValue = value;
        prop->binaryLen = len;
        properties[properties_count] = prop; // Save pointer to scructure
        properties_count++; // count the number of properties
    }
}

read_datapoint_result_t intorobotReadDataPointBool(const uint16_t dpID, bool &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = (bool)(properties[index]->value.toInt());
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointInt(const uint16_t dpID, int &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = (int)(properties[index]->value.toInt());
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointInt32(const uint16_t dpID, int32_t &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = (int32_t)(properties[index]->value.toInt());
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointUint32(const uint16_t dpID, uint32_t &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = (uint32_t)(properties[index]->value.toInt());
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointFloat(const uint16_t dpID, float &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = properties[index]->value.toFloat();
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointDouble(const uint16_t dpID, double &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = properties[index]->value.toFloat();
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointString(const uint16_t dpID, String &value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = properties[index]->value;
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointStringChar(const uint16_t dpID, char *value)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = (char *)(properties[index]->value.c_str());
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

read_datapoint_result_t intorobotReadDataPointBinary(const uint16_t dpID, uint8_t *value, uint16_t &len)
{
    int index = intorobotDiscoverProperty(dpID);
    if (index == -1) {
        return RESULT_DATAPOINT_NONE;
    }

    value = properties[index]->binaryValue;
    len = properties[index]->binaryLen;
    read_datapoint_result_t readResult = properties[index]->readFlag;
    properties[index]->readFlag = RESULT_DATAPOINT_OLD;
    return readResult;
}

void intorobotReceiveDataProcessJson(uint8_t *payload, uint32_t len)
{
    aJsonClass aJson;

    if(payload[0] != JSON_DATA_FORMAT)
    {
        return;
    }

    aJsonObject *root = aJson.parse((char *)(payload+1));
    if (root == NULL)
    {
        return;
    }

    for (int i = 0; i <= properties_count; i++)
    {
        if (0 != strcmp(properties[i]->permission, UP_ONLY))
        {
            aJsonObject* propertyObject = aJson.getObjectItem(root, String(properties[i]->dpID).c_str());
            if (propertyObject != NULL)
            {
                properties[i]->readFlag = RESULT_DATAPOINT_NEW;
                switch(properties[i]->dataType)
                {
                    case DATA_TYPE_BOOL:       //bool型
                        properties[i]->value = String(propertyObject->valuebool);
                        break;
                    case DATA_TYPE_INT:        //数值型 int型
                        properties[i]->value = String(propertyObject->valueint);
                        break;
                    case DATA_TYPE_FLOAT:      //数值型 float型
                        properties[i]->value = String(propertyObject->valuefloat);
                        break;
                    case DATA_TYPE_ENUM:       //枚举型
                        properties[i]->value = String(propertyObject->valueint);
                        break;
                    case DATA_TYPE_STRING:     //字符串型
                        properties[i]->value = String(propertyObject->valuestring);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

// Build the topic for a property
String intorobotBuildSinglePropertyJson(int property_index)
{
    aJsonClass aJson;
    String PropertyJson(JSON_DATA_FORMAT);

    aJsonObject* root = aJson.createObject();
    switch(properties[property_index]->dataType)
    {

        case DATA_TYPE_BOOL:       //bool型
            aJson.addBooleanToObject(root, String(properties[property_index]->dpID).c_str(), (bool)(properties[property_index]->value.toInt()));
            break;
        case DATA_TYPE_INT:        //数值型 int型
            aJson.addNumberToObject(root, String(properties[property_index]->dpID).c_str(), (int)(properties[property_index]->value.toInt()));
            break;
        case DATA_TYPE_FLOAT:      //数值型 float型
            aJson.addNumberToObject(root, String(properties[property_index]->dpID).c_str(), properties[property_index]->value.toFloat());
            break;
        case DATA_TYPE_ENUM:       //枚举型
            aJson.addNumberToObject(root, String(properties[property_index]->dpID).c_str(), (int)(properties[property_index]->value.toInt()));
            break;
        case DATA_TYPE_STRING:     //字符串型
            aJson.addStringToObject(root, String(properties[property_index]->dpID).c_str(), properties[property_index]->value.c_str());
            break;
        default:
            break;
    }
    char* strPtr = aJson.print(root);
    PropertyJson += strPtr;
    free(strPtr);
    aJson.deleteItem(root);

    return PropertyJson;
}

// Build the topic for a property
String intorobotBuildAllPropertyJson(void)
{
    aJsonClass aJson;

    aJsonObject* root = aJson.createObject();
    for (int i = 0; i <= properties_count; i++)
    {
        switch(properties[i]->dataType)
        {
            case DATA_TYPE_BOOL:       //bool型
                aJson.addBooleanToObject(root, String(properties[i]->dpID).c_str(), (bool)(properties[i]->value.toInt()));
                break;
            case DATA_TYPE_INT:        //数值型 int型
                aJson.addNumberToObject(root, String(properties[i]->dpID).c_str(), (int)(properties[i]->value.toInt()));
                break;
            case DATA_TYPE_FLOAT:      //数值型 float型
                aJson.addNumberToObject(root, String(properties[i]->dpID).c_str(), properties[i]->value.toFloat());
                break;
            case DATA_TYPE_ENUM:       //枚举型
                aJson.addNumberToObject(root, String(properties[i]->dpID).c_str(), (int)(properties[i]->value.toInt()));
                break;
            case DATA_TYPE_STRING:     //字符串型
                aJson.addStringToObject(root, String(properties[i]->dpID).c_str(), properties[i]->value.c_str());
                break;
            default:
                break;
        }
    }

    char* strPtr = aJson.print(root);
    String PropertyJson = strPtr;
    free(strPtr);
    aJson.deleteItem(root);

    return PropertyJson;
}

void intorobotWriteDataPointString(const uint16_t dpID, char* value)
{
    int i = intorobotDiscoverProperty(dpID);

    if (i == -1) {
        // not found, nothing to do
        return;
    }

    //只允许下发
    if (strcmp(properties[i]->permission, DOWN_ONLY) == 0) {
        SCLOUDDATA_DEBUG("only permit cloud -> terminal %d", properties[i]->dpID);
        return;
    }

    //数值未发生变化
    if (properties[i]->value.equals(value) && strcmp(properties[i]->policy, ON_CHANGE) == 0) {
        SCLOUDDATA_DEBUG("No Changes for %d:%d", properties[i]->dpID, value);
        return;
    }

    //发送时间间隔到
    if ((long)(millis() - properties[i]->runtime) >= properties[i]->lapse) {
        properties[i]->value = value;
        String payload = intorobotBuildSinglePropertyJson(i);
        intorobot_publish(API_VERSION_V2, INTOROBOT_MQTT_RX_TOPIC, (uint8_t *)payload.c_str(), strlen(payload.c_str()), 0, false);
        SCLOUDDATA_DEBUG("OK, Published");
        properties[i]->runtime = millis();
    }
}

void intorobotWriteDataPointBinary(const uint16_t dpID, uint8_t *value, uint16_t len)
{

}

void intorobotWriteDataPointAll(void)
{
    String payload = intorobotBuildAllPropertyJson();
    intorobot_publish(API_VERSION_V2, INTOROBOT_MQTT_RX_TOPIC, (uint8_t *)payload.c_str(), strlen(payload.c_str()), 0, false);
}

#endif
