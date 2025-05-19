# Minishell 🔧

A minimalist UNIX command-line interpreter written in C, implementing a subset of bash-like features.

## Features 🚀

### Core Capabilities
- **Command execution** with PATH resolution
- **Pipeline support** (`|`) for command chaining
- **Redirections**:
  - Input (`<`), output (`>`), append (`>>`)
  - Heredocuments (`<<`) with variable expansion
- **Environment management**:
  - Variable expansion (`$VAR`)
  - Special parameter `$?` for exit status
  - Built-in `env` command

### Built-in Commands
| Command    | Description                     |
|------------|---------------------------------|
| `echo`     | With `-n` option support       |
| `cd`       | Relative/absolute paths        |
| `pwd`      | Print working directory        |
| `export`   | Environment variable creation  |
| `unset`    | Variable removal               |
| `env`      | Display environment            |
| `exit`     | With optional status code      |

### Advanced Features
- **Signal handling**:
  - Ctrl-C (SIGINT) for process interruption
  - Ctrl-\ (SIGQUIT) silent handling
- **Error handling**:
  - Syntax error detection
  - Permission validation
  - Command not found handling
- **Memory management**:
  - Clean resource freeing
  - Valgrind-approved allocations

## Installation 📦

Just run make, and let it compile. Then, just run ./minishell, and enjoy!

### Dependencies
```bash
sudo apt-get install libreadline-dev  # For Debian/Ubuntu
