/* The bottom screen on a panel leased from the compositor (DRM lease), for systems whose compositor
 * drives only one of two panels: Armada's Game Mode on the AYN Thor, where gamescope shows the top
 * panel and leases the bottom one (DSI-1) to a program that asks. The GPU draws the bottom screen
 * straight into the panel's buffers, turned to the panel. Linux only. Ported from cerroverb/gba-ds
 * (branch armada-game-mode, 0ed15d5, src/drmlease.h). The touch reader is GBA DS's touch_device
 * (src/ui.c). */
#pragma once
#if defined(__linux__)
#define ALBW_DRM_LEASE 1

#include <stdbool.h>
#include <stdint.h>

/* Leases the panel named by `want` (a connector such as DSI-1, or "auto": the first internal panel
 * offered) from the compositor at GAMESCOPE_WAYLAND_DISPLAY (else WAYLAND_DISPLAY) and lights it,
 * black. False (logged) when no lease is granted. *w, *h: the panel's size upright (its mode, turned
 * by its orientation). ALBW_DRM_LEASE_TEST=WxH,ROTATION[,PPM] (tests): no compositor and no panel, a
 * WxH panel of two GBM buffers on /dev/dri/renderD128, the last one flipped to written to PPM at
 * drmlease_close. */
bool drmlease_open(const char *want, int *w, int *h);
/* the panel's orientation, counter-clockwise degrees as DRM counts them (Right Side Up: 270) */
int drmlease_rotation(void);
/* the panel's buffers (drmlease.c hands them over as a mailbox) */
#define DRMLEASE_BUFFERS 3
/* buffer i (0 to DRMLEASE_BUFFERS - 1) as a dma-buf for the GPU to draw into: XRGB8888, linear,
 * `pitch` bytes a row, the panel's own (unturned) w x h */
void drmlease_buffer(int i, int *dmabuf, int *pitch, int *w, int *h);
/* Once a frame: the compositor's and the panel's events, then the buffer to draw this frame's picture into,
 * or -1 (the last flip is still on its way, or the panel is gone: no picture this frame) */
int drmlease_free_buffer(void);
/* buffer i onto the panel once `fence` (a sync file from the GPU or -1, closed here) has signalled:
 * the kernel waits where the plane takes IN_FENCE_FD. Waits for nothing else. */
void drmlease_flip(int i, int fence);
void drmlease_close(void);

/* The panel's touchscreen, read from its input device `path` (grabbed: the compositor stops seeing
 * it). Once a frame: *down and the finger's place in the upright picture, 0..1 (the device's axes
 * turned by `rotate` degrees, -1: from the axes' shape). False when the device cannot be opened. */
bool lease_touch_poll(const char *path, int rotate, int *down, float *nx, float *ny);

#endif
