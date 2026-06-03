set(header_path "${${PROJECT_NAME}_SOURCE_DIR}/include")
set(header ${header_path}/TimedDoor.h)
set(src TimedDoor.cpp)

add_library(${PROJECT_NAME} SHARED
  ${header}
  ${src})
  
target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
         ${${PROJECT_NAME}_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
  pthread
  ${CMAKE_THREAD_LIBS_INIT})

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}.info main.cpp)
target_include_directories(${PROJECT_NAME}.info
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
         ${${PROJECT_NAME}_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}.info
  ${PROJECT_NAME}
  Threads::Threads)


install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION "${lib_dest}"
    ARCHIVE DESTINATION "${lib_dest}"
    COMPONENT library)
install(TARGETS ${PROJECT_NAME}.info
        RUNTIME DESTINATION "${tool_dest}"
    COMPONENT library)
install(FILES ${header} DESTINATION "${include_dest}")
