cmake_minimum_required(VERSION 3.10)

project(Eclipsea CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_EXTENSIONS OFF)

# 源文件
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/project/*.cpp"
)
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/project/include")

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


if(WIN32)
    # SDL2 库链接
    set(SDL2_LIBRARIES mingw32 SDL2main SDL2 SDL2_mixer SDL2_ttf SDL2_image)
    target_link_libraries(Eclipsea PRIVATE ${SDL2_LIBRARIES})


    # 加上图标（即resources连接）
    set(RESOURCE_OBJECT "${CMAKE_SOURCE_DIR}/resources/resource.o")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RESOURCE_OBJECT})


    # 拷贝 assets 文件夹
    set(ENVIRONMENT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
    set(ENVIRONMENT_DEST_DIR "${CMAKE_SOURCE_DIR}/build")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying game assets to build directory..."
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ENVIRONMENT_DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENVIRONMENT_SOURCE_DIR}" "${ENVIRONMENT_DEST_DIR}"
        COMMENT "Post-build: Copying environment directory contents to ../build"
    )
endif()

if(PSP)
    # 查找 SDL2 和 SDL2_mixer
    find_package(PkgConfig REQUIRED)

    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)


    # 添加头文件路径
    target_include_directories(${PROJECT_NAME} PRIVATE
        include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
    )

    # 链接库
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )

    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH ${CMAKE_SOURCE_DIR}/resources/icon0.png
        BACKGROUND_PATH ${CMAKE_SOURCE_DIR}/resources/pic0.png
        PREVIEW_PATH NULL
        TITLE "Eclipsea"
        VERSION 01.00
    )
endif()
