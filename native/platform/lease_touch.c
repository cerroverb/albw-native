/* The leased panel's touchscreen (drmlease.h lease_touch_poll). Ported from GBA DS's touch_device
 * (kyannon/gba-ds v1.5.6, src/ui.c evdev_touch_poll, GPLv3): one finger, its position and whether
 * it is down, from the device's own events. Here polled once a frame instead of queued as pointer
 * events. */
#include "drmlease.h"
#ifdef ALBW_DRM_LEASE

#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

static int ev_fd = -2;   /* -2: not opened yet, -1: none */
static struct { int min, max; } ev_ax[2];
static int ev_x, ev_y, ev_down, ev_rotate;
static float ev_nx, ev_ny;
static int ev_trace;            /* ALBW_TOUCH_TRACE: how old each finger down / up is when read */
static struct timeval ev_edge;  /* the last down / up event's time (CLOCK_MONOTONIC) */
static int ev_edge_new;

static float clamp01(float v) { return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v; }

static void touch_open(const char *path, int rotate) {
    /* the system call itself, not libc's open: on the AYN Thor, started from Steam (Armada's Game
     * Mode), open answered ENODEV for the touchscreens in MHGU Recomp's DRM lease build while the
     * system call worked (Steam's preloaded overlay library is the likely cause) */
    ev_fd = (int)syscall(SYS_openat, AT_FDCWD, path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (ev_fd < 0) {
        fprintf(stderr, "touch device %s: cannot open it\n", path);
        return;
    }
    for (int a = 0; a < 2; ++a) {
        struct input_absinfo ai = {0};
        if (ioctl(ev_fd, EVIOCGABS(a ? ABS_MT_POSITION_Y : ABS_MT_POSITION_X), &ai) < 0 || ai.maximum <= ai.minimum)
            ioctl(ev_fd, EVIOCGABS(a ? ABS_Y : ABS_X), &ai);
        ev_ax[a].min = ai.minimum, ev_ax[a].max = ai.maximum;
    }
    ev_rotate = rotate >= 0 ? rotate : ev_ax[0].max - ev_ax[0].min >= ev_ax[1].max - ev_ax[1].min ? 0 : 90;
    int grab = ioctl(ev_fd, EVIOCGRAB, 1);   /* ours alone while the game runs */
    ev_trace = getenv("ALBW_TOUCH_TRACE") != NULL;
    int clk = CLOCK_MONOTONIC;
    if (ev_trace) ioctl(ev_fd, EVIOCSCLOCKID, &clk);   /* event times on the clock read below */
    char name[64] = "?";
    ioctl(ev_fd, EVIOCGNAME(sizeof name), name);
    fprintf(stderr, "touch device %s (%s): x %d..%d, y %d..%d, turned %d degrees%s\n", path, name, ev_ax[0].min,
            ev_ax[0].max, ev_ax[1].min, ev_ax[1].max, ev_rotate, grab < 0 ? ", held by another program" : "");
}

bool lease_touch_poll(const char *path, int rotate, int *down, float *nx, float *ny) {
    if (ev_fd == -2) touch_open(path, rotate);
    if (ev_fd < 0) return false;
    struct input_event ie[64];
    ssize_t n;
    while ((n = read(ev_fd, ie, sizeof ie)) > 0) {
        for (int i = 0; i < (int)(n / sizeof ie[0]); ++i) {
            const struct input_event *e = &ie[i];
            if (e->type == EV_ABS && (e->code == ABS_MT_POSITION_X || e->code == ABS_X)) ev_x = e->value;
            else if (e->type == EV_ABS && (e->code == ABS_MT_POSITION_Y || e->code == ABS_Y)) ev_y = e->value;
            else if (e->type == EV_KEY && e->code == BTN_TOUCH) ev_down = e->value != 0, ev_edge = e->time, ev_edge_new = 1;
            else if (e->type == EV_ABS && e->code == ABS_MT_TRACKING_ID) ev_down = e->value >= 0, ev_edge = e->time, ev_edge_new = 1;
            else if (e->type == EV_SYN && e->code == SYN_REPORT) {
                float fx = (float)(ev_x - ev_ax[0].min) / (float)(ev_ax[0].max > ev_ax[0].min ? ev_ax[0].max - ev_ax[0].min : 1);
                float fy = (float)(ev_y - ev_ax[1].min) / (float)(ev_ax[1].max > ev_ax[1].min ? ev_ax[1].max - ev_ax[1].min : 1);
                float x = fx, y = fy;
                switch (ev_rotate) {
                case 90: x = fy, y = 1 - fx; break;
                case 180: x = 1 - fx, y = 1 - fy; break;
                case 270: x = 1 - fy, y = fx; break;
                }
                ev_nx = clamp01(x), ev_ny = clamp01(y);
            }
        }
    }
    if (ev_trace && ev_edge_new) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double age = (now.tv_sec - ev_edge.tv_sec) * 1e3 + (now.tv_nsec / 1e3 - ev_edge.tv_usec) / 1e3;
        fprintf(stderr, "touch device: finger %s, %.1f ms old when read\n", ev_down ? "down" : "up", age);
        ev_edge_new = 0;
    }
    *down = ev_down;
    *nx = ev_nx;
    *ny = ev_ny;
    return true;
}
#endif
