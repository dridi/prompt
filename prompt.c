/*-
 * Prompt
 * Copyright (C) 2017  Dridi Boukelmoune
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <readline/readline.h>
#ifdef HAVE_READLINE_HISTORY_H
#  include <readline/history.h>
#endif

#define PIPE(fds)		\
do {				\
	if (pipe(fds) != 0)	\
		fail("pipe");	\
} while (0)

#define DUP2(src, dest)			\
do {					\
	if (dup2(src, dest) != dest)	\
		fail("dup2");		\
} while (0)

static pid_t cmd_pid;
static char save_first;
static int pipe_in[2], pipe_out[2], pipe_err[2];

static void
quit(int status)
{

	rl_callback_handler_remove();
	exit(status);
}

static void
fail(const char *s)
{

	perror(s);
	quit(1);
}

static void
line_cb(char *line)
{

	if (line == NULL)
		quit(0);

	if (*line == '\0')
		return;

#ifdef HAVE_READLINE_HISTORY_H
	add_history(line);
#endif

	(void)dprintf(pipe_in[1], "%s\n", line);
	rl_callback_handler_install("> ", line_cb);
}

static void
erase_line()
{

	save_first = *rl_line_buffer;
	*rl_line_buffer = '\0';
	rl_save_prompt();
	rl_redisplay();
}

static void
restore_line()
{

	*rl_line_buffer = save_first;
	rl_restore_prompt();
	rl_forced_update_display();
}

static void
sigchld(int sig)
{
	int i, wstatus;

	(void)sig;
	if (cmd_pid == 0)
		return;

	i = waitpid(cmd_pid, &wstatus, WNOHANG);
	if (i < 0)
		fail("waitpid");

	if (WIFEXITED(wstatus)) {
		cmd_pid = 0;
		erase_line();
		kill(getpid(), SIGINT);
	}
}

static void
start(char * const *argv)
{

	(void)signal(SIGCHLD, sigchld);

	cmd_pid = fork();
	if (cmd_pid == -1)
		fail("fork");

	if (cmd_pid == 0) {
		(void)close(pipe_in[1]);
		(void)close(pipe_out[0]);
		(void)close(pipe_err[0]);
		DUP2(pipe_in[0], STDIN_FILENO);
		DUP2(pipe_out[1], STDOUT_FILENO);
		DUP2(pipe_err[1], STDERR_FILENO);
		execvp(*argv, argv);
		fail("exec");
	}

	(void)close(pipe_in[0]);
	(void)close(pipe_out[1]);
	(void)close(pipe_err[1]);
}

static void
forward_output(FILE *file, int fd)
{
	char buf[1024];
	ssize_t len;

	erase_line();

	len = read(fd, buf, sizeof buf);
	if (len < 0)
		fail("read");
	(void)fwrite(buf, (size_t)len, 1, file);
	if (buf[len - 1] != '\n')
		(void)fprintf(file, "\n");

	restore_line();
}

int
main(int argc, char * const *argv)
{
	struct pollfd pfd[3];
	int i;

	if (!isatty(STDIN_FILENO)) {
		fprintf(stderr, "Error: not running in a tty\n");
		exit(1);
	}

	if (argc == 1)
		abort();

	PIPE(pipe_in);
	PIPE(pipe_out);
	PIPE(pipe_err);

	rl_callback_handler_install("> ", line_cb);

	start(argv + 1);

	pfd[0].fd = STDIN_FILENO;
	pfd[0].events = POLLIN;
	pfd[1].fd = pipe_out[0];
	pfd[1].events = POLLIN;
	pfd[2].fd = pipe_err[0];
	pfd[2].events = POLLIN;

	while (1) {
		i = poll(pfd, 3, -1);
		if (i == -1 && errno == EINTR)
			continue;

		if (i == -1)
			fail("poll");

		if (pfd[1].revents & POLLIN)
			forward_output(stdout, pfd[1].fd);

		if (pfd[2].revents & POLLIN)
			forward_output(stderr, pfd[2].fd);

		if (pfd[0].revents & POLLIN)
			rl_callback_read_char();
	}

	return (EXIT_SUCCESS);
}
