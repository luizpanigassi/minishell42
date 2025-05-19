# Minishell 🔧

A minimalist UNIX command-line interpreter written in C, implementing a subset of bash-like features. This
is the short version. The long version is the craziest, biggest project in Ecole 42. It is a veritable monster
for us mortals with zero experience in coding. Pipes, redirects, heredoc? WHAT IN THE HELL IS THAT? Those were the
questions I had to answer before diving into it. It is not exactly a difficult project, but it is HUGE. It was
interesting to see it take shape bit by bit, and live this journey.

Learn your built-ins. Learn what you must do. Learn the libraries, the options you have in your hands, study bash,
create garbage collectors, check every single malloc you make, move slowly. Don't rush it. Otherwise you'll end up
like me, with functions having over 100 lines, refactoring like crazy, and breaking the code.

If you are a student of 42 around the world, please, take a look at my code if you must, but don't copy it. Learn
how to work stuff. My code is not perfect, I'm pretty sure I still have some kinks to work out. Good luck, friend.

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

```
make

./minishell
```
# Example session
```
minishell> ls -l | grep .c | wc -l
minishell> echo "Hello $USER" > greeting.txt
minishell> < input.txt grep error | tail -n5 >> output.txt
```

### Dependencies
```bash
sudo apt-get install libreadline-dev  # For Debian/Ubuntu
```
## Authors 👩💻👨💻

**Core Contributors**  

[Luiz Panigassi](https://github.com/luizpanigassi) - Parsing, structure, signal handling 

[João Cologne](https://github.com/jocologne) - Builtin implementation, refactoring, garbage collection

**Special Thanks**  
[42 School](https://www.42.fr/) - Project guidelines and inspiration
