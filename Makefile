# Mini-Redis Makefile

CXX     = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET  = mini-redis
SRCS    = main.cpp server.cpp store.cpp parser.cpp expiry.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	@echo "Build successful! Run with: ./mini-redis"

clean:
	rm -f $(TARGET) appendonly.log
	@echo "Cleaned build files and log"

run: all
	./$(TARGET)
