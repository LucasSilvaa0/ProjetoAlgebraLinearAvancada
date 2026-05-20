# Usage:
# make run FILE=grafico.cpp

CXX = g++
CXXFLAGS = -O2 -std=c++17
PY_INC = $(shell python3-config --includes)
NUMPY_INC = -I$(shell python3 -c "import numpy; print(numpy.get_include())")
PY_LDFLAGS = $(shell python3-config --embed --ldflags)

TARGET = $(basename $(FILE))

run:
ifndef FILE
	$(error Usage: make run FILE=your_file.cpp)
endif
	$(CXX) $(FILE) -o $(TARGET) $(CXXFLAGS) $(PY_INC) $(NUMPY_INC) $(PY_LDFLAGS)
	./$(TARGET)

clean:
	rm -f $(TARGET)