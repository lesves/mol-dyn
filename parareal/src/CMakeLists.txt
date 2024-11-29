cmake_minimum_required(VERSION 3.12)

project (parareal)

set(TARGET_NAME "parareal")

add_executable(${TARGET_NAME} "main.cpp")
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)

if(APPLE)
add_compile_options(-Xpreprocessor -fopenmp -lomp)
endif()

find_package(OpenMP REQUIRED)
target_link_libraries(${TARGET_NAME} PUBLIC OpenMP::OpenMP_CXX)

include(FetchContent)
#set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY "https://github.com/jarro2783/cxxopts.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(cxxopts)
target_link_libraries( ${TARGET_NAME} PUBLIC cxxopts )

configure_file(../vis.py vis.py COPYONLY)
execute_process(COMMAND sh -c "echo \"*\" > .gitignore")
