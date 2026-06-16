/*
 * ============================================================================
 * 文件：vehicle.c
 * 說明：Vehicle 基礎實現
 * 
 * 設計原則：
 * 1. 這個檔案只實現「通用」的初始化邏輯
 * 2. 具體的 move(), park() 等虛擬方法由子類型（Car, Bus, Truck）實現
 * 3. 這個檔案的函式只在模組內部呼叫，外部透過函式指標間接呼叫
 * ============================================================================
 */

#include "../include/vehicle.h"
#include <stdio.h>
#include <string.h>

/* ============================================================================
 * 【基礎初始化】
 * ============================================================================ */

void vehicle_init(Vehicle *v, uint16_t id, VehicleType type)
{
    if (v == NULL) return;
    
    v->id = id;
    v->type = type;
    v->current_speed = 0.0f;
    v->position_x = 0.0f;
    v->position_y = 0.0f;
    memset(v->location, 0, sizeof(v->location));
    v->is_parked = 0;
    
    /* 虛擬方法指標暫時為 NULL，由子類型設定 */
    v->get_type = NULL;
    v->move = NULL;
    v->stop = NULL;
    v->park = NULL;
    v->print_info = NULL;
}

void vehicle_destroy(Vehicle *v)
{
    if (v == NULL) return;
    /* 基礎清理（若有動態配置的資源，在這裡釋放） */
    memset(v, 0, sizeof(Vehicle));
}
