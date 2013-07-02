#! /usr/bin/env python
import os

cc = "g++"
gnuradio_includes = Split("/usr/local/include/gnuradio /usr/local/include /usr/include/python2.6")
common_includes = Split(".")
#libpath                   = Split("/usr/local/lib")
gnuradio_libs         = Split("boost_thread-mt boost_date_time-mt gnuradio-core gruel fftw3f gsl gslcblas gnuradio-usrp")
common_libs         = Split("m")
ccflags                   = Split("-O2 -Wall -Woverloaded-virtual -pthread")


env_gnuradio = Environment( CC = cc, LIBS = gnuradio_libs+common_libs, CPPPATH = gnuradio_includes+common_includes, CCFLAGS = ccflags)
env_cpp          = Environment( CC = cc, LIBS = common_libs, CPPPATH = common_includes, CCFLAGS = ccflags)

#Build correlate_sync_word block and qa

env_gnuradio.Object("correlate_sync_word", Split("correlate_sync_word.cpp"))
env_gnuradio.Object("qa_correlate_sync_word", Split("qa_correlate_sync_word.cpp"))
env_gnuradio.Program("qa_correlate_sync_word", Split("qa_correlate_sync_word.o correlate_sync_word.o"))

#Build short_codec class and qa
#

env_cpp.Object("short_codec", Split("short_codec.cpp"))
env_cpp.Object("qa_short_codec", Split("qa_short_codec.cpp"))
env_cpp.Program("qa_short_codec", Split("qa_short_codec.o short_codec.o"))

#Build soft_frame_contor and qa

env_gnuradio.Object("soft_frame_contor", Split("soft_frame_contor.cpp"))
env_gnuradio.Object("qa_soft_frame_contor", Split("qa_soft_frame_contor.cpp"))
env_gnuradio.Program("qa_soft_frame_contor", Split("soft_frame_contor.o qa_soft_frame_contor.o short_codec.o"))





