/*
 * ╔════════════════════════════════════════════════════════════════════════════╗
 * ║         城市交通管理系統 - C 語言 OOP 實現指南                               ║
 * ║                    Implementation Guide                                   ║
 * ╚════════════════════════════════════════════════════════════════════════════╝
 */

// ============================================================================
// 【1】文件結構快速導覽
// ============================================================================

/*
總文件清單：

├─ include/
│  ├─ vehicle.h                ← 基類介面（12 KB）
│  ├─ car.h                    ← Car 派生類介面
│  ├─ bus.h                    ← Bus 派生類介面
│  ├─ truck.h                  ← Truck 派生類介面
│  └─ traffic_controller.h     ← 控制器介面規格
│
├─ src/
│  ├─ vehicle.c                ← 基類實現（機制）
│  ├─ car.c                    ← Car 實現（轎車邏輯）
│  ├─ bus.c                    ← Bus 實現（公車邏輯）
│  ├─ truck.c                  ← Truck 實現（卡車邏輯）
│  ├─ signal_controller.c      ← 紅綠燈控制（決策邏輯 1）
│  ├─ parking_controller.c     ← 停車場管理（決策邏輯 2）
│  └─ main.c                   ← 完整示例（整合測試）
│
├─ Makefile                    ← 編譯配置
├─ DESIGN_DOCUMENT.h           ← 詳細設計說明
└─ IMPLEMENTATION_GUIDE.h      ← 本文件
*/

// ============================================================================
// 【2】核心類實現對照表
// ============================================================================

/*
【Vehicle 基類】（對應 PIDAW）
┌────────────────────────────────────────────────────────────────┐
│ struct Vehicle {                                               │
│   uint16_t id;                  ← 識別碼                      │
│   VehicleType type;             ← 車輛型別（ENUM）           │
│   float current_speed;          ← 目前速度                    │
│   float position_x, position_y; ← 位置座標                    │
│                                                                │
│   /* 虛擬方法 (VTable) */                                     │
│   funtypeVehicle_GetType get_type;                             │
│   funtypeVehicle_Move move;     ← 多型核心                   │
│   funtypeVehicle_Stop stop;                                    │
│   funtypeVehicle_Park park;                                    │
│   funtypeVehicle_PrintInfo print_info;                         │
│ }                                                              │
│                                                                │
│ 初始化：                                                        │
│   Vehicle v;                                                   │
│   vehicle_init(&v, 101, VEHICLE_TYPE_CAR);                     │
│   v.move = (funtypeVehicle_Move)car_move;  ← 綁定實現       │
└────────────────────────────────────────────────────────────────┘

【Car 派生類】（新增特有欄位）
┌────────────────────────────────────────────────────────────────┐
│ struct Car {                                                   │
│   Vehicle base;           ← 嵌入基類，享受虛擬方法           │
│   uint8_t passenger_count;                                     │
│   float fuel_level;                                            │
│ }                                                              │
│                                                                │
│ 初始化：                                                        │
│   Car myCar = CAR_DEFAULTS;  ← 巨集初始化（自動綁定 car_move） │
│   myCar.base.id = 101;                                         │
│   myCar.base.move(&myCar, 10);  ← 呼叫虛擬方法             │
└────────────────────────────────────────────────────────────────┘

【Bus 派生類】（不同的多型行為）
┌────────────────────────────────────────────────────────────────┐
│ struct Bus {                                                   │
│   Vehicle base;           ← 繼承虛擬方法                      │
│   uint16_t passenger_count;                                    │
│   uint8_t route_number;   ← 路線號，Car 沒有                 │
│ }                                                              │
│                                                                │
│ 多型體現：                                                      │
│   Car 和 Bus 都執行 move(10)，但邏輯完全不同！                 │
│   - Car: oil -= distance * 0.08      ← 消耗燃油                │
│   - Bus: speed = 40 * (1 - passenger_ratio)  ← 考慮乘客數    │
└────────────────────────────────────────────────────────────────┘

【Truck 派生類】（最複雜的多型實現）
┌────────────────────────────────────────────────────────────────┐
│ struct Truck {                                                 │
│   Vehicle base;                                                │
│   float cargo_weight;     ← 貨物重量影響速度和油耗           │
│   float fuel_level;                                            │
│   int8_t cargo_type[32];                                       │
│ }                                                              │
│                                                                │
│ 邏輯特殊性：                                                    │
│   truck_move(10) {                                             │
│       油耗 = 10 * 0.15 * (1 + cargo_ratio * 0.5)             │
│       速度 = base_speed * (1 - cargo_ratio * 0.6)            │
│   }                                                            │
│   = 實際物理模型，非簡單計算                                    │
└────────────────────────────────────────────────────────────────┘
*/

// ============================================================================
// 【3】多型原理三步走
// ============================================================================

/*
【Step 1】定義虛擬方法型別
────────────────────────────
在 vehicle.h:
    typedef void (*funtypeVehicle_Move)(void *self, float distance);
    
    → 這是「方法簽名」，不是實現
    → 所有車型都遵循同一個簽名

【Step 2】結構體包含函式指標
────────────────────────────
在 vehicle.h:
    struct Vehicle {
        ...
        funtypeVehicle_Move move;  ← 函式指標，不是函式
        ...
    };

    → struct 裡存指標，不存實現
    → 允許動態綁定

【Step 3】初始化時綁定具體實現
────────────────────────────
在 car.h:
    #define CAR_DEFAULTS { \
        .base = { \
            ... \
            (funtypeVehicle_Move)car_move,  ← 繫結到 car.c 的 car_move() \
            ... \
        }, \
        ... \
    }

    在 car.c:
    void car_move(Car *self, float distance) {
        // 轎車特有邏輯
        self->fuel_level -= distance * 0.08f;
        ...
    }

【執行時的多型】
────────────────────────────
    Car myCar = CAR_DEFAULTS;
    Bus myBus = BUS_DEFAULTS;
    
    // 呼叫同一方法名，執行不同邏輯！
    myCar.base.move(&myCar, 10);   ← 執行 car_move()
    myBus.base.move(&myBus, 10);   ← 執行 bus_move()
    
    ✓ 多型完成！
*/

// ============================================================================
// 【4】控制器設計模式（介面層）
// ============================================================================

/*
【為什麼需要 ITrafficController 介面？】

問題：
  - SignalController 和 ParkingController 邏輯完全不同
  - 但都要控制車輛
  - main.c 怎麼統一呼叫？

解法：定義通用介面
  
typedef struct {
    controller_id;
    control_vehicle;      ← 通用方法簽名
    get_status;
    release;
    print_info;
} ITrafficController;

【實現方式】

SignalController:
  ControlResult signal_control_vehicle(void *self, Vehicle *vehicle) {
      if (light_state == RED) return DENIED;
      if (vehicle->type == BUS) return SUCCESS;
      ...
  }

ParkingController:
  ControlResult parking_control_vehicle(void *self, Vehicle *vehicle) {
      if (find_free_space() == -1) return DENIED;
      allocate_space(vehicle);
      return SUCCESS;
  }

【通用呼叫（在 main.c）】

void apply_controller(ITrafficController *ctrl, Vehicle *vehicle) {
    ControlResult result = ctrl->control_vehicle(ctrl, vehicle);
    // ← 不知道 ctrl 實際是什麼，自動執行正確邏輯！
}

apply_controller(&signal_ctrl, &car1);   ← 執行信號燈邏輯
apply_controller(&parking_ctrl, &car1);  ← 執行停車場邏輯

✓ 完全的介面驅動，實現獨立
*/

// ============================================================================
// 【5】實現修改清單（詳細版本控制）
// ============================================================================

/*
【修改位置 1】vehicle.h
→ 定義基類結構和虛擬方法類型
→ 修改理由：建立 OOP 的基礎
→ 關鍵點：函式指標必須是 void *self 類型，便於多型轉型

【修改位置 2】vehicle.c
→ 實現 vehicle_init() 初始化基類欄位
→ 修改理由：模擬 base class constructor
→ 邏輯：初始化所有欄位為 0 或 NULL，由子類覆蓋

【修改位置 3】car.h
→ 定義 Car struct，嵌入 Vehicle base
→ 新增 passenger_count, fuel_level 特有欄位
→ 定義 CAR_DEFAULTS 巨集，綁定 car_move 等實現
→ 修改理由：展示「包含」式繼承和虛擬方法綁定

【修改位置 4】car.c
→ 實現 car_move() 邏輯：
   油耗 = distance * 0.08
   位置變化 = distance * 0.7（加速係數）
   速度 = 60 km/h（巡航速度）
→ 修改理由：
   - 轎車的物理模型
   - 多型體現：與 bus_move/truck_move 邏輯完全不同

【修改位置 5】bus.c & truck.c
→ 同樣實現各自的 *_move() 函式
→ 邏輯差異：
   Bus: 考慮乘客數影響速度，不消耗燃油
   Truck: 考慮貨物重量，複雜的油耗計算
→ 修改理由：展示真實多型複雜性

【修改位置 6】traffic_controller.h
→ 定義 ITrafficController 介面結構
→ 包含虛擬方法指標：control_vehicle, get_status 等
→ 修改理由：
   - 解耦控制器實現
   - 允許新增控制器無需修改舊代碼

【修改位置 7】signal_controller.c
→ 實現紅綠燈邏輯：
   - 基於信號燈狀態決策
   - 根據車型優先級
   - 尖峰時間限制
→ 修改理由：
   - 演示決策控制器
   - 展示私有狀態（signal_cycle, current_light_state）

【修改位置 8】parking_controller.c
→ 實現停車場邏輯：
   - 管理停泊位陣列
   - 根據車型計算費用
   - 分配/釋放資源
→ 修改理由：
   - 演示資源管理
   - 展示 private static 變數的用法

【修改位置 9】main.c
→ 8 個階段的整合示例：
   1. 物件建立
   2. 控制器初始化
   3. 紅綠燈多型示範
   4. 虛擬方法多型示範
   5. 停車場管理
   6. 資訊列印
   7. 特有方法呼叫
   8. 離開停車場
→ 修改理由：
   - 展示完整的使用流程
   - 驗證所有多型邏輯
   - 可作為教學示例
*/

// ============================================================================
// 【6】設計決策與權衡
// ============================================================================

/*
【決策 1】為什麼用 void *self 而不是具體型別？

選項 A（具體型別）:
    ❌ void car_move(Car *self, float distance);
       void bus_move(Bus *self, float distance);
    問題：
    - 每種車的函式簽名都不同
    - 介面無法統一
    - 無法實現通用 controller

選項 B（void 指標）:
    ✓ typedef void (*funtypeVehicle_Move)(void *self, float distance);
    優點：
    - 所有型別都能使用相同的函式指標
    - 介面統一
    - 實現真正的多型
    - 內部用強制轉型：Car *car = (Car *)self;


【決策 2】為什麼巨集初始化而不是 init 函式？

選項 A（函式初始化）:
    ❌ Car *myCar = malloc(sizeof(Car));
       car_init(myCar);
    問題：
    - 動態記憶體，不安全
    - 需要手動釋放
    - 嵌入式環境不適

選項 B（巨集初始化）:
    ✓ Car myCar = CAR_DEFAULTS;
    優點：
    - 棧分配，自動生命週期
    - 編譯時檢查
    - 零開銷抽象
    - 函式指標自動綁定


【決策 3】為什麼要多層級的 get_status()？

選項 A（無 get_status）:
    ❌ SignalController 自己維護狀態
       main.c 無法查詢
    問題：
    - 上層無法瞭解控制器狀態
    - 難以除錯

選項 B（統一的 get_status）:
    ✓ int signal_get_status(void *self);
       int parking_get_status(void *self);
    優點：
    - 介面統一
    - 上層可查詢狀態
    - 易於監測和除錯
    - SignalController 回報信號燈顏色
    - ParkingController 回報可用位數
*/

// ============================================================================
// 【7】編譯與執行
// ============================================================================

/*
【編譯步驟】

$ cd d:\coding\C\OOR_template

1. 清理舊檔案：
   $ make clean
   
2. 編譯所有原始碼：
   $ make
   
3. 或直接執行（自動編譯）：
   $ make run

【編譯輸出】

編譯每個 .c 檔：
  gcc -Wall -Wextra -std=c99 -g -I./include -c src/vehicle.c -o src/vehicle.o
  gcc -Wall -Wextra -std=c99 -g -I./include -c src/car.c -o src/car.o
  ...

連結所有 .o 檔：
  gcc -Wall -Wextra -std=c99 -g -o traffic_system src/*.o

結果：
  traffic_system.exe（或 traffic_system 在 Linux）

【執行程式】

$ ./traffic_system

輸出內容：
  [第一階段] 建立車輛物件
  [第二階段] 建立交通控制器
  [第三階段] 多型演示 - 通過紅綠燈
  [第四階段] 虛擬方法演示 - 不同車輛的移動邏輯
  [第五階段] 停車場管理
  [第六階段] 物件資訊總覽
  [第七階段] 車輛特有操作
  [第八階段] 車輛離開停車場
  [最終統計] 停車場狀態
*/

// ============================================================================
// 【8】擴充指南（如何添加新功能）
// ============================================================================

/*
【擴充 1】添加新車型 - Taxi（計程車）

步驟 1：建立 include/taxi.h
    typedef struct {
        Vehicle base;
        float meter_rate;     // 計費標準
        float total_revenue;  // 今日收益
    } Taxi;
    
    #define TAXI_DEFAULTS { ... }
    VehicleType taxi_get_type(Taxi *self);
    void taxi_move(Taxi *self, float distance);
    ...

步驟 2：建立 src/taxi.c
    void taxi_move(Taxi *self, float distance) {
        // 計程車邏輯
        self->fuel_level -= distance * 0.10f;
        self->current_speed = 50.0f;  // 市區速度
        self->total_revenue += distance * self->meter_rate;
    }

步驟 3：在 main.c 使用
    Taxi myTaxi = TAXI_DEFAULTS;
    myTaxi.base.move(&myTaxi, 5);
    signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&myTaxi);
    
✓ 完全無需修改 SignalController 或 ParkingController！


【擴充 2】添加新控制器 - TrafficFlowOptimizer

步驟 1：建立 src/flow_optimizer.c
    ControlResult flow_control_vehicle(void *self, Vehicle *vehicle) {
        // AI 邏輯：預測擁塞，動態調整
        if (predicted_congestion > 80%) {
            return CONTROL_RESULT_DENIED;  // 引導走其他路線
        }
        return CONTROL_RESULT_SUCCESS;
    }
    
    void flow_optimizer_init(ITrafficController *ctrl, uint16_t id) {
        ctrl->controller_id = id;
        ctrl->type = CONTROLLER_TYPE_OPTIMIZER;
        ctrl->control_vehicle = flow_control_vehicle;
        ...
    }

步驟 2：在 main.c 使用
    ITrafficController flow_ctrl;
    flow_optimizer_init(&flow_ctrl, 4001);
    
    flow_ctrl.control_vehicle(&flow_ctrl, (Vehicle *)&truck1);
    
✓ 自動與現有系統整合


【擴充 3】添加新特性 - 燃油站管理

步驟 1：建立 fuel_station.h/c
    typedef struct {
        ITrafficController base;  // 繼承控制器介面
        float fuel_available;
    } FuelStation;
    
    ControlResult fuel_station_control(void *self, Vehicle *v) {
        FuelStation *station = (FuelStation *)self;
        
        if (v->type == CAR) {
            Car *car = (Car *)v;
            if (car->fuel_level < 10.0f) {
                car_refuel(car, 20.0f);
                station->fuel_available -= 20.0f;
            }
        }
        return CONTROL_RESULT_SUCCESS;
    }

✓ 輕鬆添加新功能
*/

// ============================================================================
// 【9】關鍵代碼片段速查
// ============================================================================

/*
【虛擬方法呼叫】
    Car myCar = CAR_DEFAULTS;
    myCar.base.move(&myCar, 10.0f);  ← 呼叫虛擬方法
    // 自動執行 car_move() 中的邏輯

【型別轉換（多型指標）】
    Vehicle *vPtr = (Vehicle *)&car1;  ← 向上轉型
    vPtr->move(vPtr, 10.0f);           ← 仍執行 car_move()

【介面呼叫】
    ITrafficController *ctrl = &signal_ctrl;
    ctrl->control_vehicle(ctrl, (Vehicle *)&car1);

【狀態查詢】
    int available = parking_ctrl.get_status(&parking_ctrl);

【資源釋放】
    parking_ctrl.release(&parking_ctrl, (Vehicle *)&car1);

【資訊輸出】
    car1.base.print_info(&car1);  ← 虛擬方法
*/

// ============================================================================
// 【10】常見錯誤與除錯技巧
// ============================================================================

/*
【錯誤 1】忘記初始化函式指標
    ❌ Car myCar = {0};  // 未初始化
       myCar.base.move(&myCar, 10);  // NULL 指標呼叫，CRASH!
    
    ✓ Car myCar = CAR_DEFAULTS;  // 自動初始化所有指標

【錯誤 2】型別轉換錯誤
    ❌ signal_ctrl.control_vehicle(&parking_ctrl, ...);
       // 傳錯控制器！
    
    ✓ signal_ctrl.control_vehicle(&signal_ctrl, ...);
       // 傳遞自己

【錯誤 3】混淆 base.move 和 move
    ❌ Car myCar = CAR_DEFAULTS;
       myCar.move(&myCar, 10);  // Car 無 move 欄位！
    
    ✓ myCar.base.move(&myCar, 10);  // base 中才有 move

【錯誤 4】忘記取地址符
    ❌ car1.base.move(car1, 10);  // 應傳指標
    
    ✓ car1.base.move(&car1, 10);

【除錯技巧】
    1. 加入日誌輸出：
       printf("[DEBUG] 呼叫 %s\n", vehicle->type == VEHICLE_TYPE_CAR ? "car_move" : "其他");
    
    2. 檢查指標非空：
       if (self->move == NULL) {
           printf("[ERROR] move 指標未初始化\n");
           return;
       }
    
    3. 追蹤狀態變化：
       printf("[TRACE] 油量從 %.2f 變為 %.2f\n", old_fuel, new_fuel);
*/

// ============================================================================
// 【終極總結】快速參考卡
// ============================================================================

/*
┌─────────────────────────────────────────────────────────────────────┐
│ 【C 語言 OOP 實現速查表】                                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 【概念】          【C++ 方式】        【C 方式】                      │
│ ─────────────────────────────────────────────────────────────────  │
│ 基類          class Vehicle       struct Vehicle                 │
│ 派生類        class Car : public Vehicle    struct { Vehicle base; }
│ 虛擬方法      virtual void move()    typedef void (*move)(void *)
│ 方法呼叫      car.move()             car.base.move(&car)         │
│ 構造函數      Car() { ... }          #define CAR_DEFAULTS { ... }
│ 多型指標      Vehicle *p = &car     (Vehicle *)&car              │
│ 介面          class IEncoder       struct ITrafficController    │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│ 【編譯命令速查】                                                     │
│ ─────────────────────────────────────────────────────────────────  │
│ $ make           # 編譯                                             │
│ $ make run       # 編譯並執行                                       │
│ $ make clean     # 清理                                             │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│ 【檔案對應】                                                         │
│ ─────────────────────────────────────────────────────────────────  │
│ .h 檔   = 公開介面（用戶可見）                                       │
│ .c 檔   = 私有實現（用戶不見）                                       │
│ static = 限制檔案作用域（模擬 private）                              │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
*/
