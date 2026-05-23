#include "shell.h"
#include <stdlib.h>
#include <unistd.h>

typedef struct alias_node {
    char *name;
    char *val;
} alias_t;

static alias_t aliases[100];
static int alias_count = 0;

int print_alias(char *name)
{
    int i;
    for (i = 0; i < alias_count; i++)
    {
        if (_strcmp(aliases[i].name, name) == 0)
        {
            write(STDOUT_FILENO, aliases[i].name, _strlen(aliases[i].name));
            write(STDOUT_FILENO, "='", 2);
            write(STDOUT_FILENO, aliases[i].val, _strlen(aliases[i].val));
            write(STDOUT_FILENO, "'\n", 2);
            return (0);
        }
    }
    return (1);
}

void set_alias(char *name, char *val)
{
    int i;
    for (i = 0; i < alias_count; i++)
    {
        if (_strcmp(aliases[i].name, name) == 0)
        {
            free(aliases[i].val);
            aliases[i].val = _strdup(val);
            return;
        }
    }
    if (alias_count < 100)
    {
        aliases[alias_count].name = _strdup(name);
        aliases[alias_count].val = _strdup(val);
        alias_count++;
    }
}

int builtin_alias(char **args)
{
    int i, j;
    char *val;

    if (args[1] == NULL)
    {
        for (i = 0; i < alias_count; i++)
            print_alias(aliases[i].name);
        return (0);
    }
    for (i = 1; args[i]; i++)
    {
        val = NULL;
        for (j = 0; args[i][j]; j++)
        {
            if (args[i][j] == '=')
            {
                args[i][j] = '\0';
                val = &args[i][j + 1];
                break;
            }
        }
        if (val)
        {
            set_alias(args[i], val);
            args[i][j] = '=';
        }
        else
            print_alias(args[i]);
    }
    return (0);
}

void expand_aliases(char **args)
{
    int i, j, changed;
    if (!args || !args[0])
        return;
    
    for (j = 0; j < 10; j++)
    {
        changed = 0;
        for (i = 0; i < alias_count; i++)
        {
            if (_strcmp(aliases[i].name, args[0]) == 0)
            {
                args[0] = aliases[i].val;
                changed = 1;
                break;
            }
        }
        if (!changed) break;
    }
}
