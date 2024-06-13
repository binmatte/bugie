## Bugie: A Simple C Logging Library

Bugie is a lightweight, cross-platform C logging library designed for ease of use and basic logging needs. It provides functionality to log messages with different levels (DEBUG, INFO, WARNING, ERROR) and customize the output stream (stdout, stderr, or a file).

### Features

- **Simple API:** Easy to use and understand.
- **Log levels:** Support for DEBUG, INFO, WARNING, and ERROR levels.
- **Output customisation:** Set log output to stdout, stderr, or a specified file.
- **Thread-safe:** Uses a mutex to protect log operations from race conditions.
- **ANSI color support:** Optionally use ANSI color codes to highlight log levels.

### Usage

1. **Include the bugie.h header file:**
   ```c
   #include "bugie.h"
   ```

2. **Initialize the logging system:**
   ```c
   bugieInit();
   ```

3. **Log messages:**
   ```c
   bugieLogMessage(NULL, BUGIE_LOG_LEVEL_INFO, "MyModule", "This is an info message.");
   ```
    - `NULL` specifies the default output stream (stdout).
    - `BUGIE_LOG_LEVEL_INFO` indicates the log level.
    - `"MyModule"` is the context for the message.
    - `"This is an info message."` is the message itself.

4. **Set log output to a file:**
   ```c
   bugieSetLogOutputPath("my_log.txt");
   ```

5. **Clean up the logging system:**
   ```c
   bugieClean();
   ```

### Installation

1. **Download or clone the repository:**
   ```bash
   git clone https://github.com/binmatte/bugie.git
   ```

2. **Compile the library:**
   ```bash
   gcc -c bugie.c -o bugie.o
   ar rcs libbugie.a bugie.o
   ```

3. **Link the library to your project:**
   ```bash
   gcc program.c -o program -L. -lbugie
   ```

## License:

This library is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contribution:
Contributions are welcome! Please feel free to submit pull requests or open issues.
