
./template > build/main.cpp

g++ \
	-ggdb \
	--strict-aliasing \
	--pedantic \
	-fsanitize=undefined \
	-std=c++17 \
	-fopenmp \
	-I/usr/include/eigen3 \
	-lgsl -lgslcblas -lnetpbm -lpng -lubsan \
	build/main.cpp \
	-o build/main

g++ -ggdb use-gsl-array.cpp -lgsl -lgslcblas -lm -o build/use-gsl-array
