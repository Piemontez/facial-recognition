cmake_minimum_required(VERSION 2.8.12.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

project(FacialRecognition)

#dependencies
find_package( OpenCV REQUIRED )

#find_package(TensorFlow REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

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
    tester.cpp
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
)

target_link_libraries(facerecog-tester
  ${OpenCV_LIBS}
  ${JSONCPP_LIBRARIES}
  tensorflow
  #tensorflow_cc
  #tensorflow_framework
  )

install(TARGETS facerecog-tester DESTINATION bin)
