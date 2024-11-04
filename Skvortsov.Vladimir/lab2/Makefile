# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17

# Qt5 библиотеки и флаги
QT_LIBS = `pkg-config --cflags --libs Qt5Widgets`

# Исходные файлы
SRC_DIR = src
SRC_FILES = $(SRC_DIR)/host.cpp $(SRC_DIR)/client.cpp $(SRC_DIR)/conn_queue.cpp $(SRC_DIR)/conn_fifo.cpp $(SRC_DIR)/conn_socket.cpp

# GUI файлы
GUI_SRC_DIR = $(SRC_DIR)/gui
GUI_SRC_FILES = $(GUI_SRC_DIR)/ChatWindow.cpp $(GUI_SRC_DIR)/main.cpp

# Заголовочные файлы
INCLUDE_DIR = include

# Объектные файлы
OBJ_FILES = $(SRC_FILES:.cpp=.o)
GUI_OBJ_FILES = $(GUI_SRC_FILES:.cpp=.o)

# Исполняемые файлы
BIN_DIR = bin
HOST_EXEC = $(BIN_DIR)/host
CLIENT_EXEC = $(BIN_DIR)/client
GUI_EXEC = $(BIN_DIR)/chat_gui

# Цели
all: $(HOST_EXEC) $(CLIENT_EXEC) $(GUI_EXEC)

$(HOST_EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(CLIENT_EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(GUI_EXEC): $(GUI_OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(QT_LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) -I$(INCLUDE_DIR)

clean:
	rm -rf $(OBJ_FILES) $(GUI_OBJ_FILES) $(BIN_DIR)

.PHONY: all clean
