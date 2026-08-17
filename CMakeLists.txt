cmake_minimum_required(VERSION 3.12)

project(Eclipsea LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 自动收集源代码（src + project）
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/project/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 引入 OpenCore 引擎（基于新 OpenCore 26.1，git submodule）
add_subdirectory(modules/OpenCore)

target_include_directories(${PROJECT_NAME}
PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    OpenCore
    SDL3_mixer
)

if(WIN32)

    target_sources(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/resources/resource.rc"
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

        COMMENT "Copying assets to exe directory"
    )

    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_SOURCE_DIR}/build"
    )

endif()

if(PSP)

    find_package(PkgConfig REQUIRED)

    pkg_search_module(SDL3 REQUIRED sdl3)
    pkg_search_module(SDL3_IMAGE REQUIRED SDL3_image)
    pkg_search_module(SDL3_TTF REQUIRED SDL3_ttf)
    pkg_search_module(SDL3_MIXER REQUIRED SDL3_mixer)

    target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${SDL3_INCLUDE_DIRS}
        ${SDL3_IMAGE_INCLUDE_DIRS}
        ${SDL3_TTF_INCLUDE_DIRS}
        ${SDL3_MIXER_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SDL3_LIBRARIES}
        ${SDL3_IMAGE_LIBRARIES}
        ${SDL3_TTF_LIBRARIES}
        ${SDL3_MIXER_LIBRARIES}
    )

    if(COMMAND create_pbp_file)

        create_pbp_file(
            TARGET          ${PROJECT_NAME}
            ICON_PATH       ${CMAKE_SOURCE_DIR}/resources/icon0.png
            BACKGROUND_PATH ${CMAKE_SOURCE_DIR}/resources/pic0.png
            PREVIEW_PATH    NULL
            TITLE           "Eclipsea"
            VERSION         01.00
        )

    endif()

endif()

if(ANDROID OR TERMUX)

    find_package(PkgConfig REQUIRED)

    pkg_search_module(SDL3 REQUIRED sdl3)
    pkg_search_module(SDL3_IMAGE REQUIRED SDL3_image)
    pkg_search_module(SDL3_TTF REQUIRED SDL3_ttf)
    pkg_search_module(SDL3_MIXER REQUIRED SDL3_mixer)

    target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${SDL3_INCLUDE_DIRS}
        ${SDL3_IMAGE_INCLUDE_DIRS}
        ${SDL3_TTF_INCLUDE_DIRS}
        ${SDL3_MIXER_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${SDL3_LIBRARIES}
        ${SDL3_IMAGE_LIBRARIES}
        ${SDL3_TTF_LIBRARIES}
        ${SDL3_MIXER_LIBRARIES}
    )

endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/" DESTINATION .)

set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/build/release")

add_custom_target(EXTRACT
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Exporting game to ${EXPORT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}"
        --prefix "${EXPORT_DIR}"
    COMMENT "Exporting clean game directory"
)
