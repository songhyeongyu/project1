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
	struct alias *alias_token_struct[MAX_COMMAND_LEN];
	int arr_nr_token = 0;
	static int alias_nr_token = 0;
	int flag_pipe = 0;
	int where_is_pipe = 0;
	char pipe_first[MAX_COMMAND_LEN] = {'\0'};
	char pipe_second[MAX_COMMAND_LEN] = {'\0'};
	char *pipe_first_token[MAX_COMMAND_LEN];
	char *pipe_second_tokens[MAX_COMMAND_LEN];
	// char *alias_name_arr[MAX_COMMAND_LEN];
	// char *alias_detail_arr[MAX_COMMAND_LEN];
	if (strcmp(tokens[0], "exit") == 0)
		return 0;

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

	else if (strcmp(tokens[0], "alias") == 0)
	{
		if (nr_tokens == 1)
		{
			for (int i = 0; i < alias_nr_token; i++)
			{
				fprintf(stderr, "%s: %s\n", alias_token_struct[i]->alias_name, alias_token_struct[i]->alias_detail);
			}
			return 1;
		}

		else
		{
			alias_token_struct[alias_nr_token] = malloc(sizeof(struct alias));
			alias_token_struct[alias_nr_token]->alias_name = malloc(sizeof(char) * MAX_COMMAND_LEN);
			alias_token_struct[alias_nr_token]->alias_detail = malloc(sizeof(char) * MAX_COMMAND_LEN);
			strcat(alias_token_struct[alias_nr_token]->alias_name, tokens[1]);
			for (int i = 2; i < nr_tokens; i++)
			{
				strcat(alias_token_struct[alias_nr_token]->alias_detail, tokens[i]);
				if (i < nr_tokens - 1)
				{
					strcat(alias_token_struct[alias_nr_token]->alias_detail, " ");
				}
			}
			alias_nr_token++;

			return 1;
		}

		return 1;
	}

	else if (nr_tokens > 0)
	{
		pid_t pid;
		// pid_t ppid;
		pid = fork();
		int stat;
		// char buffer[MAX_COMMAND_LEN] = {'\0'};
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
				/*파이프가 존재 한다면 flag bit -> 0*/
				arr_nr_token = parse_command(arr, arr_tokens);
				for (int i = 0; i < arr_nr_token; i++)
				{
					if (strcmp(arr_tokens[i], "|") == 0)
					{
						flag_pipe = 1;
						where_is_pipe = i;
						break;
					}
				}

			
				if(flag_pipe == 0)
				{
					for (int i = 1; i < arr_nr_token; i++)
					{
						for (int j = 0; j < alias_nr_token; j++)
						{
							if (strcmp(arr_tokens[i], alias_token_struct[j]->alias_name) == 0)
							{
								arr_tokens[i] = strdup(alias_token_struct[j]->alias_detail);
							}
						}
					}

					execvp(tokens[0], arr_tokens);
					free_command_tokens(arr_tokens);
					return -1;
				}
				if(flag_pipe == 1){
					for(int i = 0;i<nr_tokens;i++){
						for(int j=0;j<alias_nr_token;j++){
							if(strcmp(tokens[i],alias_token_struct[j]->alias_name) == 0){
								tokens[i] = strdup(alias_token_struct[j]->alias_detail);
							}
						}
					}
				}


				if (flag_pipe == 1)
				{

					for (int i = 0; i < where_is_pipe; i++)
					{
						strcat(pipe_first, tokens[i]);
						if (i < where_is_pipe - 1)
						{
							strcat(pipe_first, " ");
						}
					}

					parse_command(pipe_first, pipe_first_token);
					for (int i = where_is_pipe + 1; i < arr_nr_token; i++)
					{
						strcat(pipe_second, tokens[i]);
						if (i < arr_nr_token - 1)
						{
							strcat(pipe_second, " ");
						}
					}
					parse_command(pipe_second, pipe_second_tokens);
					int pipefd[2];
					// int pstat;
					pid_t p_pid;
					pipe(pipefd);
					p_pid = fork();
					if (p_pid == 0)
					{
						close(pipefd[0]);
						dup2(pipefd[1], STDOUT_FILENO);
						close(pipefd[1]);
						execvp(pipe_first_token[0],pipe_first_token);
						return -1;
						exit(EXIT_FAILURE);
						
					}
					else if(p_pid >0)
					{
						
						close(pipefd[1]);
						dup2(pipefd[0], STDIN_FILENO);
						if(execvp(pipe_second_tokens[0],pipe_second_tokens)<0){
						fprintf(stderr, "Unable to execute %s\n", pipe_second_tokens[0]);
						exit(EXIT_FAILURE);
						}
						exit(EXIT_SUCCESS);
					}
					
				}
				wait(&stat);
				exit(EXIT_SUCCESS);
		

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


