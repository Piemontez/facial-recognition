cmake_minimum_required(VERSION 2.8.12.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

project(FacialRecognition)

#dependencies
find_package( OpenCV REQUIRED )

FIND_PATH(BOOST_DIR "boost")

include_directories("${BOOST_DIR}")

#find_package(TensorFlow REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

#macros
add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")

add_compile_definitions(LOADER_FOLDER="")
add_compile_definitions(PREPROCESSOR_FOLDER="")
add_compile_definitions(RECOGNIZER_FOLDER="")
add_compile_definitions(TESTER_FOLDER="")


# determine if OpenMP can/should be used
option(USE_OPENMP "Enable OpenMP?" ON) # set to OFF to disable
if(USE_OPENMP)
 FIND_PACKAGE(OpenMP)
 if(OPENMP_FOUND OR OpenMP_FOUND)
  message(STATUS "OpenMP flags = ${OpenMP_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 else(OPENMP_FOUND OR OpenMP_FOUND)
  message(STATUS "OpenMP not supported")
 endif(OPENMP_FOUND OR OpenMP_FOUND)
endif(USE_OPENMP)

# OPENGL
# set(PLATFORM_SPECIFIC_LIBS "-lpthread")
set(OpenGL_GL_PREFERENCE "LEGACY")

find_package(OpenGL)
find_package(GLUT)

include_directories(. ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

# sources icp
FILE(GLOB LIBICP_SRC_FILES "libs/icp/*.cpp")

#file to builder
add_executable(facerecog-tester
    main.cpp
    algorithmfactory.cpp
    tester.cpp
    tools.cpp
#testadores
    tester/3dtester.cpp
    tester/irtester.cpp
    tester/vistester.cpp
#imageloader
    loader/3dloader.cpp
    loader/irloader.cpp
    loader/visloader.cpp
#processadoresimagem
    processor/gaussianblur.cpp
    processor/posecorretion.cpp
    processor/lbp.cpp
    processor/dct.cpp
    processor/roi.cpp
#reconhecedores
    recognizer/pca.cpp
    recognizer/lda.cpp
    recognizer/svm.cpp
    recognizer/lpp.cpp
#libs
    libs/ganpan.cpp
    libs/afm.cpp
    libs/icp.cpp
    libs/facedetection.cpp
    libs/3dviewer.cpp
#external
    ${LIBICP_SRC_FILES}
)

target_link_libraries(facerecog-tester
  ${OpenCV_LIBS}
  ${JSONCPP_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
  tensorflow
  #tensorflow_cc
  #tensorflow_framework
  )

install(TARGETS facerecog-tester DESTINATION bin)
