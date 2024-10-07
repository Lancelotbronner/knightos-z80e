#include <z80e/debugging/commands.h>
#include <z80e/ti/asic.h>

#ifndef NOLINK
#include <sys/socket.h>
#include <sys/un.h>
#include <wordexp.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static FILE *link_output = NULL;
static FILE *link_input = NULL;
static hook_t hook;

//MARK: - Send Command

static void send_byte_from_input(debugger_t debugger) {
	if (!link_input)
		return;
	int c = getc(link_input);
	if (c == EOF) {
		fclose(link_input);
		hook_cancel(hook);
		return;
	}
	if (!link_recv_byte(debugger->asic, (uint8_t)c)) {
		debugger_print(debugger, "Warning: link not ready\n");
		ungetc(c, link_input);
	}
}

static uint8_t on_link_rx_buffer_read(void *data, uint8_t port, uint8_t value) {
	debugger_t debugger = data;
	debugger_print(debugger, "Link rx buffer read from\n");
	send_byte_from_input(debugger);
	return value;
}

static int command_send(debugger_t debugger, void *data, int argc, char **argv) {
	char *path = strdup(argv[2]);
#ifndef NOLINK
	wordexp_t p;
	if (wordexp(path, &p, 0) == 0) {
		free(path);
		path = strdup(p.we_wordv[0]);
	}
#endif

	link_input = fopen(path, "r");
	if (link_input) {
		debugger_print(debugger, "Sending file: %s\n", path);
		hook = hook_port_emplace(&debugger->asic->cpu.hook.port_in, 0x0A, 0x0A, debugger, on_link_rx_buffer_read);
		send_byte_from_input(debugger);
		free(path);
		return 0;
	}
	free(path);

	int strl = 0;
	for (int i = 1; i < argc - 1; i++)
		strl += strlen(argv[i + 1]) + 1;

	char *tmp = malloc(strl);
	char *dpointer = tmp;
	for (int i = 1; i < argc - 1; i++) {
		strcpy(dpointer, argv[i + 1]);
		dpointer += strlen(argv[i + 1]);
		*(dpointer++) = ' ';
	}
	*(dpointer - 1) = 0;

	uint8_t expr = debugger_evaluate(debugger, tmp);

	free(tmp);

	if (!link_recv_byte(debugger->asic, expr))
		debugger_print(debugger, "Calculator is not ready to receive another byte.\n");
	else
		debugger_print(debugger, "Sent %02X to calculator's link assist.\n", expr);
	return 0;
}

const struct debugger_command SendCommand = {
	.name = "send",
	.usage = "<value|path>",
	.summary = "Sends a value or file to the link port",
	.callback = command_send,
};

//MARK: - Receive Command

static int command_receive(debugger_t debugger, void *data, int argc, char **argv) {
	return 0;
}

const struct debugger_command ReceiveCommand = {
	.name = "recv",
	.usage = "(print|path)",
	.summary = "Configures link receiving behaviour",
	.description = "When receiving link data, either print or write values to a file.",
	.callback = command_receive,
};

//MARK: - Connect Command

static int command_connect(debugger_t debugger, void *data, int argc, char **argv) {
#ifdef NOLINK
	debugger_print(state, "Sockets are not supported\n");
#else
	debugger->asic->socket.listenfd.fd =
		socket(AF_UNIX, SOCK_STREAM, 0);
	fcntl(debugger->asic->socket.listenfd.fd, F_SETFL, O_NONBLOCK);
	fcntl(debugger->asic->socket.listenfd.fd, F_SETFD, FD_CLOEXEC);
	debugger->asic->socket.listenfd.events = POLLIN;
	if (debugger->asic->socket.listenfd.fd == -1) {
		debugger_print(debugger, "Unable to create socket\n");
		return 1;
	}

	struct sockaddr_un *link_sockaddr = malloc(sizeof(struct sockaddr_un));
	if (link_sockaddr == NULL) {
		debugger_print(debugger, "Can't allocate socket address\n");
		return 1;
	}

	link_sockaddr->sun_family = AF_UNIX;
	strncpy(link_sockaddr->sun_path, argv[2], sizeof(link_sockaddr->sun_path));
	link_sockaddr->sun_path[sizeof(link_sockaddr->sun_path) - 1] = 0;

	unlink(link_sockaddr->sun_path);
	if (bind(debugger->asic->socket.listenfd.fd,
				(struct sockaddr *)link_sockaddr,
				sizeof(*link_sockaddr)) == -1) {
		debugger_print(debugger, "Unable to bind socket\n");
		return 1;
	}

	if (listen(debugger->asic->socket.listenfd.fd, 3) == -1) {
		debugger_print(debugger, "Unable to listen on socket\n");
		return 1;
	}

	debugger_print(debugger, "Bound to socket at %s\n", argv[2]);
#endif
	return 0;
}

const struct debugger_command ConnectCommand = {
	.name = "connect",
	.usage = "<socket>",
	.summary = "Connects the link port to a unix socket",
	.callback = command_connect,
};

//MARK: - Dump Link Command

static int command_link(debugger_t debugger, void *data, int argc, char **argv) {
	link_device_t lstate = &debugger->asic->link;
	debugger_print(debugger, "Ready: %d\n", !lstate->assist.status.rx_ready);
	debugger_print(debugger, "Tip: %d\n", lstate->us.tip);
	debugger_print(debugger, "Ring: %d\n", lstate->us.ring);
	return 0;
}

const struct debugger_command DumpLinkCommand = {
	.name = "link",
	.usage = "",
	.summary = "Dumps the status of the link port",
	.callback = command_link,
};
