#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
  // Check if the command is NULL, as system() requires a valid string or NULL.
  if (cmd == NULL)
  {
    return false; // NULL command should return false.
  }

  // Call system() with the provided command.
  int ret = system(cmd);

  // Check if system() returned an error (-1).
  if (ret == -1)
  {
    return false;
  }

  // Check the exit status of the command executed by system().
  // WEXITSTATUS extracts the return code of the command if it terminated normally.
  if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0)
  {
    return true;
  }

  return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
  if (count < 1)
  {
    return false; // No command specified, return failure.
  }

  va_list args;
  va_start(args, count);

  // Allocate memory for the command and arguments
  char *command[count + 1];
  for (int i = 0; i < count; i++)
  {
    command[i] = va_arg(args, char *);
  }
  command[count] = NULL; // Null-terminate the arguments array.

  va_end(args);

  // Create a child process
  pid_t pid = fork();

  if (pid < 0)
  {
    // Fork failed
    return false;
  }
  else if (pid == 0)
  {
    // In the child process
    execv(command[0], command);

    // If execv() fails, exit child with failure status
    _exit(EXIT_FAILURE);
  }
  else
  {
    // In the parent process
    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        // waitpid() failed
        return false;
    }

    // Check if the child exited normally and its exit status is 0
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        return true;
    }
  }

  return false; // Command execution failed.
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
  if (count < 1 || outputfile == NULL)
  {
    return false; // Invalid input
  }

  va_list args;
  va_start(args, count);

  // Allocate memory for the command and arguments
  char *command[count + 1];
  for (int i = 0; i < count; i++)
  {
    command[i] = va_arg(args, char *);
  }
  command[count] = NULL; // Null-terminate the arguments array.

  va_end(args);

  // Create a child process
  pid_t pid = fork();

  if (pid < 0)
  {
    // Fork failed
    return false;
  }
  else if (pid == 0)
  {
    // In the child process

    // Open the output file for writing, creating it if necessary
    int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        // Failed to open the file
        _exit(EXIT_FAILURE);
    }

    // Redirect standard output (stdout) to the file
    if (dup2(fd, STDOUT_FILENO) < 0)
    {
        // Failed to redirect stdout
        close(fd);
        _exit(EXIT_FAILURE);
    }

    // Close the file descriptor as it's no longer needed
    close(fd);

    // Execute the command
    execv(command[0], command);

    // If execv() fails, exit child with failure status
    _exit(EXIT_FAILURE);
  }
  else
  {
    // In the parent process
    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
      // waitpid() failed
      return false;
    }

    // Check if the child exited normally and its exit status is 0
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
      return true;
    }
  }

  return false; // Command execution failed
}