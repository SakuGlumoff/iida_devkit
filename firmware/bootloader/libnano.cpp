#include "SEGGER_RTT.h"
#include "stm32l552xx.h"

#include <errno.h>
#include <reent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

struct _reent;

extern "C" {
	int _getpid(void) {
		return 1;
	}

	int _kill(int pid, int sig) {
		(void)pid;
		(void)sig;
		errno = EINVAL;
		return -1;
	}

	void _exit(int status) {
		_kill(status, -1);
		while (1) {} /* Make sure we hang here */
	}

	__attribute__((weak)) int _read(int file, char* ptr, int len) {
		(void)file;
		(void)ptr;
		return len;
	}

	__attribute__((weak)) int _write(int file, char* ptr, int len) {
		(void)file;
		SEGGER_RTT_Write(0, ptr, len);
		return len;
	}

	__attribute__((weak)) ssize_t
	_write_r(struct _reent* r, int file, void const* ptr, size_t len) {
		(void)file;
		(void)r;
		SEGGER_RTT_Write(0, ptr, len);
		return len;
	}

	int _close(int file) {
		(void)file;
		return -1;
	}

	int _fstat(int file, struct stat* st) {
		(void)file;
		st->st_mode = S_IFCHR;
		return 0;
	}

	int _isatty(int file) {
		(void)file;
		return 1;
	}

	int _lseek(int file, int ptr, int dir) {
		(void)file;
		(void)ptr;
		(void)dir;
		return 0;
	}

	int _open(char* path, int flags, ...) {
		(void)path;
		(void)flags;
		/* Pretend like we always fail */
		return -1;
	}

	int _wait(int* status) {
		(void)status;
		errno = ECHILD;
		return -1;
	}

	int _unlink(char* name) {
		(void)name;
		errno = ENOENT;
		return -1;
	}

	int _times(struct tms* buf) {
		(void)buf;
		return -1;
	}

	int _stat(char* file, struct stat* st) {
		(void)file;
		st->st_mode = S_IFCHR;
		return 0;
	}

	int _link(char* old, char* pNew) {
		(void)old;
		(void)pNew;
		errno = EMLINK;
		return -1;
	}

	int _fork(void) {
		errno = EAGAIN;
		return -1;
	}

	int _execve(char* name, char** argv, char** env) {
		(void)name;
		(void)argv;
		(void)env;
		errno = ENOMEM;
		return -1;
	}
}
