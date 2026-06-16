/*
 * ============================================================================
 * 文件：bus.h
 * 說明：公車（Bus）具體實現 - 繼承自 Vehicle
 *
 * 設計特色：
 * 1. Bus 有自己的專屬欄位：passenger_count, route_number, max_capacity
 * 2. Bus 的 move() 邏輯不同於 Car：加速慢、停靠站
 * 3. 演示「多型」概念：同樣呼叫 move()，不同物件有不同行為
 * ============================================================================
 */

#ifndef __BUS_H__
#define __BUS_H__

#include "vehicle.h"

/* ============================================================================
 * 【Bus 專屬常數】
 * ============================================================================ */

#define MAX_BUS_CAPACITY 50
#define BUS_TYPICAL_SPEED 40.0f  /* 公車巡航速度 40 km/h */

/* ============================================================================
 * 【Bus 結構體】
 * ============================================================================ */

typedef struct {
    /* 繼承基類 */
    Vehicle base;
    
    /* Bus 特有欄位 */
    uint16_t passenger_count;   /* 目前乘客數 */
    uint8_t route_number;       /* 路線號碼 */
    uint16_t total_passengers;  /* 今日累計乘客數 */
    
} Bus;

/* ============================================================================
 * 【初始化巨集】
 * ============================================================================ */

#define BUS_DEFAULTS { \
    .base = { \
        0,                                  /* id */ \
        VEHICLE_TYPE_BUS,                   /* type */ \
        0.0f,                               /* current_speed */ \
        0.0f,                               /* position_x */ \
        0.0f,                               /* position_y */ \
        {0},                                /* location */ \
        0,                                  /* is_parked */ \
        (funtypeVehicle_GetType)bus_get_type,     /* get_type */ \
        (funtypeVehicle_Move)bus_move,            /* move */ \
        (funtypeVehicle_Stop)bus_stop,            /* stop */ \
        (funtypeVehicle_Park)bus_park,            /* park */ \
        (funtypeVehicle_PrintInfo)bus_print_info  /* print_info */ \
    }, \
    0,              /* passenger_count */ \
    1,              /* route_number */ \
    0               /* total_passengers */ \
}

/* ============================================================================
 * 【虛擬方法宣告】
 * ============================================================================ */

VehicleType bus_get_type(Bus *self);
void bus_move(Bus *self, float distance);
void bus_stop(Bus *self);
void bus_park(Bus *self, int8_t *location);
void bus_print_info(Bus *self);

/* ============================================================================
 * 【Bus 特有方法】
 * ============================================================================ */

void bus_board_passenger(Bus *self, uint16_t count);
void bus_alight_passenger(Bus *self, uint16_t count);
void bus_set_route(Bus *self, uint8_t route_number);

#endif /* __BUS_H__ */
