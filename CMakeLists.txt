cmake_minimum_required(VERSION 2.8.3)
project(bluefox2_capture)
project(bluefox2_test)
project(blueflow)
project(videocapture)
project(videosample)
project(grid)
project(px4flow)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(mvIMPACT_LIBRARIES "opt/mvIMPACT_acquire")


FILE(GLOB SOURCE "./*.cpp")
FILE(GLOB SOURCE "./bluefox/*.cpp")
 
ADD_EXECUTABLE(bluefox2_capture ${SOURCE} bluefox_capture.cpp)
ADD_EXECUTABLE(bluefox2_test ${SOURCE} bluefox_test.cpp)
ADD_EXECUTABLE(blueflow ${SOURCE} blueflow.cpp)
ADD_EXECUTABLE(videocapture ${SOURCE} VideoCapture.cpp)
ADD_EXECUTABLE(videosample ${SOURCE} VideoSample.cpp)
ADD_EXECUTABLE(grid ${SOURCE} main.cpp)
ADD_EXECUTABLE(px4flow ${SOURCE} px4flow.cpp)



find_package(OpenCV REQUIRED)
find_package(mvIMPACT REQUIRED)
find_package(Threads)

TARGET_LINK_LIBRARIES(bluefox2_test ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(bluefox2_test ${mvIMPACT_LIBRARIES})
TARGET_LINK_LIBRARIES(bluefox2_test ${CMAKE_THREAD_LIBS_INIT})


TARGET_LINK_LIBRARIES(bluefox2_capture ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(bluefox2_capture ${mvIMPACT_LIBRARIES})


TARGET_LINK_LIBRARIES(blueflow ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(blueflow ${mvIMPACT_LIBRARIES})


TARGET_LINK_LIBRARIES(videocapture ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(videocapture ${mvIMPACT_LIBRARIES})


TARGET_LINK_LIBRARIES(videosample ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(videosample ${mvIMPACT_LIBRARIES})


TARGET_LINK_LIBRARIES(grid ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(grid ${mvIMPACT_LIBRARIES})


TARGET_LINK_LIBRARIES(px4flow ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(px4flow ${mvIMPACT_LIBRARIES})





# dynamic reconfigure
# generate_dynamic_reconfigure_options(cfg/Bluefox2Dyn.cfg)

# services
#add_service_files(FILES SetExposeSrv.srv)
#generate_messages(DEPENDENCIES std_msgs)


include_directories(${mvIMPACT_INCLUDE_DIRS})
include_directories("./bluefox")
MESSAGE(STATUS 'mvIMPACT_LIBRARIES: ${mvIMPACT_LIBRARIES}')
MESSAGE(STATUS 'mvIMPACT_LIBRARY_DIR: ${mvIMPACT_LIBRARY_DIR}')
MESSAGE(STATUS 'OpenCV_LIBS: ${OpenCV_LIBS}')

#add_subdirectory(src)

# dependencies
