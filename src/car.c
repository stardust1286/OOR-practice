/*
 * ============================================================================
 * 文件：car.c
 * 說明：Car（轎車）具體實現
 *
 * 實現邏輯：
 * 1. 虛擬方法：car_move() 是轎車特有的移動邏輯（加速快、省油）
 * 2. 每個方法都檢查 self 指標（同 C++ 的 this）
 * 3. 修改 Car 的私有欄位，例如 fuel_level 消耗
 * ============================================================================
 */

#include "../include/car.h"
#include <stdio.h>
#include <string.h>

/* ============================================================================
 * 【虛擬方法實現】
 * ============================================================================ */

VehicleType car_get_type(Car *self)
{
    if (self == NULL) return VEHICLE_TYPE_UNKNOWN;
    return self->base.type;
}

void car_move(Car *self, float distance)
{
    if (self == NULL) return;
    if (self->base.is_parked) {
        printf("[Car %d] Warning: Vehicle is parked, cannot move\n", self->base.id);
        return;
    }
    
    /* 轎車加速：每 km 消耗 0.08L 燃油 */
    float fuel_consumption = distance * 0.08f;
    
    if (self->fuel_level < fuel_consumption) {
        printf("[Car %d] Warning: Insufficient fuel, cannot drive\n", self->base.id);
        return;
    }
    
    self->fuel_level -= fuel_consumption;
    self->base.position_x += distance * 0.7f;  /* 轎車加速係數 0.7 */
    self->base.position_y += distance * 0.3f;
    self->base.current_speed = 60.0f;  /* 轎車典型巡航速度 60 km/h */
    
    printf("[Car %d] Moved %.2f km, Position: (%.2f, %.2f), Remaining fuel: %.2f L\n",
           self->base.id, distance, 
           self->base.position_x, self->base.position_y,
           self->fuel_level);
}

void car_stop(Car *self)
{
    if (self == NULL) return;
    self->base.current_speed = 0.0f;
    printf("[Car %d] Stopped (not parked)\n", self->base.id);
}

void car_park(Car *self, int8_t *location)
{
    if (self == NULL || location == NULL) return;
    
    self->base.is_parked = 1;
    self->base.current_speed = 0.0f;
    strncpy(self->base.location, (char *)location, 31);
    self->base.location[31] = '\0';
    
    printf("[Car %d] Parked at: %s\n", self->base.id, self->base.location);
}

void car_print_info(Car *self)
{
    if (self == NULL) return;
    
    printf("\n========== Car Information ==========");
    printf("\nID: %d\n", self->base.id);
    printf("Type: Car\n");
    printf("Position: (%.2f, %.2f)\n", self->base.position_x, self->base.position_y);
    printf("Current speed: %.2f km/h\n", self->base.current_speed);
    printf("Passengers: %d / %d\n", self->passenger_count, MAX_PASSENGERS);
    printf("Fuel: %.2f / %.2f L\n", self->fuel_level, MAX_FUEL);
    printf("AC: %s\n", self->has_ac ? "Yes" : "No");
    printf("Status: %s", self->base.is_parked ? "Parked" : "Driving");
    if (self->base.is_parked) {
        printf(" (Location: %s)", self->base.location);
    }
    printf("\n======================================\n\n");
}

/* ============================================================================
 * 【Car 特有方法實現】
 * ============================================================================ */

void car_add_passenger(Car *self, uint8_t count)
{
    if (self == NULL) return;
    
    uint8_t available = MAX_PASSENGERS - self->passenger_count;
    uint8_t to_add = (count > available) ? available : count;
    
    self->passenger_count += to_add;
    printf("[Car %d] %d passengers boarded, Total: %d\n", 
           self->base.id, to_add, self->passenger_count);
}

void car_remove_passenger(Car *self, uint8_t count)
{
    if (self == NULL) return;
    
    uint8_t to_remove = (count > self->passenger_count) ? self->passenger_count : count;
    self->passenger_count -= to_remove;
    printf("[Car %d] %d passengers alighted, Total: %d\n",
           self->base.id, to_remove, self->passenger_count);
}

void car_refuel(Car *self, float amount)
{
    if (self == NULL) return;
    
    float available = MAX_FUEL - self->fuel_level;
    float to_refuel = (amount > available) ? available : amount;
    
    self->fuel_level += to_refuel;
    printf("[Car %d] Refueled %.2f L, Current fuel: %.2f / %.2f L\n",
           self->base.id, to_refuel, self->fuel_level, MAX_FUEL);
}
