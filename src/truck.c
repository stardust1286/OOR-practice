/*
 * ============================================================================
 * 文件：truck.c
 * 說明：Truck（卡車）具體實現
 *
 * 多型示例：
 * 1. truck_move() 與 car_move(), bus_move() 邏輯完全不同
 * 2. 卡車要考慮貨物重量影響速度和油耗
 * 3. 這展示了 OOP 多型的威力：同一介面，三種不同行為
 * ============================================================================
 */

#include "../include/truck.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ============================================================================
 * 【虛擬方法實現】
 * ============================================================================ */

VehicleType truck_get_type(Truck *self)
{
    if (self == NULL) return VEHICLE_TYPE_UNKNOWN;
    return self->base.type;
}

void truck_move(Truck *self, float distance)
{
    if (self == NULL) return;
    if (self->base.is_parked) {
        printf("[Truck %d] Warning: Truck is parked, cannot move\n", self->base.id);
        return;
    }
    
    /* 卡車邏輯：貨物越重，速度越慢，油耗越大 */
    float weight_factor = 1.0f + (self->cargo_weight / MAX_CARGO_WEIGHT) * 0.5f;
    float speed_reduction = (self->cargo_weight / MAX_CARGO_WEIGHT) * 0.6f;
    
    self->base.current_speed = TRUCK_TYPICAL_SPEED * (1.0f - speed_reduction);
    if (self->base.current_speed < 20.0f) self->base.current_speed = 20.0f;
    
    /* 油耗 = 基礎油耗 * 重量係數 * 距離 */
    float fuel_consumption = distance * 0.15f * weight_factor;
    
    if (self->fuel_level < fuel_consumption) {
        printf("[Truck %d] Warning: Insufficient fuel, cannot drive\n", self->base.id);
        return;
    }
    
    self->fuel_level -= fuel_consumption;
    self->base.position_x += distance * 0.6f;
    self->base.position_y += distance * 0.4f;
    
    printf("[Truck %d] Moved %.2f km (Cargo: %.2f kg), Position: (%.2f, %.2f), "
           "Speed: %.2f km/h, Fuel: %.2f L\n",
           self->base.id, distance, self->cargo_weight,
           self->base.position_x, self->base.position_y,
           self->base.current_speed, self->fuel_level);
}

void truck_stop(Truck *self)
{
    if (self == NULL) return;
    self->base.current_speed = 0.0f;
    printf("[Truck %d] Stopped (not parked)\n", self->base.id);
}

void truck_park(Truck *self, int8_t *location)
{
    if (self == NULL || location == NULL) return;
    
    self->base.is_parked = 1;
    self->base.current_speed = 0.0f;
    strncpy(self->base.location, (char *)location, 31);
    self->base.location[31] = '\0';
    
    printf("[Truck %d] Parked at: %s\n", self->base.id, self->base.location);
}

void truck_print_info(Truck *self)
{
    if (self == NULL) return;
    
    printf("\n========== Truck Information ==========");
    printf("\nID: %d\n", self->base.id);
    printf("Type: Truck\n");
    printf("Position: (%.2f, %.2f)\n", self->base.position_x, self->base.position_y);
    printf("Current speed: %.2f km/h\n", self->base.current_speed);
    printf("Cargo: %.2f / %.2f kg\n", self->cargo_weight, MAX_CARGO_WEIGHT);
    printf("Cargo type: %s\n", self->cargo_type[0] ? (char *)self->cargo_type : "Empty");
    printf("Fuel: %.2f L\n", self->fuel_level);
    printf("Status: %s", self->base.is_parked ? "Parked" : "Driving");
    if (self->base.is_parked) {
        printf(" (Location: %s)", self->base.location);
    }
    printf("\n=======================================\n\n");
}

/* ============================================================================
 * 【Truck 特有方法實現】
 * ============================================================================ */

void truck_load_cargo(Truck *self, float weight, int8_t *cargo_type)
{
    if (self == NULL) return;
    
    if (self->cargo_weight + weight > MAX_CARGO_WEIGHT) {
        printf("[Truck %d] Warning: Overweight! Can only load %.2f kg more\n",
               self->base.id, MAX_CARGO_WEIGHT - self->cargo_weight);
        return;
    }
    
    self->cargo_weight += weight;
    if (cargo_type != NULL) {
        strncpy(self->cargo_type, cargo_type, 31);
        self->cargo_type[31] = '\0';
    }
    
    printf("[Truck %d] Loaded %.2f kg, Total cargo: %.2f kg\n",
           self->base.id, weight, self->cargo_weight);
}

void truck_unload_cargo(Truck *self)
{
    if (self == NULL) return;
    
    printf("[Truck %d] Unloaded %.2f kg of cargo\n", self->base.id, self->cargo_weight);
    self->cargo_weight = 0.0f;
    memset(self->cargo_type, 0, sizeof(self->cargo_type));
}

void truck_refuel(Truck *self, float amount)
{
    if (self == NULL) return;
    
    float available = 200.0f - self->fuel_level;  /* 假設油箱容量 200L */
    float to_refuel = (amount > available) ? available : amount;
    
    self->fuel_level += to_refuel;
    printf("[Truck %d] Refueled %.2f L, Current fuel: %.2f L\n",
           self->base.id, to_refuel, self->fuel_level);
}
