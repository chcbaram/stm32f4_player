cmake_minimum_required(VERSION 3.12)

set(PRJ_NAME stm32f4_player)


project(${PRJ_NAME}
  LANGUAGES ASM C CXX
)


set(EXECUTABLE ${PRJ_NAME}.elf)


file(GLOB SRC_FILES CONFIGURE_DEPENDS
  src/bsp/*.c
  src/hw/*.c
  src/hw/driver/*.c
  src/hw/driver/hangul/*.c
  src/hw/driver/lcd/*.c  
  src/hw/driver/usb/*.c
  src/hw/driver/usb/core/*.c
  src/hw/driver/usb/usb_cdc/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
  src/ap/*.cpp
  src/ap/*.c
  src/common/*.c    
  lib/STM32F4xx_HAL_Driver/Src/*.c
)

add_executable(${EXECUTABLE} 
  src/main.cpp
  src/bsp/startup/startup_stm32f411ceux.s

  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
)



target_include_directories(${EXECUTABLE} PRIVATE 
  src  
  src/ap
  src/bsp
  src/common 
  src/common/core
  src/common/hw/include 
  src/hw
  lib

  src/hw/driver/usb
  src/hw/driver/usb/core
  src/hw/driver/usb/usb_cdc
  src/hw/driver/usb/usb_msc
  
  lib/CMSIS/Include
  lib/CMSIS/Device/ST/STM32F4xx/Include
  lib/CMSIS/DSP/Include
  lib/STM32F4xx_HAL_Driver/Inc
)

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DSTM32F411xE
  -DARM_MATH_CM4
  -DLFS_THREADSAFE
  )

target_compile_options(${EXECUTABLE} PRIVATE
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard

  -fdata-sections
  -ffunction-sections

  -Wall

  -O0
  )

target_link_options(${EXECUTABLE} PRIVATE
  -T../src/bsp/ldscript/STM32F411CEUX_FLASH.ld
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16 
  -mfloat-abi=hard
  -specs=nano.specs
  -lc
  -lm
  -lstdc++
  -lsupc++
  -lnosys
  -Wl,-Map=${PRJ_NAME}.map,--cref
  -Wl,--gc-sections
  -Xlinker -print-memory-usage -Xlinker
  )


add_custom_command(TARGET ${EXECUTABLE} 
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
    COMMENT "Invoking: Make Binary"
    )

#set_target_properties(${EXECUTABLE} PROPERTIES SUFFIX ".elf")
#add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
#        COMMAND arm-none-eabi-size --format=berkeley --totals "$<TARGET_FILE:${EXECUTABLE}>"
#        COMMENT "Invoking: Cross ARM GNU Print Size")