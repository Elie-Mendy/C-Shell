# bash-shell : a clone of the famous shell

- [Introduction](#introduction)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Compilation](#compilation)
- [Usage](#usage)
- [Supported Commands](#supported-commands)
- [Example Usage](#example-usage)
- [Author](#author)
- [Contribution](#contribution)
- [License](#license)


## Introduction

This project aims to provide hands-on understanding of basic shell programming concepts, process management, and input/output redirections on Linux systems.

> [!CAUTION]
> This shell implementation is intended for educational purposes only. 
> It should not be used in a production environment.

## Features

- Reading and parsing command lines
- Execution of commands in foreground and background
- Handling input/output redirections
- Processing pipes (`|`) for inter-process communication
- Implementation of custom commands like `cd` and `exit`

## Prerequisites

- Unix-like operating system (Linux, macOS)
- GCC compiler
- Readline library (for command autocompletion and history)

## Compilation

To compile bash-shell, use the following command:
```sh
gcc -o bash-shell main.c
```

## Usage

To launch the shell, simply run:
```sh
./bash-shell
```

### Supported Commands

bash-shell supports all standard commands available on a Unix shell, as well as some custom commands:
- `cd <dir>` : Change the current directory.
- `exit` : Exit the shell.
- `<command> &` : Execute the command in the background.
- `<command> > <file>` : Redirect standard output to a file.
- `<command> < <file>` : Redirect standard input from a file.
- `<command> | <command>` : Use a pipe to connect two commands.

## Example Usage

Here are some example commands you can run in `bash-shell`:

```sh
cd /home/user
ls -l
cat file.txt | grep "pattern"
gcc -o myprog myprog.c > output.log
./myprog &
```

## Author

- **Elie MENDY**

## Contribution

Contributions are welcome! If you would like to improve this project, please follow these steps:

1. Fork the repository
2. Create a branch for your feature (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

> [!TIP]
> If this project has been helpful to you, consider leaving a star to show your support!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
