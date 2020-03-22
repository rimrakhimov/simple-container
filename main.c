#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mount.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 1024

static char child_stack[1048576];

const char* img_name = "fs_image.img";
const char* mount_dir = "/home";

static void print_pid() {
	printf("PID: %ld\n", (long)getpid());
	printf("Parent PID: %ld\n", (long)getppid());
}

static void print_net(char* prefix) {
	printf("\t%s 'net' Namespace:\n\n", prefix);
	system("ip link");
	printf("\n\n");
}

// concatenate two strings with space between
// total length of which does not exceed MAX_COMMAND_LENGTH
static void concat_for_command(char* res, const char* s1, const char* s2) {
	memset(res, 0, MAX_COMMAND_LENGTH);
	strcpy(res, s1);
	strcat(res, " ");
	strcat(res, s2);
}

static void exec_command(const char* command_prefix, const char* param) {
	char command[MAX_COMMAND_LENGTH];
	concat_for_command(command, command_prefix, param);
	system(command);
}

// creates mounted
static void create_mnt_namespace_and_print(const char* img_name, const char* mount_dir) {
	unshare(CLONE_NEWNS);
	system("mount --make-private -o remount /");

	// initialize filesystem that will be mounted
	exec_command("sh", "create_fs.sh");

	// show content of the directory before mounting
	printf("\n\tContainer '%s' directory before mounting:\n", mount_dir);
	exec_command("ls", mount_dir);

	char param[1024];
	concat_for_command(param, img_name, mount_dir);
	exec_command("mount -t ext4 -o loop", param);
	printf("\n");
	exec_command("mkdir", "/home/cont_folder");
	printf("\n\tContainer '%s' directory after mounting\n", mount_dir);
	exec_command("ls", mount_dir);
}

static int child_fn() {
	// allows parent process to set up network
	sleep(3);
	system("ifconfig veth1 10.1.1.2/24 up");
	print_net("New");


	printf("\t\tMOUNTING PROCESS\n");
	create_mnt_namespace_and_print(img_name, mount_dir);
	system("mount -t proc proc /proc --make-private");

	// allows parent process to print mounted directory
	sleep(20);

	printf("\n\n\t\tCHILD PROCESS TABLES\n\n");
	print_pid();
	printf("\n");
	system("ps");

	// clean-up
	while (! umount("/home")) ;

	while (! umount("/proc")) ;
/*	char* command;
	size_t n = 1024;
	command = malloc(n);
	while (1) {
		printf("\n$ ");

		getline(&command, &n, stdin);
		if (command == NULL) {
			printf("the End");
		}
		system(command);
	} */
	return 0;
}

int main() {
	printf("\t\tCLONING...\n\n");
	pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWPID | CLONE_NEWNET | SIGCHLD, NULL);
	printf("clone() = %ld\n", (long)child_pid);
	printf("\n");

	printf("\t\tNETWORK SETTING\n\n");
	char command[MAX_COMMAND_LENGTH];
	char spid[12];
	sprintf(spid, "%d", child_pid);
	concat_for_command(command, "ip link add name veth0 type veth peer name veth1 netns", spid);
	system(command);
	system("ifconfig veth0 10.1.1.1/24 up");
 	print_net("Original");

	// allows child process to create new mount namespace
	//  and mount a file system
	sleep(15);

	printf("\n\tOriginal process. Folder '%s' content:\n", mount_dir);
	exec_command("ls", mount_dir);

	waitpid(child_pid, NULL, 0);
	return 0;
}
