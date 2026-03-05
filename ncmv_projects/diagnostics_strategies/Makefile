CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2 -g
INCLUDES := -I.

TARGET   := gnss_diag_test
SRCS     := test_gnss_diag.cpp

LIBS     := -lm

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) gnss_diagnostics.h gnss_diagnostics_impl.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
