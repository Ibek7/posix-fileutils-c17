#!/usr/bin/env bash
#set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT" || exit 1

echo "Building..."
make >/dev/null

echo "--- stat README.md ---"
./stat README.md || true

echo "--- myls ---"
./myls | head -n 20 || true

echo "--- myls -l . ---"
./myls -l . | head -n 40 || true

echo "--- mytail 5 README.md ---"
./mytail 5 README.md || true

echo "--- myfind . (first 50 lines) ---"
./myfind . | head -n 50 || true

echo "Smoke tests finished."
