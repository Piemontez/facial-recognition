cmake_minimum_required(VERSION 2.8.12.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

project(FacialRecognition)

#dependencies
find_package( OpenCV REQUIRED )

#macros
add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")

add_compile_definitions(LOADER_FOLDER="")
add_compile_definitions(PREPROCESSOR_FOLDER="")
add_compile_definitions(RECOGNIZER_FOLDER="")
add_compile_definitions(TESTER_FOLDER="")


#file to builder
add_executable(facerecog-tester
	main.cpp
	algorithmfactory.cpp
	algorithmtest.cpp
	tester.cpp)

target_link_libraries(facerecog-tester
  ${JSONCPP_LIBRARIES})

install(TARGETS facerecog-tester DESTINATION bin)
