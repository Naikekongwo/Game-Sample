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

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# SDL2 库链接
set(SDL2_LIBRARIES mingw32 SDL2main SDL2 SDL2_mixer)
target_link_libraries(Eclipsea PRIVATE ${SDL2_LIBRARIES})

# 加上图标（即resources连接）
set(RESOURCE_OBJECT "${CMAKE_SOURCE_DIR}/resources/resource.o")
target_link_libraries(${PROJECT_NAME} PRIVATE ${RESOURCE_OBJECT})


# 拷贝 assets 文件夹
set(ENVIRONMENT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ENVIRONMENT_DEST_DIR "${CMAKE_SOURCE_DIR}/build")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "正在复制游戏资源文件到构建目录（../build）"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ENVIRONMENT_DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENVIRONMENT_SOURCE_DIR}" "${ENVIRONMENT_DEST_DIR}"
    COMMENT "Post-build: Copying environment directory contents to ../build"
)
