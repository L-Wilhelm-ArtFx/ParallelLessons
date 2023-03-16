cmake_minimum_required(VERSION 3.15)

project(Parallessons)

set(CMAKE_CXX_STANDARD 20)

file( GLOB APP_SOURCES src/lessons/*.cpp )
foreach( testsourcefile ${APP_SOURCES} )
    get_filename_component(testname ${testsourcefile} NAME_WE)
    add_executable( ${testname} ${testsourcefile} )
    target_compile_definitions(${testname} PRIVATE LESSON_NAME="${testname}")
    include_directories(src/common)
endforeach( testsourcefile ${APP_SOURCES} )