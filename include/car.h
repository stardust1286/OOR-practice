/*
 * ============================================================================
 * 文件：car.h
 * 說明：轎車（Car）具體實現 - 繼承自 Vehicle
 *
 * 設計理念：
 * 1. Car struct 內嵌 Vehicle struct（即「包含」基類）
 * 2. Car 特有的欄位：passenger_count, fuel_level
 * 3. Car 實現自己的虛擬方法：car_move(), car_park() 等
 * 4. 透過巨集 CAR_DEFAULTS 進行「初始化」
 * ============================================================================
 */

#ifndef __CAR_H__
#define __CAR_H__

#include "vehicle.h"

/* ============================================================================
 * 【Car 專屬常數】
 * ============================================================================ */

#define MAX_PASSENGERS 5
#define MAX_FUEL 60.0f  /* 公升 */

/* ============================================================================
 * 【Car 結構體】- 相當於 C++ 的派生類
 * ============================================================================ */

typedef struct {
    /* ─────────────────────────────────────────────────
     * 繼承欄位 (Inherited from Vehicle)
     * ───────────────────────────────────────────────── */
    
    Vehicle base;  /* 包含基類，實現「繼承」 */
    
    /* ─────────────────────────────────────────────────
     * Car 特有欄位 (Car-Specific Fields)
     * ───────────────────────────────────────────────── */
    
    uint8_t passenger_count;      /* 目前乘客數 */
    float fuel_level;             /* 燃油量 (L) */
    uint8_t has_ac;               /* 是否有冷氣 (0/1) */
    
} Car;

/* ============================================================================
 * 【初始化巨集】- Car 的構造函數
 * ============================================================================ */

/*
 * CAR_DEFAULTS 
 * 說明：用巨集定義 Car 的預設初始化
 * 用法：Car my_car = CAR_DEFAULTS;
 *       my_car.base.id = 101;
 */
#define CAR_DEFAULTS { \
    .base = { \
        0,                                  /* id */ \
        VEHICLE_TYPE_CAR,                   /* type */ \
        0.0f,                               /* current_speed */ \
        0.0f,                               /* position_x */ \
        0.0f,                               /* position_y */ \
        {0},                                /* location */ \
        0,                                  /* is_parked */ \
        (funtypeVehicle_GetType)car_get_type,     /* get_type */ \
        (funtypeVehicle_Move)car_move,            /* move */ \
        (funtypeVehicle_Stop)car_stop,            /* stop */ \
        (funtypeVehicle_Park)car_park,            /* park */ \
        (funtypeVehicle_PrintInfo)car_print_info  /* print_info */ \
    }, \
    0,          /* passenger_count */ \
    MAX_FUEL,   /* fuel_level - 滿油 */ \
    1           /* has_ac - 有冷氣 */ \
}

/* ============================================================================
 * 【Car 的虛擬方法實現】
 * ============================================================================ */

VehicleType car_get_type(Car *self);
void car_move(Car *self, float distance);
void car_stop(Car *self);
void car_park(Car *self, int8_t *location);
void car_print_info(Car *self);

/* ============================================================================
 * 【Car 特有方法】
 * ============================================================================ */

void car_add_passenger(Car *self, uint8_t count);
void car_remove_passenger(Car *self, uint8_t count);
void car_refuel(Car *self, float amount);

#endif /* __CAR_H__ */
