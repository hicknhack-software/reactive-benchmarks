TEMPLATE = app
CONFIG += console c++14
CONFIG -= qt

# your local path where libraries reside
LIB_BASE = C:/C/Lib

# Find GCC major version
*-g++: {
    GCC_VERSION = $$system("gcc -dumpversion")
    GCC_VERSION2 = $$split(GCC_VERSION, .)
    GCC_MAJOR = $$first(GCC_VERSION2)
    CONFIG += g++$$GCC_MAJOR
}

# Google Benchmark Library
# https://github.com/google/benchmark
INCLUDEPATH += $${LIB_BASE}/benchmark/include
win32-g++:g++4: LIBS += -L$${LIB_BASE}/benchmark/mingw4/lib
win32-g++:g++5: LIBS += -L$${LIB_BASE}/benchmark/mingw5/lib
win32-msvc2015: LIBS += -L$${LIB_BASE}/benchmark/VS2015/lib
LIBS += -lbenchmark

# Tuning options for different compilers
win32-g++ {
  # generate assembler output
  #QMAKE_CXXFLAGS += -S -fverbose-asm -g
  # enhance optimizer
  #QMAKE_CXXFLAGS_RELEASE -= -O2
  #QMAKE_CXXFLAGS_RELEASE += -O3
}
# use "CONFIG+=clang" when invoking qmake
clang {
  # enable C++14 on clang compiler
  QMAKE_CXXFLAGS += -Xclang -std=c++14
  # optimize build arguments
  QMAKE_CXXFLAGS_RELEASE = /O2 /MD /GA /fp:fast /arch:SSE2
}
win32-msvc2015:clang {
  # make clang look like MSVC2015
  QMAKE_CXXFLAGS += -fms-compatibility-version=19.00.23026
}
win32-msvc2015:!clang {
  # optimize build arguments
  QMAKE_CXXFLAGS_RELEASE = /Ox /MD /GA /GL /fp:fast /arch:SSE2
  # create assembler output
  QMAKE_CXXFLAGS += /FAus
  # profile guided optimisations
  #QMAKE_LFLAGS_RELEASE += /LTCG:PGINSTRUMENT
  #QMAKE_LFLAGS_RELEASE += /LTCG:PGOPTIMIZE
}
