set(WINDOWS_ST_CLT_PATH "C:/ST/STM32CubeCLT_1.16.0/GNU-tools-for-STM32/bin")
set(MAC_ST_CLT_PATH "/opt/ST/STM32CubeCLT/GNU-tools-for-STM32/bin/")
if(EXISTS "${WINDOWS_ST_CLT_PATH}")
    set(TOOLCHAIN_DIRECTORIES ${WINDOWS_ST_CLT_PATH})
elseif(EXISTS "${MAC_ST_CLT_PATH}")
    set(TOOLCHAIN_DIRECTORIES ${MAC_ST_CLT_PATH})
else()
    # Try to find an STM32CubeIDE installation to use for the toolchain.
    file(GLOB TOOLCHAIN_DIRECTORIES
    "C:/ST/STM32CubeIDE_*/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*/tools/bin/"
    "/opt/st/stm32cubeide_*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*/tools/bin/"
    "/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*/tools/bin/"
    )
endif()
list(LENGTH TOOLCHAIN_DIRECTORIES TOOLCHAIN_DIRECTORIES_COUNT)

if(TOOLCHAIN_DIRECTORIES_COUNT LESS 1)
    message(WARNING "Could not find an STM32CubeIDE installation. Falling back to tools available on PATH.")
else()
    list(GET TOOLCHAIN_DIRECTORIES 0 TOOLCHAIN_DIRECTORY)
    if (TOOLCHAIN_DIRECTORIES_COUNT GREATER 1)
        message(STATUS "Found multiple STM32CubeIDE installations. Using \"${TOOLCHAIN_DIRECTORY}\".")
    endif()
endif()

if(WIN32)
    set(TOOLCHAIN_SUFFIX ".exe")
endif()

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(TOOLCHAIN_PREFIX                "arm-none-eabi-")
if(DEFINED TOOLCHAIN_DIRECTORY)
    set(TOOLCHAIN_PREFIX            "${TOOLCHAIN_DIRECTORY}/${TOOLCHAIN_PREFIX}")
endif()
set(FLAGS                           "-Wl,--gc-sections -static -fdata-sections -ffunction-sections -u _printf_float --specs=nano.specs -mtune=cortex-m33 -fstack-usage -fcyclomatic-complexity -flto=auto")
set(ASM_FLAGS                       "-x assembler-with-cpp")
set(CFLAGS                          "-std=gnu17 -Wall -Wextra -Wshadow -Wcast-align -Wunused -Wsign-conversion -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough -Wduplicated-branches -Wlogical-op")
set(CPP_FLAGS                       "-std=gnu++20 -fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-use-cxa-atexit -Woverloaded-virtual -Wnon-virtual-dtor -Wsuggest-override")

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX} ${FLAGS} ${CFLAGS})
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER} ${ASM_FLAGS})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_SUFFIX} ${FLAGS} ${CPP_FLAGS})
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy${TOOLCHAIN_SUFFIX})
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size${TOOLCHAIN_SUFFIX})
set(CMAKE_OBJDUMP                   ${TOOLCHAIN_PREFIX}objdump${TOOLCHAIN_SUFFIX})
set(CMAKE_STRIP                     ${TOOLCHAIN_PREFIX}strip${TOOLCHAIN_SUFFIX})

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)