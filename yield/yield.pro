include("../common.pri")

SOURCES += \
    YieldBench.cpp

win32-msvc2015:!clang {
  # activate experimental "yield", "async" and "await"
  QMAKE_CXXFLAGS += /await
}
