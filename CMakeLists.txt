# 指定最小版本
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(Eclipsea CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 在 Windows 下编译
if(WIN32)
    # 编译资源包
    set(RESOURCE_RC "${CMAKE_SOURCE_DIR}/resource.rc")
    set(RESOURCE_OBJ "${CMAKE_BINARY_DIR}/resource.o")

    add_custom_command(
        OUTPUT ${RESOURCE_OBJ}
        COMMAND ${CMAKE_RC_COMPILER} ${RESOURCE_RC} -O coff -o ${RESOURCE_OBJ} --use-temp-file
        DEPENDS ${RESOURCE_RC}
        COMMENT "Compiling resource.rc"
    )
endif()

# 递归所有源代码文件
file(GLOB_RECURSE SOURCE_FILES
    #根目录
    "${CMAKE_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/engine/*.cpp"
)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(WIN32)
    add_custom_target(resource_build DEPENDS ${RESOURCE_OBJ})
    add_dependencies(Eclipsea resource_build)
    target_link_options(Eclipsea PRIVATE ${RESOURCE_OBJ})
endif()

# SDL2 路径（可自定义或使用 find_package）
set(SDL2_LIBRARIES mingw32 SDL2main SDL2 SDL2_image)

# 链接 SDL2 库
target_link_libraries(Eclipsea PRIVATE ${SDL2_LIBRARIES})

# 定义源目录和目标构建目录
set(ENVIRONMENT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/data")
set(ENVIRONMENT_DEST_DIR "${CMAKE_SOURCE_DIR}/../build")

# 递归复制 environment 的内容（不包括 environment 文件夹本身）
add_custom_command(TARGET Eclipsea POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "正在复制游戏资源文件到构建目录（../build）"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ENVIRONMENT_DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENVIRONMENT_SOURCE_DIR}" "${ENVIRONMENT_DEST_DIR}"
    COMMENT "Post-build: Copying environment directory contents to ../build"
)