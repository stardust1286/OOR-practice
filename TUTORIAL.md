# C 語言物件導向設計（OOR）完整教學手冊

> **適合對象**：學過基礎 C 語法（變數、函式、指標），但還不懂 OOP 的初學者  
> **學習目標**：理解如何用 C 語言模擬 C++ 的類別、繼承、多型

---

## 目錄

1. [前言：為什麼需要 OOP？](#1-前言)
2. [三個核心工具](#2-三個核心工具)
3. [資料夾結構與職責](#3-資料夾結構)
4. [程式層次架構圖](#4-程式層次架構)
5. [include 依賴關係圖](#5-include-依賴關係)
6. [程式執行流程圖](#6-程式執行流程)
7. [資料流架構](#7-資料流架構)
8. [函式呼叫關係圖](#8-函式呼叫關係)
9. [逐檔案詳細說明](#9-逐檔案說明)
   - [vehicle.h — 基類定義](#91-vehicleh)
   - [vehicle.c — 基類實現](#92-vehiclec)
   - [car.h — Car 派生類定義](#93-carh)
   - [car.c — Car 實現](#94-carc)
   - [bus.h — Bus 派生類定義](#95-bush)
   - [bus.c — Bus 實現](#96-busc)
   - [truck.h — Truck 派生類定義](#97-truckh)
   - [truck.c — Truck 實現](#98-truckc)
   - [traffic_controller.h — 控制器介面](#99-traffic_controllerh)
   - [signal_controller.c — 紅綠燈控制器](#910-signal_controllerc)
   - [parking_controller.c — 停車場控制器](#911-parking_controllerc)
   - [main.c — 主程式整合](#912-mainc)
10. [設計模式總結](#10-設計模式總結)
11. [自學練習題](#11-練習題)

---

## 1. 前言：為什麼需要 OOP？

### 1.1 傳統過程導向的問題

假設你要管理兩輛車的速度：

```c
// 傳統寫法 — 資料與函式是分開的
float car1_speed = 0;
float car2_speed = 0;
char  car1_location[32] = "";
char  car2_location[32] = "";

void move_car1(float distance) {
    car1_speed = 60;
    // ... 只能操作 car1 的全域變數
}

void move_car2(float distance) {
    car2_speed = 60;
    // ... 要寫一份幾乎一樣的程式碼！
}
```

**問題**：如果要 100 輛車，就要宣告 100 份全域變數，寫 100 個函式。程式碼爆炸。

### 1.2 物件導向的解法

把「**資料**」和「**操作資料的函式**」**綁在一起**，形成一個「**物件（Object）**」。

```c
// OOP 寫法 — 資料與函式綁在一起
Car car1 = CAR_DEFAULTS;   // 一個物件包含所有資料
Car car2 = CAR_DEFAULTS;   // 另一個物件，完全獨立

car1.base.move(&car1, 10); // 叫 car1 去移動
car2.base.move(&car2, 50); // 叫 car2 去移動（同一套程式碼！）
```

**好處**：資料隔離、程式碼重用、可以很容易地新增第 3、第 4 輛車。

### 1.3 C 語言 OOP 三大概念

| 概念 | 意思 | 本專案怎麼實現 |
|------|------|-------------|
| **封裝 (Encapsulation)** | 資料和方法包在一起，外部透過介面操作 | `struct` 包含欄位和函式指標 |
| **繼承 (Inheritance)** | 子類別擁有父類別的所有東西，再加上自己的 | `struct Car { Vehicle base; ... }` 嵌入基類 |
| **多型 (Polymorphism)** | 同樣的呼叫方式，不同物件有不同行為 | 函式指標在不同 struct 指向不同函式 |

---

## 2. 三個核心工具

C 語言本身不支援 OOP，但可以用這三個工具模擬出來：

### 工具 1：`typedef struct` — 相當於 Class

```c
// C++ 的 Class
class Car {
public:
    int id;
    float fuel;
    void move(float d);  // 方法
};

// C 的等效寫法
typedef struct {
    int id;
    float fuel;
    void (*move)(void *self, float d);  // 函式指標 = 方法
} Car;
```

**關鍵差異**：C 的 struct 沒有隱含的 `this` 指標，所以要手動傳入 `void *self`。

### 工具 2：函式指標 — 相當於虛擬方法

```c
// 定義函式指標型別（相當於宣告「這個方法的簽名」）
typedef void (*funtypeVehicle_Move)(void *self, float distance);

// 結構體裡放函式指標
typedef struct {
    funtypeVehicle_Move move;  // 這個欄位「指向」一個函式
} Vehicle;

// 使用時：
car1.base.move(&car1, 10);  // 透過函式指標呼叫
//              ↑
//       傳入自己（相當於 C++ 的 this）
```

**為什麼叫「虛擬方法」？** 因為同一個 `move` 欄位，在不同物件裡會指向不同的函式（`car_move`、`bus_move`、`truck_move`），這就是多型。

### 工具 3：`#define` 巨集 — 相當於建構子（Constructor）

```c
// C++ 的建構子
Car::Car() {
    this->id = 0;
    this->fuel = 60.0;
    // 綁定方法...
}

// C 的等效寫法（巨集初始化）
#define CAR_DEFAULTS {          \
    .base = {                   \
        0,              /* id */\
        VEHICLE_TYPE_CAR,       \
        (funtypeVehicle_Move)car_move,  /* ← 在這裡綁定函式 */ \
    },                          \
    60.0f,          /* fuel */  \
}

// 使用：
Car car1 = CAR_DEFAULTS;  // ← 相當於 new Car()
```

---

## 3. 資料夾結構

```
OOR_template/
│
├── include/                    ← 公開介面（只放宣告，不放實現）
│   ├── vehicle.h               ← 基類 Vehicle 的結構定義
│   ├── car.h                   ← Car 的結構定義，繼承 Vehicle
│   ├── bus.h                   ← Bus 的結構定義，繼承 Vehicle
│   ├── truck.h                 ← Truck 的結構定義，繼承 Vehicle
│   └── traffic_controller.h   ← 控制器的介面規格
│
├── src/                        ← 私有實現（放具體邏輯）
│   ├── vehicle.c               ← Vehicle 的通用初始化函式
│   ├── car.c                   ← Car 的所有方法實現
│   ├── bus.c                   ← Bus 的所有方法實現
│   ├── truck.c                 ← Truck 的所有方法實現
│   ├── signal_controller.c    ← 紅綠燈控制器的邏輯
│   ├── parking_controller.c   ← 停車場控制器的邏輯
│   └── main.c                  ← 主程式，把所有東西串起來
│
└── Makefile                    ← 告訴編譯器怎麼把這些檔案組合
```

### 為什麼要 include/ 和 src/ 分開？

這模擬了 C++ 的 **public（公開）** 和 **private（私有）** 的概念：

```
使用者（main.c）只能看到 include/ 的 .h 檔
                    ↓
        知道有 car_move() 這個函式存在
        知道 Car 裡面有哪些欄位
                    ↓
使用者看不到 src/ 的 .c 檔
                    ↓
        不知道 car_move() 裡面的燃油消耗計算公式
        不能直接存取 static 私有狀態
```

---

## 4. 程式層次架構

```
┌─────────────────────────────────────────────────────────┐
│                    Layer 5: Application                 │
│                        main.c                           │
│         (整合所有物件，執行 8 個演示階段)                 │
└─────────────────────┬───────────────┬───────────────────┘
                      │               │
        ┌─────────────▼──┐    ┌───────▼─────────────┐
        │  Layer 4:       │    │  Layer 4:            │
        │  SignalCtrl     │    │  ParkingCtrl         │
        │  (signal_       │    │  (parking_           │
        │  controller.c)  │    │  controller.c)       │
        └────────┬────────┘    └──────────┬───────────┘
                 │                        │
        ┌────────▼────────────────────────▼──────────┐
        │           Layer 3: Interface               │
        │          ITrafficController                │
        │       (traffic_controller.h)               │
        └────────────────────┬───────────────────────┘
                             │ 操作
        ┌────────────────────▼───────────────────────┐
        │           Layer 2: Derived Classes         │
        │  ┌──────────┐  ┌──────────┐  ┌──────────┐ │
        │  │  Car      │  │  Bus     │  │  Truck   │ │
        │  │(car.h/c)  │  │(bus.h/c) │  │(truck.h/c│ │
        │  └────┬──────┘  └────┬─────┘  └────┬─────┘ │
        └───────┼──────────────┼──────────────┼───────┘
                │              │              │
        ┌───────▼──────────────▼──────────────▼───────┐
        │           Layer 1: Base Class               │
        │                  Vehicle                    │
        │             (vehicle.h / vehicle.c)         │
        └─────────────────────────────────────────────┘
```

**從下往上讀**：
- **Layer 1** 定義所有車輛共用的基礎（id、速度、位置、函式指標）
- **Layer 2** 每種車型繼承 Layer 1，並加上自己的特有欄位和邏輯
- **Layer 3** 定義控制器的規格（所有控制器都要遵守的介面）
- **Layer 4** 根據 Layer 3 的規格，實現具體的控制邏輯
- **Layer 5** 主程式把所有東西組合起來使用

---

## 5. include 依賴關係

箭頭代表「A → B」表示「A 需要引入 B」：

```
main.c
  ├── #include "../include/vehicle.h"
  ├── #include "../include/car.h"         car.h → vehicle.h
  ├── #include "../include/bus.h"         bus.h → vehicle.h
  ├── #include "../include/truck.h"       truck.h → vehicle.h
  └── #include "../include/traffic_controller.h"
                                          traffic_controller.h → vehicle.h

signal_controller.c
  ├── #include "../include/traffic_controller.h"
  ├── #include "../include/car.h"
  ├── #include "../include/bus.h"
  └── #include "../include/truck.h"

parking_controller.c  (同 signal_controller.c)

car.c
  └── #include "../include/car.h"   (car.h 已含 vehicle.h)

bus.c
  └── #include "../include/bus.h"

truck.c
  └── #include "../include/truck.h"

vehicle.c
  └── #include "../include/vehicle.h"
```

**核心規則**：所有人都依賴 `vehicle.h`，它是整個系統的基礎。

---

## 6. 程式執行流程

執行 `./traffic_system` 後，`main()` 依序做了 8 件事：

```
程式啟動
    │
    ▼
[Stage 1] 建立三個車輛物件
    │  Car car1 = CAR_DEFAULTS;      ← 巨集展開，所有欄位初始化，函式指標綁定
    │  Bus bus1 = BUS_DEFAULTS;
    │  Truck truck1 = TRUCK_DEFAULTS;
    │
    ▼
[Stage 2] 建立兩個控制器
    │  signal_controller_init(&signal_ctrl, 1001);
    │  parking_controller_init(&parking_ctrl, 2001);
    │
    ▼
[Stage 3] 紅綠燈多型演示
    │  signal_ctrl.get_status()  ← 查詢信號燈（此時是紅燈）
    │  signal_ctrl.control_vehicle(car1)  ← 車輛嘗試通過，被擋下（紅燈）
    │  signal_ctrl.control_vehicle(bus1)
    │  signal_ctrl.control_vehicle(truck1)
    │
    ▼
[Stage 4] 虛擬方法多型演示
    │  car1.base.move(10.5)   ← 呼叫 car_move()：消耗燃油，位置移動
    │  bus1.base.move(8.0)    ← 呼叫 bus_move()：考慮乘客數影響速度
    │  truck1.base.move(15.0) ← 呼叫 truck_move()：空車移動
    │  [裝載 5000kg 貨物]
    │  truck1.base.move(10.0) ← 重載移動，速度從 50→35，油耗增加
    │
    ▼
[Stage 5] 停車場管理
    │  parking_ctrl.control_vehicle(car1)  ← 分配停泊位 0，收費 5.00
    │  car1.base.park("Zone-A Spot-01")   ← 記錄停泊位置
    │  [同樣處理 bus1、truck1]
    │
    ▼
[Stage 6] 列印所有物件資訊
    │  car1.base.print_info()   ← 輸出 car1 的所有狀態
    │  signal_ctrl.print_info() ← 輸出控制器狀態
    │  parking_ctrl.print_info()
    │
    ▼
[Stage 7] 車輛特有操作
    │  car_add_passenger(&car1, 3)
    │  car_refuel(&car1, 20.0)
    │  bus_board_passenger(&bus1, 30)
    │  truck_unload_cargo(&truck1)
    │
    ▼
[Stage 8] 離開停車場
    │  parking_ctrl.release(car1) ← 釋放停泊位 0
    │  parking_ctrl.release(bus1) ← 釋放停泊位 1
    │  parking_ctrl.release(truck1)
    │
    ▼
程式結束（return EXIT_SUCCESS）
```

---

## 7. 資料流架構

### 7.1 物件建立時的資料流

```
CAR_DEFAULTS 巨集展開
    │
    │  .base.id = 0
    │  .base.type = VEHICLE_TYPE_CAR
    │  .base.current_speed = 0.0f
    │  .base.position_x = 0.0f
    │  .base.position_y = 0.0f
    │  .base.move = car_move        ← 函式指標綁定！
    │  .base.stop = car_stop
    │  .base.park = car_park
    │  .base.print_info = car_print_info
    │  .passenger_count = 0
    │  .fuel_level = 60.0f
    │  .has_ac = 1
    ▼
Car car1（棧上的記憶體，大小固定）
```

### 7.2 move() 呼叫時的資料流

```
car1.base.move(&car1, 10.5)
    │
    │  1. 查表：car1.base.move 指向 car_move
    │  2. 呼叫：car_move(&car1, 10.5)
    │  3. 在 car_move 內部：
    │     self = (Car *)&car1         ← void* 轉型為 Car*
    │     distance = 10.5
    │     fuel_consumption = 10.5 * 0.08 = 0.84L
    │     self->fuel_level -= 0.84   (60.0 → 59.16)
    │     self->base.position_x += 10.5 * 0.7 = 7.35
    │     self->base.position_y += 10.5 * 0.3 = 3.15
    │     self->base.current_speed = 60.0
    ▼
car1 物件狀態改變：fuel=59.16, pos=(7.35,3.15), speed=60
```

### 7.3 控制器呼叫時的資料流

```
signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&car1)
    │
    │  1. 查表：signal_ctrl.control_vehicle 指向 signal_control_vehicle
    │  2. 呼叫：signal_control_vehicle(&signal_ctrl, &car1.base)
    │  3. 在函式內部：
    │     檢查 current_light_state（private static，此時 = 0 = 紅燈）
    │     → 回傳 CONTROL_RESULT_DENIED（拒絕通行）
    ▼
car1 物件不受影響（只是查詢，控制器做決策）
signal_ctrl 的 private 狀態（signal_cycle, total_passed）可能改變
```

### 7.4 停車場資源管理的資料流

```
[分配]
parking_ctrl.control_vehicle(&parking_ctrl, (Vehicle*)&car1)
    │
    │  搜尋 parking_spaces[0..49]，找到第一個 = 0 的位置（假設是 0）
    │  parking_spaces[0] = 1          ← 標記佔用
    │  parked_vehicle_ids[0] = 101    ← 記錄是哪輛車
    │  parking_revenue += 5.0         ← 累計收入
    ▼
停車場內部狀態改變（private static 陣列）

[釋放]
parking_ctrl.release(&parking_ctrl, (Vehicle*)&car1)
    │
    │  搜尋 parked_vehicle_ids[] 找到 id=101 的位置（是 0）
    │  parking_spaces[0] = 0          ← 標記空閒
    │  parked_vehicle_ids[0] = 0
    ▼
停泊位 0 重新可用
```

---

## 8. 函式呼叫關係

### 8.1 Vehicle 相關呼叫鏈

```
main.c
  └── [建立物件] Car car1 = CAR_DEFAULTS
         └── [巨集展開] .base.move = (funtypeVehicle_Move)car_move
  
  └── car1.base.move(&car1, 10.5)         ← 觸發
         └── car_move(Car *self, float distance)    [car.c]
                ├── 讀取 self->fuel_level
                ├── 修改 self->fuel_level
                ├── 修改 self->base.position_x
                └── printf(...)

  └── car1.base.park(&car1, "Zone-A")     ← 觸發
         └── car_park(Car *self, int8_t *location)  [car.c]
                ├── 修改 self->base.is_parked = 1
                ├── strncpy(self->base.location, location, 31)
                └── printf(...)

  └── car1.base.print_info(&car1)         ← 觸發
         └── car_print_info(Car *self)              [car.c]
                └── printf(所有欄位)
```

### 8.2 Controller 相關呼叫鏈

```
main.c
  └── signal_controller_init(&signal_ctrl, 1001)  [signal_controller.c]
         ├── signal_ctrl.control_vehicle = signal_control_vehicle
         ├── signal_ctrl.get_status = signal_get_status
         └── signal_ctrl.print_info = signal_print_info

  └── signal_ctrl.get_status(&signal_ctrl)        ← 觸發
         └── signal_get_status(void *self)
                ├── signal_cycle++
                ├── 根據 signal_cycle % 15 更新 current_light_state
                └── 回傳 current_light_state

  └── signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle*)&car1)
         └── signal_control_vehicle(void *self, Vehicle *vehicle)
                ├── 檢查 current_light_state
                ├── 根據 vehicle->type 決定通行結果
                └── 回傳 CONTROL_RESULT_SUCCESS / DENIED / ERROR
```

### 8.3 完整函式索引表

| 函式名稱 | 位置 | 呼叫者 | 功能 |
|---------|------|--------|------|
| `vehicle_init()` | vehicle.c | 基礎設施，一般不直接呼叫 | 初始化 Vehicle 基礎欄位 |
| `vehicle_destroy()` | vehicle.c | 需要清理時呼叫 | 清空 Vehicle 記憶體 |
| `car_get_type()` | car.c | 透過 `base.get_type` 指標 | 回傳 `VEHICLE_TYPE_CAR` |
| `car_move()` | car.c | 透過 `base.move` 指標 | 移動轎車，消耗燃油 |
| `car_stop()` | car.c | 透過 `base.stop` 指標 | 速度設為 0 |
| `car_park()` | car.c | 透過 `base.park` 指標 | 標記停泊，記錄位置 |
| `car_print_info()` | car.c | 透過 `base.print_info` 指標 | 列印所有 car 欄位 |
| `car_add_passenger()` | car.c | main.c 直接呼叫 | 增加乘客數 |
| `car_remove_passenger()` | car.c | main.c 直接呼叫 | 減少乘客數 |
| `car_refuel()` | car.c | main.c 直接呼叫 | 補充燃油 |
| `bus_move()` | bus.c | 透過 `base.move` 指標 | 移動公車，考慮乘客數 |
| `bus_board_passenger()` | bus.c | main.c 直接呼叫 | 乘客上車 |
| `bus_alight_passenger()` | bus.c | main.c 直接呼叫 | 乘客下車 |
| `bus_set_route()` | bus.c | main.c 直接呼叫 | 設定路線號 |
| `truck_move()` | truck.c | 透過 `base.move` 指標 | 移動卡車，考慮載重 |
| `truck_load_cargo()` | truck.c | main.c 直接呼叫 | 裝載貨物 |
| `truck_unload_cargo()` | truck.c | main.c 直接呼叫 | 卸載貨物 |
| `truck_refuel()` | truck.c | main.c 直接呼叫 | 補充燃油 |
| `signal_controller_init()` | signal_controller.c | main.c | 初始化紅綠燈控制器 |
| `signal_control_vehicle()` | signal_controller.c | 透過 `control_vehicle` 指標 | 決策通行結果 |
| `signal_get_status()` | signal_controller.c | 透過 `get_status` 指標 | 更新並回傳燈號狀態 |
| `parking_controller_init()` | parking_controller.c | main.c | 初始化停車場控制器 |
| `parking_control_vehicle()` | parking_controller.c | 透過 `control_vehicle` 指標 | 分配停泊位 |
| `parking_release()` | parking_controller.c | 透過 `release` 指標 | 釋放停泊位 |
| `calculate_parking_fee()` | parking_controller.c | 僅 parking_control_vehicle() | 計算停泊費用（private） |

---

## 9. 逐檔案說明

---

### 9.1 vehicle.h

**職責**：定義整個系統的基礎——所有車輛共用的資料結構和方法規格。

**位置**：`include/vehicle.h`

#### 9.1.1 列舉型別 `VehicleType`

```c
typedef enum {
    VEHICLE_TYPE_UNKNOWN = 0,
    VEHICLE_TYPE_CAR = 1,
    VEHICLE_TYPE_BUS = 2,
    VEHICLE_TYPE_TRUCK = 3
} VehicleType;
```

**用途**：讓控制器可以根據車型做不同決策（`if (vehicle->type == VEHICLE_TYPE_BUS) ...`）。  
**為什麼用 enum？** 比直接用數字 `0、1、2` 更易讀，也更不容易出錯。

#### 9.1.2 函式指標型別定義（Virtual Method Table）

```c
typedef VehicleType (*funtypeVehicle_GetType)(void *self);
typedef void        (*funtypeVehicle_Move)   (void *self, float distance);
typedef void        (*funtypeVehicle_Stop)   (void *self);
typedef void        (*funtypeVehicle_Park)   (void *self, int8_t *location);
typedef void        (*funtypeVehicle_PrintInfo)(void *self);
```

**逐行解讀**：

```c
typedef void (*funtypeVehicle_Move)(void *self, float distance);
//      ↑        ↑                   ↑             ↑
//   回傳型別  指標名稱            第1個參數      第2個參數
//            (這是一個指向函式的指標)
```

- `void *self`：指向「呼叫這個方法的物件」（相當於 C++ 的 `this`）
- 用 `void *` 是因為可以接受任何型別（Car\*、Bus\*、Truck\* 都可以）

**比喻**：這些型別定義就像「職位規格」——規定了一個「移動函式」必須接受什麼參數、回傳什麼值。Car、Bus、Truck 都要提供符合規格的函式。

#### 9.1.3 `struct Vehicle`（基類主體）

```c
typedef struct Vehicle {
    // ===== 資料欄位（Data Fields）=====
    uint16_t id;             // 車輛識別碼，0~65535
    VehicleType type;        // 車輛種類（Car/Bus/Truck）
    float current_speed;     // 目前時速 km/h
    float position_x;        // 地圖座標 X
    float position_y;        // 地圖座標 Y
    int8_t location[32];     // 停泊位置的文字描述（最多31個字元）
    uint8_t is_parked;       // 0=行駛中，1=已停泊

    // ===== 方法指標（Function Pointers / VTable）=====
    funtypeVehicle_GetType    get_type;     // 查詢車型
    funtypeVehicle_Move       move;         // 移動
    funtypeVehicle_Stop       stop;         // 停止
    funtypeVehicle_Park       park;         // 停泊
    funtypeVehicle_PrintInfo  print_info;   // 列印資訊
} Vehicle;
```

**記憶體佈局**（每個 Vehicle 佔的記憶體）：

```
Vehicle struct 在記憶體中的樣子：
┌──────────┬──────────┬──────────────┬──────────────┬───────────┐
│ id(2B)   │type(4B)  │speed(4B)     │pos_x(4B)     │pos_y(4B) │
├──────────┴──────────┴──────────────┴──────────────┴───────────┤
│ location[32] (32 bytes)                                        │
├────────────────────────────────────────────────────────────────┤
│ is_parked(1B)  │ [padding]                                     │
├────────────────────────────────────────────────────────────────┤
│ get_type (函式指標，8B on 64-bit)                              │
├────────────────────────────────────────────────────────────────┤
│ move     (函式指標，8B)                                        │
├────────────────────────────────────────────────────────────────┤
│ stop / park / print_info  (各 8B)                              │
└────────────────────────────────────────────────────────────────┘
```

#### 9.1.4 初始化巨集 `VEHICLE_INIT_BASE`

```c
#define VEHICLE_INIT_BASE(type_enum) { \
    0,              /* id = 0 */         \
    (type_enum),    /* 傳入的車型 */      \
    0.0f,           /* current_speed */  \
    0.0f, 0.0f,     /* position */       \
    {0},            /* location (清空)*/ \
    0,              /* is_parked = 0 */  \
    NULL, NULL, NULL, NULL, NULL  /* 函式指標待子類設定 */ \
}
```

這個巨集很少單獨使用，通常由 `CAR_DEFAULTS`、`BUS_DEFAULTS` 等子類巨集調用。

---

### 9.2 vehicle.c

**職責**：實現 `vehicle.h` 宣告的兩個通用函式。

**位置**：`src/vehicle.c`

#### `vehicle_init()` — 基礎初始化

```c
void vehicle_init(Vehicle *v, uint16_t id, VehicleType type)
{
    if (v == NULL) return;   // 防禦性檢查：指標為 NULL 時安全退出
    
    v->id = id;
    v->type = type;
    v->current_speed = 0.0f;
    v->position_x = 0.0f;
    v->position_y = 0.0f;
    memset(v->location, 0, sizeof(v->location));  // 把 location 清空為全 '\0'
    v->is_parked = 0;
    
    // 函式指標先設為 NULL，由子類（Car/Bus/Truck）的初始化來設定
    v->get_type = NULL;
    v->move = NULL;
    v->stop = NULL;
    v->park = NULL;
    v->print_info = NULL;
}
```

**注意**：本專案主要用巨集初始化（`CAR_DEFAULTS` 等），`vehicle_init()` 提供另一種初始化方式。

#### `vehicle_destroy()` — 清理

```c
void vehicle_destroy(Vehicle *v)
{
    if (v == NULL) return;
    memset(v, 0, sizeof(Vehicle));  // 把整個 struct 清零
}
```

**用途**：在物件不再需要時清空記憶體，防止敏感資料殘留。

---

### 9.3 car.h

**職責**：定義轎車（Car）的結構，繼承 Vehicle 並添加轎車特有欄位和方法。

**位置**：`include/car.h`

#### 9.3.1 Car 結構體（派生類）

```c
typedef struct {
    Vehicle base;             // ← 第一個欄位必須是 Vehicle！（繼承的關鍵）
    
    uint8_t passenger_count; // Car 特有：目前乘客數（最多 5 人）
    float fuel_level;         // Car 特有：燃油量（最多 60L）
    uint8_t has_ac;           // Car 特有：是否有冷氣（0/1）
} Car;
```

**為什麼 `Vehicle base` 必須放第一個？**

```
Car 的記憶體佈局：
┌───────────────────────────────────────────┐
│  Vehicle base（放在最前面）                │
│  ┌─────────────────────────────────────┐  │
│  │ id, type, speed, position...         │  │
│  │ move, stop, park, print_info...      │  │
│  └─────────────────────────────────────┘  │
│  passenger_count (1B)                     │
│  fuel_level (4B)                          │
│  has_ac (1B)                              │
└───────────────────────────────────────────┘

因為 Vehicle 在最前面，所以：
&car1 的地址 == &car1.base 的地址

這讓以下轉型是合法且正確的：
(Vehicle *)&car1 == &car1.base  ✓
```

#### 9.3.2 初始化巨集 `CAR_DEFAULTS`

```c
#define CAR_DEFAULTS {                              \
    .base = {                                       \
        0,                                          \  // id = 0（之後再設定）
        VEHICLE_TYPE_CAR,                           \  // 標記這是轎車
        0.0f, 0.0f, 0.0f,                          \  // speed, pos_x, pos_y
        {0}, 0,                                     \  // location, is_parked
        (funtypeVehicle_GetType)car_get_type,       \  // ← 綁定 car_get_type
        (funtypeVehicle_Move)car_move,              \  // ← 綁定 car_move
        (funtypeVehicle_Stop)car_stop,              \  // ← 綁定 car_stop
        (funtypeVehicle_Park)car_park,              \  // ← 綁定 car_park
        (funtypeVehicle_PrintInfo)car_print_info    \  // ← 綁定 car_print_info
    },                                              \
    0,          // passenger_count = 0              \
    MAX_FUEL,   // fuel_level = 60.0f（滿油）        \
    1           // has_ac = 1（有冷氣）              \
}
```

**「綁定」是什麼意思？**

```c
(funtypeVehicle_Move)car_move
// 強制轉型：把 car_move 的型別轉為通用函式指標型別
// 這樣 base.move 就指向了 car_move 這個函式
```

之後呼叫 `car1.base.move(&car1, 10)` 時：
1. 查詢 `car1.base.move` 這個指標
2. 發現它指向 `car_move`
3. 呼叫 `car_move(&car1, 10)`

#### 9.3.3 函式宣告

```c
// 虛擬方法（實現在 car.c，透過函式指標呼叫）
VehicleType car_get_type(Car *self);
void car_move(Car *self, float distance);
void car_stop(Car *self);
void car_park(Car *self, int8_t *location);
void car_print_info(Car *self);

// Car 特有方法（直接呼叫）
void car_add_passenger(Car *self, uint8_t count);
void car_remove_passenger(Car *self, uint8_t count);
void car_refuel(Car *self, float amount);
```

---

### 9.4 car.c

**職責**：實現 `car.h` 宣告的所有函式——轎車的具體邏輯。

**位置**：`src/car.c`

#### `car_move()` — 轎車移動邏輯

```c
void car_move(Car *self, float distance)
{
    // 步驟 1：防禦性檢查
    if (self == NULL) return;
    
    // 步驟 2：停泊狀態檢查
    if (self->base.is_parked) {
        printf("Warning: Vehicle is parked\n");
        return;
    }
    
    // 步驟 3：計算燃油消耗（轎車：0.08L/km）
    float fuel_consumption = distance * 0.08f;
    
    // 步驟 4：燃油不足時拒絕移動
    if (self->fuel_level < fuel_consumption) {
        printf("Warning: Insufficient fuel\n");
        return;
    }
    
    // 步驟 5：扣除燃油
    self->fuel_level -= fuel_consumption;
    
    // 步驟 6：更新位置（轎車：x 方向走 70%，y 方向走 30%）
    self->base.position_x += distance * 0.7f;
    self->base.position_y += distance * 0.3f;
    
    // 步驟 7：設定速度
    self->base.current_speed = 60.0f;  // 轎車巡航速度
    
    // 步驟 8：輸出日誌
    printf("[Car %d] Moved %.2f km...\n", self->base.id, distance);
}
```

**與 bus_move() 的差異**（多型的體現）：

| 項目 | car_move() | bus_move() | truck_move() |
|------|-----------|-----------|-------------|
| 油耗係數 | 0.08 L/km | 0（用電） | 0.15 × 重量係數 |
| 速度 | 60 km/h | 40 × (1 - 乘客率) | 50 × (1 - 載重率) |
| 特殊邏輯 | 燃油檢查 | 乘客數影響速度 | 貨物重量影響速度和油耗 |

#### `car_park()` — 轎車停泊

```c
void car_park(Car *self, int8_t *location)
{
    if (self == NULL || location == NULL) return;
    
    self->base.is_parked = 1;           // 標記已停泊
    self->base.current_speed = 0.0f;    // 速度歸零
    strncpy(self->base.location, (char *)location, 31);  // 複製位置字串
    self->base.location[31] = '\0';     // 確保字串以 null 結尾（安全寫法）
    
    printf("[Car %d] Parked at: %s\n", self->base.id, self->base.location);
}
```

**`strncpy` 的安全寫法**：
- `strncpy(dest, src, 31)` 最多複製 31 個字元（防止 buffer overflow）
- `location[31] = '\0'` 手動加上結尾（因為 strncpy 不保證加上 null）

#### `car_add_passenger()` — 乘客上車

```c
void car_add_passenger(Car *self, uint8_t count)
{
    // 計算「還可以上多少人」（不超過 MAX_PASSENGERS）
    uint8_t available = MAX_PASSENGERS - self->passenger_count;
    
    // 取 count 和 available 的較小值（防止超載）
    uint8_t to_add = (count > available) ? available : count;
    
    self->passenger_count += to_add;
    printf("[Car %d] %d passengers boarded\n", self->base.id, to_add);
}
```

**三元運算子說明**：
```c
uint8_t to_add = (count > available) ? available : count;
// 如果 count > available，就取 available（最多只能這麼多）
// 否則取 count（請求的數量）
```

---

### 9.5 bus.h

**職責**：定義公車（Bus）結構，繼承 Vehicle。

**位置**：`include/bus.h`

#### Bus 結構體

```c
typedef struct {
    Vehicle base;              // 繼承基類（必須第一個）
    
    uint16_t passenger_count;  // 目前乘客數（最多 50 人）
    uint8_t route_number;      // 路線號（如 15 路、25 路）
    uint16_t total_passengers; // 今日累計乘客人次
} Bus;
```

**與 Car 的差異**：
- `passenger_count` 是 `uint16_t`（最多 50 人，需要比 Car 的 5 人更大的型別）
- 多了 `route_number` 和 `total_passengers`（公車特有）
- 沒有 `fuel_level`（公車用電）

#### BUS_DEFAULTS 巨集

初始化時綁定 `bus_move`、`bus_park` 等，而不是 `car_move`。這是多型的關鍵——同樣的 `base.move` 欄位，Car 和 Bus 指向不同的函式。

---

### 9.6 bus.c

**職責**：實現公車的所有方法。

**位置**：`src/bus.c`

#### `bus_move()` — 公車移動邏輯

```c
void bus_move(Bus *self, float distance)
{
    // 乘客影響速度：乘客越多，速度越慢
    // speed_factor 從 1.0（空車）降到最低 0.3（滿載）
    float speed_factor = 1.0f - (self->passenger_count * 0.01f);
    if (speed_factor < 0.3f) speed_factor = 0.3f;  // 最低 30% 速度
    
    // 更新位置（公車走的方向比較均等）
    self->base.position_x += distance * 0.5f * speed_factor;
    self->base.position_y += distance * 0.5f * speed_factor;
    
    // 速度受乘客數影響
    self->base.current_speed = BUS_TYPICAL_SPEED * speed_factor;
    // BUS_TYPICAL_SPEED = 40 km/h
    
    printf("[Bus %d (Route %d)] Traveling %.2f km...\n", ...);
}
```

**計算範例**：
- 空車：`speed_factor = 1.0`，速度 = 40 km/h
- 25 人：`speed_factor = 1.0 - 25*0.01 = 0.75`，速度 = 30 km/h
- 50 人：`speed_factor = 1.0 - 50*0.01 = 0.5`，速度 = 20 km/h
- 100 人（超載）：`speed_factor = max(0.3) = 0.3`，速度 = 12 km/h

#### `bus_board_passenger()` — 乘客上車

```c
void bus_board_passenger(Bus *self, uint16_t count)
{
    uint16_t available = MAX_BUS_CAPACITY - self->passenger_count;
    uint16_t to_board = (count > available) ? available : count;
    
    self->passenger_count += to_board;
    self->total_passengers += to_board;  // 累計乘客（今日統計）
    
    printf("[Bus %d] %d passengers boarded\n", ...);
}
```

---

### 9.7 truck.h

**職責**：定義卡車（Truck）結構，繼承 Vehicle。

**位置**：`include/truck.h`

#### Truck 結構體

```c
typedef struct {
    Vehicle base;              // 繼承基類

    float cargo_weight;        // 目前載重（kg，最多 10000kg）
    float fuel_level;          // 燃油量（L）
    int8_t cargo_type[32];     // 貨物描述（文字，如 "Construction Material"）
} Truck;
```

**與 Car 的差異**：
- 有 `cargo_weight`（影響速度和油耗的核心欄位）
- `fuel_level` 是 100L（比 Car 的 60L 更大的油箱）
- 多了 `cargo_type` 字串欄位

---

### 9.8 truck.c

**職責**：實現卡車的所有方法。最複雜的移動邏輯。

**位置**：`src/truck.c`

#### `truck_move()` — 卡車移動邏輯（最複雜）

```c
void truck_move(Truck *self, float distance)
{
    // ── 計算重量影響係數 ──
    // 載重越大，油耗越高（最多 +50%）
    float weight_factor = 1.0f + (self->cargo_weight / MAX_CARGO_WEIGHT) * 0.5f;
    
    // ── 計算速度降低係數 ──
    // 載重越大，速度越慢（最多降低 60%）
    float speed_reduction = (self->cargo_weight / MAX_CARGO_WEIGHT) * 0.6f;
    
    // ── 計算實際速度 ──
    self->base.current_speed = TRUCK_TYPICAL_SPEED * (1.0f - speed_reduction);
    if (self->base.current_speed < 20.0f) self->base.current_speed = 20.0f; // 最低 20 km/h
    
    // ── 計算油耗 ──
    float fuel_consumption = distance * 0.15f * weight_factor;
    // 空車：distance * 0.15 * 1.0
    // 滿載：distance * 0.15 * 1.5
    
    // 燃油不足檢查
    if (self->fuel_level < fuel_consumption) { ... return; }
    
    self->fuel_level -= fuel_consumption;
    self->base.position_x += distance * 0.6f;
    self->base.position_y += distance * 0.4f;
}
```

**計算範例（distance = 10km）**：

| 載重 | weight_factor | speed_reduction | 速度 | 油耗 |
|-----|--------------|----------------|-----|------|
| 0 kg（空車） | 1.0 | 0.0 | 50 km/h | 10 × 0.15 × 1.0 = 1.5L |
| 5000 kg（半載） | 1.25 | 0.3 | 35 km/h | 10 × 0.15 × 1.25 = 1.875L |
| 10000 kg（滿載） | 1.5 | 0.6 | 20 km/h | 10 × 0.15 × 1.5 = 2.25L |

#### `truck_load_cargo()` — 裝載貨物

```c
void truck_load_cargo(Truck *self, float weight, int8_t *cargo_type)
{
    // 超重保護
    if (self->cargo_weight + weight > MAX_CARGO_WEIGHT) {
        printf("Warning: Overweight!\n");
        return;
    }
    
    self->cargo_weight += weight;
    
    // 複製貨物類型字串（若有提供）
    if (cargo_type != NULL) {
        strncpy(self->cargo_type, cargo_type, 31);
        self->cargo_type[31] = '\0';
    }
}
```

---

### 9.9 traffic_controller.h

**職責**：定義所有控制器的「介面規格」，任何控制器都必須遵守這個格式。

**位置**：`include/traffic_controller.h`

#### 為什麼需要這個介面？

如果沒有統一介面：

```c
// 沒有介面的情況 — 每個控制器呼叫方式不同
signal_control(my_signal, vehicle);     // 信號燈用這個
manage_parking(my_parking, vehicle);    // 停車場用這個
highway_check(my_highway, vehicle);    // 高速公路用這個
// ↑ main.c 必須了解每個控制器的細節，耦合度高
```

有了統一介面：

```c
// 有介面 — 所有控制器用相同方式呼叫
ctrl.control_vehicle(&ctrl, vehicle);  // 不管什麼控制器都這樣呼叫
// ↑ main.c 完全不需要知道 ctrl 的實際型別
```

#### 列舉型別

```c
// 控制器類型
typedef enum {
    CONTROLLER_TYPE_SIGNAL = 1,    // 信號燈
    CONTROLLER_TYPE_PARKING = 2,   // 停車場
    CONTROLLER_TYPE_HIGHWAY = 3    // 高速公路（可擴充）
} ControllerType;

// 控制結果
typedef enum {
    CONTROL_RESULT_SUCCESS = 0,    // 允許通行/停泊
    CONTROL_RESULT_DENIED = 1,     // 拒絕
    CONTROL_RESULT_ERROR = 2       // 錯誤
} ControlResult;
```

#### `ITrafficController` 介面結構體

```c
typedef struct {
    // 識別資訊
    uint16_t controller_id;         // 控制器的 ID
    ControllerType type;            // 控制器種類
    
    // 虛擬方法（函式指標）
    funtypeController_ControlVehicle  control_vehicle; // 控制車輛
    funtypeController_GetStatus       get_status;      // 查詢狀態
    funtypeController_Release         release;         // 釋放資源
    funtypeController_PrintInfo       print_info;      // 列印資訊
} ITrafficController;
```

**使用方式**：

```c
ITrafficController signal_ctrl;  // 宣告一個控制器
signal_controller_init(&signal_ctrl, 1001);  // 初始化，綁定函式

// 使用時，不需要知道是信號燈還是停車場
ControlResult result = signal_ctrl.control_vehicle(&signal_ctrl, vehicle);
```

---

### 9.10 signal_controller.c

**職責**：實現紅綠燈控制器的具體邏輯。

**位置**：`src/signal_controller.c`

#### 私有狀態（用 static 隱藏）

```c
static uint32_t signal_cycle = 0;         // 信號週期計數器
static uint8_t current_light_state = 0;  // 燈號：0=紅, 1=綠, 2=黃
static uint32_t total_passed = 0;         // 累計通過數
```

**為什麼用 `static`？**

```c
static uint8_t current_light_state = 0;
// ↑
// static 讓這個變數只在本檔案（signal_controller.c）內可見
// main.c 或其他 .c 檔無法直接存取
// 這模擬了 C++ 的 private 成員變數
```

#### `signal_get_status()` — 更新燈號

```c
int signal_get_status(void *self)
{
    signal_cycle++;  // 每次呼叫，週期加 1
    
    // 15 個週期的循環（紅5→綠5→黃5→重複）
    switch (signal_cycle % 15) {
        case 0 ... 4:    current_light_state = 0; break;  // 紅燈（0~4）
        case 5 ... 9:    current_light_state = 1; break;  // 綠燈（5~9）
        case 10 ... 14:  current_light_state = 2; break;  // 黃燈（10~14）
    }
    
    return current_light_state;
}
```

**注意**：`case 0 ... 4:` 是 GCC 的 range case 擴充語法，相當於 `case 0: case 1: case 2: case 3: case 4:`。

#### `signal_control_vehicle()` — 核心決策邏輯

```c
ControlResult signal_control_vehicle(void *self, Vehicle *vehicle)
{
    ITrafficController *controller = (ITrafficController *)self;
    // ↑ void* → ITrafficController* 型別轉換（才能讀 controller_id）
    
    // 決策 1：紅燈/黃燈 → 全部攔截
    if (current_light_state != 1) {
        return CONTROL_RESULT_DENIED;
    }
    
    // 決策 2：公車優先
    if (vehicle->type == VEHICLE_TYPE_BUS) {
        total_passed++;
        return CONTROL_RESULT_SUCCESS;
    }
    
    // 決策 3：卡車在尖峰時間延遲（用 signal_cycle % 3 模擬）
    if (vehicle->type == VEHICLE_TYPE_TRUCK) {
        if (signal_cycle % 3 == 0) return CONTROL_RESULT_DENIED;
        total_passed++;
        return CONTROL_RESULT_SUCCESS;
    }
    
    // 決策 4：轎車正常通過
    if (vehicle->type == VEHICLE_TYPE_CAR) {
        total_passed++;
        return CONTROL_RESULT_SUCCESS;
    }
    
    return CONTROL_RESULT_ERROR;
}
```

**決策流程圖**：

```
vehicle 到達信號燈
        │
        ▼
  是綠燈嗎？
  ├── 否 → DENIED（紅燈/黃燈停車）
  └── 是
        │
        ▼
  vehicle->type 是什麼？
  ├── BUS    → SUCCESS（公車優先）
  ├── TRUCK  → 是尖峰嗎（cycle%3==0）？
  │             ├── 是 → DENIED
  │             └── 否 → SUCCESS
  ├── CAR    → SUCCESS（正常通過）
  └── 其他  → ERROR
```

#### `signal_controller_init()` — 初始化

```c
void signal_controller_init(ITrafficController *controller, uint16_t id)
{
    if (controller == NULL) return;
    
    controller->controller_id = id;
    controller->type = CONTROLLER_TYPE_SIGNAL;
    
    // 綁定所有函式指標（讓介面指向實際的函式）
    controller->control_vehicle = signal_control_vehicle;
    controller->get_status      = signal_get_status;
    controller->release         = signal_release;
    controller->print_info      = signal_print_info;
}
```

**這個函式做了什麼**：把 `ITrafficController` 的 4 個函式指標都設定好，之後就可以透過介面呼叫了。

---

### 9.11 parking_controller.c

**職責**：實現停車場控制器的邏輯，管理停泊位的分配和釋放。

**位置**：`src/parking_controller.c`

#### 私有狀態

```c
#define MAX_PARKING_SPACES 50

static uint8_t parking_spaces[MAX_PARKING_SPACES];      // 50 個停泊位狀態
static uint16_t parked_vehicle_ids[MAX_PARKING_SPACES]; // 記錄哪輛車在哪個位置
static float parking_revenue = 0.0f;                    // 累計收入
```

**記憶體示意圖**：

```
parking_spaces:      [0, 1, 1, 0, 0, 0, ..., 0]
                      ↑  ↑  ↑
                    空 有 有  空...
                      
parked_vehicle_ids: [0, 101, 201, 0, 0, ..., 0]
                        ↑    ↑
                      car1  bus1
```

#### `calculate_parking_fee()` — 費用計算（private 函式）

```c
static float calculate_parking_fee(Vehicle *vehicle)
//     ↑
//  static 讓此函式只在本檔案內可用（private）
{
    switch (vehicle->type) {
        case VEHICLE_TYPE_CAR:   return 5.0f;
        case VEHICLE_TYPE_BUS:   return 15.0f;  // 但公車實際上是免費的
        case VEHICLE_TYPE_TRUCK: return 20.0f;
        default:                 return 0.0f;
    }
}
```

#### `parking_control_vehicle()` — 分配停泊位

```c
ControlResult parking_control_vehicle(void *self, Vehicle *vehicle)
{
    // 步驟 1：線性搜尋找空位
    int free_space = -1;
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parking_spaces[i] == 0) {  // 找到空位
            free_space = i;
            break;
        }
    }
    
    // 步驟 2：停車場滿了
    if (free_space == -1) return CONTROL_RESULT_DENIED;
    
    // 步驟 3：計算費用（公車免費）
    float fee = 0.0f;
    if (vehicle->type != VEHICLE_TYPE_BUS) {
        fee = calculate_parking_fee(vehicle);
        parking_revenue += fee;
    }
    
    // 步驟 4：標記停泊位為已佔用
    parking_spaces[free_space] = 1;
    parked_vehicle_ids[free_space] = vehicle->id;
    
    return CONTROL_RESULT_SUCCESS;
}
```

#### `parking_release()` — 釋放停泊位

```c
void parking_release(void *self, Vehicle *vehicle)
{
    // 搜尋哪個位置停了這輛車
    for (int i = 0; i < MAX_PARKING_SPACES; i++) {
        if (parking_spaces[i] == 1 && parked_vehicle_ids[i] == vehicle->id) {
            // 找到了！清空這個位置
            parking_spaces[i] = 0;
            parked_vehicle_ids[i] = 0;
            printf("Vehicle ID=%d left (space %d released)\n", ...);
            return;
        }
    }
    // 如果沒找到，函式直接結束（找不到就不做任何事）
}
```

---

### 9.12 main.c

**職責**：把所有模組串起來，演示完整的 OOP 特性。

**位置**：`src/main.c`

#### extern 宣告說明

```c
extern void parking_controller_init(ITrafficController *controller, uint16_t id);
extern void signal_controller_init(ITrafficController *controller, uint16_t id);
```

**為什麼需要 `extern`？**

`signal_controller_init` 定義在 `signal_controller.c`，但沒有對應的 `.h` 檔。`extern` 告訴編譯器「這個函式在其他地方，連結時會找到它」。

正式專案應該為每個控制器建立 `.h` 檔，這裡簡化處理。

#### Stage 3 詳解 — 型別轉換

```c
signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&car1);
//                                         ↑
//                              把 Car* 轉型為 Vehicle*
```

**為什麼可以這樣轉型？**

```c
struct Car {
    Vehicle base;   // 第一個成員
    ...
};

&car1       → 指向 Car struct 的起始位址
&car1.base  → 指向 Vehicle 部分的起始位址
// 因為 Vehicle 是第一個成員，兩者的起始位址相同！
// 所以 (Vehicle *)&car1 是安全的
```

#### Stage 4 — 多型的核心展示

```c
// 同樣的語法，三種不同的執行結果
car1.base.move(&car1, 10.5f);    // → car_move()   油耗 0.84L
bus1.base.move(&bus1, 8.0f);     // → bus_move()   無油耗，考慮乘客
truck1.base.move(&truck1, 15.0f);// → truck_move() 油耗 2.25L

// 多型的本質：
// 三個物件的 .base.move 欄位指向不同的函式！
// car1.base.move    → car_move
// bus1.base.move    → bus_move
// truck1.base.move  → truck_move
```

---

## 10. 設計模式總結

### 10.1 C 語言 OOP 的三大要素對照表

| C++ 概念 | C 語言實現方式 | 本專案範例 |
|---------|-------------|---------|
| `class` | `typedef struct` | `typedef struct { ... } Car;` |
| `public` 成員 | `.h` 檔宣告的欄位和函式 | `car.h` 中宣告的所有內容 |
| `private` 成員 | `static` 變數/函式，隱藏在 `.c` 檔 | `signal_controller.c` 的 `static uint32_t signal_cycle` |
| 繼承 | struct 嵌入（第一個欄位是父類） | `struct Car { Vehicle base; ... }` |
| 虛擬函式 | 函式指標（放在 struct 中） | `funtypeVehicle_Move move;` |
| 建構子 | `#define` 巨集 + `init()` 函式 | `CAR_DEFAULTS` + `signal_controller_init()` |
| `this` | 每個方法的第一個參數 `void *self` | `void car_move(Car *self, float distance)` |
| `virtual` 呼叫 | 透過函式指標呼叫 | `car1.base.move(&car1, 10)` |
| 介面（Interface） | 只有函式指標的 struct | `ITrafficController` |
| 多型 | 不同物件的相同欄位指向不同函式 | Car/Bus/Truck 的 `move` 指向不同函式 |

### 10.2 何時使用這個模式？

**適合使用的場景**：
- 需要多個「同類型但行為不同」的物件（多種感測器、多種編碼器、多種控制器）
- 嵌入式系統（無法使用 C++，但又需要 OOP 思想）
- 希望程式碼模組化、易於擴充

**不適合使用的場景**：
- 簡單的小型程式（過度設計）
- 對執行效率要求極高的即時控制迴圈（函式指標有間接呼叫開銷）

### 10.3 開放封閉原則（Open-Closed Principle）

本設計遵循「**對擴充開放，對修改封閉**」：

**新增一種車型（Motorcycle）**：
1. 建立 `include/motorcycle.h`，定義 `Motorcycle` struct
2. 建立 `src/motorcycle.c`，實作 `motorcycle_move()` 等
3. 在 `main.c` 使用 `Motorcycle moto = MOTORCYCLE_DEFAULTS;`
4. 現有的 `Signal/ParkingController` **不需要修改**，自動支援新車型

**新增一種控制器（HighwayController）**：
1. 建立 `src/highway_controller.c`，實作介面函式
2. 在 `main.c` 呼叫 `highway_controller_init(&highway_ctrl, 3001)`
3. 現有的 `Car/Bus/Truck` **不需要修改**

---

## 11. 練習題

### 基礎練習

**Q1**：在 `car.h` 中有這段程式碼，請解釋每個 `\` 的作用：
```c
#define CAR_DEFAULTS { \
    .base = { \
```

**Q2**：為什麼 `car_move` 的第一個參數是 `Car *self` 而不是 `Vehicle *self`？

**Q3**：`static float parking_revenue = 0.0f;` 這個變數可以在 `main.c` 直接讀取嗎？為什麼？

### 進階練習

**Q4**：請新增一種車輛類型 `Motorcycle`（機車），要求：
- 最高速度 80 km/h
- 油耗 0.03 L/km
- 可以插隊（停車費只要 2 元）
- 建立 `motorcycle.h` 和 `motorcycle.c`

**Q5**：請修改 `signal_control_vehicle()`，讓救護車（`VEHICLE_TYPE_AMBULANCE`）可以在任何燈號通行，並且其他車輛需要自動讓路（印出提示訊息）。

**Q6**：`parking_control_vehicle()` 目前用線性搜尋找空位，時間複雜度是 O(n)。請設計一個改良版本，讓它用 O(1) 的時間找到空位（提示：用一個 `next_free_space` 變數追蹤下一個空位）。

### 思考題

**Q7**：本系統使用 `(Vehicle *)&car1` 這樣的型別轉換，在什麼情況下這個轉換是**不安全**的（可能產生 bug）？

**Q8**：為什麼函式指標的參數型別要用 `void *self` 而不是 `Vehicle *self`？如果改成 `Vehicle *self` 會有什麼問題？

---

## 附錄：常用語法速查

### 函式指標宣告

```c
// 宣告一個函式指標型別
typedef 回傳型別 (*型別名稱)(參數1型別, 參數2型別);

// 範例：
typedef void (*funtypeVehicle_Move)(void *self, float distance);
```

### 巨集多行定義

```c
#define 巨集名稱 {  \
    欄位1 = 值1,   \  // ← 每行結尾要有 '\' 表示繼續
    欄位2 = 值2    \  // ← 最後一行不需要 '\'，但必須緊接著 '}'
}
```

### 透過函式指標呼叫

```c
// 定義
Car car1 = CAR_DEFAULTS;         // car1.base.move 指向 car_move

// 呼叫（等效）
car1.base.move(&car1, 10);       // 透過結構體欄位呼叫
(*car1.base.move)(&car1, 10);    // 明確解引用（效果相同）
```

### void* 轉型

```c
void car_move(Car *self, float distance) {
    // self 是 void*，需要轉型才能存取 Car 的欄位
    // 注意：這裡的參數宣告已經是 Car *self，不需要手動轉型
}

// 呼叫時
car1.base.move(&car1, 10);
//              ↑
//   &car1 是 Car*，但巨集定義的型別是 void*，會自動轉換
```

---

*本教材涵蓋了 C 語言物件導向設計的核心概念，適合搭配實際編譯執行加深理解。*  
*建議先執行 `make run` 看完整輸出，再對照本文件逐步理解每個步驟。*
