cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME HAL)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/Src/Key.c
        Core/Inc/Key.h
        Core/Src/OLED.c
        Core/Inc/OLED.h
        Core/Src/OLED_Data.c
        Core/Inc/OLED_Data.h
        Core/Src/LED.c
        Core/Inc/LED.h
        Core/Src/MyI2C.c
        Core/Inc/MyI2C.h
        Core/Src/MPU6050.c
        Core/Inc/MPU6050.h
        Core/Inc/MPU6050_Reg.h
        Core/Src/PWM.c
        Core/Inc/PWM.h
        Core/Src/Motor.c
        Core/Inc/Motor.h
        Core/Src/Encoder.c
        Core/Inc/Encoder.h
        Core/Src/Serial.c
        Core/Inc/Serial.h
        Core/Src/BlueSerial.c
        Core/Inc/BlueSerial.h
        Core/Src/PID.c
        Core/Inc/PID.h
        Core/App/Tasks/AppState.c
        Core/App/Types/AppState.h
        Core/App/Tasks/app_tasks.c
        Core/App/Types/app_tasks.h
        Core/App/Tasks/KeyTask.c
        Core/App/Types/KeyTask.h
        Core/App/Tasks/CommTask.c
        Core/App/Types/CommTask.h
        Core/App/Tasks/DisplayTask.c
        Core/App/Types/DisplayTask.h)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 应用任务模块的头文件（CubeMX 生成的那份 include 列表里没有 Core/App）。
    # 五个 .h 都在这一个目录里，所以 .c 里的 #include "Xxx.h" 不用写路径。
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/App/Types
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    m

    # Add user defined libraries
)

# 使 printf/vsprintf 支持 %f 浮点格式化
# newlib-nano（--specs=nano.specs）默认不含浮点格式化，%f 会被静默忽略、不输出任何内容
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u _printf_float)
