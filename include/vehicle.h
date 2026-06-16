/*
 * ============================================================================
 * 文件：vehicle.h
 * 說明：城市交通管理系統 - 基礎車輛物件定義
 *       
 * 設計理念：
 * 1. struct Vehicle 相當於 C++ 的基類（Base Class）
 * 2. 透過函式指標實現虛擬方法（Virtual Method）
 * 3. 用巨集定義初始化邏輯（模擬構造函數 Constructor）
 * 4. .h 檔定義介面（Public），.c 檔實現細節（Private）
 * ============================================================================
 */

#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include <stdint.h>

/* ============================================================================
 * 【常數定義】
 * ============================================================================ */

#define MAX_VEHICLE_ID 1000
#define MAX_SPEED 200.0f  /* km/h */

/* 車輛類型列舉 */
typedef enum {
    VEHICLE_TYPE_UNKNOWN = 0,
    VEHICLE_TYPE_CAR = 1,
    VEHICLE_TYPE_BUS = 2,
    VEHICLE_TYPE_TRUCK = 3
} VehicleType;

/* ============================================================================
 * 【函式指標型別定義】- 虛擬方法表（VTable）
 * ============================================================================ */

/* 獲取車輛類型 */
typedef VehicleType (*funtypeVehicle_GetType)(void *self);

/* 移動車輛 */
typedef void (*funtypeVehicle_Move)(void *self, float distance);

/* 停止車輛 */
typedef void (*funtypeVehicle_Stop)(void *self);

/* 停泊車輛 */
typedef void (*funtypeVehicle_Park)(void *self, int8_t *location);

/* 獲取車輛資訊（用於列印/日誌） */
typedef void (*funtypeVehicle_PrintInfo)(void *self);

/* ============================================================================
 * 【基礎車輛結構體】- 相當於 C++ 的 Base Class
 * ============================================================================ */

typedef struct Vehicle {
    /* ─────────────────────────────────────────────────
     * 公開欄位 (Public Fields)
     * ───────────────────────────────────────────────── */
    
    uint16_t id;                  /* 車輛 ID */
    VehicleType type;             /* 車輛類型 */
    float current_speed;          /* 目前速度 (km/h) */
    float position_x;             /* 位置 X 座標 */
    float position_y;             /* 位置 Y 座標 */
    int8_t location[32];          /* 停泊位置描述 */
    uint8_t is_parked;            /* 是否已停泊 (0=行駛中, 1=已停泊) */

    /* ─────────────────────────────────────────────────
     * 虛擬方法指標 (Virtual Methods) - 相當於 C++ 的 vptr
     * ───────────────────────────────────────────────── */
    
    funtypeVehicle_GetType    get_type;     /* 取得車型 */
    funtypeVehicle_Move       move;         /* 移動 */
    funtypeVehicle_Stop       stop;         /* 停止 */
    funtypeVehicle_Park       park;         /* 停泊 */
    funtypeVehicle_PrintInfo  print_info;   /* 列印資訊 */
    
} Vehicle;

/* ============================================================================
 * 【初始化巨集】- 相當於 Constructor
 * ============================================================================ */

/*
 * 基礎初始化巨集（由子類型延伸使用）
 * 用法：Vehicle my_vehicle = VEHICLE_INIT_BASE(CAR_IMPLEMENTATION);
 */
#define VEHICLE_INIT_BASE(type_enum) { \
    0,                              /* id */ \
    (type_enum),                    /* type */ \
    0.0f,                           /* current_speed */ \
    0.0f,                           /* position_x */ \
    0.0f,                           /* position_y */ \
    {0},                            /* location */ \
    0,                              /* is_parked */ \
    NULL, NULL, NULL, NULL, NULL    /* 函式指標，由子類賦值 */ \
}

/* ============================================================================
 * 【函式宣告】
 * ============================================================================ */

/*
 * vehicle_init() - 通用初始化函式
 * 說明：初始化基礎欄位（由子類型呼叫）
 */
void vehicle_init(Vehicle *v, uint16_t id, VehicleType type);

/*
 * vehicle_destroy() - 清理資源
 */
void vehicle_destroy(Vehicle *v);

#endif /* __VEHICLE_H__ */
