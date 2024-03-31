/**********************************************************************
 * Copyright (c) 2020-2024
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parser.h"
#include "list_head.h"
extern char **environ;

/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */
struct alias
{
	char *alias_detail;
	char *alias_name;
};

int run_command(int nr_tokens, char *tokens[])
{

	char arr[MAX_COMMAND_LEN] = {'\0'};
	char *arr_tokens[MAX_COMMAND_LEN];
	struct alias *alias_token_struct = (struct alias *)malloc(sizeof(struct alias));
	alias_token_struct->alias_name = (char *)malloc(sizeof(char) * sizeof(MAX_COMMAND_LEN));
	alias_token_struct->alias_detail = (char *)malloc(sizeof(char) * sizeof(MAX_COMMAND_LEN));
	int alias_name_nr_token = 0;
	// int alias_detail_nr_token = 0;
	char alias_name[MAX_COMMAND_LEN] = {'\0'};
	char alias_detail[MAX_COMMAND_LEN] = {'\0'};
	char *alias_name_tokens[MAX_COMMAND_LEN];
	// char *alias_detail_tokens[MAX_COMMAND_LEN];

	if (strcmp(tokens[0], "exit") == 0)
		return 0;

	else if (strcmp(tokens[0], "alias") == 0)
	{
		pid_t pid;
		pid = fork();
		int stat;

		// printf("%d",alias_nr_token);
		if (nr_tokens > 1)
		{
			if (pid == 0)
			{

				// printf("%s", tokens[1]);
				for (int i = 1; i < nr_tokens; i++)
				{
					if (i == 1)
					{
						strcat(alias_token_struct->alias_name, tokens[i]);
						printf("%s", tokens[i]);
						printf("alias: %s", alias_token_struct->alias_name);
					}
					else if (i != 1)
					{
						strcat(alias_token_struct->alias_detail, tokens[i]);
					}

					if (i != 1 && i < nr_tokens - 1)
					{
						strcat(alias_token_struct->alias_detail, " ");
					}
				}
				strcpy(alias_name, alias_token_struct->alias_name);
				printf("%s\n", alias_name);
				strcpy(alias_detail, alias_token_struct->alias_detail);
				printf("%s\n", alias_detail);
				strcpy(alias_token_struct->alias_name, alias_token_struct->alias_detail);

				return 0;
			}
			else
			{
				wait(&stat);
			}
		}
		else if (nr_tokens == 1)
		{
			if (pid == 0)
			{
				printf("alias: %s", alias_token_struct->alias_name);
				printf("%s", alias_name);
				printf("%s", alias_name);
				alias_name_nr_token = parse_command(alias_name, alias_name_tokens);
				for (int i = 0; i < alias_name_nr_token; i++)
				{
					printf("%s", alias_name_tokens[i]);
				}

				return 0;
			}
			else
			{
				wait(&stat);
			}
		}

		return 1;
	}

	else if (strcmp(tokens[0], "cd") == 0)
	{

		if (nr_tokens == 1 || strcmp(tokens[1], "~") == 0)
		{
			chdir(getenv("HOME"));
		}

		else if (nr_tokens == 1)
		{
			chdir(getenv("HOME"));
		}

		else
		{
			chdir(tokens[1]);
		}

		return 1;
	}

	else if (nr_tokens > 0)
	{
		pid_t pid;
		pid = fork();
		int stat;

		if (nr_tokens == 1)
		{

			if (pid == 0)
			{
				execlp(tokens[0], tokens[0], NULL);
				return -1;
			}
			else
			{
				wait(&stat);
			}
		}

		else if (nr_tokens > 1)
		{

			if (pid == 0)
			{
				for (int i = 0; i < nr_tokens; i++)
				{
					strcat(arr, tokens[i]);
					if (i < nr_tokens - 1)
					{
						strcat(arr, " ");
					}
				}
				parse_command(arr, arr_tokens);
				execvp(tokens[0], arr_tokens);
				free_command_tokens(arr_tokens);
				return -1;
			}

			else
			{
				wait(&stat);
			}
		}

		return 1;
	}

	return -1;
}

/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
int initialize(int argc, char *const argv[])
{
	return 0;
}

/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
void finalize(int argc, char *const argv[])
{
}
