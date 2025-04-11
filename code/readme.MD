

# Custom C Shell Program

This custom C shell program provides basic shell functionalities, including command execution, directory navigation, command logging, process management, and file listing with custom color coding.

## Features

### 1. Custom Prompt
- Displays a prompt in the format `<Username@SystemName:~>`.
- If the last foreground command took more than 2 seconds, the prompt shows `<Username@SystemName:~ Command : TimeInSeconds>`.

### 2. Directory Navigation (`hop` Command)
- Navigate to directories using special symbols:
  - `.`: Current directory.
  - `..`: Parent directory.
  - `~`: Home directory.
  - `-`: Previous directory.
- Supports both absolute and relative paths.
- Allows sequential execution of multiple paths provided as arguments.

### 3. Command Logging (`log` Command)
- Maintains a history of up to 15 commands across sessions, stored in `history.txt`.
- Logs commands in both a file and an in-memory `commandLog` array.
- Excludes duplicate consecutive commands from logging.
- Supports:
  - `log`: Displays the log.
  - `log purge`: Clears the log.
  - `log execute <index>`: Executes the command at the specified index in the log.

### 4. Process Management
- **Foreground and Background Processes**:
  - Commands run normally in the foreground.
  - Use `&` to run processes in the background, even if `&` appears mid-input.
  - Displays the process ID (PID) for background processes.
  - Notifies when a background process completes, indicating whether it exited normally or with an error.

### 5. File Listing (`reveal` Command)
- Lists files and directories in lexicographic order.
- Supports `-l` (detailed listing) and `-a` (show hidden files) flags.
- Handles special symbols (`.`, `..`, `~`, `-`) as in the `hop` command.
- Color codes output:
  - Green for executables.
  - White for regular files.
  - Blue for directories.

### 6. Process Information (`proclore` Command)
- Provides detailed information about running processes.
- Displays information such as PID, state, memory usage, and owner.
- Useful for monitoring system resource usage and process status.

### 7. File Operations (`seek` Command)
- Allows reading from and writing to files using the `dup` and `dup2` system calls.
- Facilitates simple file operations, such as redirecting input/output.
- Works within the constraints of basic system calls like `scanf` and `printf` for file handling.


### 8. Process Creation (neonate Command) [Bonus]
- Command: neonate -n [time_arg]

- Prints the PID of the most recently created process on the system every [time_arg] seconds.

- Continues to print the PID every [time_arg] seconds until the user presses the key 'x'.


### 9. Alias Management (alias Command)
- Command: alias [alias_name]=[command]

- Creates or lists aliases for commands.

- Create Alias: alias [alias_name]=[command]
- List Aliases: alias
- 2 functions: 
### a)mk_hop:

- creates a new directory in the given path and hops on that directory
  
### b)hop_seek xyz

- hops on xyz directory and search for the same in that directory


### 10. I/O Redirection
- I/O Redirection changes the default input/output to a file.

- Supported Redirections
-  > : Redirects output to a file. The file will be created if it does not exist. If it exists, it will be overwritten.
-  >>: Redirects output to a file, appending to the file if it already exists. The file will be created if it does not exist.
- <: Redirects input from a file. If the file does not exist, an error message "No such input file found!" will be displayed.


### 11. Pipes
- Pipes are used to pass information between commands. The output of the command on the left is used as input for the command on the right.

- Supported Pipe Functionality
- |: Connects the output of the command on the left to the input of the command on the right.


### 12. Activities
- The activities command prints a list of all processes currently running that were spawned by your shell. The output is displayed in lexicographic order and includes the following information:

- Command Name: The name of the command.
- PID: The process ID.
- State: The current state of the process (Running or Stopped).

### 13. Signals
- The ping command sends a signal to a specified process. You need to provide the PID of the process and the signal number. If the process does not exist, an error message is displayed. The signal number should be taken modulo 32 before checking which signal it corresponds to.

### Signal Handling:

- Ctrl - C: Interrupt any currently running foreground process by sending it the SIGINT signal. It has no effect if no foreground process is currently running.
- Ctrl - D: Log out of your shell (after killing all processes) while having no effect on the actual terminal.
- Ctrl - Z: Push the (if any) running foreground process to the background and change its state from "Running" to "Stopped". It has no effect on the shell if no foreground process is running.

### 14.fg Command
-The fg command brings a running or stopped background process to the foreground, allowing it to take control of the terminal.

#### Parameters:

- <pid>: The process ID of the background process to bring to the foreground.

#### Behavior:

- If the process with the specified PID exists and is currently running or stopped, it will be brought to the foreground, and its state will be changed to "Running".
- If no process with the specified PID exists, print "No such process found".


### 15.bg Command
- The bg command changes the state of a stopped background process to running (in the background).

#### Parameters:

- <pid>: The process ID of the stopped background process to change to running.

#### Behavior:

- If the process with the specified PID exists and is currently stopped, its state will be changed to "Running" (in the background).
- If no process with the specified PID exists, print "No such process found".


### 18. iMan
- The iMan command fetches man pages from the internet using sockets and outputs them to the terminal. It retrieves man pages from http://man.he.net/ and displays the content, including any HTML tags present.

#### Parameters:

- <command_name>: The name of the man page you want to fetch.
#### Behavior:

- Fetches the man page for the given <command_name> from http://man.he.net/.
- If the page exists, the man page content is printed to the terminal.
- If the page does not exist, it prints the response from the page as is, including any HTML tags.
-If more than one argument is provided, only the first argument is considered.
- The header received during the GET request is not printed.
