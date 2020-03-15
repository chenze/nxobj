cmake_minimum_required(VERSION 3.13)

project(nxobj)

add_executable(test_nxobj examples/nxobj.c src/nxobj.c src/regex.c)
add_executable(test_regex examples/regex.c src/nxobj.c src/regex.c)