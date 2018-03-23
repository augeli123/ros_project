cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0012 NEW)

PROJECT(Chapter1)

# Requires OpenCV
FIND_PACKAGE( OpenCV REQUIRED )
MESSAGE("OpenCV version : ${OpenCV_VERSION}")

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

ADD_EXECUTABLE(sift sift.cpp )
TARGET_LINK_LIBRARIES( sift ${OpenCV_LIBS} )

ADD_EXECUTABLE(finnal finnal.cpp )
TARGET_LINK_LIBRARIES( finnal ${OpenCV_LIBS} )

ADD_EXECUTABLE(ear_1 ear_1.cpp )
TARGET_LINK_LIBRARIES( ear_1 ${OpenCV_LIBS} )
ADD_EXECUTABLE(ear_2 ear_2.cpp )
TARGET_LINK_LIBRARIES( ear_2 ${OpenCV_LIBS} )
ADD_EXECUTABLE(ear_3 ear_3.cpp )
TARGET_LINK_LIBRARIES( ear_3 ${OpenCV_LIBS} )

