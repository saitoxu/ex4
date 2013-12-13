CXX = g++
TARGET = kadai1_1
CXXFLAGS = -Wall
SRCS = kadai1_1.cpp training_data.cpp svm.cpp quadprog++.cc
OBJS := kadai1_1.o training_data.o svm.o quadprog++.o

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS)
