/*
 * ============================================================================
 * 文件：bus.c
 * 說明：Bus（公車）具體實現
 *
 * 特色說明：
 * 1. bus_move() 邏輯完全不同於 car_move()
 * 2. 公車不消耗燃油（用電），只關心乘客數量
 * 3. 展示了多型：同樣的 vehicle->move() 呼叫，不同物件執行不同邏輯
 * ============================================================================
 */

#include "../include/bus.h"
#include <stdio.h>
#include <string.h>

/* ============================================================================
 * 【虛擬方法實現】
 * ============================================================================ */

VehicleType bus_get_type(Bus *self)
{
    if (self == NULL) return VEHICLE_TYPE_UNKNOWN;
    return self->base.type;
}

void bus_move(Bus *self, float distance)
{
    if (self == NULL) return;
    if (self->base.is_parked) {
        printf("[Bus %d] Warning: Bus is docked, cannot move\n", self->base.id);
        return;
    }
    
    /* 公車加速邏輯：重量大，加速慢，但公開路線相對規律 */
    float speed_factor = 1.0f - (self->passenger_count * 0.01f);  /* 乘客多速度慢 */
    if (speed_factor < 0.3f) speed_factor = 0.3f;
    
    self->base.position_x += distance * 0.5f * speed_factor;
    self->base.position_y += distance * 0.5f * speed_factor;
    self->base.current_speed = BUS_TYPICAL_SPEED * speed_factor;
    
    printf("[Bus %d (Route %d)] Traveling %.2f km, Position: (%.2f, %.2f), Speed: %.2f km/h, Passengers: %d\n",
           self->base.id, self->route_number, distance,
           self->base.position_x, self->base.position_y,
           self->base.current_speed, self->passenger_count);
}

void bus_stop(Bus *self)
{
    if (self == NULL) return;
    self->base.current_speed = 0.0f;
    printf("[Bus %d] Stopped (not docked)\n", self->base.id);
}

void bus_park(Bus *self, int8_t *location)
{
    if (self == NULL || location == NULL) return;
    
    self->base.is_parked = 1;
    self->base.current_speed = 0.0f;
    strncpy(self->base.location, (char *)location, 31);
    self->base.location[31] = '\0';
    
    printf("[Bus %d] Docked at: %s\n", self->base.id, self->base.location);
}

void bus_print_info(Bus *self)
{
    if (self == NULL) return;
    
    printf("\n========== Bus Information ==========");
    printf("\nID: %d\n", self->base.id);
    printf("Type: Bus\n");
    printf("Route: %d\n", self->route_number);
    printf("Position: (%.2f, %.2f)\n", self->base.position_x, self->base.position_y);
    printf("Current speed: %.2f km/h\n", self->base.current_speed);
    printf("Passengers: %d / %d\n", self->passenger_count, MAX_BUS_CAPACITY);
    printf("Total passengers today: %d\n", self->total_passengers);
    printf("Status: %s", self->base.is_parked ? "Docked" : "Traveling");
    if (self->base.is_parked) {
        printf(" (Location: %s)", self->base.location);
    }
    printf("\n======================================\n\n");
}

/* ============================================================================
 * 【Bus 特有方法實現】
 * ============================================================================ */

void bus_board_passenger(Bus *self, uint16_t count)
{
    if (self == NULL) return;
    
    uint16_t available = MAX_BUS_CAPACITY - self->passenger_count;
    uint16_t to_board = (count > available) ? available : count;
    
    self->passenger_count += to_board;
    self->total_passengers += to_board;
    
    printf("[Bus %d] %d passengers boarded, Total: %d / %d\n",
           self->base.id, to_board, self->passenger_count, MAX_BUS_CAPACITY);
}

void bus_alight_passenger(Bus *self, uint16_t count)
{
    if (self == NULL) return;
    
    uint16_t to_alight = (count > self->passenger_count) ? self->passenger_count : count;
    self->passenger_count -= to_alight;
    
    printf("[Bus %d] %d passengers alighted, Total: %d / %d\n",
           self->base.id, to_alight, self->passenger_count, MAX_BUS_CAPACITY);
}

void bus_set_route(Bus *self, uint8_t route_number)
{
    if (self == NULL) return;
    self->route_number = route_number;
    printf("[Bus %d] Route changed to: %d\n", self->base.id, route_number);
}
