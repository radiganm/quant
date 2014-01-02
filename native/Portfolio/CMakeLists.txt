## CMakeLists.txt
## Mac Radigan

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_PREFIX_PATH /home/mac/sandbox/Markowitz-v2/Markowitz-Demo/Markowitz-C++/build/cmake)
set(CMAKE_MODULE_PATH /home/mac/sandbox/Markowitz-v2/Markowitz-Demo/Markowitz-C++/build/cmake)

project(Markowitz)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
file(MAKE_DIRECTORY ./bin ./lib)

#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#configure_file(Doxygen.in bin/Doxyfile @ONLY)
#add_custom_target(doc doxygen bin/Doxyfile WORKING_DIRECTORY bin  COMMENT "Generating API documentation with Doxygen" VERBATIM)
#endif(DOXYGEN_FOUND)

list(APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")

#find_package(Armadillo REQUIRED)
find_package(Log4cxx REQUIRED)

set(Boost_ADDITIONAL_VERSIONS "1.41" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0")
set(BOOST_ROOT "$ENV{HOME}/usr")
find_package(Boost 1.41 COMPONENTS program_options property_tree filesystem regex REQUIRED) 
include_directories(./include ../System/include /usr/include/root /opt/octave/include/octave-3.6.4 ${BOOST_INCLUDE_DIR} ${LOG4CXX_INCLUDE_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib ${Boost_LIBRARY_DIRS} /usr/lib64/root)
add_library(markowitz SHARED ./src/Series.cxx ./src/Portfolio.cxx ./src/Figure.cxx)
add_executable(./bin/markowitz ./src/markowitz.cxx)
target_link_libraries(./bin/markowitz markowitz boost_program_options boost_system boost_filesystem Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix MathCore Thread m dl armadillo blas lapack log4cxx)

enable_testing()
add_test(testThread ./bin/markowitz -f ./portfolios/portfolio-AAPL_JPM_LMT_XOM.xml)
#add_test(testUnit1 ./test/test1.cxx)
#add_test(testUnit2 ./test/test2.py)

## *EOF*
