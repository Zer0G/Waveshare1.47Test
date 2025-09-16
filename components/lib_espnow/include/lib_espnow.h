#pragma once

#include "esp_err.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifndef LIB_ESPNOW_H
#define LIB_ESPNOW_H

/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
#if CONFIG_ESPNOW_WIFI_MODE_STATION
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
#endif

#define ESPNOW_QUEUE_SIZE           6

#define IS_BROADCAST_ADDR(addr) (memcmp(addr, s_libEspNow_broadcast_mac, ESP_NOW_ETH_ALEN) == 0)

typedef enum {
    LIB_ESPNOW_SEND_CB,
    LIB_ESPNOW_RECV_CB,
} libEspNow_event_id_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} libEspNow_event_send_cb_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} libEspNow_event_recv_cb_t;

typedef union {
    libEspNow_event_send_cb_t send_cb;
    libEspNow_event_recv_cb_t recv_cb;
} libEspNow_event_info_t;

/* When ESPNOW sending or receiving callback function is called, post event to ESPNOW task. */
typedef struct {
    libEspNow_event_id_t id;
    libEspNow_event_info_t info;
} libEspNow_event_t;

typedef enum {
    LIB_ESPNOW_DATA_BROADCAST,
    LIB_ESPNOW_DATA_UNICAST,
    LIB_ESPNOW_DATA_MAX,
    LIB_ESPNOW_DATA_ERROR,
}libEspNow_messagetype_t;

/* User defined field of ESPNOW data in this example. */
typedef struct {
    uint8_t type;                         //Broadcast or unicast ESPNOW data.
    uint8_t state;                        //Indicate that if has received broadcast ESPNOW data or not.
    uint16_t seq_num;                     //Sequence number of ESPNOW data.
    uint16_t crc;                         //CRC16 value of ESPNOW data.
    uint32_t magic;                       //Magic number which is used to determine which device to send unicast ESPNOW data.
    uint8_t payload[0];                   //Real payload of ESPNOW data.
} __attribute__((packed)) libEspNow_data_t;

/* Parameters of sending ESPNOW data. */
typedef struct {
    bool unicast;                         //Send unicast ESPNOW data.
    bool broadcast;                       //Send broadcast ESPNOW data.
    uint8_t state;                        //Indicate that if has received broadcast ESPNOW data or not.
    uint32_t magic;                       //Magic number which is used to determine which device to send unicast ESPNOW data.
    uint16_t count;                       //Total count of unicast ESPNOW data to be sent.
    uint16_t delay;                       //Delay between sending two ESPNOW data, unit: ms.
    int len;                              //Length of ESPNOW data to be sent, unit: byte.
    uint8_t *buffer;                      //Buffer pointing to ESPNOW data.
    uint8_t dest_mac[ESP_NOW_ETH_ALEN];   //MAC address of destination device.
} libEspNow_send_param_t;


extern uint8_t s_libEspNow_server_mac[ESP_NOW_ETH_ALEN]; 



void libEspNow_wifi_init(void);
esp_err_t libEspNow_init(void);
void libEpsNow_send_data(uint8_t *dest_mac, uint8_t *data, uint16_t len);
void libEspNow_receive_cplt(libEspNow_messagetype_t type,uint8_t *src_mac, uint8_t *data, int data_len);
void libEspNow_send_cplt(uint8_t *dest_mac);


#endif

#ifdef __cplusplus
}
#endif
