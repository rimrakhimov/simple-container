#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static char child_stack[1048576];

static void print_pid() {
	printf("PID: %ld\n", (long)getpid());
	printf("Parent PID: %ld\n", (long)getppid());
}

static void print_net(char* prefix) {
	printf("%s 'net' Namespace:\n", prefix);
	system("ip link");
	printf("\n\n");
}

static void create_mnt_namespace() {
	printf("New mount ");
}

static int child_fn() {
	print_net("New");

	print_pid();

	char* command;
	size_t n = 1024;
	command = malloc(n);
	while (1) {
		printf("\n$ ");

		getline(&command, &n, stdin);
		/* if (command == NULL) {
			printf("the End");
		} */
		system(command);
	}
	return 0;
}

int main() {
	system("sh new.sh");
	const char *s = getenv("IMAGE_PATH");
	printf("%s\n", s);

/* 	print_net("Original");

	pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, NULL);
	printf("clone() = %ld\n", (long)child_pid);
	printf("\n");

	waitpid(child_pid, NULL, 0); */
	return 0;
}
