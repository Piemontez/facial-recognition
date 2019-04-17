
cmake_minimum_required(VERSION 2.8.12.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

project(FacialRecognition)

add_executable(facerecog-tester
	main.cpp
	algorithmfactory.cpp
	algorithmtest.cpp
	tester.cpp)

target_link_libraries(facerecog-tester
  ${JSONCPP_LIBRARIES})

install(TARGETS facerecog-tester DESTINATION bin)
