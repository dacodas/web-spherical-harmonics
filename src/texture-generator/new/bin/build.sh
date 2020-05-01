bin/template > build/main.cpp

# g++ \
# 	-ggdb \
# 	--strict-aliasing \
# 	--pedantic \
# 	-fsanitize=undefined \
# 	-std=c++17 \
# 	-fopenmp \
# 	-I/usr/include/eigen3 \
# 	-lgsl -lgslcblas -lnetpbm -lpng -lubsan \
# 	build/main.cpp \
# 	-o build/main

CXX="g++"
CFLAGS=
CXXFLAGS="-ggdb"

$CXX $CXXFLAGS src/png.cpp -lpng -c -o build/png.o
$CXX $CXXFLAGS src/serialization.cpp -c -o build/serialization.o
$CXX $CXXFLAGS -std=c++17 src/fill-and-convert.cpp -c -o build/fill-and-convert.o
$CXX $CXXFLAGS -std=c++17 -fopenmp build/png.o build/serialization.o build/fill-and-convert.o -lpng -lgsl -lgslcblas src/use-gsl-array.cpp -lstdc++fs -o build/use-gsl-array
$CXX $CXXFLAGS -fopenmp src/morlet.cpp -lgsl -lgslcblas -lpng build/*.o -o build/morlet
