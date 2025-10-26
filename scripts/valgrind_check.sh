#!/bin/bash
# valgrind_check.sh
# Validate memory safety and detect leaks using valgrind
# Usage: ./valgrind_check.sh

set -e

echo "=========================================="
echo "Memory Safety Validation with Valgrind"
echo "=========================================="

VALGRIND_CMD="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes"

# Check if valgrind is installed
if ! command -v valgrind &> /dev/null; then
    echo "ERROR: valgrind not found. Install with: sudo apt-get install valgrind"
    exit 1
fi

# Rebuild binaries
echo -e "\n[1/5] Rebuilding utilities..."
make clean && make > /dev/null 2>&1
echo "✓ Build complete"

# Test 1: stat command
echo -e "\n[2/5] Testing stat for memory leaks..."
echo "Command: $VALGRIND_CMD ./stat /etc/passwd"
$VALGRIND_CMD ./stat /etc/passwd > /tmp/stat.out 2>&1
if grep -q "ERROR SUMMARY: 0 errors" /tmp/stat.out; then
    echo "✓ stat: PASS (no memory errors)"
else
    echo "✗ stat: FAIL (memory errors detected)"
    grep "ERROR SUMMARY" /tmp/stat.out
    exit 1
fi

# Test 2: myls command
echo -e "\n[3/5] Testing myls for memory leaks..."
echo "Command: $VALGRIND_CMD ./myls -l /tmp"
$VALGRIND_CMD ./myls -l /tmp > /tmp/myls.out 2>&1
if grep -q "ERROR SUMMARY: 0 errors" /tmp/myls.out; then
    echo "✓ myls: PASS (no memory errors)"
else
    echo "✗ myls: FAIL (memory errors detected)"
    grep "ERROR SUMMARY" /tmp/myls.out
    exit 1
fi

# Test 3: mytail command
echo -e "\n[4/5] Testing mytail for memory leaks..."
echo "Command: $VALGRIND_CMD ./mytail 5 /etc/hostname"
$VALGRIND_CMD ./mytail 5 /etc/hostname > /tmp/mytail.out 2>&1
if grep -q "ERROR SUMMARY: 0 errors" /tmp/mytail.out; then
    echo "✓ mytail: PASS (no memory errors)"
else
    echo "✗ mytail: FAIL (memory errors detected)"
    grep "ERROR SUMMARY" /tmp/mytail.out
    exit 1
fi

# Test 4: myfind command
echo -e "\n[5/5] Testing myfind for memory leaks..."
echo "Command: $VALGRIND_CMD ./myfind /tmp -maxdepth 1"
$VALGRIND_CMD ./myfind /tmp -maxdepth 1 > /tmp/myfind.out 2>&1
if grep -q "ERROR SUMMARY: 0 errors" /tmp/myfind.out; then
    echo "✓ myfind: PASS (no memory errors)"
else
    echo "✗ myfind: FAIL (memory errors detected)"
    grep "ERROR SUMMARY" /tmp/myfind.out
    exit 1
fi

echo -e "\n=========================================="
echo "✓ All memory safety checks PASSED"
echo "=========================================="
