cmake_minimum_required(VERSION 3.12)

project (parareal)

set(TARGET_NAME "parareal")

add_executable(${TARGET_NAME} "main.cpp")
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 23)

#find_package(Boost 1.66 COMPONENTS mpi REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS}) 
#target_link_libraries(parareal ${Boost_LIBRARIES})

if(APPLE)
add_compile_options(-Xpreprocessor -fopenmp -lomp)
endif()

find_package(OpenMP REQUIRED)
target_link_libraries(parareal PUBLIC OpenMP::OpenMP_CXX)

configure_file(../vis.py vis.py COPYONLY)
execute_process(COMMAND sh -c "echo \"*\" > .gitignore")
