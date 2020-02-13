find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

add_executable( ${PROJECT_NAME}
  main.cpp
  mainwindow.cpp
  board.cpp
  board_area.cpp
  pattern.cpp
  )

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/patterns" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})
