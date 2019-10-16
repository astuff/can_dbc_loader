cmake_minimum_required(VERSION 3.5)
project(can_dbc_loader)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

include_directories(include)

add_library(
  can_dbc_loader SHARED
  src/database.cpp
)

install(
  TARGETS can_dbc_loader
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
)
