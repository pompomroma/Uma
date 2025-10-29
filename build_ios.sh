#!/bin/bash

# iOS build script for Racing Game 3D Mobile

set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
REPO_ROOT="$SCRIPT_DIR"
IOS_DIR="$REPO_ROOT/ios"
BUILD_DIR="$IOS_DIR/build"
DIST_DIR="$IOS_DIR/dist"

SCHEME="RacingGame3DiOS"
BUILD_TYPE="Release"
DO_INSTALL=false

while [[ $# -gt 0 ]]; do
  case "$1" in
    --debug)
      BUILD_TYPE="Debug"; shift ;;
    --release)
      BUILD_TYPE="Release"; shift ;;
    --install)
      DO_INSTALL=true; shift ;;
    *)
      # Keep compatibility with old usage: first arg as build type
      if [[ "$1" == "Debug" || "$1" == "Release" ]]; then
        BUILD_TYPE="$1"; shift
      else
        echo "Unknown option: $1"; echo "Usage: ./build_ios.sh [--debug|--release] [--install]"; exit 2
      fi
      ;;
  esac
done

echo "========================================="
echo "Building Racing Game 3D for iOS ($BUILD_TYPE)"
echo "Repo: $REPO_ROOT"
echo "========================================="

# Check if running on macOS
if [[ "$OSTYPE" != darwin* ]]; then
  echo "Note: You're not on macOS. We'll generate an Xcode project you can open on a Mac."
fi

# macOS requirements
if [[ "$OSTYPE" == darwin* ]]; then
  if ! command -v xcodebuild >/dev/null 2>&1; then
    echo "Error: Xcode command line tools not found. Install Xcode from the App Store, then run 'xcode-select --install'." >&2
    exit 1
  fi
  if ! command -v cmake >/dev/null 2>&1; then
    echo "Error: CMake is not installed. Install via Homebrew: 'brew install cmake'." >&2
    exit 1
  fi
fi

mkdir -p "$BUILD_DIR" "$DIST_DIR"

echo "Generating Xcode project..."
if [[ "$OSTYPE" == darwin* ]]; then
  # Use real Xcode generator on macOS
  cmake -S "$IOS_DIR" -B "$BUILD_DIR" -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DPLATFORM_IOS=1 \
    -DPLATFORM_MOBILE=1
else
  # Create a lightweight Xcode project structure for transfer
  echo "Creating portable Xcode project for transfer to macOS..."
  mkdir -p "$BUILD_DIR/RacingGame3DiOS.xcodeproj"
  # Generate minimal pbxproj with references to existing sources
  cat > "$BUILD_DIR/RacingGame3DiOS.xcodeproj/project.pbxproj" << 'EOF'
// !$*UTF8*$!
{
	archiveVersion = 1;
	classes = {
	};
	objectVersion = 56;
	objects = {

/* Begin PBXBuildFile section */
		A1000001000000000000001 /* Game.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000011 /* Game.cpp */; };
		A1000001000000000000002 /* Vector3.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000012 /* Vector3.cpp */; };
		A1000001000000000000003 /* Matrix4.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000013 /* Matrix4.cpp */; };
		A1000001000000000000004 /* Quaternion.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000014 /* Quaternion.cpp */; };
		A1000001000000000000005 /* Camera.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000015 /* Camera.cpp */; };
		A1000001000000000000006 /* Car.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000016 /* Car.cpp */; };
		A1000001000000000000007 /* PhysicsEngine.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000017 /* PhysicsEngine.cpp */; };
		A1000001000000000000008 /* Renderer.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000018 /* Renderer.cpp */; };
		A1000001000000000000009 /* InputManager.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000019 /* InputManager.cpp */; };
		A100000100000000000000A /* TouchInputManager.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A100000100000000000001A /* TouchInputManager.cpp */; };
		A100000100000000000000B /* Track.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A100000100000000000001B /* Track.cpp */; };
		A100000100000000000000C /* Shader.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A100000100000000000001C /* Shader.cpp */; };
		A100000100000000000000D /* Player.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A100000100000000000001D /* Player.cpp */; };
		A100000100000000000000E /* Projectile.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A100000100000000000001E /* Projectile.cpp */; };
		A100000100000000000000F /* Shield.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A100000100000000000001F /* Shield.cpp */; };
		A1000001000000000000010 /* CombatManager.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000020 /* CombatManager.cpp */; };
		A1000001000000000000021 /* MobileUI.cpp in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000031 /* MobileUI.cpp */; };
		A1000001000000000000022 /* ios_main.mm in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000032 /* ios_main.mm */; };
		A1000001000000000000023 /* GameViewController.mm in Sources */ = {isa = PBXBuildFile; fileRef = A1000001000000000000033 /* GameViewController.mm */; };
		A1000001000000000000024 /* UIKit.framework in Frameworks */ = {isa = PBXBuildFile; fileRef = A1000001000000000000034 /* UIKit.framework */; };
		A1000001000000000000025 /* Foundation.framework in Frameworks */ = {isa = PBXBuildFile; fileRef = A1000001000000000000035 /* Foundation.framework */; };
		A1000001000000000000026 /* Metal.framework in Frameworks */ = {isa = PBXBuildFile; fileRef = A1000001000000000000036 /* Metal.framework */; };
		A1000001000000000000027 /* MetalKit.framework in Frameworks */ = {isa = PBXBuildFile; fileRef = A1000001000000000000037 /* MetalKit.framework */; };
		A1000001000000000000028 /* QuartzCore.framework in Frameworks */ = {isa = PBXBuildFile; fileRef = A1000001000000000000038 /* QuartzCore.framework */; };
		A1000001000000000000029 /* GameController.framework in Frameworks */ = {isa = PBXBuildFile; fileRef = A1000001000000000000039 /* GameController.framework */; };
/* End PBXBuildFile section */

/* Begin PBXFileReference section */
		A1000001000000000000011 /* Game.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Game.cpp; sourceTree = "<group>"; };
		A1000001000000000000012 /* Vector3.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Vector3.cpp; sourceTree = "<group>"; };
		A1000001000000000000013 /* Matrix4.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Matrix4.cpp; sourceTree = "<group>"; };
		A1000001000000000000014 /* Quaternion.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Quaternion.cpp; sourceTree = "<group>"; };
		A1000001000000000000015 /* Camera.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Camera.cpp; sourceTree = "<group>"; };
		A1000001000000000000016 /* Car.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Car.cpp; sourceTree = "<group>"; };
		A1000001000000000000017 /* PhysicsEngine.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = PhysicsEngine.cpp; sourceTree = "<group>"; };
		A1000001000000000000018 /* Renderer.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Renderer.cpp; sourceTree = "<group>"; };
		A1000001000000000000019 /* InputManager.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = InputManager.cpp; sourceTree = "<group>"; };
		A100000100000000000001A /* TouchInputManager.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = TouchInputManager.cpp; sourceTree = "<group>"; };
		A100000100000000000001B /* Track.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Track.cpp; sourceTree = "<group>"; };
		A100000100000000000001C /* Shader.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Shader.cpp; sourceTree = "<group>"; };
		A100000100000000000001D /* Player.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Player.cpp; sourceTree = "<group>"; };
		A100000100000000000001E /* Projectile.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Projectile.cpp; sourceTree = "<group>"; };
		A100000100000000000001F /* Shield.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = Shield.cpp; sourceTree = "<group>"; };
		A1000001000000000000020 /* CombatManager.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = CombatManager.cpp; sourceTree = "<group>"; };
		A1000001000000000000031 /* MobileUI.cpp */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = MobileUI.cpp; sourceTree = "<group>"; };
		A1000001000000000000032 /* ios_main.mm */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.objcpp; path = ios_main.mm; sourceTree = "<group>"; };
		A1000001000000000000033 /* GameViewController.mm */ = {isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.objcpp; path = GameViewController.mm; sourceTree = "<group>"; };
		A1000001000000000000034 /* UIKit.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = UIKit.framework; path = System/Library/Frameworks/UIKit.framework; sourceTree = SDKROOT; };
		A1000001000000000000035 /* Foundation.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = Foundation.framework; path = System/Library/Frameworks/Foundation.framework; sourceTree = SDKROOT; };
		A1000001000000000000036 /* Metal.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = Metal.framework; path = System/Library/Frameworks/Metal.framework; sourceTree = SDKROOT; };
		A1000001000000000000037 /* MetalKit.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = MetalKit.framework; path = System/Library/Frameworks/MetalKit.framework; sourceTree = SDKROOT; };
		A1000001000000000000038 /* QuartzCore.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = QuartzCore.framework; path = System/Library/Frameworks/QuartzCore.framework; sourceTree = SDKROOT; };
		A1000001000000000000039 /* GameController.framework */ = {isa = PBXFileReference; lastKnownFileType = wrapper.framework; name = GameController.framework; path = System/Library/Frameworks/GameController.framework; sourceTree = SDKROOT; };
		A100000100000000000003A /* RacingGame3DiOS.app */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = RacingGame3DiOS.app; sourceTree = BUILT_PRODUCTS_DIR; };
		A100000100000000000003B /* Info.plist */ = {isa = PBXFileReference; lastKnownFileType = text.plist.xml; path = Info.plist; sourceTree = "<group>"; };
/* End PBXFileReference section */

/* Begin PBXFrameworksBuildPhase section */
		A100000100000000000003C /* Frameworks */ = {
			isa = PBXFrameworksBuildPhase;
			buildActionMask = 2147483647;
			files = (
				A1000001000000000000024 /* UIKit.framework in Frameworks */,
				A1000001000000000000025 /* Foundation.framework in Frameworks */,
				A1000001000000000000026 /* Metal.framework in Frameworks */,
				A1000001000000000000027 /* MetalKit.framework in Frameworks */,
				A1000001000000000000028 /* QuartzCore.framework in Frameworks */,
				A1000001000000000000029 /* GameController.framework in Frameworks */,
			);
			runOnlyForDeploymentPostprocessing = 0;
		};
/* End PBXFrameworksBuildPhase section */

/* Begin PBXGroup section */
		A100000100000000000003D /* RacingGame3DiOS */ = {
			isa = PBXGroup;
			children = (
				A100000100000000000003E /* Source Files */,
				A100000100000000000003F /* iOS Files */,
				A100000100000000000003B /* Info.plist */,
			);
			path = RacingGame3DiOS;
			sourceTree = "<group>";
		};
		A100000100000000000003E /* Source Files */ = {
			isa = PBXGroup;
			children = (
				A1000001000000000000011 /* Game.cpp */,
				A1000001000000000000012 /* Vector3.cpp */,
				A1000001000000000000013 /* Matrix4.cpp */,
				A1000001000000000000014 /* Quaternion.cpp */,
				A1000001000000000000015 /* Camera.cpp */,
				A1000001000000000000016 /* Car.cpp */,
				A1000001000000000000017 /* PhysicsEngine.cpp */,
				A1000001000000000000018 /* Renderer.cpp */,
				A1000001000000000000019 /* InputManager.cpp */,
				A100000100000000000001A /* TouchInputManager.cpp */,
				A100000100000000000001B /* Track.cpp */,
				A100000100000000000001C /* Shader.cpp */,
				A100000100000000000001D /* Player.cpp */,
				A100000100000000000001E /* Projectile.cpp */,
				A100000100000000000001F /* Shield.cpp */,
				A1000001000000000000020 /* CombatManager.cpp */,
				A1000001000000000000031 /* MobileUI.cpp */,
			);
			name = "Source Files";
			path = "../../src";
			sourceTree = "<group>";
		};
		A100000100000000000003F /* iOS Files */ = {
			isa = PBXGroup;
			children = (
				A1000001000000000000032 /* ios_main.mm */,
				A1000001000000000000033 /* GameViewController.mm */,
			);
			name = "iOS Files";
			sourceTree = "<group>";
		};
		A1000001000000000000040 /* Frameworks */ = {
			isa = PBXGroup;
			children = (
				A1000001000000000000034 /* UIKit.framework */,
				A1000001000000000000035 /* Foundation.framework */,
				A1000001000000000000036 /* Metal.framework */,
				A1000001000000000000037 /* MetalKit.framework */,
				A1000001000000000000038 /* QuartzCore.framework */,
				A1000001000000000000039 /* GameController.framework */,
			);
			name = Frameworks;
			sourceTree = "<group>";
		};
		A1000001000000000000041 /* Products */ = {
			isa = PBXGroup;
			children = (
				A100000100000000000003A /* RacingGame3DiOS.app */,
			);
			name = Products;
			sourceTree = "<group>";
		};
		A1000001000000000000042 /* RacingGame3DiOS */ = {
			isa = PBXGroup;
			children = (
				A100000100000000000003D /* RacingGame3DiOS */,
				A1000001000000000000040 /* Frameworks */,
				A1000001000000000000041 /* Products */,
			);
			sourceTree = "<group>";
		};
/* End PBXGroup section */

/* Begin PBXNativeTarget section */
		A1000001000000000000043 /* RacingGame3DiOS */ = {
			isa = PBXNativeTarget;
			buildConfigurationList = A1000001000000000000044 /* Build configuration list for PBXNativeTarget "RacingGame3DiOS" */;
			buildPhases = (
				A1000001000000000000045 /* Sources */,
				A100000100000000000003C /* Frameworks */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = RacingGame3DiOS;
			productName = RacingGame3DiOS;
			productReference = A100000100000000000003A /* RacingGame3DiOS.app */;
			productType = "com.apple.product-type.application";
		};
/* End PBXNativeTarget section */

/* Begin PBXProject section */
		A1000001000000000000046 /* Project object */ = {
			isa = PBXProject;
			attributes = {
				BuildIndependentTargetsInParallel = 1;
				LastUpgradeCheck = 1500;
				TargetAttributes = {
					A1000001000000000000043 = {
						CreatedOnToolsVersion = 15.0;
					};
				};
			};
			buildConfigurationList = A1000001000000000000047 /* Build configuration list for PBXProject "RacingGame3DiOS" */;
			compatibilityVersion = "Xcode 14.0";
			developmentRegion = en;
			hasScannedForEncodings = 0;
			knownRegions = (
				en,
				Base,
			);
			mainGroup = A1000001000000000000042 /* RacingGame3DiOS */;
			productRefGroup = A1000001000000000000041 /* Products */;
			projectDirPath = "";
			projectRoot = "";
			targets = (
				A1000001000000000000043 /* RacingGame3DiOS */,
			);
		};
/* End PBXProject section */

/* Begin PBXSourcesBuildPhase section */
		A1000001000000000000045 /* Sources */ = {
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
				A1000001000000000000001 /* Game.cpp in Sources */,
				A1000001000000000000002 /* Vector3.cpp in Sources */,
				A1000001000000000000003 /* Matrix4.cpp in Sources */,
				A1000001000000000000004 /* Quaternion.cpp in Sources */,
				A1000001000000000000005 /* Camera.cpp in Sources */,
				A1000001000000000000006 /* Car.cpp in Sources */,
				A1000001000000000000007 /* PhysicsEngine.cpp in Sources */,
				A1000001000000000000008 /* Renderer.cpp in Sources */,
				A1000001000000000000009 /* InputManager.cpp in Sources */,
				A100000100000000000000A /* TouchInputManager.cpp in Sources */,
				A100000100000000000000B /* Track.cpp in Sources */,
				A100000100000000000000C /* Shader.cpp in Sources */,
				A100000100000000000000D /* Player.cpp in Sources */,
				A100000100000000000000E /* Projectile.cpp in Sources */,
				A100000100000000000000F /* Shield.cpp in Sources */,
				A1000001000000000000010 /* CombatManager.cpp in Sources */,
				A1000001000000000000021 /* MobileUI.cpp in Sources */,
				A1000001000000000000022 /* ios_main.mm in Sources */,
				A1000001000000000000023 /* GameViewController.mm in Sources */,
			);
			runOnlyForDeploymentPostprocessing = 0;
		};
/* End PBXSourcesBuildPhase section */

/* Begin XCBuildConfiguration section */
		A1000001000000000000048 /* Debug */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				ALWAYS_SEARCH_USER_PATHS = NO;
				ASSETCATALOG_COMPILER_GENERATE_SWIFT_ASSET_SYMBOL_EXTENSIONS = YES;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++20";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_ENABLE_OBJC_WEAK = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = dwarf;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_TESTABILITY = YES;
				ENABLE_USER_SCRIPT_SANDBOXING = YES;
				GCC_C_LANGUAGE_STANDARD = gnu17;
				GCC_DYNAMIC_NO_PIC = NO;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_OPTIMIZATION_LEVEL = 0;
				GCC_PREPROCESSOR_DEFINITIONS = (
					"DEBUG=1",
					"$(inherited)",
				);
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				IPHONEOS_DEPLOYMENT_TARGET = 13.0;
				LOCALIZATION_PREFERS_STRING_CATALOGS = YES;
				MTL_ENABLE_DEBUG_INFO = INCLUDE_SOURCE;
				MTL_FAST_MATH = YES;
				ONLY_ACTIVE_ARCH = YES;
				SDKROOT = iphoneos;
			};
			name = Debug;
		};
		A1000001000000000000049 /* Release */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				ALWAYS_SEARCH_USER_PATHS = NO;
				ASSETCATALOG_COMPILER_GENERATE_SWIFT_ASSET_SYMBOL_EXTENSIONS = YES;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++20";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_ENABLE_OBJC_WEAK = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = "dwarf-with-dsym";
				ENABLE_NS_ASSERTIONS = NO;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_USER_SCRIPT_SANDBOXING = YES;
				GCC_C_LANGUAGE_STANDARD = gnu17;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				IPHONEOS_DEPLOYMENT_TARGET = 13.0;
				LOCALIZATION_PREFERS_STRING_CATALOGS = YES;
				MTL_ENABLE_DEBUG_INFO = NO;
				MTL_FAST_MATH = YES;
				SDKROOT = iphoneos;
				VALIDATE_PRODUCT = YES;
			};
			name = Release;
		};
		A100000100000000000004A /* Debug */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;
				ASSETCATALOG_COMPILER_GLOBAL_ACCENT_COLOR_NAME = AccentColor;
				CODE_SIGN_STYLE = Automatic;
				CURRENT_PROJECT_VERSION = 1;
				DEVELOPMENT_TEAM = "";
				GENERATE_INFOPLIST_FILE = YES;
				INFOPLIST_FILE = Info.plist;
				INFOPLIST_KEY_UIApplicationSupportsIndirectInputEvents = YES;
				INFOPLIST_KEY_UILaunchStoryboardName = LaunchScreen;
				INFOPLIST_KEY_UIMainStoryboardFile = Main;
				INFOPLIST_KEY_UISupportedInterfaceOrientations_iPad = "UIInterfaceOrientationPortrait UIInterfaceOrientationPortraitUpsideDown UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				INFOPLIST_KEY_UISupportedInterfaceOrientations_iPhone = "UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/Frameworks",
				);
				MARKETING_VERSION = 1.0;
				PRODUCT_BUNDLE_IDENTIFIER = "com.racinggame.mobile";
				PRODUCT_NAME = "$(TARGET_NAME)";
				SWIFT_EMIT_LOC_STRINGS = YES;
				TARGETED_DEVICE_FAMILY = "1,2";
			};
			name = Debug;
		};
		A100000100000000000004B /* Release */ = {
			isa = XCBuildConfiguration;
			buildSettings = {
				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;
				ASSETCATALOG_COMPILER_GLOBAL_ACCENT_COLOR_NAME = AccentColor;
				CODE_SIGN_STYLE = Automatic;
				CURRENT_PROJECT_VERSION = 1;
				DEVELOPMENT_TEAM = "";
				GENERATE_INFOPLIST_FILE = YES;
				INFOPLIST_FILE = Info.plist;
				INFOPLIST_KEY_UIApplicationSupportsIndirectInputEvents = YES;
				INFOPLIST_KEY_UILaunchStoryboardName = LaunchScreen;
				INFOPLIST_KEY_UIMainStoryboardFile = Main;
				INFOPLIST_KEY_UISupportedInterfaceOrientations_iPad = "UIInterfaceOrientationPortrait UIInterfaceOrientationPortraitUpsideDown UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				INFOPLIST_KEY_UISupportedInterfaceOrientations_iPhone = "UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/Frameworks",
				);
				MARKETING_VERSION = 1.0;
				PRODUCT_BUNDLE_IDENTIFIER = "com.racinggame.mobile";
				PRODUCT_NAME = "$(TARGET_NAME)";
				SWIFT_EMIT_LOC_STRINGS = YES;
				TARGETED_DEVICE_FAMILY = "1,2";
			};
			name = Release;
		};
/* End XCBuildConfiguration section */

/* Begin XCConfigurationList section */
		A1000001000000000000044 /* Build configuration list for PBXNativeTarget "RacingGame3DiOS" */ = {
			isa = XCConfigurationList;
			buildConfigurations = (
				A100000100000000000004A /* Debug */,
				A100000100000000000004B /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		};
		A1000001000000000000047 /* Build configuration list for PBXProject "RacingGame3DiOS" */ = {
			isa = XCConfigurationList;
			buildConfigurations = (
				A1000001000000000000048 /* Debug */,
				A1000001000000000000049 /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		};
/* End XCConfigurationList section */
	};
	rootObject = A1000001000000000000046 /* Project object */;
}
EOF

  # Create project.xcworkspace directory and contents
  mkdir -p "$BUILD_DIR/RacingGame3DiOS.xcodeproj/project.xcworkspace"

  cat > "$BUILD_DIR/RacingGame3DiOS.xcodeproj/project.xcworkspace/contents.xcworkspacedata" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<Workspace
   version = "1.0">
   <FileRef
      location = "self:RacingGame3DiOS.xcodeproj">
   </FileRef>
</Workspace>
EOF

  # Create xcschemes directory and scheme
  mkdir -p "$BUILD_DIR/RacingGame3DiOS.xcodeproj/xcshareddata/xcschemes"

  cat > "$BUILD_DIR/RacingGame3DiOS.xcodeproj/xcshareddata/xcschemes/RacingGame3DiOS.xcscheme" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<Scheme
   LastUpgradeVersion = "1500"
   version = "1.7">
   <BuildAction
      parallelizeBuildables = "YES"
      buildImplicitDependencies = "YES">
      <BuildActionEntries>
         <BuildActionEntry
            buildForTesting = "YES"
            buildForRunning = "YES"
            buildForProfiling = "YES"
            buildForArchiving = "YES"
            buildForAnalyzing = "YES">
            <BuildableReference
               BuildableIdentifier = "primary"
               BlueprintIdentifier = "A1000001000000000000043"
               BuildableName = "RacingGame3DiOS.app"
               BlueprintName = "RacingGame3DiOS"
               ReferencedContainer = "container:RacingGame3DiOS.xcodeproj">
            </BuildableReference>
         </BuildActionEntry>
      </BuildActionEntries>
   </BuildAction>
   <TestAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      shouldUseLaunchSchemeArgsEnv = "YES"
      shouldAutocreateTestPlan = "YES">
   </TestAction>
   <LaunchAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      launchStyle = "0"
      useCustomWorkingDirectory = "NO"
      ignoresPersistentStateOnLaunch = "NO"
      debugDocumentVersioning = "YES"
      debugServiceExtension = "internal"
      allowLocationSimulation = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "A1000001000000000000043"
            BuildableName = "RacingGame3DiOS.app"
            BlueprintName = "RacingGame3DiOS"
            ReferencedContainer = "container:RacingGame3DiOS.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </LaunchAction>
   <ProfileAction
      buildConfiguration = "Release"
      shouldUseLaunchSchemeArgsEnv = "YES"
      savedToolIdentifier = ""
      useCustomWorkingDirectory = "NO"
      debugDocumentVersioning = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "A1000001000000000000043"
            BuildableName = "RacingGame3DiOS.app"
            BlueprintName = "RacingGame3DiOS"
            ReferencedContainer = "container:RacingGame3DiOS.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </ProfileAction>
   <AnalyzeAction
      buildConfiguration = "Debug">
   </AnalyzeAction>
   <ArchiveAction
      buildConfiguration = "Release"
      revealArchiveInOrganizer = "YES">
   </ArchiveAction>
</Scheme>
EOF

  echo
  echo "Note: Cross-compilation to iOS from non-macOS isn't supported."
  echo "The Xcode project has been generated for transfer to a Mac."
fi

if [[ "$OSTYPE" == darwin* ]]; then
  echo "Building for iOS device..."
  xcodebuild -project "$BUILD_DIR/RacingGame3DiOS.xcodeproj" \
    -scheme "$SCHEME" \
    -configuration "$BUILD_TYPE" \
    -sdk iphoneos \
    -destination 'generic/platform=iOS' \
    -allowProvisioningUpdates \
    clean build

  APP_DIR="$BUILD_DIR/$BUILD_TYPE-iphoneos/$SCHEME.app"
  echo "App bundle: $APP_DIR"

  if $DO_INSTALL; then
    echo "Archiving app for export..."
    ARCHIVE_PATH="$DIST_DIR/$SCHEME.xcarchive"
    xcodebuild -project "$BUILD_DIR/RacingGame3DiOS.xcodeproj" \
      -scheme "$SCHEME" \
      -configuration "$BUILD_TYPE" \
      -sdk iphoneos \
      -destination 'generic/platform=iOS' \
      -archivePath "$ARCHIVE_PATH" \
      -allowProvisioningUpdates \
      archive

    cat > "$DIST_DIR/ExportOptions.plist" << 'EOP'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>method</key>
  <string>development</string>
  <key>signingStyle</key>
  <string>automatic</string>
  <key>stripSwiftSymbols</key>
  <true/>
  <key>compileBitcode</key>
  <false/>
  <key>destination</key>
  <string>export</string>
  <key>iCloudContainerEnvironment</key>
  <string>Development</string>
</dict>
</plist>
EOP

    echo "Exporting IPA..."
    xcodebuild -exportArchive \
      -archivePath "$ARCHIVE_PATH" \
      -exportOptionsPlist "$DIST_DIR/ExportOptions.plist" \
      -exportPath "$DIST_DIR"

    IPA_PATH="$DIST_DIR/$SCHEME.ipa"
    if [[ -f "$IPA_PATH" ]]; then
      echo "IPA generated: $IPA_PATH"
      if xcrun -f devicectl >/dev/null 2>&1; then
        echo "Attempting to install to a connected device using devicectl..."
        # Will prompt selection in Xcode if multiple devices; otherwise install to default connected device
        set +e
        xcrun devicectl device install app "$IPA_PATH" || true
        set -e
        echo "If installation did not start, open Xcode and run the app on your device."
      elif command -v ios-deploy >/dev/null 2>&1; then
        echo "Installing via ios-deploy..."
        ios-deploy --bundle "$APP_DIR" --justlaunch || true
      else
        echo "To install: open the project in Xcode or install 'ios-deploy' via Homebrew."
      fi
    fi
  fi

  echo ""
  echo "========================================="
  echo "iOS build completed successfully!"
  echo "Open this project in Xcode: $BUILD_DIR/RacingGame3DiOS.xcodeproj"
  echo "Select your Team, then run on a device (⌘R)."
  echo "========================================="
else
  echo ""
  echo "========================================="
  echo "Xcode project generated successfully!"
  echo "Project location: $BUILD_DIR/RacingGame3DiOS.xcodeproj"
  echo "Transfer the repo to a Mac, then run this same script or open in Xcode."
  echo "========================================="
fi
