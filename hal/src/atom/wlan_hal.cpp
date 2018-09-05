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

#include <stdio.h>
#include "enums_hal.h"
#include "wlan_hal.h"
#include "wiring_ex_process.h"

//=======net notify===========
static HAL_NET_Callbacks netCallbacks = { 0 };

void HAL_NET_SetCallbacks(const HAL_NET_Callbacks* callbacks, void* reserved)
{
    netCallbacks.notify_connected = callbacks->notify_connected;
    netCallbacks.notify_disconnected = callbacks->notify_disconnected;
    netCallbacks.notify_dhcp = callbacks->notify_dhcp;
    netCallbacks.notify_can_shutdown = callbacks->notify_can_shutdown;
}

void HAL_NET_notify_connected()
{
    if (netCallbacks.notify_connected) {
        netCallbacks.notify_connected();
    }
}

void HAL_NET_notify_disconnected()
{
    if (netCallbacks.notify_disconnected) {
        netCallbacks.notify_disconnected();
    }
}

void HAL_NET_notify_dhcp(bool dhcp)
{
    if (netCallbacks.notify_dhcp) {
        netCallbacks.notify_dhcp(dhcp); // dhcp dhcp
    }
}

void HAL_NET_notify_can_shutdown()
{
    if (netCallbacks.notify_can_shutdown) {
        netCallbacks.notify_can_shutdown();
    }
}

uint32_t HAL_NET_SetNetWatchDog(uint32_t timeOutInMS)
{
    return 0;
}

//=======wifi activate/deactivate===========
wlan_result_t HAL_WLAN_On(void)
{
    return 0;
}

wlan_result_t HAL_WLAN_Off(void)
{
    return 0;
}

void HAL_WLAN_Setup(void)
{
}

//=======wifi connect===========
wlan_result_t HAL_WLAN_Connect(void)
{
    Process Proc;

    Proc.begin("wifi_connect");
    Proc.addParameter("BEGIN");
    Proc.runAsynchronously();
    return 0;
}

wlan_result_t HAL_WLAN_Disconnect(void)
{
    return 0;
}

void HAL_WLAN_Connect_Cancel(bool called_from_isr)
{

}

int HAL_WLAN_Connected_Rssi(void)
{
    String tmp;
    int32_t rssi = 0;
    Process Proc;

    Proc.begin("wifi_get_current");
    Proc.addParameter("RSSI");
    int res = Proc.run();
    if(res == 0) {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        rssi=tmp.toInt();
    }
    return rssi;
}

void HAL_WLAN_Drive(void)
{
}

//================credentials======================
int HAL_WLAN_Clear_Credentials()
{
    return 0;
}

int HAL_WLAN_Has_Credentials(void)
{
    return 0;
}

bool HAL_WLAN_Reset_Credentials_Store_Required(void)
{
    return false;
}

wlan_result_t HAL_WLAN_Reset_Credentials_Store(void)
{
    HAL_WLAN_Clear_Credentials();
    return 0;
}

int HAL_WLAN_Get_Credentials(wlan_scan_result_t callback, void* callback_data)
{
    return 0;
}

int HAL_WLAN_Set_Credentials(WLanCredentials* c)
{
    Process Proc;

    Proc.begin("wifi_set_credentials");
    Proc.addParameter(c->ssid);
    if(c->password) {
        Proc.addParameter(c->password);
    } else {
        Proc.addParameter("");
    }
    Proc.addParameter("auto");

    switch(c->security)
    {
        case WLAN_SEC_WEP :
            Proc.addParameter("WEP");
            break;
        case WLAN_SEC_WPA :
            Proc.addParameter("WPAPSK/AES");
            break;
        case WLAN_SEC_WPA2 :
            Proc.addParameter("WPA2PSK/AES");
            break;
        default:
            Proc.addParameter("NONE");
            break;
    };
    Proc.run();
    return 0;
}

//==============imlink==================
void HAL_WLAN_Imlink_Start(void)
{
    Process Proc;

    HAL_WLAN_Imlink_Stop();
    Proc.begin("imlink_config");
    Proc.addParameter("SET");
    Proc.run();
}

imlink_status_t HAL_WLAN_Imlink_Get_Status(void)
{
    return IMLINK_SUCCESS;
}

void HAL_WLAN_Imlink_Stop(void)
{
    Process Proc;

    Proc.begin("sc_config_kill");
    Proc.run();
}

void HAL_WLAN_Fetch_Ipconfig(WLanConfig* config)
{
    uint8_t addr_data[4];
    uint8_t mac[6];
    int res;
    String tmp;
    Process Proc;

    Proc.begin("wifi_get_ifconfig");
    Proc.addParameter("LOCALIP");
    res = Proc.run();
    if(res == 0) {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        sscanf(tmp.c_str(),"%d.%d.%d.%d",&addr_data[0],&addr_data[1],&addr_data[2],&addr_data[3]);
        config->nw.aucIP.ipv4 = IPADR(addr_data[0], addr_data[1], addr_data[2], addr_data[3]);
    }

    Proc.begin("wifi_get_ifconfig");
    Proc.addParameter("SUBNETMASK");
    res = Proc.run();
    if(res == 0)
    {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        sscanf(tmp.c_str(),"%d.%d.%d.%d",&addr_data[0],&addr_data[1],&addr_data[2],&addr_data[3]);
        config->nw.aucSubnetMask.ipv4 = IPADR(addr_data[0], addr_data[1], addr_data[2], addr_data[3]);
    }

    Proc.begin("wifi_get_ifconfig");
    Proc.addParameter("GATEWAYIP");
    res = Proc.run();
    if(res == 0) {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        sscanf(tmp.c_str(),"%d.%d.%d.%d",&addr_data[0],&addr_data[1],&addr_data[2],&addr_data[3]);
        config->nw.aucDefaultGateway.ipv4 = IPADR(addr_data[0], addr_data[1], addr_data[2], addr_data[3]);
    }

    Proc.begin("wifi_get_ifconfig");
    Proc.addParameter("MACADDRESS");
    res = Proc.run();
    if(res == 0) {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        sscanf(tmp.c_str(),"%x:%x:%x:%x:%x:%x",&mac[5],&mac[4],&mac[3],&mac[2],&mac[1],&mac[0]);
        memcpy(config->nw.uaMacAddr,mac,6);
    }

    Proc.begin("wifi_get_current");
    Proc.addParameter("SSID");
    res = Proc.run();
    if(res == 0) {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        memcpy(config->uaSSID, tmp.c_str(), 32);
    }

    Proc.begin("wifi_get_current");
    Proc.addParameter("BSSID");
    res = Proc.run();
    if(res == 0) {
        while (Proc.available())
        {
            tmp+=(char)Proc.read();
        }
        sscanf(tmp.c_str(),"%x:%x:%x:%x:%x:%x",&mac[5],&mac[4],&mac[3],&mac[2],&mac[1],&mac[0]);
        memcpy(config->BSSID,mac,6);
    }
}

void HAL_WLAN_Set_Error_Count(uint32_t errorCount)
{
}

/**
 * Sets the IP source - static or dynamic.
 */
void HAL_WLAN_Set_Ipaddress_Source(IPAddressSource source, bool persist, void* reserved)
{
}

/**
 * Sets the IP Addresses to use when the device is in static IP mode.
 * @param device
 * @param netmask
 * @param gateway
 * @param dns1
 * @param dns2
 * @param reserved
 */
void HAL_WLAN_Set_Ipaddress(const HAL_IPAddress* device, const HAL_IPAddress* netmask,
        const HAL_IPAddress* gateway, const HAL_IPAddress* dns1, const HAL_IPAddress* dns2, void* reserved)
{

}

int HAL_WLAN_Scan(wlan_scan_result_t callback, void* cookie)
{
    return -1;
}

/**
 * wifi get station and ap mac addr
 */
int HAL_WLAN_Get_Macaddr(uint8_t *stamacaddr, uint8_t *apmacaddr)
{
    return 0;
}
