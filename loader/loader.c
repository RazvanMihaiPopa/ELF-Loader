/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "exec_parser.h"



static so_exec_t *exec;
static char *path1;

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	/* TODO - actual loader implementation */
	int i=0;

	for (i = 0; i < exec->segments_no; i++) {
		/* check to see in which segment the fault occurs */
		if (((uintptr_t)info->si_addr) >= exec->segments[i].vaddr &&
		((uintptr_t)info->si_addr) <= exec->segments[i].vaddr+exec->segments[i].mem_size) {
			/* once found, break */
			break;
		}
	}
	/* not in a segment || code for invalid permissions */
	if (i >= exec->segments_no || info->si_code == SEGV_ACCERR || signum != SIGSEGV) {
		/* we use default handler */
		signal(signum, SIG_DFL);
		raise(signum);
	} else if (info->si_code == SEGV_MAPERR) { /* code for address not maped */
		/* we create the file descriptor, using the global path1 */
		int fd = open(path1, O_RDONLY);
		/* x = offset where we want to map the page */
		int x = 0;
		/* offset where the fault happens */
		int fault_offset = (int)info->si_addr - (int)exec->segments[i].vaddr;

		while (x + getpagesize() <= fault_offset)
			x += getpagesize();
		if (x >= exec->segments[i].file_size && x <= exec->segments[i].mem_size) {
			/* using MAP_ANONYMOUS the mapping is initialized with 0 */
			mmap(((void *)exec->segments[i].vaddr + x), (size_t)getpagesize(), PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
		} else if (x + getpagesize() <= exec->segments[i].file_size) {
			/* we use mmap, while also initializing it using fd and given offset for that  */
			mmap(((void *)exec->segments[i].vaddr + x), (size_t)getpagesize(), PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_FIXED, fd, exec->segments[i].offset + x);
		} else if (x < exec->segments[i].file_size) {
			/* we map, copying what's left and setting 0 for the portion after file_size */
			mmap(((void *)exec->segments[i].vaddr + x), (size_t)getpagesize(), PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_FIXED, fd, exec->segments[i].offset + x);
			memset(((void *)exec->segments[i].vaddr + exec->segments[i].file_size), 0,
			x + getpagesize() - exec->segments[i].file_size);
		}
		/* change to the right permissions stored in exec->segments[i].perm */
		mprotect(((void *)exec->segments[i].vaddr + x), (size_t)getpagesize(), exec->segments[i].perm);
		close(fd);
	}
}

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	path1 = malloc(strlen(path));
	strcpy(path1, path);
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	so_start_exec(exec, argv);

	return -1;
}
