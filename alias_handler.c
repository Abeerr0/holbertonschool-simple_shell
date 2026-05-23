#include "shell.h"
#include <stdlib.h>
#include <unistd.h>

typedef struct alias_node {
    char *name;
    char *val;
} alias_t;

static alias_t aliases[100];
static int alias_count = 0;

static int my_strcmp(char *s1, char *s2)
{
    if (!s1 || !s2) return (1);
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

static int my_strlen(char *s)
{
    int i = 0;
    while (s && s[i])
        i++;
    return (i);
}

static char *my_strdup(char *s)
{
    char *d;
    int i, len = my_strlen(s);

    d = malloc(len + 1);
    if (!d)
        return (NULL);
    for (i = 0; i <= len; i++)
        d[i] = s[i];
    return (d);
}

int print_alias(char *name)
{
    int i;
    for (i = 0; i < alias_count; i++)
    {
        if (my_strcmp(aliases[i].name, name) == 0)
        {
            write(STDOUT_FILENO, aliases[i].name, my_strlen(aliases[i].name));
            write(STDOUT_FILENO, "='", 2);
            write(STDOUT_FILENO, aliases[i].val, my_strlen(aliases[i].val));
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
        if (my_strcmp(aliases[i].name, name) == 0)
        {
            free(aliases[i].val);
            aliases[i].val = my_strdup(val);
            return;
        }
    }
    if (alias_count < 100)
    {
        aliases[alias_count].name = my_strdup(name);
        aliases[alias_count].val = my_strdup(val);
        alias_count++;
    }
}

int builtin_alias(char **args)
{
    int i, j, ret = 0;
    char *val, *temp_val;

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
            temp_val = val;
            if (temp_val[0] == '\'' && temp_val[my_strlen(temp_val) - 1] == '\'')
            {
                temp_val[my_strlen(temp_val) - 1] = '\0';
                temp_val++;
            }
            set_alias(args[i], temp_val);
            args[i][j] = '=';
        }
        else
        {
            if (print_alias(args[i]) != 0)
                ret = 1;
        }
    }
    return (ret);
}

void expand_aliases(char **args)
{
    int i, j, changed;
    char *current;

    if (!args || !args[0])
        return;

    current = args[0];
    for (j = 0; j < 10; j++)
    {
        changed = 0;
        for (i = 0; i < alias_count; i++)
        {
            if (my_strcmp(aliases[i].name, current) == 0)
            {
                current = aliases[i].val;
                changed = 1;
                break;
            }
        }
        if (!changed) break;
    }
    
    if (current != args[0])
    {
        args[0] = my_strdup(current);
    }
}
