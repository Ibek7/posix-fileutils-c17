#!/bin/bash
# performance_bench.sh - Benchmarking suite for posix-fileutils

set -e

RESULTS_DIR="benchmarks"
mkdir -p "$RESULTS_DIR"

echo "=== POSIX FileUtils Performance Benchmarks ==="

# stat benchmark
echo "Benchmarking stat()..."
time_stat() {
    for i in {1..1000}; do
        ./stat /etc/passwd >/dev/null 2>&1
    done
}
echo "stat (1000 iterations):"
time time_stat
echo

# myls benchmark
echo "Benchmarking myls..."
time_myls() {
    for i in {1..100}; do
        ./myls -l /usr/bin >/dev/null 2>&1
    done
}
echo "myls -l (100 iterations on /usr/bin):"
time time_myls
echo

# mytail benchmark
echo "Benchmarking mytail..."
# Create test file
dd if=/dev/zero bs=1M count=100 of=/tmp/large_file.txt 2>/dev/null
time_mytail() {
    for i in {1..50}; do
        ./mytail 100 /tmp/large_file.txt >/dev/null 2>&1
    done
}
echo "mytail 100 lines (50 iterations on 100MB file):"
time time_mytail
rm /tmp/large_file.txt

# myfind benchmark
echo "Benchmarking myfind..."
time_myfind() {
    ./myfind /usr/share >/dev/null 2>&1
}
echo "myfind /usr/share:"
time time_myfind
echo

echo "=== Benchmark Summary ==="
echo "All tests completed successfully"
echo "Results show typical performance characteristics"
