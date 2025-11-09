cmake_minimum_required(VERSION 3.12)

project(Eclipsea CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 内存优化编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wl,--stack,268435456")
    # 减少调试信息以节省内存
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g1")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /stack:268435456")
endif()

# 使用 CONFIGURE_DEPENDS 自动更新文件列表
file(GLOB_RECURSE SOURCE_FILES 
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/project/*.cpp"
)

# 头文件目录
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 设置目标属性以进一步优化内存使用
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

if(WIN32)
    # SDL2 库链接
    set(SDL2_LIBRARIES mingw32 SDL2main SDL2 SDL2_mixer SDL2_ttf SDL2_image)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

    # 加上图标（即resources连接）
    set(RESOURCE_OBJECT "${CMAKE_SOURCE_DIR}/resources/resource.o")
    if(EXISTS ${RESOURCE_OBJECT})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${RESOURCE_OBJECT})
    else()
        message(WARNING "Resource object file not found: ${RESOURCE_OBJECT}")
    endif()

    # 拷贝 assets 文件夹到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                "${CMAKE_SOURCE_DIR}/assets" 
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying game assets to build directory"
    )

    # 设置输出目录
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
    )
endif()

if(PSP)
    # 查找 SDL2 和 SDL2_mixer
    find_package(PkgConfig REQUIRED)

    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)

    # 添加头文件路径
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )

    # 链接库
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )

    # PSP 特定配置
    if(COMMAND create_pbp_file)
        create_pbp_file(
            TARGET ${PROJECT_NAME}
            ICON_PATH ${CMAKE_SOURCE_DIR}/resources/icon0.png
            BACKGROUND_PATH ${CMAKE_SOURCE_DIR}/resources/pic0.png
            PREVIEW_PATH NULL
            TITLE "Eclipsea"
            VERSION 01.00
        )
    endif()
endif()

# 可选的：添加编译特性检测
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    # 使用现代目标包含目录语法
    target_include_directories(${PROJECT_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    )
endif()

# 可选的：并行编译支持
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
    # 检测处理器核心数
    include(ProcessorCount)
    ProcessorCount(NUM_PROCESSORS)
    if(NUM_PROCESSORS EQUAL 0)
        set(NUM_PROCESSORS 1)
    endif()
    
    # 设置并行编译
    if(CMAKE_GENERATOR MATCHES "Makefiles")
        set(CMAKE_BUILD_PARALLEL_LEVEL ${NUM_PROCESSORS})
    endif()
endif()

# 编译后信息显示
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed: $<TARGET_FILE:${PROJECT_NAME}>"
)

# 安装目标（可选）
if(NOT PSP AND NOT MSVC)
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )
    
    install(DIRECTORY assets/
        DESTINATION share/eclipsea
    )
endif()