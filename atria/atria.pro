include("../common.pri")

# atria
INCLUDEPATH += $${LIB_BASE}/atria/src
INCLUDEPATH += $${LIB_BASE}/atria/compat
INCLUDEPATH += $${LIB_BASE}/atria/third-party/eggs-variant/include

# Boost
BOOST_DIR = $${LIB_BASE}/boost_1_55_0
INCLUDEPATH += $${BOOST_DIR}

SOURCES += \
    AtriaBench.cpp

