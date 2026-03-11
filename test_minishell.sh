#!/bin/bash
cd '/mnt/c/Users/hatic/OneDrive/Masaüstü/minishell/minishell'

echo "=== UNSET TESTS ==="
printf 'unset ""\n' | ./minishell 2>/dev/null; echo "unset empty: $?"
printf 'unset =\n' | ./minishell 2>/dev/null; echo "unset =: $?"
printf 'unset "TES.T"\n' | ./minishell 2>/dev/null; echo "unset TES.T: $?"
printf 'unset -TEST\n' | ./minishell 2>/dev/null; echo "unset -TEST: $?"

echo ""
echo "=== EXPORT TESTS ==="
printf 'export --TEST=123\n' | ./minishell 2>/dev/null; echo "export --TEST: $?"
printf 'export -TEST=100\n' | ./minishell 2>/dev/null; echo "export -TEST: $?"
printf 'export =====123\n' | ./minishell 2>/dev/null; echo "export ====123: $?"

echo ""
echo "=== STDOUT TEST ==="
printf 'echo hello world\n' | ./minishell 2>/dev/null
printf 'echo hello\necho world\n' | ./minishell 2>/dev/null

echo ""
echo "=== EXIT TEST ==="
printf 'exit ""\n' | ./minishell 2>/dev/null; echo "exit empty: $?"
printf 'exit +1\n' | ./minishell 2>/dev/null; echo "exit +1: $?"
printf 'exit ++1\n' | ./minishell 2>/dev/null; echo "exit ++1: $?"
