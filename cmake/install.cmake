
set(LIB_NAME qlib)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_BINDIR bin)
set(export_dest_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME})
set(config_targets_file "${LIB_NAME}ConfigTargets.cmake")
set(project_config_in "${PROJECT_SOURCE_DIR}/cmake/${LIB_NAME}Config.cmake.in")
set(project_config_out "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake")

add_library(${LIB_NAME}_header_only INTERFACE)
target_include_directories(${LIB_NAME}_header_only INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
                                                          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
# target_link_libraries(${LIB_NAME}_header_only INTERFACE spdlog::spdlog_header_only)
target_compile_definitions(${LIB_NAME}_header_only INTERFACE ${DFLAGS})

install(FILES ${PROJECT_SOURCE_DIR}/cmake/tests.cmake DESTINATION . RENAME CMakeLists.txt)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/tests DESTINATION .)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION .)
install(TARGETS ${LIB_NAME} ${LIB_NAME}_header_only
    EXPORT ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

export(TARGETS ${LIB_NAME} ${LIB_NAME}_header_only NAMESPACE ${LIB_NAME}::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${config_targets_file}")
install(EXPORT ${LIB_NAME} DESTINATION ${export_dest_dir} NAMESPACE ${LIB_NAME}:: FILE ${config_targets_file})

include(CMakePackageConfigHelpers)
configure_package_config_file("${project_config_in}" "${project_config_out}" INSTALL_DESTINATION ${export_dest_dir})
install(FILES "${project_config_out}" DESTINATION "${export_dest_dir}")
