TEMPLATE = subdirs

SUBDIRS += Loop Container # baselines
SUBDIRS += rxCpp rxMini   # reactive Extension APIs
SUBDIRS += MyRange        # range based APIs

# Transducer APIs
SUBDIRS += trMini

!win32-msvc2015: SUBDIRS += atria # atria does not work with MSVC
clang: SUBDIRS += atria           # but it works with clang faking MSVC

# yield C++17 extension
win32-msvc2015:!clang: SUBDIRS += yield
