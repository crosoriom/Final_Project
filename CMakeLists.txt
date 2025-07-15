cmake_minimum_required(VERSION 3.22)

project(Final_Project)
enable_language(C ASM)

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          ARM)

set(TOOLCHAIN_PREFIX                arm-none-eabi-)
set(FLAGS                           "-fdata-sections -ffunction-sections")

set(CMAKE_C_FLAGS                   "${FLAGS} -mcpu=cortex-m4 -mthumb -O2")
set(CMAKE_ASM_FLAGS                 "-mcpu=cortex-m4 -mthumb -x assembler-with-cpp")

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)

set(CMAKE_C_STANDART                11)
set(CMAKE_C_STANDART_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/User)
include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/drivers)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/startup_stm32l476rgtx.s
    ${CMAKE_SOURCE_DIR}/drivers/ringBuffer/ringBuffer.c
    ${CMAKE_SOURCE_DIR}/drivers/keyPad/keypad.c
    ${CMAKE_SOURCE_DIR}/drivers/SSD1306/ssd1306.c
    ${CMAKE_SOURCE_DIR}/drivers/SSD1306/font.c
    ${CMAKE_SOURCE_DIR}/src/systick.c
    ${CMAKE_SOURCE_DIR}/src/syscfg.c
    ${CMAKE_SOURCE_DIR}/src/flash.c
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/gpio.c
    ${CMAKE_SOURCE_DIR}/src/exti.c
    ${CMAKE_SOURCE_DIR}/src/nvic.c
    ${CMAKE_SOURCE_DIR}/src/uart.c
    ${CMAKE_SOURCE_DIR}/src/i2c.c
    ${CMAKE_SOURCE_DIR}/src/tim.c
    ${CMAKE_SOURCE_DIR}/src/rcc.c
    ${CMAKE_SOURCE_DIR}/User/syscalls.c
    ${CMAKE_SOURCE_DIR}/User/sysmem.c
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

set(linker_script_SRC ${linker_script_SRC} ${CMAKE_SOURCE_DIR}/STM32L476RGTX_FLASH.ld)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE 
    -T${linker_script_SRC}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -u _printf_float
    --specs=nosys.specs
    -Wl,--start-group
    -lc -lm
    -Wl,--end-group
    -Wl,-z,max-page-size=8
    -Wl,--print-memory-usage
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)
