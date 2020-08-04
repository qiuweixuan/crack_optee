DEBUG = -g
EXTERN_LIB = -L./lib/openssl/lib -lcrypto -lpthread -ldl
INCLUDE = -I./include/ 
CXX = g++
TARGET = crack
SRC1 = $(wildcard ./src/*.cpp)
SRC2 = $(wildcard ./*.cpp)
OBJ1 = $(patsubst %.cpp, %.o, $(SRC1))
OBJ2 = $(patsubst %.cpp, %.o, $(SRC2))
SRC = ./src/

$(TARGET):$(OBJ1) $(OBJ2)
	$(CXX) $^ -o $@ $(EXTERN_LIB)

#编译SRC变量代表的目录下的.cpp文件
%.o:$(SRC)%.cpp
	$(CXX) $(DEBUG) -std=c++14 -c $< -o $@ $(INCLUDE)

#编译当前目录下的.cpp文件
%.o:%.cpp
	$(CXX) $(DEBUG) -std=c++14 -c $< -o $@ $(INCLUDE)

#防止外面有clean文件，阻止执行clean
.PHONY:clean

clean:
	-rm -rf $(TARGET) $(OBJ1) $(OBJ2)
