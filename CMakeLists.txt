cmake_minimum_required(VERSION 3.16)
project(TetgenV2 LANGUAGES CXX)

option(ENABLE_VTK_VIEWER "Build VTU viewer based on VTK" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(tetgen_lib STATIC
  tetgen.cxx
  predicates.cxx
)
target_compile_definitions(tetgen_lib PUBLIC TETLIBRARY)
target_include_directories(tetgen_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nas2tet
  src/main.cpp
  src/nastran_io.cpp
)
target_include_directories(nas2tet PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(nas2tet PRIVATE tetgen_lib)

if(ENABLE_VTK_VIEWER)
  find_package(VTK QUIET COMPONENTS
    CommonCore
    CommonDataModel
    IOXML
    InteractionStyle
    RenderingCore
    RenderingOpenGL2
  )

  if(VTK_FOUND)
    add_executable(tet_view src/vtu_viewer.cpp)
    target_link_libraries(tet_view PRIVATE ${VTK_LIBRARIES})
    target_compile_definitions(tet_view PRIVATE HAS_VTK=1)
    message(STATUS "VTK found, tet_view will be built.")
  else()
    message(WARNING "VTK not found, skip tet_view target. You can still use output .vtu in ParaView.")
  endif()
endif()
