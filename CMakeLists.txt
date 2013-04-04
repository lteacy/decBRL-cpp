########################################
# set up the project basic information #
########################################

# project name
PROJECT(DECBRL-CPP)

# minimum cmake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# add or remove debugging info
SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE Release)

# verbose makefile
SET(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# output directory for binaries and libraries
SET(BIN ${CMAKE_SOURCE_DIR}/bin)
SET(LIB ${CMAKE_SOURCE_DIR}/lib)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.40.0 COMPONENTS random)
find_package(Eigen3 3.1.0)

###########################################
# Generate Documentation                  #
###########################################
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
   ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMENT "Generating API documentation with Doxygen" VERBATIM
   )
endif(DOXYGEN_FOUND)

###########################################
# Dump exec and libraries in the same place
###########################################

# set the output directories
SET(LIBRARY_OUTPUT_PATH ${LIB} CACHE PATH "Output directory for the libraries")
SET(EXECUTABLE_OUTPUT_PATH ${BIN} CACHE PATH "Output directory for the executables")
SET(MAXSUM_ROOT ${CMAKE_SOURCE_DIR}/../maxsum-cpp)

# allow linking to these directories
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS} ${MAXSUM_ROOT}/include ${EIGEN3_INCLUDE_DIR})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH} ${MAXSUM_ROOT}/lib)

############################
# make an uninstall target #
############################

#CONFIGURE_FILE(
# 	"${CMAKE_CURRENT_SOURCE_DIR}/UninstallScript.cmake.in"
#  	"${CMAKE_CURRENT_BINARY_DIR}/UninstallScript.cmake"
#  	IMMEDIATE @ONLY
#)

#ADD_CUSTOM_TARGET(uninstall
#  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/UninstallScript.cmake")

#############################
# compile project library   #
#############################
FILE(GLOB DEC_BRL_SRC src/*.cpp)
ADD_LIBRARY(DecBRL SHARED ${DEC_BRL_SRC})
TARGET_LINK_LIBRARIES(DecBRL ${Boost_LIBRARIES})

###############################
# build test harnesses        #
###############################
ADD_EXECUTABLE(mdpHarness tests/mdpHarness.cpp)
ADD_EXECUTABLE(bqMDPHarness tests/bqMDPHarness.cpp)
ADD_EXECUTABLE(bqFacMDPHarness tests/bqFacMDPHarness.cpp)
ADD_EXECUTABLE(bmMDPHarness tests/bmMDPHarness.cpp)
ADD_EXECUTABLE(bmFacMDPHarness tests/bmFacMDPHarness.cpp)
ADD_EXECUTABLE(facMDPHarness tests/facMDPHarness.cpp)
ADD_EXECUTABLE(randHarness tests/randHarness.cpp)
ADD_EXECUTABLE(vpiHarness tests/vpiHarness.cpp)
ADD_EXECUTABLE(vecHarness tests/vecHarness.cpp)
ADD_EXECUTABLE(colourHarness tests/colourHarness.cpp)
ADD_EXECUTABLE(BoostArrayHarness tests/BoostArrayHarness.cpp)
ADD_EXECUTABLE(DirichletHarness tests/DirichletHarness.cpp)
ADD_EXECUTABLE(TransBeliefHarness tests/TransBeliefHarness.cpp)
TARGET_LINK_LIBRARIES(mdpHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(bqMDPHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(bqFacMDPHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(bmMDPHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(bmFacMDPHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(facMDPHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(randHarness DecBRL)
TARGET_LINK_LIBRARIES(vpiHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(vecHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(colourHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(BoostArrayHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(DirichletHarness MaxSum DecBRL)
TARGET_LINK_LIBRARIES(TransBeliefHarness MaxSum DecBRL)

###############################
# enable testing              #
###############################
ENABLE_TESTING()
ADD_TEST(VEC_TEST ${CMAKE_SOURCE_DIR}/bin/vecHarness)
ADD_TEST(SINGLE_MDP_QLEARNING_TEST ${CMAKE_SOURCE_DIR}/bin/mdpHarness Testing/Temporary/singleQLearning.csv)
ADD_TEST(SINGLE_MDP_BAYESQ_TEST ${CMAKE_SOURCE_DIR}/bin/bqMDPHarness Testing/Temporary/singleBayesQ.csv)
ADD_TEST(SINGLE_MDP_MODEL_BAYES_TEST ${CMAKE_SOURCE_DIR}/bin/bmMDPHarness Testing/Temporary/singleModelBayes.csv)
ADD_TEST(FACTORED_MDP_QLEARNING_TEST ${CMAKE_SOURCE_DIR}/bin/facMDPHarness Testing/Temporary/facQLearning.csv)
ADD_TEST(FACTORED_MDP_BAYESQ_TEST ${CMAKE_SOURCE_DIR}/bin/bqFacMDPHarness Testing/Temporary/facBayesQ.csv)
ADD_TEST(FACTORED_MDP_MODEL_BAYES_TEST ${CMAKE_SOURCE_DIR}/bin/bmFacMDPHarness Testing/Temporary/facModelBayes.csv)
ADD_TEST(RAND_TEST ${CMAKE_SOURCE_DIR}/bin/randHarness)
#ADD_TEST(VPI_TEST ${CMAKE_SOURCE_DIR}/bin/vpiHarness)
ADD_TEST(COLOUR_TEST ${CMAKE_SOURCE_DIR}/bin/colourHarness)
ADD_TEST(BOOST_ARRAY_TEST ${CMAKE_SOURCE_DIR}/bin/BoostArrayHarness)
ADD_TEST(DIRICHLET_TEST ${CMAKE_SOURCE_DIR}/bin/DirichletHarness)
ADD_TEST(TRANS_BELIEF_TEST ${CMAKE_SOURCE_DIR}/bin/TransBeliefHarness)

