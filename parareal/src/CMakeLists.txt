cmake_minimum_required(VERSION 3.5)

project (parareal)

set(TARGET_NAME "parareal")

add_executable(${TARGET_NAME} "main.cpp")
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 23)

find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
target_link_libraries(parareal ${MPI_C_LIBRARIES})

configure_file(../vis.py vis.py COPYONLY)
execute_process(COMMAND sh -c "echo \"*\" > .gitignore")
