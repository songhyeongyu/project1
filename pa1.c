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

int run_command(int nr_tokens, char *tokens[])
{	
	
	char arr[MAX_COMMAND_LEN] = {'\0'};
	char *arr_tokens[MAX_COMMAND_LEN];
	
	if (strcmp(tokens[0], "cd") == 0)
	{
		if (nr_tokens == 1)
		{
			chdir(getenv("HOME"));
		}

		else if (nr_tokens > 1 && strcmp(tokens[1], "~") == 0)
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

		if (nr_tokens == 1)
		{

			if (pid == 0)
			{
				execlp(tokens[0], tokens[0], NULL);
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
				parse_command(arr,arr_tokens);
				execvp(tokens[0], arr_tokens);
				free_command_tokens(arr_tokens);
				
			}
		}

		return 1;
	}

	if (strcmp(tokens[0], "exit") == 0)
		return 0;

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
