#-------------------------------------------------
#
# Project created by QtCreator 
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = benchmark
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/local/include/gnuradio /usr/local/include /usr/include/python2.6
LIBS += -lboost_thread-mt -lboost_date_time-mt -lgnuradio-core -lgruel -lfftw3f -lgsl -lgslcblas -lgnuradio-usrp -lm \
        -L/usr/local/lib -lrt

SOURCES += \
    queue_watcher_thread.cpp \
    packet_utils.cpp \
    correlate_sync_word.cpp \
    string_functions.cpp \
    main.cpp \
    reception_statistics.cpp \
    scrambler.cc \
    soft_slicer.cpp \
    bpsk_hierblock.cpp \
    tx_path_hierblock.cpp \
    rx_path_hierblock.cpp \
    top_block_simulation.cpp \
    soft_frame_contor.cpp \
    short_codec.cpp \
    mod_base.cpp \
    rx_callback.cpp

HEADERS += \
    queue_watcher_thread.h \
    packet_utils.h \
    correlate_sync_word.h \
    string_functions.h \
    reception_statistics.h \
    scrambler.h \
    soft_slicer.h \
    bpsk_hierblock.h \
    tx_path_hierblock.h \
    rx_path_hierblock.h \
    rx_callback.h \
    top_block_simulation.h \
    soft_frame_contor.h \
    short_codec.h \
    mod_base.h

QMAKE_CXXFLAGS += -g -O3 -Wall -Woverloaded-virtual -pthread
