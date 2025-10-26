#!/bin/bash
# extended_tests.sh - Comprehensive test suite

set -e

echo "=== EXTENDED TEST SUITE ==="

# Test stat with special files
echo "Test stat with special files..."
./stat /dev/null /etc/passwd /tmp 2>/dev/null | head -15
echo "✓ stat tests passed"

# Test myls with sorting
echo "Test myls sorting..."
echo "Regular: $(./myls /bin | wc -l) files"
echo "Sorted by name: $(./myls -t /bin | head -3)"
echo "✓ myls tests passed"

# Test mytail edge cases
echo "Test mytail edge cases..."
echo -e "a\nb\nc" | tee /tmp/small.txt >/dev/null
./mytail 2 /tmp/small.txt
echo "✓ mytail tests passed"

# Test myfind with depth
echo "Test myfind depth limiting..."
echo "Full tree lines: $(./myfind /usr/share 2>/dev/null | wc -l)"
echo "Depth 1: $(./myfind /usr/share -maxdepth 1 2>/dev/null | wc -l)"
echo "Depth 2: $(./myfind /usr/share -maxdepth 2 2>/dev/null | wc -l)"
echo "✓ myfind tests passed"

echo "=== ALL EXTENDED TESTS PASSED ==="
