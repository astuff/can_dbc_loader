cmake_minimum_required(VERSION 3.5)
project(can_dbc_loader)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(BUILD_EXAMPLES ON)

include_directories(include)

add_library(
  can_dbc_loader SHARED
  src/attribute.cpp
  src/bus_node.cpp
  src/comment.cpp
  src/signal.cpp
  src/message.cpp
  src/database.cpp
)

if(BUILD_EXAMPLES)
  add_executable(
    parse_types
    examples/parse_types.cpp
  )

  target_link_libraries(
    parse_types
    can_dbc_loader
  )

  add_executable(
    read_dbc
    examples/read_dbc.cpp
  )

  target_link_libraries(
    read_dbc
    can_dbc_loader
  )
endif()

install(
  TARGETS can_dbc_loader
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
  RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
)
