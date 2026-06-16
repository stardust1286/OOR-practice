/*
 * ============================================================================
 * 文件：truck.h
 * 說明：卡車（Truck）具體實現 - 繼承自 Vehicle
 *
 * 設計亮點：
 * 1. Truck 額外欄位：cargo_weight（貨物重量）
 * 2. truck_move() 邏輯：根據貨物重量決定速度和油耗
 * 3. 演示「多型」的實用場景：不同貨車在城市系統中的不同行為
 * ============================================================================
 */

#ifndef __TRUCK_H__
#define __TRUCK_H__

#include "vehicle.h"

/* ============================================================================
 * 【Truck 專屬常數】
 * ============================================================================ */

#define MAX_CARGO_WEIGHT 10000.0f  /* kg */
#define TRUCK_TYPICAL_SPEED 50.0f  /* km/h */

/* ============================================================================
 * 【Truck 結構體】
 * ============================================================================ */

typedef struct {
    /* 繼承基類 */
    Vehicle base;
    
    /* Truck 特有欄位 */
    float cargo_weight;         /* 貨物重量 (kg) */
    float fuel_level;           /* 燃油量 (L) */
    int8_t cargo_type[32];      /* 貨物類型描述 */
    
} Truck;

/* ============================================================================
 * 【初始化巨集】
 * ============================================================================ */

#define TRUCK_DEFAULTS { \
    .base = { \
        0,                                  /* id */ \
        VEHICLE_TYPE_TRUCK,                 /* type */ \
        0.0f,                               /* current_speed */ \
        0.0f,                               /* position_x */ \
        0.0f,                               /* position_y */ \
        {0},                                /* location */ \
        0,                                  /* is_parked */ \
        (funtypeVehicle_GetType)truck_get_type,     /* get_type */ \
        (funtypeVehicle_Move)truck_move,            /* move */ \
        (funtypeVehicle_Stop)truck_stop,            /* stop */ \
        (funtypeVehicle_Park)truck_park,            /* park */ \
        (funtypeVehicle_PrintInfo)truck_print_info  /* print_info */ \
    }, \
    0.0f,           /* cargo_weight */ \
    100.0f,         /* fuel_level */ \
    {0}             /* cargo_type */ \
}

/* ============================================================================
 * 【虛擬方法宣告】
 * ============================================================================ */

VehicleType truck_get_type(Truck *self);
void truck_move(Truck *self, float distance);
void truck_stop(Truck *self);
void truck_park(Truck *self, int8_t *location);
void truck_print_info(Truck *self);

/* ============================================================================
 * 【Truck 特有方法】
 * ============================================================================ */

void truck_load_cargo(Truck *self, float weight, int8_t *cargo_type);
void truck_unload_cargo(Truck *self);
void truck_refuel(Truck *self, float amount);

#endif /* __TRUCK_H__ */
