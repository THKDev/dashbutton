TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += asio/asio/include

SOURCES += \
        main.cc

HEADERS += \
    asio_ethernet.hpp
