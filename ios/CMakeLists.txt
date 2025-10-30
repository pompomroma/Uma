cmake_minimum_required(VERSION 3.18)
project(RacingGame3DiOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# iOS specific settings
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Source files
set(SOURCES
    # Core game files
    ../src/Game.cpp
    ../src/Math/Vector3.cpp
    ../src/Math/Matrix4.cpp
    ../src/Math/Quaternion.cpp
    ../src/Camera/Camera.cpp
    ../src/Physics/Car.cpp
    ../src/Physics/PhysicsEngine.cpp
    ../src/Rendering/Renderer.cpp
    ../src/Input/InputManager.cpp
    ../src/Input/TouchInputManager.cpp
    ../src/World/Track.cpp
    ../src/Utils/Shader.cpp
    ../src/Combat/Player.cpp
    ../src/Combat/Projectile.cpp
    ../src/Combat/Shield.cpp
    ../src/Combat/CombatManager.cpp
    ../src/UI/MobileUI.cpp
    
    # iOS specific
    ios_main.mm
    GameViewController.mm
)

# iOS framework dependencies
find_library(UIKIT_LIBRARY UIKit)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(METAL_LIBRARY Metal)
find_library(METALKIT_LIBRARY MetalKit)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(GAMECONTROLLER_LIBRARY GameController)

# Create iOS app bundle
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})

# Set bundle properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.racinggame.mobile"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
    XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ""
    XCODE_ATTRIBUTE_DEVELOPMENT_ASSET_PATHS "\"${CMAKE_CURRENT_SOURCE_DIR}/Assets.xcassets\""
    XCODE_ATTRIBUTE_LAUNCH_SCREEN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LaunchScreen.storyboard"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0"
    XCODE_ATTRIBUTE_MARKETING_VERSION "1.0.0"
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION "1"
    XCODE_ATTRIBUTE_GENERATE_INFOPLIST_FILE "NO"
    XCODE_ATTRIBUTE_INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_GLOBAL_ACCENT_COLOR_NAME "AccentColor"
    XCODE_ATTRIBUTE_UIApplicationSupportsIndirectInputEvents "YES"
    XCODE_ATTRIBUTE_UILaunchStoryboardName "LaunchScreen"
    XCODE_ATTRIBUTE_UIMainStoryboardFile ""
    XCODE_ATTRIBUTE_UISupportedInterfaceOrientations_iPad "UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight"
    XCODE_ATTRIBUTE_UISupportedInterfaceOrientations_iPhone "UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight"
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks"
    XCODE_ATTRIBUTE_SWIFT_EMIT_LOC_STRINGS "YES"
    XCODE_ATTRIBUTE_SWIFT_VERSION "5.0"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK "YES"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "gnu++17"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "gnu17"
    XCODE_ATTRIBUTE_ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES "NO"
    XCODE_ATTRIBUTE_EMBEDDED_CONTENT_CONTAINS_SWIFT "NO"
    XCODE_ATTRIBUTE_SWIFT_OPTIMIZATION_LEVEL "-Onone"
    XCODE_ATTRIBUTE_SWIFT_COMPILATION_MODE "singlefile"
    XCODE_ATTRIBUTE_SWIFT_ACTIVE_COMPILATION_CONDITIONS "DEBUG"
    XCODE_ATTRIBUTE_GCC_PREPROCESSOR_DEFINITIONS "DEBUG=1"
    XCODE_ATTRIBUTE_MTL_ENABLE_DEBUG_INFO "INCLUDE_SOURCE"
    XCODE_ATTRIBUTE_MTL_FAST_MATH "YES"
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES"
    XCODE_ATTRIBUTE_VALIDATE_PRODUCT "YES"
    XCODE_ATTRIBUTE_COPY_PHASE_STRIP "NO"
    XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT "NO"
    XCODE_ATTRIBUTE_SEPARATE_STRIP "NO"
    XCODE_ATTRIBUTE_STRIP_SWIFT_SYMBOLS "YES"
    XCODE_ATTRIBUTE_SYMBOLS_HIDDEN_BY_DEFAULT "YES"
    XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING "YES"
    XCODE_ATTRIBUTE_DONT_GENERATE_INFOPLIST_FILE "NO"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIApplicationSupportsIndirectInputEvents "YES"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UILaunchStoryboardName "LaunchScreen"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UISupportedInterfaceOrientations_iPad "UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UISupportedInterfaceOrientations_iPhone "UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIStatusBarHidden "YES"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIViewControllerBasedStatusBarAppearance "NO"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIRequiresFullScreen "YES"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIRequiredDeviceCapabilities "arm64 metal"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSRequiresIPhoneOS "YES"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIDeviceFamily "1 2"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSApplicationCategoryType "public.app-category.games"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_ITSAppUsesNonExemptEncryption "NO"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_NSAppTransportSecurity_NSAllowsArbitraryLoads "YES"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIBackgroundModes "audio"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_NSCameraUsageDescription "This app does not use the camera."
    XCODE_ATTRIBUTE_INFOPLIST_KEY_NSMicrophoneUsageDescription "This app does not use the microphone."
    XCODE_ATTRIBUTE_INFOPLIST_KEY_MinimumOSVersion "13.0"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleShortVersionString "1.0.0"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleVersion "1"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDisplayName "Racing Game 3D"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleName "RacingGame3DiOS"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleIdentifier "com.racinggame.mobile"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleExecutable "RacingGame3DiOS"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundlePackageType "APPL"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleInfoDictionaryVersion "6.0"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDevelopmentRegion "en"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleIconFiles "AppIcon"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIApplicationSceneManifest_UIApplicationSupportsMultipleScenes "NO"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIApplicationSceneManifest_UISceneConfigurations_UIWindowSceneSessionRoleApplication_UISceneConfigurationName "Default Configuration"
    XCODE_ATTRIBUTE_INFOPLIST_KEY_UIApplicationSceneManifest_UISceneConfigurations_UIWindowSceneSessionRoleApplication_UISceneDelegateClassName "SceneDelegate"
)

# Link frameworks
target_link_libraries(${PROJECT_NAME}
    ${UIKIT_LIBRARY}
    ${FOUNDATION_LIBRARY}
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
    ${GAMECONTROLLER_LIBRARY}
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLATFORM_IOS=1
    GRAPHICS_METAL=1
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -fobjc-arc
    -fvisibility=hidden
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3
        -ffast-math
        -DNDEBUG
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O0
        -g
        -DDEBUG
    )
endif()
