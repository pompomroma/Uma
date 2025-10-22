#!/bin/bash

# Mobile Build Script for Racing Game 3D
# Supports Android and iOS builds

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [android|ios|both] [debug|release]"
    echo ""
    echo "Platforms:"
    echo "  android  - Build for Android (requires Android SDK/NDK)"
    echo "  ios      - Build for iOS (requires Xcode, macOS only)"
    echo "  both     - Build for both platforms"
    echo ""
    echo "Build types:"
    echo "  debug    - Debug build with symbols"
    echo "  release  - Optimized release build"
    echo ""
    echo "Examples:"
    echo "  $0 android debug"
    echo "  $0 ios release"
    echo "  $0 both release"
}

# Function to check prerequisites
check_prerequisites() {
    local platform=$1
    
    log_info "Checking prerequisites for $platform..."
    
    case $platform in
        android)
            if ! command -v gradle &> /dev/null; then
                log_error "Gradle not found. Please install Android Studio or Gradle."
                return 1
            fi
            
            if [ -z "$ANDROID_HOME" ]; then
                log_error "ANDROID_HOME not set. Please set up Android SDK."
                return 1
            fi
            
            if [ -z "$ANDROID_NDK_HOME" ] && [ -z "$NDK_ROOT" ]; then
                log_error "Android NDK not found. Please install Android NDK."
                return 1
            fi
            ;;
            
        ios)
            if [[ "$OSTYPE" != "darwin"* ]]; then
                log_error "iOS builds require macOS."
                return 1
            fi
            
            if ! command -v xcodebuild &> /dev/null; then
                log_error "Xcode not found. Please install Xcode."
                return 1
            fi
            ;;
    esac
    
    log_success "Prerequisites check passed for $platform"
    return 0
}

# Function to prepare assets
prepare_assets() {
    log_info "Preparing game assets..."
    
    # Create assets directory if it doesn't exist
    mkdir -p "$PROJECT_ROOT/assets/shaders"
    mkdir -p "$PROJECT_ROOT/assets/textures"
    mkdir -p "$PROJECT_ROOT/assets/models"
    
    # Copy or generate basic assets
    if [ ! -f "$PROJECT_ROOT/assets/shaders/basic.vert" ]; then
        log_info "Creating basic vertex shader..."
        cat > "$PROJECT_ROOT/assets/shaders/basic.vert" << 'EOF'
#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoord;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Color = aColor;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
EOF
    fi
    
    if [ ! -f "$PROJECT_ROOT/assets/shaders/basic.frag" ]; then
        log_info "Creating basic fragment shader..."
        cat > "$PROJECT_ROOT/assets/shaders/basic.frag" << 'EOF'
#version 300 es
precision mediump float;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 ambientLight;

void main() {
    // Ambient
    vec3 ambient = ambientLight * Color;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * Color;
    
    // Simple specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
EOF
    fi
    
    log_success "Assets prepared"
}

# Function to build for Android
build_android() {
    local build_type=$1
    
    log_info "Building for Android ($build_type)..."
    
    cd "$PROJECT_ROOT/android"
    
    # Clean previous builds
    log_info "Cleaning previous Android builds..."
    ./gradlew clean
    
    # Build based on type
    if [ "$build_type" = "release" ]; then
        log_info "Building Android release..."
        ./gradlew assembleRelease
        
        if [ $? -eq 0 ]; then
            log_success "Android release build completed!"
            log_info "APK location: android/app/build/outputs/apk/release/app-release.apk"
        else
            log_error "Android release build failed!"
            return 1
        fi
    else
        log_info "Building Android debug..."
        ./gradlew assembleDebug
        
        if [ $? -eq 0 ]; then
            log_success "Android debug build completed!"
            log_info "APK location: android/app/build/outputs/apk/debug/app-debug.apk"
        else
            log_error "Android debug build failed!"
            return 1
        fi
    fi
    
    cd "$PROJECT_ROOT"
    return 0
}

# Function to build for iOS
build_ios() {
    local build_type=$1
    
    log_info "Building for iOS ($build_type)..."
    
    cd "$PROJECT_ROOT"
    
    # Create iOS build directory
    mkdir -p "build/ios"
    cd "build/ios"
    
    # Configure with CMake for iOS
    log_info "Configuring iOS build with CMake..."
    cmake ../.. \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="YOUR_TEAM_ID" \
        -DIOS=1 \
        -G Xcode
    
    if [ $? -ne 0 ]; then
        log_error "CMake configuration failed for iOS!"
        return 1
    fi
    
    # Build with Xcode
    if [ "$build_type" = "release" ]; then
        log_info "Building iOS release..."
        xcodebuild -project RacingGame3D.xcodeproj -scheme RacingGame3D -configuration Release -destination generic/platform=iOS build
    else
        log_info "Building iOS debug..."
        xcodebuild -project RacingGame3D.xcodeproj -scheme RacingGame3D -configuration Debug -destination generic/platform=iOS build
    fi
    
    if [ $? -eq 0 ]; then
        log_success "iOS $build_type build completed!"
    else
        log_error "iOS $build_type build failed!"
        return 1
    fi
    
    cd "$PROJECT_ROOT"
    return 0
}

# Function to install Android APK
install_android() {
    local build_type=$1
    
    log_info "Installing Android APK..."
    
    if [ "$build_type" = "release" ]; then
        APK_PATH="android/app/build/outputs/apk/release/app-release.apk"
    else
        APK_PATH="android/app/build/outputs/apk/debug/app-debug.apk"
    fi
    
    if [ -f "$APK_PATH" ]; then
        adb install -r "$APK_PATH"
        if [ $? -eq 0 ]; then
            log_success "APK installed successfully!"
            log_info "You can now launch the app on your Android device."
        else
            log_error "Failed to install APK. Make sure your device is connected and USB debugging is enabled."
        fi
    else
        log_error "APK not found at $APK_PATH"
    fi
}

# Main function
main() {
    local platform=${1:-""}
    local build_type=${2:-"debug"}
    
    if [ -z "$platform" ]; then
        show_usage
        exit 1
    fi
    
    log_info "Starting mobile build process..."
    log_info "Platform: $platform"
    log_info "Build type: $build_type"
    
    # Prepare assets
    prepare_assets
    
    case $platform in
        android)
            check_prerequisites "android" || exit 1
            build_android "$build_type" || exit 1
            
            # Ask if user wants to install
            read -p "Do you want to install the APK on a connected device? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                install_android "$build_type"
            fi
            ;;
            
        ios)
            check_prerequisites "ios" || exit 1
            build_ios "$build_type" || exit 1
            ;;
            
        both)
            check_prerequisites "android" || exit 1
            check_prerequisites "ios" || exit 1
            
            build_android "$build_type" || exit 1
            build_ios "$build_type" || exit 1
            ;;
            
        *)
            log_error "Unknown platform: $platform"
            show_usage
            exit 1
            ;;
    esac
    
    log_success "Mobile build process completed!"
}

# Run main function with all arguments
main "$@"