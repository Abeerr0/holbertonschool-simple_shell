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

int main(int ac, char **av)
{
    char *line = NULL, *args[100], *orig_args[100], *commands[100], *token;
    ssize_t read_bytes;
    size_t len = 0;
    int last_status = 0, k, i, cmd_count;

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

        /* 1. التقسيم حسب الفاصل ; أولاً */
        cmd_count = 0;
        token = strtok(line, ";");
        while (token != NULL && cmd_count < 100)
        {
            commands[cmd_count++] = token;
            token = strtok(NULL, ";");
        }
        commands[cmd_count] = NULL;

        /* 2. تنفيذ الأوامر واحداً تلو الآخر */
        for (i = 0; i < cmd_count; i++)
        {
            parse_command(commands[i], args);
            if (args[0] == NULL)
                continue;

            for (k = 0; args[k]; k++)
                orig_args[k] = args[k];
            orig_args[k] = NULL;

            expand_variables(args, last_status);
            execute_command(args, av, line, &last_status);

            for (k = 0; args[k]; k++)
                if (args[k] != orig_args[k])
                    free(args[k]);
        }
    }
    free(line);
    free_env();
    return (last_status);
}
