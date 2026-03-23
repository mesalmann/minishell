#!/bin/bash

# Simple Minishell Test Suite
MINISHELL="./minishell"
PASS=0
FAIL=0

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

run_test() {
    local name="$1"
    local cmd="$2"
    local expected="$3"
    
    result=$(echo "$cmd" | $MINISHELL 2>&1 | head -1)
    
    if [[ "$result" == *"$expected"* ]]; then
        echo -e "${GREEN}✓${NC} $name"
        ((PASS++))
    else
        echo -e "${RED}✗${NC} $name"
        echo "  Expected: $expected"
        echo "  Got:      $result"
        ((FAIL++))
    fi
}

cd /Users/merve/Desktop/minishell

echo "========== MINISHELL TEST SUITE =========="
echo ""

# ENV Tests
echo "--- ENV Commands ---"
run_test "env pwd shows /Users" "env pwd" "/Users"
run_test "env echo works" "env echo hello" "hello"
run_test "env ls works" "env ls" "minishell"
run_test "env identifier validation" "env 123=x 2>&1" "not a valid identifier"
run_test "env multiple vars" "env VAR1=a VAR2=b echo ok" "ok"

echo ""
echo "--- EXPORT Commands ---"
run_test "export and echo" "export X=hello && echo \$X" "hello"
run_test "export identifier validation" "export 123=x 2>&1" "not a valid identifier"
run_test "export single quote format" "export xyz.abc 2>&1" "'xyz.abc'"
run_test "export append op" "export VAR=a && export VAR+=b && echo \$VAR" "ab"

echo ""
echo "--- ECHO Commands ---"
run_test "echo basic" "echo hello" "hello"
run_test "echo multiple args" "echo a b c" "a b c"
run_test "echo -n flag" "echo -n test | wc -c" "4"

echo ""
echo "--- PWD Commands ---"
run_test "pwd returns path" "pwd" "/Users/merve/Desktop/minishell"

echo ""
echo "--- PIPE Tests ---"
run_test "pipe echo to cat" "echo hello | cat" "hello"
run_test "pipe echo to grep" "echo test | grep test" "test"

echo ""
echo "========== RESULTS =========="
echo -e "Passed: ${GREEN}$PASS${NC}"
echo -e "Failed: ${RED}$FAIL${NC}"
echo "Total: $((PASS + FAIL))"

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✓ ALL TESTS PASSED!${NC}"
    exit 0
else
    echo -e "${RED}✗ Some tests failed${NC}"
    exit 1
fi
