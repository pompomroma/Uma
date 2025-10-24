# Minimal toolchain for Xcode project generation on non-macOS
set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_OSX_SYSROOT iphoneos)
