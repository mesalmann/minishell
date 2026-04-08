*This project has been created as part of the 42 curriculum by mesalman, hdere.*

# Minishell — As beautiful as a shell

## Description

Minishell is a project from the 42 curriculum that challenges you to build a simplified Unix shell from scratch in C.

The goal is to re-implement core behaviors of `bash`, including:
- command execution  
- piping  
- redirections  
- environment variable handling  
- signal management  
- built-in commands  

Through this project, you gain deep, hands-on knowledge of how **processes**, **file descriptors**, and **inter-process communication (IPC)** work at the system level.

---

## Key Features

- Interactive prompt with command history (via `readline`)
- Command execution using absolute paths, relative paths, or `$PATH` lookup
- Pipes (`|`) for chaining multiple commands
- Redirections:
  - `<` input
  - `>` output
  - `>>` append
  - `<<` heredoc
- Environment variable expansion (`$VAR`, `$?`)
- Quote handling:
  - `'` → no interpretation
  - `"` → `$` expansion allowed
- Signal handling:
  - `ctrl-C`, `ctrl-D`, `ctrl-\` behaving like bash
- Built-in commands:
  - `echo`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`
- Only one global variable (used for signal handling)

---

## Architecture Overview

The project is structured into several modular components, each responsible for a specific stage of command processing:

### Lexer
Splits the raw user input into meaningful tokens such as words, operators, and quoted strings.

### Parser
Transforms the token stream into a structured representation (AST or pipeline), defining command relationships and execution order.

### Expander
Resolves environment variables (e.g., `$PATH`, `$HOME`, `$?`) and applies quoting rules according to shell behavior.

### Executor
Handles process creation and command execution using system calls such as `fork`, `execve`, `dup2`, and `pipe`.

### Signals
Manages signal behavior (`SIGINT`, `SIGQUIT`, etc.) to mimic bash-like interactive behavior.

---

## Instructions

### Requirements

- C compiler (`cc`) with `-Wall -Wextra -Werror`
- GNU `readline` library installed
- `make`

### macOS setup (if needed)

```bash
brew install readline
export LDFLAGS="-L$(brew --prefix readline)/lib"
export CPPFLAGS="-I$(brew --prefix readline)/include"
```

### Compilation

```bash
make
```

This will compile the project and produce the `minishell` executable. The Makefile uses `-Wall -Wextra -Werror` flags and will not relink unnecessarily.

### Running

```bash
./minishell
```

You will be greeted with an interactive prompt. From there, you can type any command just as you would in bash.

### Cleaning up

```bash
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + full recompilation
```

---

## Resources

### Official Documentation & References

- [GNU Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
- [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)
- [POSIX Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)
- [Linux man pages — execve(2)](https://man7.org/linux/man-pages/man2/execve.2.html)
- [Linux man pages — fork(2)](https://man7.org/linux/man-pages/man2/fork.2.html)
- [Linux man pages — pipe(2)](https://man7.org/linux/man-pages/man2/pipe.2.html)
- [Linux man pages — dup2(2)](https://man7.org/linux/man-pages/man2/dup2.2.html)
- [Linux man pages — sigaction(2)](https://man7.org/linux/man-pages/man2/sigaction.2.html)
- [Linux man pages — waitpid(2)](https://man7.org/linux/man-pages/man2/waitpid.2.html)

### Articles & Tutorials

- [Writing Your Own Shell — Stephen Brennan](https://brennan.io/2015/01/16/write-a-shell-in-c/)
- [Shell Command Language — Open Group](https://pubs.opengroup.org/onlinepubs/009695399/utilities/xcu_chap02.html)
- [Understanding File Descriptors and Redirection](https://www.cs.cornell.edu/courses/cs414/2004su/homework/shell/shell.html)

### AI Usage

AI tools (ChatGPT / Claude) were used during this project for the following tasks:

- **Conceptual explanations**: Understanding how `fork`/`execve`/`waitpid` interact, how pipe file descriptors need to be managed across child processes, and how heredoc behavior differs from standard redirections.
- **Debugging assistance**: Identifying issues with signal handling edge cases and file descriptor leaks by describing symptoms and asking for potential causes.
- **Code review suggestions**: Asking for feedback on specific functions (e.g., the tokenizer/lexer logic) to spot edge cases we may have missed.
