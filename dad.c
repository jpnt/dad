#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "config.h"

const DaemonGroups* daemon_groups_find(const char *program)
{
	for (int i = 0; dg[i].program != NULL; i++) {
		if (strcmp(dg[i].program, program) == 0) {
			return &dg[i];
		}
	}
	return NULL;
}

void execute_command(const char *cmd)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0) {
		if (execlp("sh", "sh", "-c", cmd, (char*)NULL) != 0) {
			perror("execlp");
			exit(EXIT_FAILURE);
		}
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		fprintf(stderr, "error: command '%s' failed with exit status %d\n",
				cmd, WEXITSTATUS(status));
	}
}

void manage_daemons(const char *action, const char *const daemons[])
{
	for (int i = 0; daemons[i] != NULL; i++) {
		char cmd[256];
		snprintf(cmd, sizeof(cmd), "%s %s", action, daemons[i]);
		execute_command(cmd);
	}
}

void start_daemons(const char *const daemons[])
{
	manage_daemons(DAEMON_START, daemons);
}

void stop_daemons(const char *const daemons[])
{
	manage_daemons(DAEMON_STOP, daemons);
}

void run_program(const char *program, const char *const daemons[])
{
	start_daemons(daemons);

	execute_command(program);

	stop_daemons(daemons);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s {start|stop|run} <program>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const DaemonGroups *dg = NULL;
	const char *action = argv[1];
	const char *program = argv[2];

	dg = daemon_groups_find(program);
	if (dg == NULL) {
		fprintf(stderr, "error: unknown program: %s\n", program);
		return EXIT_FAILURE;
	}

	if (strcmp(action, "start") == 0) {
		start_daemons(dg->daemons);
	}
	else if (strcmp(action, "stop") == 0) {
		stop_daemons(dg->daemons);
	}
	else if (strcmp(action, "run") == 0) {
		run_program(program, dg->daemons);
	}
	else {
		fprintf(stderr, "error: invalid action: %s\n", action);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
