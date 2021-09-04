# -*- cmake -*-

cmake_minimum_required(VERSION 3.10)

project(dsremap-proxy VERSION 1.0)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

find_package(PkgConfig REQUIRED)

# We should also find_package nlohmann::json but the RPi OS package
# does not include the necessary cmake files...

pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(BLUEZ REQUIRED bluez)
pkg_check_modules(ZLIB REQUIRED zlib)
pkg_check_modules(SOUP REQUIRED libsoup-2.4)

option(ENABLE_ASAN "Enable address sanitizer" OFF)
if (ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif ()

option(USE_SPDLOG "Use spdlog for logging" ON)
if (USE_SPDLOG)
  find_package(spdlog REQUIRED)
  add_definitions(-DUSE_SPDLOG)
endif ()

add_compile_options(-Wall -Wno-psabi)
add_definitions(-DENABLE_TIMING)
add_definitions(-DTARGET_PC)
add_definitions(-DCONFIG_PATH="/opt/dsremap/bytecode.bin")
add_definitions(-DVERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${VERSION_MINOR})
add_definitions(-DVERSION_PATCH=${VERSION_PATCH})

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/../arduino
  ${GLIB_INCLUDE_DIRS}
  ${BLUEZ_INCLUDE_DIRS}
  ${SOUP_INCLUDE_DIRS}
  )
link_directories(dsremap-proxy
  ${GLIB_LIBRARY_DIRS}
  ${BLUEZ_LIBRARY_DIRS}
  ${SOUP_LIBRARY_DIRS}
  )

add_library(dsremap-lib STATIC
  src/utils/format.h
  src/utils/format.cpp
  src/utils/File.cpp
  src/utils/File.h
  src/utils/Logger.cpp
  src/utils/Logger.h
  src/utils/Application.cpp
  src/utils/Application.h
  src/utils/BytecodeFile.cpp
  src/utils/BytecodeFile.h

  src/bluetooth/BTUtils.cpp
  src/bluetooth/BTUtils.h
  src/bluetooth/BluetoothAcceptor.cpp
  src/bluetooth/BluetoothAcceptor.h
  src/bluetooth/SDPClientFactory.cpp
  src/bluetooth/SDPClientFactory.h
  src/bluetooth/SonyControllerClientFactory.cpp
  src/bluetooth/SonyControllerClientFactory.h
  src/bluetooth/BTDevice.h
  src/bluetooth/BTDevice.cpp
  src/bluetooth/Proxy.h
  src/bluetooth/Proxy.cpp

  src/usb/DescriptorParser.cpp
  src/usb/DescriptorParser.h
  src/usb/USBDevice.cpp
  src/usb/USBDevice.h
  src/usb/Endpoint.cpp
  src/usb/Endpoint.h
  src/usb/OutEndpoint.cpp
  src/usb/OutEndpoint.h
  src/usb/InEndpoint.cpp
  src/usb/InEndpoint.h
  src/usb/ControlEndpoint.cpp
  src/usb/ControlEndpoint.h
  src/usb/Interface.cpp
  src/usb/Interface.h
  src/usb/HIDInterface.cpp
  src/usb/HIDInterface.h
  src/usb/AudioInterface.cpp
  src/usb/AudioInterface.h
  src/usb/DummyAudioInterface.cpp
  src/usb/DummyAudioInterface.h
  src/usb/FunctionFS.cpp
  src/usb/FunctionFS.h
  src/usb/Dualshock4.cpp
  src/usb/Dualshock4.h
  src/usb/Dualshock4HIDInterface.cpp
  src/usb/Dualshock4HIDInterface.h
  src/usb/Dualsense.cpp
  src/usb/Dualsense.h
  src/usb/DualsenseHIDInterface.cpp
  src/usb/DualsenseHIDInterface.h
  )

add_executable(dsremap-proxy
  proxy.cpp

  src/Dualshock4Proxy.cpp
  src/Dualshock4Proxy.h
  src/AuthChallenge.cpp
  src/AuthChallenge.h

  src/HTTPServer.cpp
  src/HTTPServer.h

  ${PROJECT_SOURCE_DIR}/../arduino/dsremap/VM.h
  ${PROJECT_SOURCE_DIR}/../arduino/dsremap/VM.cpp
  ${PROJECT_SOURCE_DIR}/../arduino/dsremap/IMUIntegrator.h
  ${PROJECT_SOURCE_DIR}/../arduino/dsremap/IMUIntegrator.cpp
  )

add_executable(psctrl
  psctrl.cpp
  )

target_link_libraries(dsremap-proxy
  dsremap-lib
  ${GLIB_LIBRARIES}
  ${BLUEZ_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${SOUP_LIBRARIES}
  rt
  pthread
  )

target_link_libraries(psctrl
  dsremap-lib
  ${GLIB_LIBRARIES}
  ${BLUEZ_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${SOUP_LIBRARIES}
  rt
  pthread
  )

if (USE_SPDLOG)
  target_link_libraries(dsremap-proxy spdlog::spdlog)
  target_link_libraries(psctrl spdlog::spdlog)
endif ()

add_custom_target(installer ALL
  DEPENDS dsremap-proxy
  COMMAND ${CMAKE_COMMAND} -E make_directory dist
  COMMAND ${CMAKE_COMMAND} -E make_directory dist/root
  COMMAND ${CMAKE_COMMAND} -E make_directory dist/root/opt
  COMMAND ${CMAKE_COMMAND} -E make_directory dist/root/opt/dsremap
  COMMAND ${CMAKE_COMMAND} -E make_directory dist/root/etc
  COMMAND ${CMAKE_COMMAND} -E make_directory dist/root/etc/init.d
  COMMAND ${CMAKE_COMMAND} -E copy dsremap-proxy dist/root/opt/dsremap/dsremap-proxy
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/dsremap-initscript dist/root/etc/init.d/dsremap
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/post.sh dist/post.sh
  COMMAND ${PROJECT_SOURCE_DIR}/build-installer.sh ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
  )
