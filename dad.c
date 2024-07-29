#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config.h"

#ifdef DAD_RUNIT
#ifdef DAD_SYSTEMD
#error "DAD_RUNIT and DAD_SYSTEMD cannot be both defined at the same time"
#endif
#endif

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
		printf("dad: cmd: %s\n", cmd);
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

void enable_daemons(const char *const daemons[])
{
#ifdef DAD_SYSTEMD
	manage_daemons(DAEMON_ENABLE, daemons);
	return;
#endif
#ifdef DAD_RUNIT
	for (int i = 0; daemons[i] != NULL; i++) {
		char cmd[256];
		snprintf(cmd, sizeof(cmd), "%s %s/%s %s/",
				DAEMON_ENABLE, SV_DIR, daemons[i], SV_DIR_ENABLED);
		printf("dad: cmd: %s\n", cmd);
		execute_command(cmd);
	}
	return;
#endif
	fprintf(stderr, "error: init system not defined");
	exit(EXIT_FAILURE);
}

void disable_daemons(const char *const daemons[])
{
#ifdef DAD_SYSTEMD
	manage_daemons(DAEMON_DISABLE, daemons);
	return;
#endif
#ifdef DAD_RUNIT
	for (int i = 0; daemons[i] != NULL; i++) {
		char cmd[256];
		snprintf(cmd, sizeof(cmd), "%s %s/%s",
				DAEMON_DISABLE, SV_DIR_ENABLED, daemons[i]);
		printf("dad: cmd: %s\n", cmd);
		execute_command(cmd);
	}
	return;
#endif
	fprintf(stderr, "error: init system not defined");
	exit(EXIT_FAILURE);
}

void run_program(const char *program, const char *const daemons[], int stop_disable_after_run)
{
	enable_daemons(daemons);
	start_daemons(daemons);

	execute_command(program);

	if (stop_disable_after_run) {
		stop_daemons(daemons);
		disable_daemons(daemons);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s {start|stop|enable|disable|run} <program>\n", argv[0]);
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
	else if (strcmp(action, "enable") == 0) {
		enable_daemons(dg->daemons);
	}
	else if (strcmp(action, "disable") == 0) {
		disable_daemons(dg->daemons);
	}
	else if (strcmp(action, "run") == 0) {
		run_program(program, dg->daemons, dg->stop_disable_after_run);
	}
	else {
		fprintf(stderr, "error: invalid action: %s\n", action);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
