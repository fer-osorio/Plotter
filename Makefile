all: Plotter


WARNINGS = -Wall -Weffc++ -Wextra -Wsign-conversion -pedantic-errors
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
STANDARD = -std=c++2a
SOURCE = Plotter.cpp ExpressionReader/ExpressionReader.cpp BMP/Bitmap.cpp

Plotter: Makefile $(SOURCE)
	$(CXX) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(SOURCE)

clean:
	rm -f Plotter

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./Plotter

