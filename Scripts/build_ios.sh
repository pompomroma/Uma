#!/usr/bin/env bash
set -euo pipefail

# Usage: UE5_ROOT="/Users/Shared/Epic Games/UE_5.4" ./Scripts/build_ios.sh [Configuration]
# Configuration defaults to Shipping. Requires macOS with Xcode and UE5 iOS support installed.

CONFIGURATION=${1:-Shipping}
PROJECT_DIR=$(cd "$(dirname "$0")/.." && pwd)
PROJECT_UPROJ="$PROJECT_DIR/Racer.uproject"
UE5_ROOT=${UE5_ROOT:-"/Users/Shared/Epic Games/UE_5.4"}

if [[ ! -f "$PROJECT_UPROJ" ]]; then
  echo "Missing $PROJECT_UPROJ" >&2
  exit 1
fi

UAT="$UE5_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
if [[ ! -x "$UAT" ]]; then
  echo "RunUAT not found at $UAT" >&2
  exit 1
fi

ARCHIVE_DIR="$PROJECT_DIR/Build/IOS"
mkdir -p "$ARCHIVE_DIR"

"$UAT" BuildCookRun \
  -project="${PROJECT_UPROJ}" \
  -noP4 -utf8output \
  -platform=IOS -clientconfig="$CONFIGURATION" -target=Racer \
  -build -cook -pak -prereqs -stage -archive \
  -archivedirectory="${ARCHIVE_DIR}" \
  -compressed -nodebuginfo

echo "iOS build archived under $ARCHIVE_DIR"
