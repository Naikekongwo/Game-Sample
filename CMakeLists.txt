cmake_minimum_required(VERSION 3.12)
project(Eclipsea CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 使用 CONFIGURE_DEPENDS 自动更新文件列表
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS 
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
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
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

if(TERMUX OR ANDROID)
    # 在 Termux (Android) 环境中查找和链接 SDL2 库
    find_package(PkgConfig REQUIRED)
    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
    pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
    
    # 添加头文件路径
    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
    )
    
    # 链接库
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
    )
    
    # 拷贝 assets 文件夹到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
        COMMENT "Copying game assets to build directory"
    )
endif()

# 编译后信息显示
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed: $<TARGET_FILE:${PROJECT_NAME}>"
)