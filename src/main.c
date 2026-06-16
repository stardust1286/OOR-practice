/*
 * ============================================================================
 * 文件：main.c
 * 說明：城市交通管理系統 - 完整示例
 *
 * 程式目的：
 * 1. 展示 C 語言 OOP 的完整應用
 * 2. 演示多型概念：同一個函式呼叫，不同物件有不同行為
 * 3. 展示介面分離設計：上層不需知道具體實現細節
 * 
 * 執行流程：
 * 1. 建立 3 個不同類型的車輛（Car, Bus, Truck）
 * 2. 建立 2 個不同的控制器（Signal, Parking）
 * 3. 模擬多型呼叫：透過相同的方法簽名，執行不同的邏輯
 * 4. 觀察資料流和狀態變化
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/vehicle.h"
#include "../include/car.h"
#include "../include/bus.h"
#include "../include/truck.h"
#include "../include/traffic_controller.h"

/* ============================================================================
 * 【外部函式宣告】- 來自其他 .c 檔
 * ============================================================================ */

/* Parking Controller */
extern void parking_controller_init(ITrafficController *controller, uint16_t id);
extern void parking_release(void *self, Vehicle *vehicle);

/* Signal Controller */
extern void signal_controller_init(ITrafficController *controller, uint16_t id);

/* ============================================================================
 * 【主程式】
 * ============================================================================ */

int main(void)
{
    printf("\n");
    printf("+============================================================+\n");
    printf("|  C Language OOP - City Traffic Management System Demo      |\n");
    printf("|  Demonstrating: Encapsulation, Inheritance, Polymorphism  |\n");
    printf("+============================================================+\n\n");

    /* ================================================================
     * [Stage 1] Create Vehicle Objects - Demonstrate Encapsulation
     * ================================================================ */
    
    printf("[Stage 1] Create Vehicle Objects\n");
    printf("----------------------------------\n\n");
    
    /* Create Car */
    Car car1 = CAR_DEFAULTS;
    car1.base.id = 101;
    car1.passenger_count = 1;
    printf("  + Created Car car1 (ID=101)\n");
    
    /* Create Bus */
    Bus bus1 = BUS_DEFAULTS;
    bus1.base.id = 201;
    bus1.route_number = 15;
    printf("  + Created Bus bus1 (ID=201, Route=15)\n");
    
    /* Create Truck */
    Truck truck1 = TRUCK_DEFAULTS;
    truck1.base.id = 301;
    printf("  + Created Truck truck1 (ID=301)\n\n");
    
    /* ================================================================
     * [Stage 2] Create Controllers - Demonstrate Interface
     * ================================================================ */
    
    printf("[Stage 2] Create Traffic Controllers\n");
    printf("--------------------------------------\n\n");
    
    ITrafficController signal_ctrl;
    signal_controller_init(&signal_ctrl, 1001);
    
    ITrafficController parking_ctrl;
    parking_controller_init(&parking_ctrl, 2001);
    printf("\n");
    
    /* ================================================================
     * [Stage 3] Polymorphism Demo - Same call, different behavior
     * ================================================================ */
    
    printf("[Stage 3] Polymorphism Demo - Pass through signal light\n");
    printf("--------------------------------------------------------\n\n");
    
    /* Query signal light state */
    int light_state = signal_ctrl.get_status(&signal_ctrl);
    (void)light_state;
    
    /* Three different vehicle types pass through the signal */
    ControlResult result;
    result = signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&car1);
    result = signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&bus1);
    result = signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&truck1);
    (void)result;
    printf("\n");
    
    /* ================================================================
     * [Stage 4] Virtual Method Demo - Different move() logic
     * ================================================================ */
    
    printf("[Stage 4] Virtual Method Demo - Different move() per vehicle\n");
    printf("--------------------------------------------------------------\n\n");
    
    /* Core of Polymorphism:
     * Same function pointer call, three objects execute different logic!
     */
    
    printf(">> Car move\n");
    car1.base.move(&car1, 10.5f);
    
    printf("\n>> Bus move\n");
    bus1.base.move(&bus1, 8.0f);
    
    printf("\n>> Truck move (empty)\n");
    truck1.base.move(&truck1, 15.0f);
    
    /* Load cargo */
    printf("\n>> Truck load cargo\n");
    truck1.base.move(&truck1, 5.0f);
    
    truck_load_cargo(&truck1, 5000.0f, (int8_t *)"Construction Material");
    printf("\n>> Truck move (heavy load) - observe speed and fuel change\n");
    truck1.base.move(&truck1, 10.0f);
    
    printf("\n");
    
    /* ================================================================
     * [Stage 5] Parking Management - State management demo
     * ================================================================ */
    
    printf("[Stage 5] Parking Management\n");
    printf("------------------------------\n\n");
    
    printf(">> Park all three vehicles\n");
    parking_ctrl.control_vehicle(&parking_ctrl, (Vehicle *)&car1);
    car1.base.park(&car1, (int8_t *)"Zone-A Spot-01");
    
    printf("\n");
    parking_ctrl.control_vehicle(&parking_ctrl, (Vehicle *)&bus1);
    bus1.base.park(&bus1, (int8_t *)"Bus Bay 01");
    
    printf("\n");
    parking_ctrl.control_vehicle(&parking_ctrl, (Vehicle *)&truck1);
    truck1.base.park(&truck1, (int8_t *)"Truck Zone 01");
    
    printf("\n");
    
    /* ================================================================
     * [Stage 6] Print Info - Show each object state
     * ================================================================ */
    
    printf("[Stage 6] Object Information Overview\n");
    printf("--------------------------------------\n\n");
    
    printf(">> Vehicle Info\n");
    car1.base.print_info(&car1);
    bus1.base.print_info(&bus1);
    truck1.base.print_info(&truck1);
    
    printf(">> Controller Status\n");
    signal_ctrl.print_info(&signal_ctrl);
    parking_ctrl.print_info(&parking_ctrl);
    
    /* ================================================================
     * [Stage 7] Type-Specific Operations
     * ================================================================ */
    
    printf("[Stage 7] Type-Specific Operations\n");
    printf("------------------------------------\n\n");
    
    printf(">> Car operations\n");
    car_add_passenger(&car1, 3);
    car_refuel(&car1, 20.0f);
    
    printf("\n>> Bus operations\n");
    bus_board_passenger(&bus1, 30);
    bus_board_passenger(&bus1, 20);  /* auto-capped at max capacity */
    bus_set_route(&bus1, 25);
    
    printf("\n>> Truck operations\n");
    truck_refuel(&truck1, 50.0f);
    truck_unload_cargo(&truck1);
    
    printf("\n");
    
    /* ================================================================
     * [Stage 8] Leave Parking Lot
     * ================================================================ */
    
    printf("[Stage 8] Vehicles Leave Parking Lot\n");
    printf("--------------------------------------\n\n");
    
    parking_ctrl.release(&parking_ctrl, (Vehicle *)&car1);
    parking_ctrl.release(&parking_ctrl, (Vehicle *)&bus1);
    parking_ctrl.release(&parking_ctrl, (Vehicle *)&truck1);
    
    printf("\n");
    
    /* ================================================================
     * [Final] Statistics
     * ================================================================ */
    
    printf("[Final] Parking Lot Status\n");
    printf("---------------------------\n\n");
    
    parking_ctrl.print_info(&parking_ctrl);
    
    printf("\n+============================================================+\n");
    printf("|                   Program Complete                         |\n");
    printf("|  This demo shows C OOP core concepts:                      |\n");
    printf("|  + Encapsulation  - struct bundles data and methods        |\n");
    printf("|  + Inheritance    - Vehicle base embedded in Car/Bus/Truck |\n");
    printf("|  + Polymorphism   - same call, different object behavior   |\n");
    printf("|  + Interface      - ITrafficController specification       |\n");
    printf("|  + State Mgmt     - each object manages its own state      |\n");
    printf("+============================================================+\n\n");
    
    return EXIT_SUCCESS;
}
