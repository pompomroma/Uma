#!/bin/bash

# Android build script
echo "Building Racing Game 3D for Android..."

# Check for required environment variables
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "Error: ANDROID_NDK_HOME is not set"
    echo "Please set ANDROID_NDK_HOME to your Android NDK path"
    exit 1
fi

if [ -z "$ANDROID_SDK_HOME" ]; then
    echo "Error: ANDROID_SDK_HOME is not set"
    echo "Please set ANDROID_SDK_HOME to your Android SDK path"
    exit 1
fi

# Set build type (Release or Debug)
BUILD_TYPE=${1:-Release}
echo "Build type: $BUILD_TYPE"

# Create build directory
BUILD_DIR="build-android"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake for all Android ABIs
ABIS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")

for ABI in "${ABIS[@]}"; do
    echo "Building for ABI: $ABI"
    
    ABI_BUILD_DIR="android-$ABI"
    mkdir -p $ABI_BUILD_DIR
    cd $ABI_BUILD_DIR
    
    cmake ../.. \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=$ABI \
        -DANDROID_PLATFORM=android-21 \
        -DANDROID_STL=c++_static \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DANDROID=1
    
    # Build
    make -j$(nproc)
    
    cd ..
done

cd ..

# Build APK with Gradle
echo "Building APK with Gradle..."
cd android

if [ "$BUILD_TYPE" == "Release" ]; then
    ./gradlew assembleRelease
    echo "APK built at: android/app/build/outputs/apk/release/app-release.apk"
else
    ./gradlew assembleDebug
    echo "APK built at: android/app/build/outputs/apk/debug/app-debug.apk"
fi

echo "Android build complete!"