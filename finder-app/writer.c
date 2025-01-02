#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  // Open syslog with LOG_USER facility
  openlog("writer", LOG_PID | LOG_CONS, LOG_USER);

  // Check argument count
  if (argc != 3) {
    syslog(LOG_ERR, "Insufficient arguments. Usage: %s <file> <string>", argv[0]);
    fprintf(stderr, "Insufficient arguments. Usage: %s <file> <string>\n", argv[0]);
    closelog();
    return 1;
  }

  const char *writefile = argv[1];
  const char *writestr = argv[2];

  // Open the file for writing (create if not exists, truncate if exists)
  int fd = open(writefile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    syslog(LOG_ERR, "Error opening or creating file: %s", writefile);
    perror("Error opening file");
    closelog();
    return 1;
  }

  // Write the string to the file
  ssize_t bytes_written = write(fd, writestr, strlen(writestr));
  if (bytes_written == -1) {
    syslog(LOG_ERR, "Error writing to file: %s", writefile);
    perror("Error writing to file");
    close(fd);
    closelog();
    return 1;
  }

  // Log the successful write operation
  syslog(LOG_DEBUG, "Writing '%s' to '%s'", writestr, writefile);

  // Clean up
  close(fd);
  closelog();

  return 0;
}
