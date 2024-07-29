#ifndef CONFIG_H
#define CONFIG_H

/* runit */
#define DAD_RUNIT
#define SV_DIR		"/etc/sv"
#define SV_DIR_ENABLED	"/var/service"
#define DAEMON_START	"sv up"
#define DAEMON_STOP	"sv down"
#define DAEMON_ENABLE	"ln -s"
#define DAEMON_DISABLE	"unlink"

/* systemd */
//#define DAD_SYSTEMD
//#define DAEMON_START	"systemctl start"
//#define DAEMON_STOP	"systemctl stop"
//#define DAEMON_ENABLE	"systemctl enable"
//#define DAEMON_DISABLE	"systemctl disable"

typedef struct {
	const char *program;
	const char *daemons[4];
	int stop_disable_after_run;
} DaemonGroups;

static const DaemonGroups dg[] = { \
	{"virt-manager", {"virtlockd", "virtlogd", "libvirtd", NULL}, 0},
	{NULL, {NULL}, 0}
};

#endif /* !CONFIG_H */
