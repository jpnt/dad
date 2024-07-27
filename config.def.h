#ifndef CONFIG_H
#define CONFIG_H

#define DAEMON_START	"sudo sv up"
#define DAEMON_STOP	"sudo sv down"

typedef struct {
	const char *program;
	const char *daemons[4];
} DaemonGroups;

static const DaemonGroups dg[] = { \
	{"virt-manager", {"virtlockd", "virtlogd", "libvirtd", NULL}},
	{NULL, {NULL}}
};

#endif /* !CONFIG_H */
