/*
 * ============================================================================
 * 文件：signal_controller.c
 * 說明：紅綠燈控制器（Signal Traffic Light Controller）
 *
 * 設計示例：
 * 1. 實現 ITrafficController 介面
 * 2. 特定邏輯：根據車輛型別決定通行權
 * 3. 演示多型：同一個 ControlResult = control(vehicle) 呼叫
 *    但根據 vehicle->type 有不同結果
 * ============================================================================
 */

#include "../include/traffic_controller.h"
#include "../include/car.h"
#include "../include/bus.h"
#include "../include/truck.h"
#include <stdio.h>

/* ============================================================================
 * 【本地狀態 - Private 欄位】
 * ============================================================================ */

static uint32_t signal_cycle = 0;           /* 信號週期計數 */
static uint8_t current_light_state = 0;    /* 0=紅, 1=綠, 2=黃 */
static uint32_t total_passed = 0;          /* 累計通過車輛數 */

/* ============================================================================
 * 【核心控制邏輯】
 * ============================================================================ */

ControlResult signal_control_vehicle(void *self, Vehicle *vehicle)
{
    if (self == NULL || vehicle == NULL) {
        return CONTROL_RESULT_ERROR;
    }
    
    ITrafficController *controller = (ITrafficController *)self;
    
    /* 邏輯 1：檢查信號燈狀態 */
    if (current_light_state != 1) {  /* 非綠燈 */
        printf("[SignalController %d] RED/YELLOW light, Vehicle ID=%d blocked\n",
               controller->controller_id, vehicle->id);
        return CONTROL_RESULT_DENIED;
    }
    
    /* 邏輯 2：公車優先通行 */
    if (vehicle->type == VEHICLE_TYPE_BUS) {
        printf("[SignalController %d] BUS priority pass, Vehicle ID=%d\n",
               controller->controller_id, vehicle->id);
        total_passed++;
        return CONTROL_RESULT_SUCCESS;
    }
    
    /* 邏輯 3：卡車在非尖峰時間通行 */
    if (vehicle->type == VEHICLE_TYPE_TRUCK) {
        if (signal_cycle % 3 == 0) {  /* 模擬尖峰時間判定 */
            printf("[SignalController %d] TRUCK ID=%d delayed during peak hour\n",
                   controller->controller_id, vehicle->id);
            return CONTROL_RESULT_DENIED;
        }
        printf("[SignalController %d] TRUCK ID=%d passed\n",
               controller->controller_id, vehicle->id);
        total_passed++;
        return CONTROL_RESULT_SUCCESS;
    }
    
    /* 邏輯 4：轎車正常通行 */
    if (vehicle->type == VEHICLE_TYPE_CAR) {
        printf("[SignalController %d] CAR ID=%d passed\n",
               controller->controller_id, vehicle->id);
        total_passed++;
        return CONTROL_RESULT_SUCCESS;
    }
    
    return CONTROL_RESULT_ERROR;
}

int signal_get_status(void *self)
{
    if (self == NULL) return -1;
    
    signal_cycle++;
    
    /* 模擬信號燈週期：每 3 個週期循環一次 */
    switch (signal_cycle % 15) {
        case 0 ... 4:    current_light_state = 0; break;  /* 紅燈 5 秒 */
        case 5 ... 9:    current_light_state = 1; break;  /* 綠燈 5 秒 */
        case 10 ... 14:  current_light_state = 2; break;  /* 黃燈 5 秒 */
    }
    
    return current_light_state;
}

void signal_release(void *self, Vehicle *vehicle)
{
    if (self == NULL || vehicle == NULL) return;
    
    /* No resource to release for a signal light */
    printf("[SignalController] Vehicle ID=%d passed the signal point\n", vehicle->id);
}

void signal_print_info(void *self)
{
    if (self == NULL) return;
    
    ITrafficController *controller = (ITrafficController *)self;
    const char *light_name[] = {"RED", "GREEN", "YELLOW"};
    
    printf("\n========== Signal Controller ==========");
    printf("\nController ID: %d\n", controller->controller_id);
    printf("Type: Signal Traffic Light\n");
    printf("Current state: %s\n", light_name[current_light_state]);
    printf("Signal cycle: %d\n", signal_cycle);
    printf("Total vehicles passed: %d\n", total_passed);
    printf("=======================================\n\n");
}

/* ============================================================================
 * 【初始化函式】
 * ============================================================================ */

void signal_controller_init(ITrafficController *controller, uint16_t id)
{
    if (controller == NULL) return;
    
    controller->controller_id = id;
    controller->type = CONTROLLER_TYPE_SIGNAL;
    controller->control_vehicle = signal_control_vehicle;
    controller->get_status = signal_get_status;
    controller->release = signal_release;
    controller->print_info = signal_print_info;
    
    printf("[Init] Signal controller %d initialized\n", id);
}
