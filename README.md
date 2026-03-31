*This project has been created as part of the 42 curriculum by mesalman, hdere.*

# Minishell — As beautiful as a shell

## Description

Minishell is a project from the 42 curriculum that challenges you to build a simplified Unix shell from scratch in C. The goal is to re-implement core behaviors of `bash`, including command execution, piping, redirections, environment variable handling, signal management, and built-in commands. Through this project, you gain deep, hands-on knowledge of how processes, file descriptors, and inter-process communication work at the system level.

Key features implemented:
- Interactive prompt with command history (via `readline`)
- Command execution using absolute paths, relative paths, or `$PATH` lookup
- Pipes (`|`) chaining multiple commands
- Redirections: `<`, `>`, `>>`, and here-document (`<<`)
- Environment variable expansion (`$VAR`, `$?`)
- Single quotes (no interpretation) and double quotes (`$` expanded)
- Signal handling: `ctrl-C`, `ctrl-D`, `ctrl-\` behaving like bash
- Built-in commands: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- One global variable only (stores signal number)

---

## Instructions

### Requirements

- C compiler (`cc`) with `-Wall -Wextra -Werror`
- GNU `readline` library installed
- `make`

On macOS, readline may require:
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
- **README drafting**: The initial structure of this README was generated with AI assistance and then reviewed, corrected, and completed by the project authors.

> All AI-generated content was reviewed, understood, and validated before being included in the project. No AI-generated code was used without full comprehension and testing by the authors.
