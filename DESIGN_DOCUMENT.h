/*
 * ╔════════════════════════════════════════════════════════════════════════════╗
 * ║           C 語言 OOP 城市交通管理系統 - 完整設計規劃文檔                      ║
 * ║                                                                            ║
 * ║ 作者：AI Assistant                                                         ║
 * ║ 日期：2026-05-29                                                           ║
 * ║ 概述：基於教學文檔的 OOR 概念，建立城市導向的物件模型和多型設計             ║
 * ╚════════════════════════════════════════════════════════════════════════════╝
 */

// ============================================================================
// 【第一部分】需求回顧與任務拆解
// ============================================================================

/*
【需求拆解】

主要需求：
✓ 在 C 語言中實現城市導向的 OOR 框架
✓ 利用 typedef, struct, 函式指標模擬繼承和多型
✓ 用 .h/.c 分離實現 public/private 語義
✓ 基於 PIDAW 等教學案例的概念

任務拆解（已完成所有項目）：

  ├─【階段 1】基礎物件建模
  │  ├─ vehicle.h/c        → 基類定義（相當於 C++ Vehicle class）
  │  └─ 函式指標 VTable    → 虛擬方法表
  │
  ├─【階段 2】派生類實現
  │  ├─ car.h/c            → Car 繼承 Vehicle，特有欄位：passenger_count, fuel_level
  │  ├─ bus.h/c            → Bus 繼承 Vehicle，特有欄位：passenger_count, route
  │  └─ truck.h/c          → Truck 繼承 Vehicle，特有欄位：cargo_weight
  │
  ├─【階段 3】介面層設計
  │  ├─ traffic_controller.h → ITrafficController 介面規格
  │  ├─ signal_controller.c  → 紅綠燈控制具體實現
  │  └─ parking_controller.c → 停車場管理具體實現
  │
  └─【階段 4】整合與測試
     └─ main.c              → 完整示例程式，展示多型呼叫
*/

// ============================================================================
// 【第二部分】架構設計（基於教學文檔）
// ============================================================================

/*
【設計對應關係】

教學文檔中的概念      →      本專案的實現

1. Struct 當作 Class
   ─────────────────────
   PIDAW struct           →      Vehicle struct
   - 包含資料欄位         →      id, type, speed, position...
   - 包含函式指標         →      get_type(), move(), stop(), park()

2. 函式指標 = 虛擬方法
   ─────────────────────
   funtypePIDAW_Calc     →      funtypeVehicle_Move
   funtypePIDAW_Reset    →      funtypeVehicle_Stop
   
   用法：
   C++: myPid.calc()                →   C: myPid.calc(&myPid)
   
   本案例：
   C++: myCar.move(10)              →   C: myCar.base.move(&myCar, 10)

3. 巨集初始化 = 構造函數
   ─────────────────────
   #define PIDAW_DEFAULTS { ... }  →   #define CAR_DEFAULTS { ... }
                                       #define BUS_DEFAULTS { ... }
   
   用法：
   PIDAW myPid = PIDAW_DEFAULTS;   →   Car myCar = CAR_DEFAULTS;
                                       Bus myBus = BUS_DEFAULTS;

4. 多型（Polymorphism）
   ─────────────────────
   IEncoder 介面 → 多種編碼器實現    →   ITrafficController 介面 → 多種控制器實現
   
   TamagawaEncoder                 →   SignalController
   SyntecEncoder                   →   ParkingController
   EnDatEncoder                    →   HighwayController（可擴充）

5. 資料流與狀態管理
   ─────────────────────
   【教學案例】
   編碼器 → FeedbackReporter → PIDAW(速度) → PIDAW(電流) → 馬達
   
   【本案例】
   Vehicle(多種型別) → Controller → 決策(通行/停泊/拒絕)
*/

// ============================================================================
// 【第三部分】實現細節與設計決策
// ============================================================================

/*
【1. 基類設計 - vehicle.h/c】

設計邏輯：
┌─────────────────────────────────────────────────────────────┐
│ struct Vehicle {                                            │
│     // 公開欄位（外部可訪問）                              │
│     id, type, current_speed, position_x, position_y         │
│     location, is_parked                                     │
│                                                             │
│     // 虛擬方法（函式指標）                                │
│     get_type, move, stop, park, print_info                  │
│ }                                                           │
└─────────────────────────────────────────────────────────────┘

核心原理（對應 PIDAW 模式）：
✓ 所有資料放在 struct 內 → 封裝
✓ 函式指標由子類型綁定 → 多型
✓ .h 檔只暴露介面，.c 檔隱藏實現 → public/private 分離

修改位置與理由：
- vehicle_init()
  理由：初始化通用欄位，由每個派生類呼叫（模擬 super() 建構子）
  
- 虛擬方法 = NULL
  理由：基類不提供實現，由派生類替換成具體函式指標


【2. 派生類設計 - car.h/c, bus.h/c, truck.h/c】

設計模式（包含基類）：
┌─────────────────────────────────────────┐
│ struct Car {                            │
│     Vehicle base;  ← 嵌入基類           │
│     passenger_count;  ← Car 特有欄位    │
│     fuel_level;                         │
│ }                                       │
└─────────────────────────────────────────┘

為什麼用嵌入而不是指標？
✓ 靜態記憶體配置（韌體風格，無 malloc）
✓ 便於棧分配：Car myCar = CAR_DEFAULTS;
✓ 訪問效率高：myCar.base.id（無指標解引用）

修改位置與理由：

car_move():
  理由：
  - 轎車油耗邏輯：每 km 消耗 0.08L（比重卡省油）
  - 檢查 base.is_parked 和 fuel_level
  - 計算位置變化時用不同係數（0.7x）
  - 多型體現：同一 move() 簽名，三種車的行為完全不同

truck_move():
  理由：
  - 卡車油耗邏輯：基礎油耗 * (1 + 載重/max) * 距離
  - 速度受載重影響：speed = base_speed * (1 - load_ratio * 0.6)
  - 對應實際物理：重車慢且耗油多
  - 演示邏輯複雜性：展示私有欄位可以有複雜計算

bus_move():
  理由：
  - 公車不消耗燃油（用電力）
  - 乘客多會影響速度：speed = base_speed * (1 - passenger_ratio * 0.01)
  - 對應實際場景：人多車慢


【3. 介面層設計 - traffic_controller.h】

為什麼需要介面？
✓ 上層（main.c）不需知道具體是哪個控制器
✓ 所有控制器遵循統一的方法簽名
✓ 新增控制器時，只需實現同一個介面
✓ 實現開閉原則（Open for extension, Close for modification）

設計：
┌───────────────────────────────────────────────┐
│ typedef struct {                              │
│     controller_id;                           │
│     type;  ← 標識控制器型別                  │
│     control_vehicle;  ← 核心方法指標        │
│     get_status;                              │
│     release;                                 │
│     print_info;                              │
│ } ITrafficController;                        │
└───────────────────────────────────────────────┘

這對應教學文檔中的 IEncoder 介面設計。


【4. 控制器具體實現】

signal_controller.c（紅綠燈控制）
  邏輯：
  ✓ static uint8_t current_light_state    → private 狀態
  ✓ 檢查信號燈：紅燈→拒絕，綠燈→檢查車型
  ✓ 公車優先：bus→永遠通過
  ✓ 卡車延遲：尖峰時間拒絕卡車
  ✓ 轎車正常：car→正常通過
  
  修改位置：
  - signal_control_vehicle()：根據 vehicle->type 和 current_light_state 決策
  - signal_get_status()：模擬 15 個週期的信號燈變化（紅→綠→黃）
  
  理由：展示基於物件型別的多型決策

parking_controller.c（停車場管理）
  邏輯：
  ✓ static uint8_t parking_spaces[50]     → private 停泊位陣列
  ✓ static float parking_revenue          → private 收入統計
  ✓ 分配停泊位：遍尋空位，無空位拒絕
  ✓ 收費策略：Car→5元/h，Bus→免費，Truck→20元/h
  ✓ 釋放資源：remove from array when vehicle leave
  
  修改位置：
  - parking_control_vehicle()：為車輛分配停泊位
  - parking_release()：釋放停泊位資源
  - parking_get_status()：回報可用停泊位數
  
  理由：
  - 展示狀態管理：private 陣列維護系統狀態
  - 展示資源管理：分配和釋放操作
  - 對應教學文檔中的「資源管理」設計


【5. 主程式設計 - main.c】

八個階段的設計：

Stage 1：物件建立
  Car car1 = CAR_DEFAULTS;        ← 巨集初始化（模擬 new）
  
Stage 2：控制器建立
  signal_controller_init(&signal_ctrl, 1001);
  
Stage 3：紅綠燈多型示範
  signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&car1);
  信號燈邏輯對三種車執行三種不同結果
  
Stage 4：虛擬方法多型示範
  car1.base.move(&car1, 10.5);      ← 轎車移動
  bus1.base.move(&bus1, 8.0);       ← 公車移動
  truck1.base.move(&truck1, 10.0);  ← 卡車移動
  
  「同一方法簽名，三種行為」= 多型核心
  
Stage 5：停車場管理
  parking_ctrl.control_vehicle(&parking_ctrl, (Vehicle *)&car1);
  parking_ctrl.release(&parking_ctrl, (Vehicle *)&car1);
  
Stage 6-8：資訊輸出和統計

修改點與理由：
- 型別轉換 (Vehicle *)：利用 Vehicle 是第一個欄位的特性
  理由：在 C 語言中模擬多態指標
  
- 函式呼叫方式：obj.method(&obj, ...)
  理由：C 無 this 指標，必須顯式傳遞 self
*/

// ============================================================================
// 【第四部分】多型原理的 Trace 驗證
// ============================================================================

/*
【多型執行流程追蹤】

場景：三輛不同型別的車都執行 move(10 km)

int main() {
    Car car1 = CAR_DEFAULTS;        // car1.base.type = VEHICLE_TYPE_CAR
    Bus bus1 = BUS_DEFAULTS;        // bus1.base.type = VEHICLE_TYPE_BUS
    Truck truck1 = TRUCK_DEFAULTS;  // truck1.base.type = VEHICLE_TYPE_TRUCK

    // 虛擬方法指標已在巨集初始化時綁定：
    // car1.base.move       = (funtypeVehicle_Move)car_move
    // bus1.base.move       = (funtypeVehicle_Move)bus_move
    // truck1.base.move     = (funtypeVehicle_Move)truck_move
}

【執行 car1.base.move(&car1, 10)】
   ↓
car_move((Car *)&car1, 10)  ← 透過指標，調到正確函式
   ↓
計算：
   oil_consumption = 10 * 0.08 = 0.8L
   position_x += 10 * 0.7 = 7.0
   speed = 60 km/h
   ✓ 符合轎車特性

【執行 bus1.base.move(&bus1, 8)】
   ↓
bus_move((Bus *)&bus1, 8)  ← 透過指標，調到另一個函式
   ↓
計算：
   speed_factor = 1.0 - (0 * 0.01) = 1.0  ← 基於乘客數
   position_x += 8 * 0.5 * 1.0 = 4.0
   speed = 40 * 1.0 = 40 km/h
   ✓ 不消耗燃油，速度慢

【執行 truck1.base.move(&truck1, 10)】
   ↓
truck_move((Truck *)&truck1, 10)  ← 另一個完全不同的實現
   ↓
計算：
   weight_factor = 1.0 + (0 / 10000) * 0.5 = 1.0
   speed = 50 * (1 - 0 * 0.6) = 50 km/h
   oil_consumption = 10 * 0.15 * 1.0 = 1.5L
   ✓ 空車，油耗適中

【多型的核心】
相同的呼叫：obj.base.move(&obj, distance)
不同的執行：根據 obj->base.move 指向的函式指標而決定
結果：三種完全不同的邏輯
*/

// ============================================================================
// 【第五部分】Public/Private 分離設計
// ============================================================================

/*
【.h 檔的角色（Public Interface）】

include/vehicle.h:
  ✓ 結構體定義（使用者需要知道大小以棧分配）
  ✓ 函式指標型別定義
  ✓ 巨集初始化定義
  ✓ 公開函式宣告：vehicle_init(), vehicle_destroy()
  
  ✗ 隱藏：實現細節（來自 vehicle.c）
  ✗ 隱藏：內部函式

【.c 檔的角色（Private Implementation）】

src/vehicle.c:
  ✓ 函式完整實現
  ✓ 內部只對本檔案可見的 static 函式
  ✓ 具體邏輯，外部無法直接呼叫
  
  例如：只有 .c 檔知道移動計算的公式

src/car.c:
  ✓ car_move() 的具體邏輯：油耗 0.08L/km, 加速係數 0.7
  ✓ 只有 .c 檔知道
  ✓ 外部透過 car1.base.move(&car1, 10) 間接呼叫

【C++ 對比】
C++: class Car {
     private:
         void move(float distance);  ← 編譯器強制封裝
     };
     
C:  // car.h (public interface)
    void car_move(Car *self, float distance);  ← 宣告，但無法強制
    
    // car.c (private implementation)
    static void car_move_internal(...);  ← 用 static 隱藏
    void car_move(Car *self, float distance) {  ← 外部可見
        // 邏輯
    }

【模擬私有化的技巧】

1. 靜態變數封裝狀態
   static uint8_t parking_spaces[50];      ← 只有 .c 檔可訪問
   外部無法直接修改停泊位

2. 靜態函式隱藏內部邏輯
   static float calculate_parking_fee(Vehicle *v);
   ← 只在 parking_controller.c 內部使用，外部無法呼叫

3. 介面指標抽象細節
   typedef ControlResult (*funtypeController_ControlVehicle)(void *self, Vehicle *vehicle);
   ← 外部只知道簽名，不知道實現
*/

// ============================================================================
// 【第六部分】檔案結構與編譯配置
// ============================================================================

/*
【專案結構】

OOR_template/
├── include/                    ← 公開介面（對應 .h 檔）
│   ├── vehicle.h               → 基類介面
│   ├── car.h                   → Car 派生類介面
│   ├── bus.h                   → Bus 派生類介面
│   ├── truck.h                 → Truck 派生類介面
│   └── traffic_controller.h    → 控制器介面規格
│
├── src/                        ← 實現細節（對應 .c 檔）
│   ├── vehicle.c               → 基類實現
│   ├── car.c                   → Car 實現
│   ├── bus.c                   → Bus 實現
│   ├── truck.c                 → Truck 實現
│   ├── signal_controller.c     → 紅綠燈控制實現
│   ├── parking_controller.c    → 停車場管理實現
│   └── main.c                  → 主程式
│
├── Makefile                    → 編譯配置
└── README.md                   → 說明文檔

【編譯流程】

$ make clean          ← 清理舊檔案
$ make build          ← 編譯所有 .c → .o
$ make link           ← 連結 .o → 可執行檔
$ make run            ← 執行

【編譯規則細節】

%.o: %.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
    
    ├─ $<  = 輸入檔案（.c）
    ├─ $@  = 輸出檔案（.o）
    └─ -I./include = 搜尋 include 目錄

traffic_system: $(OBJECTS)
    $(CC) $(CFLAGS) -o $@ $^
    
    └─ 連結所有 .o 檔成可執行檔
*/

// ============================================================================
// 【第七部分】設計模式總結
// ============================================================================

/*
【本設計採用的 OOP 三大核心】

1. 封裝 (Encapsulation)
   ───────────────────────
   ✓ Vehicle struct 封裝車輛所有狀態和方法
   ✓ Car struct 嵌入 Vehicle，加上 Car 特有欄位
   ✓ 私有欄位用 .c 檔隱藏，公開介面透過 .h 暴露
   
   效果：
   - 上層不需知道內部怎麼實現
   - 改變實現時，上層無需改動
   - 例：改變 car_move() 的油耗計算，main.c 無需修改

2. 繼承 (Inheritance)
   ───────────────────────
   ✓ 所有車型都包含 Vehicle 基類欄位
   ✓ Car, Bus, Truck 都有相同的虛擬方法簽名
   ✓ 子類新增自己的特有欄位（passenger_count 等）
   
   效果：
   - 程式碼重用：共享 Vehicle 的初始化邏輯
   - 多重行為：同一方法名，不同實現
   - 靈活擴充：新增車型只需複製 car.h/c 並修改

3. 多型 (Polymorphism)
   ───────────────────────
   ✓ 透過函式指標實現虛擬方法
   ✓ 同一個 move() 呼叫，三種車執行不同邏輯
   ✓ SignalController 和 ParkingController 同一個 control_vehicle() 卻有不同決策
   
   效果：
   - 通用程式碼：main.c 對所有車呼叫 move()，不用 if-else
   - 決策靈活：新增控制器無需修改舊程式
   - 表現力強：客碳邏輯直觀

【C 與 C++ 對比】

C++:
  class Vehicle {
  public:
      virtual void move(float d) = 0;
      virtual void stop() = 0;
  };
  
  class Car : public Vehicle {
  public:
      void move(float d) override { ... }
      void stop() override { ... }
  };
  
  Car myCar;
  myCar.move(10);  ← 編譯器自動查詢虛擬表

C:
  typedef struct {
      Vehicle base;
  } Car;
  
  Car myCar = CAR_DEFAULTS;
  myCar.base.move(&myCar, 10);  ← 手動透過指標呼叫
  
  ✗ 繁瑣但更透明
  ✓ 理解多型的精髓
  ✓ 嵌入式環境常用
*/

// ============================================================================
// 【第八部分】擴充示例（如何新增功能）
// ============================================================================

/*
【情景 1：新增一種控制器 - HighwayController】

步驟 1：在 traffic_controller.h 新增型別
  typedef enum {
      ...
      CONTROLLER_TYPE_HIGHWAY = 3
  } ControllerType;

步驟 2：建立 highway_controller.c
  static uint16_t vehicle_count = 0;
  
  ControlResult highway_control_vehicle(void *self, Vehicle *vehicle) {
      // 高速公路邏輯：限制卡車速度，禁止行人
      if (vehicle->type == VEHICLE_TYPE_CAR) {
          vehicle->current_speed = 120;  // 限速 120
      } else if (vehicle->type == VEHICLE_TYPE_TRUCK) {
          vehicle->current_speed = 80;   // 卡車限速 80
      }
      vehicle_count++;
      return CONTROL_RESULT_SUCCESS;
  }
  
  void highway_controller_init(ITrafficController *c, uint16_t id) {
      c->controller_id = id;
      c->type = CONTROLLER_TYPE_HIGHWAY;
      c->control_vehicle = highway_control_vehicle;
      ...
  }

步驟 3：在 main.c 使用
  ITrafficController highway_ctrl;
  highway_controller_init(&highway_ctrl, 3001);
  
  highway_ctrl.control_vehicle(&highway_ctrl, (Vehicle *)&car1);

✓ 完全無需修改舊代碼，體現開閉原則


【情景 2：新增一種車型 - MotorCycle】

步驟 1：建立 motorcycle.h
  typedef struct {
      Vehicle base;
      uint8_t has_helmet;
  } MotorCycle;
  
  #define MOTORCYCLE_DEFAULTS { ... }
  
  VehicleType motorcycle_get_type(MotorCycle *self);
  void motorcycle_move(MotorCycle *self, float distance);
  ...

步驟 2：建立 motorcycle.c
  void motorcycle_move(MotorCycle *self, float distance) {
      // 機車邏輯：速度快，省油，靈活
      self->fuel_level -= distance * 0.03f;  // 超省油！
      self->current_speed = 80.0f;
      ...
  }

步驟 3：在 main.c 使用
  MotorCycle moto1 = MOTORCYCLE_DEFAULTS;
  moto1.base.move(&moto1, 50);  // 自動呼叫 motorcycle_move()
  
  signal_ctrl.control_vehicle(&signal_ctrl, (Vehicle *)&moto1);
  // SignalController 無需修改，自動支援新車型！

✓ 真正的開放封閉原則
*/

// ============================================================================
// 【第九部分】常見問題與設計決策】
// ============================================================================

/*
Q1: 為什麼用 struct 而不是指標？

A: struct 嵌入而非指標分配
   
   ❌ 不推薦（指標方式）:
      Car *car1 = malloc(sizeof(Car));  ← 動態記憶體
      car1->move(car1, 10);
   
   ✓ 推薦（嵌入方式）:
      Car car1 = CAR_DEFAULTS;          ← 棧分配
      car1.base.move(&car1, 10);
   
   理由：
   - 韌體環境通常避免 malloc（不確定性）
   - 棧分配更快，更安全
   - 編譯時大小已知，無需運行時分配
   - 回收自動（出作用域自動清空）


Q2: 為什麼要 (Vehicle *)car1 類型轉換？

A: Vehicle 是第一個欄位，記憶體地址相同
   
   struct Car {
       Vehicle base;  ← 第一個欄位
       int fuel;
   };
   
   &car1 的地址 == &car1.base 的地址
   
   因此：(Vehicle *)&car1 == &car1.base
   
   ✓ 利用這個特性模擬多態指標


Q3: 為什麼要多層級的 get_status()？

A: 不同控制器的狀態不同
   
   SignalController::get_status()
   ← 回報信號燈顏色（紅/黃/綠）
   
   ParkingController::get_status()
   ← 回報可用停泊位數
   
   透過相同的方法簽名，回報不同的狀態
   = 多型的又一個體現


Q4: 為什麼參數是 void *self？

A: 通用介面設計
   
   typedef ControlResult (*funtypeController_ControlVehicle)(
       void *self, Vehicle *vehicle
   );
   
   ✓ void *self 允許任何型別的控制器傳入
   ✓ 內部強制轉型：ITrafficController *controller = (ITrafficController *)self;
   ✓ 實現通用介面的靈活性


Q5: 如何實現私有方法？

A: 使用 static 限制檔案作用域
   
   在 car.c:
   static void car_calculate_fuel_consumption(Car *self, float distance) {
       // 只有 car.c 內部能呼叫
   }
   
   在 car.h 中不宣告此函式
   
   ✓ 外部無法呼叫
   ✓ 編譯器只暴露非 static 函式
*/

// ============================================================================
// 【總結】
// ============================================================================

/*
本設計示例展示了如何在 C 語言中實現完整的 OOP 模式：

✅ 【成就】
  ✓ struct + 函式指標實現了 C++ 的虛擬方法
  ✓ 巨集初始化實現了構造函數的語義
  ✓ 多型呼叫：同一方法，三種不同行為
  ✓ 介面分離：控制器可獨立實現
  ✓ 私有化：用 static 和 .c 檔隱藏細節
  ✓ 可擴充：新增車型或控制器無需改動舊代碼

📚 【對應教學文檔的概念】
  ✓ PIDAW 的 struct 封裝模式
  ✓ 函式指標虛擬方法表 (VTable)
  ✓ 巨集 DEFAULTS 初始化
  ✓ IEncoder 多型介面設計
  ✓ 資料流單向鏈式處理

🎓 【學習價值】
  ✓ 理解 C 語言 OOP 的核心：透明性與靈活性
  ✓ 嵌入式系統常用設計模式
  ✓ 為學習 C++ 打好基礎
  ✓ 架構設計思維訓練
*/
