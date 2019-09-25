#include "monty.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

global_t global = { 0, NULL, NULL };

/**
 * bye - destructor
 */
void bye(void) __attribute__((destructor));
void bye(void)
{
	free_stackt(&global.stack);
	free(global.line);
	fclose(global.file);
}

/**
 * main - entry point for program
 * @argc: number of arguments provided to the program (should be 2)
 * @argv: arguments provided to the program
 *
 * Return: EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
	ssize_t bytes_read;
	char *command;
	size_t line_size = 0, i;
	unsigned int ln = 0;
	void (*command_func)(stack_t **, unsigned int);

	if (argc != 2)
	{
		perror("USAGE: monty file");
		exit(EXIT_FAILURE);
	}

	global.file = fopen(argv[1], "r");
	if (!global.file)
	{
		dprintf(STDERR_FILENO, "Error: Can't open file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		ln++;
		global.line = NULL;
		line_size = 0;
		bytes_read = getline(&global.line, &line_size, global.file);
		if (bytes_read == -1)
			break;

		command = strtok(global.line, " ");
		if (!command)
		{
			free(global.line);
			continue;
		}

		if (strcmp(command, "push") == 0)
		{
			global.value = strtok(NULL, " ");
			global.value = strtok(global.value, "\n");
			global.value = strtok(global.value, "\t");
		}
		command = strtok(command, "\n");
		command = strtok(command, "\t");

		command_func = get_command(command);
		if (!command_func)
		{
			dprintf(STDERR_FILENO, "L%ud: unknown instruction %s", ln, command);
			exit(EXIT_FAILURE);
		}

		command_func(&global.stack, ln);

		free(global.line);
	}

	return (EXIT_SUCCESS);
}