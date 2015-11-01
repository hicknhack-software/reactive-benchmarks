include("../common.pri")

SOURCES += \
    ContainerBench.cpp

# Microsoft RxCpp (Reactive Extensions for C++)
# https://github.com/Reactive-Extensions/RxCpp/issues/19
INCLUDEPATH += $${LIB_BASE}/RxCpp/Rx/v2/src
