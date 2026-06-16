/*
 * ============================================================================
 * 文件：parking_controller.c
 * 說明：停車場管理控制器（Parking Controller）
 *
 * 設計特色：
 * 1. 實現 ITrafficController 介面
 * 2. 管理停泊位數量和車輛停泊
 * 3. 展示不同控制器邏輯：根據車型決定停泊費用和時間限制
 * ============================================================================
 */

#include "../include/traffic_controller.h"
#include "../include/car.h"
#include "../include/bus.h"
#include "../include/truck.h"
#include <stdio.h>
#include <string.h>

/* ============================================================================
 * 【本地狀態 - Private 欄位】
 * ============================================================================ */

#define MAX_PARKING_SPACES 50

static uint8_t parking_spaces[MAX_PARKING_SPACES];    /* 0=空, 1=佔用 */
static uint16_t parked_vehicle_ids[MAX_PARKING_SPACES];
static float parking_revenue = 0.0f;                  /* 停泊收入 */

/* ============================================================================
 * 【停泊費用計算】
 * ============================================================================ */

static float calculate_parking_fee(Vehicle *vehicle)
{
    if (vehicle == NULL) return 0.0f;
    
    /* 根據車型決定費用：小型車便宜，大型車貴 */
    switch (vehicle->type) {
        case VEHICLE_TYPE_CAR:
            return 5.0f;      /* 轎車：5 元/小時 */
        case VEHICLE_TYPE_BUS:
            return 15.0f;     /* 公車：15 元/小時（但通常免費） */
        case VEHICLE_TYPE_TRUCK:
            return 20.0f;     /* 卡車：20 元/小時 */
        default:
            return 0.0f;
    }
}

/* ============================================================================
 * 【核心控制邏輯】
 * ============================================================================ */

ControlResult parking_control_vehicle(void *self, Vehicle *vehicle)
{
    if (self == NULL || vehicle == NULL) {
        return CONTROL_RESULT_ERROR;
    }
    
    ITrafficController *controller = (ITrafficController *)self;
    
    /* 邏輯 1：尋找空停泊位 */
    int free_space = -1;
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parking_spaces[i] == 0) {
            free_space = i;
            break;
        }
    }
    
    if (free_space == -1) {
        printf("[ParkingController %d] FULL - Vehicle ID=%d denied\n",
               controller->controller_id, vehicle->id);
        return CONTROL_RESULT_DENIED;
    }
    
    /* 邏輯 2：公車通常免費停泊 */
    float fee = 0.0f;
    if (vehicle->type != VEHICLE_TYPE_BUS) {
        fee = calculate_parking_fee(vehicle);
        parking_revenue += fee;
    }
    
    /* 邏輯 3：配置停泊位 */
    parking_spaces[free_space] = 1;
    parked_vehicle_ids[free_space] = vehicle->id;
    
    printf("[ParkingController %d] OK - Vehicle ID=%d parked at space %d, Fee: %.2f\n",
           controller->controller_id, vehicle->id, free_space, fee);
    
    return CONTROL_RESULT_SUCCESS;
}

int parking_get_status(void *self)
{
    if (self == NULL) return -1;
    
    int available = 0;
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parking_spaces[i] == 0) available++;
    }
    
    return available;
}

void parking_release(void *self, Vehicle *vehicle)
{
    if (self == NULL || vehicle == NULL) return;
    
    /* 尋找並釋放停泊位 */
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parking_spaces[i] == 1 && parked_vehicle_ids[i] == vehicle->id) {
            parking_spaces[i] = 0;
            parked_vehicle_ids[i] = 0;
            printf("[ParkingController] Vehicle ID=%d left the lot (space %d released)\n",
                   vehicle->id, i);
            return;
        }
    }
}

void parking_print_info(void *self)
{
    if (self == NULL) return;
    
    ITrafficController *controller = (ITrafficController *)self;
    
    int available = 0, occupied = 0;
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parking_spaces[i] == 0) available++;
        else occupied++;
    }
    
    printf("\n========== Parking Controller ==========");
    printf("\nController ID: %d\n", controller->controller_id);
    printf("Type: Parking Management\n");
    printf("Total spaces: %d\n", MAX_PARKING_SPACES);
    printf("Occupied: %d\n", occupied);
    printf("Available: %d\n", available);
    printf("Total revenue: %.2f\n", parking_revenue);
    printf("=========================================\n\n");
}

/* ============================================================================
 * 【初始化函式】
 * ============================================================================ */

void parking_controller_init(ITrafficController *controller, uint16_t id)
{
    if (controller == NULL) return;
    
    /* 初始化停泊位 */
    memset(parking_spaces, 0, sizeof(parking_spaces));
    memset(parked_vehicle_ids, 0, sizeof(parked_vehicle_ids));
    parking_revenue = 0.0f;
    
    controller->controller_id = id;
    controller->type = CONTROLLER_TYPE_PARKING;
    controller->control_vehicle = parking_control_vehicle;
    controller->get_status = parking_get_status;
    controller->release = parking_release;
    controller->print_info = parking_print_info;
    
    printf("[Init] Parking controller %d initialized (%d spaces)\n", id, MAX_PARKING_SPACES);
}
