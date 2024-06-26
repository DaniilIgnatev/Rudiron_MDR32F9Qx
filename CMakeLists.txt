# This file is part of Arduino_Core_Rudiron_MDR32F9Qx.

# Arduino_Core_Rudiron_MDR32F9Qx is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Arduino_Core_Rudiron_MDR32F9Qx is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with Arduino_Core_Rudiron_MDR32F9Qx. If not, see <https://www.gnu.org/licenses/>.

# Author: Daniil Ignatev

cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(platform "MDR32F9Qx") # Аппаратная платформа
set(board_name "Rudiron_Buterbrod_R9_16MHz") # Модель платы
set(f_cpu "64000000") # Максимальная частота процессора

# Определение пути до Arduino15
if(WIN32)
        set(ARDUINO15_DIR "$ENV{USERPROFILE}/AppData/Local/Arduino15")

        # Генератор проекта
        set(CMAKE_GENERATOR "Ninja")
        set(CMAKE_MAKE_PROGRAM "${ARDUINO15_DIR}/packages/Rudiron/tools/ninja/default/ninja" CACHE FILEPATH "Path to Ninja executable")
        message("Generator is set to ${CMAKE_GENERATOR}, located in ${CMAKE_MAKE_PROGRAM}")
elseif(APPLE)
        set(ARDUINO15_DIR "$ENV{HOME}/Library/Arduino15")
        set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
else()
        set(ARDUINO15_DIR "$ENV{HOME}/.arduino15")
        set(CMAKE_GENERATOR "Ninja")
        set(CMAKE_MAKE_PROGRAM "${ARDUINO15_DIR}/packages/Rudiron/tools/ninja/default/ninja" CACHE FILEPATH "Path to Ninja executable")
        message("Generator is set to ${CMAKE_GENERATOR}, located in ${CMAKE_MAKE_PROGRAM}")
endif()

string(REPLACE "\\" "/" ARDUINO15_DIR ${ARDUINO15_DIR})
message(STATUS "Arduino15 directory: ${ARDUINO15_DIR}")

# Определение пути до последней версии ARDUIRON_CORE_RUDIRON_MDR32F9Qx (не требуется)

# Компилятор
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
enable_language(C ASM)

# Название проекта
project(Sketch)
set(OUTPUT_NAME "${PROJECT_NAME}.elf")

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
else()
        message("Build type = ${CMAKE_BUILD_TYPE}")
endif()

# Стандартные пути core
set(core_path "${CMAKE_SOURCE_DIR}/cores/arduino")
set(core_path_system "${core_path}/system")
set(core_path_system_CMSIS_Source "${core_path_system}/CMSIS/Source")
set(core_path_system_CMSIS_Include "${core_path_system}/CMSIS/Include")
set(core_path_system_StdPeriph_Driver_src "${core_path_system}/StdPeriph_Driver/src")
set(core_path_system_StdPeriph_Driver_inc "${core_path_system}/StdPeriph_Driver/inc")
set(core_path_system_StdPeriph_Driver_src_USB_Library "${core_path_system_StdPeriph_Driver_src}/USB_Library")
set(core_path_system_StdPeriph_Driver_inc_USB_Library "${core_path_system_StdPeriph_Driver_inc}/USB_Library")

# Перечень исходных файлов core
file(GLOB_RECURSE core_sources
        ${core_path}/*
        ${core_path_system}/*
        ${core_path_system_CMSIS_Source}/*
        ${core_path_system_StdPeriph_Driver_src}/*
        ${core_path_system_StdPeriph_Driver_src_USB_Library}/*
)

# Перечень путей к файлам заголовков core
set(core_includes
        ${core_path}
        ${core_path_system}
        ${core_path_system_CMSIS_Include}
        ${core_path_system_StdPeriph_Driver_inc}
        ${core_path_system_StdPeriph_Driver_inc_USB_Library}
)

# Стандартные пути variant
set(variant_path "${CMAKE_SOURCE_DIR}/variants/${board_name}")

# Перечень исходных файлов variant
file(GLOB_RECURSE variant_sources
        ${variant_path}/*
)

# Перечень путей к файлам заголовков variant
set(variant_includes
        ${variant_path}
)

# Путь до скрипта линковщика
set(LINKER_FILE "${core_path_system}/linker_scripts/${platform}.ld")

# Поиск исходных файлов в корневом каталоге
file(GLOB SKETCH_SOURCES
        *.c
        *.cpp
)

# Поиск исходных файлов в подкаталоге
set(subfolder_path "subfolder")
file(GLOB SKETCH_SUBFOLDER_SOURCES
        subfolder/*.c
        subfolder/*.cpp
)

# Путь до корневой папки библиотек
set(libraries_path "libraries")

# Подключение указанных библиотек. По возможности используется CMakeLists файл
file(STRINGS libraries.txt library_names)

foreach(lib_name ${library_names})
        set(library_path "${libraries_path}/${lib_name}")

        if(EXISTS "${library_path}/CMakeLists.txt")
                add_subdirectory(${libraries_path}/${lib_name})
                target_link_libraries(${OUTPUT_NAME} ${lib_name})
        else()
                list(APPEND libraries_includes ${library_path})
                list(APPEND libraries_includes ${library_path}/src)

                file(GLOB_RECURSE lib_sources
                        ${library_path}/*.c
                        ${library_path}/*.cpp
                )
                list(APPEND libraries_sources ${lib_sources})
        endif()
endforeach()

# Перечень путей к файлам исходного кода
add_executable(${OUTPUT_NAME}
        ${SKETCH_SOURCES}
        ${SKETCH_SUBFOLDER_SOURCES}

        ${core_sources}
        ${variant_sources}

        ${libraries_sources}
)

# Перечень путей к файлам заголовков
include_directories(
        ${subfolder_path}

        ${core_includes}
        ${variant_includes}

        ${libraries_includes}
)

# Флаги компиляции
target_compile_definitions(${OUTPUT_NAME} PUBLIC

        # USB_IRQ -- включает обработчик прерывания USB
        # HCLK_DISABLE -- работа от внутреннего кварцевого резонатора
)

# Параметры компиляции
target_compile_options(${OUTPUT_NAME} PRIVATE
        "-c"
        "-mcpu=cortex-m3"
        "-mthumb"
        "-gdwarf-2" # generate debugging information in the DWARF 2 format.
        "-MD" # automatically generate a list of dependencies
        "-MMD" # behaves similarly to -MD but does not list system headers as dependencies
        "-MP" # to generate "phantom" targets for all the prerequisites in the dependency file

        # "-MF"
        # "-Wall"
        "-mapcs-frame" # the compiler generates stack frames in a manner consistent with the ARM Procedure Call Standard
        "-mthumb-interwork" # can safely call ARM state functions from Thumb state and vice versa
        "-fdata-sections" # the compiler will place each data item into its own individual section in the output
        "-ffunction-sections" # the compiler places each function into its own individual section in the output
        "-D__STARTUP_CLEAR_BSS" # clear bss bytes
        "-D_start=main" # entry point
        "-pipe" # GCC uses pipes instead of temporary files to communicate between the various stages of the compilation
        "-fno-exceptions" # to disable exception handling in C++ programs

        # "-fno-threadsafe-statics"# tells the compiler not to generate the extra code required to ensure that the local static variable initialization is thread-safe
        "-DUSE_${platform}" # arduino platform name
        "-DF_CPU=${f_cpu}" # arduino cpu max frequency
        "-DARDUINO=10607" # arduino version
        "-DARDUINO_${board_name}" # arduino board name
        "-DARDUINO_ARCH_${platform}" # arduino arch name
)

# Уровни оптимизации
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        target_compile_options(${OUTPUT_NAME} PRIVATE
                "-Og"
                "-g"
        )
else()
        target_compile_options(${OUTPUT_NAME} PRIVATE
                "-O2"
        )
endif()

# Параметры компоновки
target_link_options(${OUTPUT_NAME} PRIVATE
        "-mcpu=cortex-m3"
        "-T${LINKER_FILE}"
        "-lc" # link against libc.a or libc.so
        "-lstdc++" # link against libstdc++.a or libstdc++.so
        "-lm" # link against libm.a or libm.so
        "-lgcc" # link againgst libgcc.a or libgcc.so
        "--specs=nano.specs" # to request the use of the "nano" version of the C library (libc)
        "-Wl,--print-memory-usage"

        # "-Wl,--cref"
        "-Wl,--check-sections"
        "-Wl,--gc-sections"
        "-Wl,--unresolved-symbols=report-all"
        "-Wl,--warn-common"

        # "-Wl,--warn-section-align"
        "-ffreestanding"
)

# Постобработка прошивки
add_custom_command(TARGET ${OUTPUT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${OUTPUT_NAME} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${OUTPUT_NAME} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${OUTPUT_NAME} ${PROJECT_NAME}.bin)

# Вывод информации о размере памяти для продвинутых пользователей
add_custom_command(TARGET ${OUTPUT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${OUTPUT_NAME})
