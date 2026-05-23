#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

extern char **environ;

int _strlen(char *s);
int _strcmp(char *s1, char *s2);
char *_strcpy(char *dest, char *src);
char *_strcat(char *dest, char *src);
char *_strdup(char *str);
int is_delim(char c, const char *delim);
char *_strtok(char *str, const char *delim);
char *_getenv(const char *name);
int handle_builtins(char **args, char *line, char **av, int *last_status);
int handle_cd(char **args, char **av, int *last_status);
int match_env_name(char *env_var, char *name);
int _setenv(char *name, char *value);
int _unsetenv(char *name);
int handle_env_builtins(char **args);
char *get_location(char *command);
int execute_command(char **args, char **av, char *line, int *last_status);
void parse_command(char *line, char **args);
void init_env(void);
void free_env(void);
void sigint_handler(int sig);
char *_itoa(int num);
void expand_variables(char **args, int last_status);
void remove_comments(char *line);
void handle_file_input(char **av);
int handle_help(char **args);

/* Alias functions */
int builtin_alias(char **args);
void expand_aliases(char **args);

#endif
