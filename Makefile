# ============================================================================
# Makefile - 城市交通管理系統編譯配置
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
INCLUDES = -I./include

# 原始檔案
SOURCES = src/vehicle.c src/car.c src/bus.c src/truck.c \
          src/signal_controller.c src/parking_controller.c src/main.c

# 目的檔案
OBJECTS = $(SOURCES:.c=.o)

# 執行檔
TARGET = traffic_system

# ============================================================================
# 編譯規則
# ============================================================================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "✓ 編譯完成: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "✓ 編譯: $<"

# ============================================================================
# 清理規則
# ============================================================================

clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "✓ 已清理目的檔案"

run: $(TARGET)
	./$(TARGET)

help:
	@echo "使用方法："
	@echo "  make          - 編譯程式"
	@echo "  make run      - 編譯並執行"
	@echo "  make clean    - 清理編譯產物"
	@echo "  make help     - 顯示此幫助訊息"

.PHONY: all clean run help
