CXX      = clang++
CXXFLAGS = -std=c++17 -O2 -pthread

TARGET = false_sharing
SRC    = false_sharing.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
