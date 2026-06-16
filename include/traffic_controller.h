/*
 * ============================================================================
 * 文件：traffic_controller.h
 * 說明：城市交通控制器介面（ITrafficController）
 *
 * 設計理念：
 * 1. 這個檔案定義「介面規格」，類似 C++ 的 abstract interface
 * 2. 定義所有控制器都必須實現的函式指標型別
 * 3. 實現「多型」的關鍵：不同控制器共享同一個介面
 * 4. 上層呼叫者（main.c）不需要知道具體是哪個控制器，只需呼叫介面函式
 * ============================================================================
 */

#ifndef __TRAFFIC_CONTROLLER_H__
#define __TRAFFIC_CONTROLLER_H__

#include "vehicle.h"

/* ============================================================================
 * 【控制器類型列舉】
 * ============================================================================ */

typedef enum {
    CONTROLLER_TYPE_UNKNOWN = 0,
    CONTROLLER_TYPE_SIGNAL = 1,      /* 紅綠燈控制器 */
    CONTROLLER_TYPE_PARKING = 2,     /* 停車場控制器 */
    CONTROLLER_TYPE_HIGHWAY = 3      /* 高速公路控制器 */
} ControllerType;

/* ============================================================================
 * 【控制結果列舉】
 * ============================================================================ */

typedef enum {
    CONTROL_RESULT_SUCCESS = 0,
    CONTROL_RESULT_DENIED = 1,       /* 不允許通行/停泊 */
    CONTROL_RESULT_ERROR = 2         /* 錯誤 */
} ControlResult;

/* ============================================================================
 * 【介面方法型別 - 虛擬方法表 (VTable)】
 * ============================================================================ */

/* 控制車輛 - 核心方法 */
typedef ControlResult (*funtypeController_ControlVehicle)(void *self, Vehicle *vehicle);

/* 查詢車道/停泊位狀態 */
typedef int (*funtypeController_GetStatus)(void *self);

/* 釋放資源（停泊位等） */
typedef void (*funtypeController_Release)(void *self, Vehicle *vehicle);

/* 列印控制器資訊 */
typedef void (*funtypeController_PrintInfo)(void *self);

/* ============================================================================
 * 【ITrafficController 介面結構體】
 * ============================================================================ */

typedef struct {
    /* 識別資訊 */
    uint16_t controller_id;
    ControllerType type;
    
    /* 虛擬方法（函式指標） */
    funtypeController_ControlVehicle  control_vehicle;
    funtypeController_GetStatus       get_status;
    funtypeController_Release         release;
    funtypeController_PrintInfo       print_info;
    
} ITrafficController;

#endif /* __TRAFFIC_CONTROLLER_H__ */
