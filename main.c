#include "shell.h"

void sigint_handler(int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\n($) ", 5);
}

void init_env(void)
{
    int i = 0;
    char **new_env;

    if (!environ)
    {
        environ = malloc(sizeof(char *));
        if (environ)
            environ[0] = NULL;
        return;
    }
    while (environ[i])
        i++;
    new_env = malloc(sizeof(char *) * (i + 1));
    if (!new_env)
        exit(1);
    for (i = 0; environ[i]; i++)
        new_env[i] = _strdup(environ[i]);
    new_env[i] = NULL;
    environ = new_env;
}

void free_env(void)
{
    int i = 0;

    if (environ)
    {
        for (i = 0; environ[i]; i++)
            free(environ[i]);
        free(environ);
        environ = NULL;
    }
}

int parse_operators(char *line, char **cmds, int *ops)
{
    int i, c_idx = 1;

    cmds[0] = line;
    ops[0] = 0;
    for (i = 0; line[i]; i++)
    {
        if (line[i] == ';')
        {
            line[i] = '\0';
            cmds[c_idx] = &line[i + 1];
            ops[c_idx++] = 0;
        }
        else if (line[i] == '&' && line[i + 1] == '&')
        {
            line[i] = '\0';
            line[i + 1] = '\0';
            cmds[c_idx] = &line[i + 2];
            ops[c_idx++] = 1;
            i++;
        }
        else if (line[i] == '|' && line[i + 1] == '|')
        {
            line[i] = '\0';
            line[i + 1] = '\0';
            cmds[c_idx] = &line[i + 2];
            ops[c_idx++] = 2;
            i++;
        }
    }
    cmds[c_idx] = NULL;
    return (c_idx);
}

void process_commands(char **c, int *ops, int c_id, char **av, char *l, int *ls)
{
    char *args[100], *orig_args[100];
    int i, k;

    for (i = 0; i < c_id; i++)
    {
        if (ops[i] == 1 && *ls != 0)
            continue;
        if (ops[i] == 2 && *ls == 0)
            continue;

        parse_command(c[i], args);
        if (args[0] == NULL)
            continue;

        for (k = 0; args[k]; k++)
            orig_args[k] = args[k];
        orig_args[k] = NULL;

        expand_aliases(args);

        if (_strcmp(args[0], "alias") == 0)
        {
            *ls = builtin_alias(args);
            continue;
        }

        expand_variables(args, *ls);
        execute_command(args, av, l, ls);

        for (k = 0; args[k]; k++)
            if (args[k] != orig_args[k])
                free(args[k]);
    }
}

int main(int ac, char **av)
{
    char *line = NULL, *cmds[100];
    int ops[100], last_status = 0, c_idx;
    ssize_t read_bytes;
    size_t len = 0;

    (void)ac;
    signal(SIGINT, sigint_handler);
    init_env();
    while (1)
    {
        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 4);
        read_bytes = getline(&line, &len, stdin);
        if (read_bytes <= 0)
        {
            if (isatty(STDIN_FILENO) && read_bytes == 0)
                write(STDOUT_FILENO, "\n", 1);
            free(line);
            free_env();
            exit(last_status);
        }
        if (line[read_bytes - 1] == '\n')
            line[read_bytes - 1] = '\0';
        remove_comments(line);

        c_idx = parse_operators(line, cmds, ops);
        process_commands(cmds, ops, c_idx, av, line, &last_status);
    }
    free(line);
    free_env();
    return (last_status);
}
