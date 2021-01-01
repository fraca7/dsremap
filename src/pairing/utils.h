
#ifndef _UTILS_H
#define _UTILS_H

/**
 * Returns 0 if the file exists, an error code otherwise (like ENOENT)
 */
int test_file(const char* path);

/**
 * Launch a shell command. Returns 0 if successful
 */
int launch_command(const char* cmd);

/**
 * Writes a string to a file. Returns 0 if successful.
 */
int echo_to(const char* path, const char* str);

#endif /* _UTILS_H */
