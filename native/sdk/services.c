/* Native replacements for the 3DS system-service SDK layer the game calls:
 *   srv (service manager), APT (applet manager, incl. the software-keyboard library applet),
 *   NDM, PTM (pedometer), CECD (StreetPass), CFG (system settings), HID (input), and the
 *   unidentified session 0x716780 that the APT sleep code talks to (probably cam:u).
 *
 * Rule: no emulation. Nothing here builds IPC command buffers or answers svc calls. Each function
 * replaces an SDK API function (normally the one that writes the IPC header to TLS+0x80 and runs
 * svc 0x32, or the closest function above it that also owns other svcs) by what it MEANS on a PC:
 *   - there is nothing to connect to: service sessions always open,
 *   - the application runs in the foreground: no home menu, no sleep, no notifications,
 *     no other applets, no network, StreetPass storage is empty,
 *   - system settings are fixed defaults (English, Europe/UK, user name "Link"),
 *   - input comes from the native platform layer (plat_input()).
 * The game-side logic above these functions stays decompiled.
 *
 * Conventions: func_<USA address>; the callers are decompiled C using the ARM argument order
 * (r0-r3, then stack), result codes in r0 (0 = success, bit 31 = failure). Every function lists
 * the USA address, the IPC command it replaces, the signature evidence (EU disassembly of
 * eu_code.bin; EU address given where it differs) and a confidence.
 *
 * Service session handles: the SDK keeps one session handle per service in a global and some
 * of its code depends on the handle being non-zero (cecd: USA 003D1240 spins until the cecd:u
 * handle is set; 003D1320/003D1798/003D10C4/003D12B8 dispatch on cecd:s/cecd:u handle != 0).
 * GetServiceHandle therefore returns a real native handle: an event object of the native os
 * layer (native/sdk/os.c, albw_os_create_event) that is never signalled. It only stands for
 * "an open session"; CloseHandle (os area) releases it normally. This matters for the services
 * other areas own too (fs:USER, gsp::Gpu, dsp::DSP also come through srv GetServiceHandle).
 * The finalizers of the services in this file are replaced anyway (no svc left in them).
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../platform/platform.h"
#include "../runtime/runtime.h"

/* ---------------------------------------------------------------------------------------------
 * shared helpers
 * ------------------------------------------------------------------------------------------- */

#define RESULT_OK 0u
/* srv: name longer than 8 characters (constant in the literal pool of USA 00104868) */
#define RESULT_SRV_NAME_TOO_LONG 0xD9006405u
/* cecd: "message box / file not found" (status level, CEC module, description 1008); the game
 * checks for exactly this code and treats it as "no StreetPass data" (USA 004C5DA8 returns 1) */
#define RESULT_CECD_NOT_FOUND 0xC8A10BF0u

/* fallback session token if the native os layer cannot create a handle (never expected) */
#define NATIVE_HANDLE_FALLBACK 0x7E0000FFu

/* native os layer (native/sdk/os.c, see os.md) */
uint32_t albw_os_create_event(uint32_t *out, int32_t reset_type);

static void put_u16le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

/* ASCII -> UTF-16LE into a byte buffer of max_units code units (NUL included) */
static size_t put_utf16(uint8_t *dst, size_t max_units, const char *s) {
    size_t n = 0;
    if (!max_units) return 0;
    for (; s[n] && n + 1 < max_units; n++) put_u16le(dst + 2 * n, (uint8_t)s[n]);
    put_u16le(dst + 2 * n, 0);
    return n;
}

static int env_int(const char *name, int def, int lo, int hi) {
    const char *v = getenv(name);
    if (!v || !*v) return def;
    int x = atoi(v);
    return x < lo || x > hi ? def : x;
}

/* ---------------------------------------------------------------------------------------------
 * srv: service manager
 * ------------------------------------------------------------------------------------------- */

/* a new session object: a never-signalled native event (one-shot) */
static uint32_t new_session_handle(void) {
    uint32_t h = 0;
    if (albw_os_create_event(&h, 0) != RESULT_OK || h == 0) h = NATIVE_HANDLE_FALLBACK;
    return h;
}

/* USA 00104800  srv::Initialize() -> Result.
 * Original: lock, refcount at 0x716178, else connect to port "srv:" (svc ConnectToPort) and
 * RegisterClient (0x00010002). Native: nothing to connect to. Evidence: no args, result tested by
 * every caller (00108010, 00104368, 00102794, 003D127C). Confidence: high. */
uint32_t func_00104800(void) { return RESULT_OK; }

/* USA 00104868  srv::GetServiceHandle(Handle *out, const char *name, s32 nameLen, u32 flags).
 * Original: fails if srv is not initialised or nameLen > 8, else IPC 0x00050100 (00108878).
 * Native: every service exists; return a new session handle (see the top of the file). Evidence: EU 00104868 checks r2 > 8 and
 * tail-calls 00108878 whose r0 = out, r1 = name (copied), r2/r3 = len/flags; callers pass
 * (&handleGlobal, name, strlen(name), 0). Confidence: high. */
uint32_t func_00104868(uint32_t *out, const char *name, int32_t len, uint32_t flags) {
    if (len > 8) return RESULT_SRV_NAME_TOO_LONG;
    if (out) *out = new_session_handle();
    return RESULT_OK;
}

/* USA 00108878  srv IPC GetServiceHandle 0x00050100 (Handle *out, const char *name, u32 len,
 * u32 flags). Only called by 00104868; replaced for completeness. Confidence: high. */
uint32_t func_00108878(uint32_t *out, const char *name, uint32_t len, uint32_t flags) {
    return func_00104868(out, name, (int32_t)len, flags);
}

/* USA 0010489C  srv IPC RegisterClient 0x00010002 () -> Result. Confidence: high. */
uint32_t func_0010489C(void) { return RESULT_OK; }

/* USA 001029DC  srv IPC EnableNotification 0x00020000 (Handle *outSemaphore) -> Result.
 * Evidence: stores cmd[3] to *r0. Native: no notifications; the semaphore is a never-signalled
 * native object. Confidence: high. */
uint32_t func_001029DC(uint32_t *out_semaphore) {
    if (out_semaphore) *out_semaphore = new_session_handle();
    return RESULT_OK;
}

/* USA 001048D8  srv IPC ReceiveNotification 0x000B0000 (u32 *outNotificationId) -> Result.
 * Native: there is never a notification (id 0). Confidence: high. */
uint32_t func_001048D8(uint32_t *out_id) {
    if (out_id) *out_id = 0;
    return RESULT_OK;
}

/* USA 00101538  srv: enable notifications and start the srv notification thread
 * (EnableNotification 001029DC, then thread creation with entry 00102820 which loops on
 * svc WaitSynchronization + ReceiveNotification). Native: no notifications, no thread.
 * Evidence: EU 00101538, returns the EnableNotification result (r4). Confidence: high. */
uint32_t func_00101538(void) { return RESULT_OK; }

/* USA 00101528  first start-up step (00100C18): srv::Initialize (00104800) then falls through
 * into 00101538. Ghidra decompiled both bodies into this symbol, so it is replaced as well.
 * Confidence: high. */
uint32_t func_00101528(void) {
    func_00104800();
    return func_00101538();
}

/* ---------------------------------------------------------------------------------------------
 * APT: applet manager. Native meaning: the game is the only application, permanently in the
 * foreground; there is no home menu, no sleep, no power button, no other applet, and no
 * parameter / notification ever arrives.
 * ------------------------------------------------------------------------------------------- */

/* APT lock (the SDK serialises all APT IPC with a kernel mutex from GetLockHandle and opens the
 * APT:U session per locked section). Native: a recursive mutex, no session. */
static pthread_mutex_t apt_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

/* USA 00108DF8  APT lock: svc WaitSynchronization1 on the APT mutex, then
 * srv GetServiceHandle("APT:U") into the session global. () -> void. Confidence: high. */
void func_00108DF8(void) { pthread_mutex_lock(&apt_mutex); }

/* USA 00108F4C  APT unlock: svc CloseHandle(session), svc ReleaseMutex(APT mutex). () -> void.
 * Confidence: high. */
void func_00108F4C(void) { pthread_mutex_unlock(&apt_mutex); }

/* USA 0010190C  APT::Initialize(u32 appId, u32 attributes, u32 threadPriority) -> Result.
 * Original: GetLockHandle (0x00010040), Initialize (0x00020080: notification + resume events),
 * create the events and the APT event thread (00102C40 -> thread 00104F90 waiting on them).
 * Native: no events and no thread; the application is simply running. The SDK's "initialised"
 * byte (data 0x714838+2) is left clear on purpose: the pad/touch readers and USA 0010145C only
 * consult the APT state when it is set. Evidence: called from 00100C38 as
 * (0x300, attr & ~7, 0xF); result tested (bit 31). Confidence: high. */
uint32_t func_0010190C(uint32_t app_id, uint32_t attributes, uint32_t priority) { return RESULT_OK; }

void func_00102A2C(int32_t notify);

/* USA 00101B1C  APT start-up after Initialize (called with 1 from 00100C94): optional applet-flag
 * reset (00102A2C(0)), Enable (0x00030040), then -- for an application -- 001053A8, which saves
 * the gsp/dsp state and blocks until the Wakeup parameter arrives, and stores the wake-up
 * parameter / deliver arg. Native: the application is already awake in the foreground; keep the
 * flag reset (pure data write), skip the wait. (int resetFlag) -> void. Confidence: high. */
void func_00101B1C(int32_t reset_flag) {
    if (reset_flag) func_00102A2C(0);
}

/* USA 00101A70  APT: prepare to close the application (u8 cancelPreload) -> Result. Original:
 * 00102E40 (jump-to-home preparation, may save gsp/dsp state) then loops on
 * PrepareToCloseApplication (0x00220040) while the result says "busy". Native: nothing to
 * prepare. Evidence: callers 00100C94 / 0014AD6C, result checked with ThrowFatalErr.
 * Confidence: high. */
uint32_t func_00101A70(uint32_t cancel_preload) { return RESULT_OK; }

/* USA 001017AC  APT: CloseApplication (0x00270044) then svc ExitProcess; never returns.
 * (const void *param, u32 paramSize, Handle handle). Native meaning: the program ends.
 * Confidence: high. */
_Noreturn void func_001017AC(const void *param, uint32_t param_size, uint32_t handle) {
    fprintf(stderr, "services: application closed (APT CloseApplication)\n");
    plat_shutdown();
    exit(0);
}

/* USA 00101BE8  APT/system finalisation on exit: tears down the gsp/dsp/APT objects, unmaps the
 * shared memory blocks, signals the APT thread and calls the registered applet callbacks with
 * "deregistered". Native: nothing was created, nothing to tear down. () -> void.
 * Confidence: medium (area assignment: apt; it also touches gsp/dsp objects). */
void func_00101BE8(void) {}

/* USA 00105928 (EU 00105928; was named 00105960 by the old map)  APT IPC SetApplicationCpuTimeLimit 0x004F0080 (u32 fixed, u32 percent) -> Result
 * (polls with GetSystemTick while the system is busy). Called via 00102E08/00102E14 under the
 * APT lock. Native: the host scheduler owns the CPU. Confidence: high. */
uint32_t func_00105928(uint32_t fixed, uint32_t percent) { return RESULT_OK; }

/* USA 00102FB4  APT IPC Initialize 0x00020080 (u32 appId, u32 attr, Handle *notifyEvent,
 * Handle *resumeEvent) -> Result. Native: no events (0). Confidence: high. */
uint32_t func_00102FB4(uint32_t app_id, uint32_t attr, uint32_t *notify_event, uint32_t *resume_event) {
    if (notify_event) *notify_event = 0;
    if (resume_event) *resume_event = 0;
    return RESULT_OK;
}

/* USA 00103004  APT IPC GetLockHandle 0x00010040 (Handle *outLock, u32 flags, u32 *outAttr,
 * u32 *outState) -> Result. Evidence: *r2 = cmd[2], *r3 = cmd[3], *r0 = cmd[5].
 * Native: no kernel mutex, attributes 0, state 0. Confidence: high. */
uint32_t func_00103004(uint32_t *out_lock, uint32_t flags, uint32_t *out_attr, uint32_t *out_state) {
    if (out_lock) *out_lock = 0;
    if (out_attr) *out_attr = 0;
    if (out_state) *out_state = 0;
    return RESULT_OK;
}

/* USA 00103060  APT IPC CloseApplication 0x00270044 (const void *param, u32 size, Handle h).
 * Confidence: high. */
uint32_t func_00103060(const void *param, uint32_t size, uint32_t handle) { return RESULT_OK; }

/* USA 001030B4  APT IPC PrepareToCloseApplication 0x00220040 (u8 cancelPreload).
 * Confidence: high. */
uint32_t func_001030B4(uint32_t cancel_preload) { return RESULT_OK; }

/* USA 001030F0  APT IPC Enable 0x00030040 (u32 attributes). Confidence: high. */
uint32_t func_001030F0(uint32_t attributes) { return RESULT_OK; }

/* USA 001058A8 (EU 001058A8; old map name 001058EC)  APT IPC IsRegistered 0x00090040 (u32 appId, u8 *outRegistered).
 * Native: no other applet (home menu 0x101, 0x200/0x400 groups) exists. Confidence: high. */
uint32_t func_001058A8(uint32_t app_id, uint8_t *out_registered) {
    if (out_registered) *out_registered = 0;
    return RESULT_OK;
}

/* USA 001058EC (EU 001058EC; old map name 00105928)  APT IPC 0x003B0040 (u8) -- CancelLibraryApplet / close
 * preparation step, result only. Confidence: medium (command name), high (signature). */
uint32_t func_001058EC(uint32_t flag) { return RESULT_OK; }

/* USA 00109208  APT IPC SleepSystem 0x00420080 (u64 time as r0:r1). Confidence: high. */
uint32_t func_00109208(uint32_t time_lo, uint32_t time_hi) { return RESULT_OK; }

/* USA 00109240  APT IPC NotifyToWait 0x00430040 (u32 appId). Confidence: high. */
uint32_t func_00109240(uint32_t app_id) { return RESULT_OK; }

/* USA 00109278  APT IPC ReplySleepQuery 0x003E0080 (u32 appId, u8 reply). Native: the host
 * never asks to sleep; accept any reply. Confidence: high. */
uint32_t func_00109278(uint32_t app_id, uint32_t reply) { return RESULT_OK; }

/* USA 001092BC  APT IPC InquireNotification 0x000B0040 (u32 appId, u8 *outNotification).
 * Native: APT_NOTIFICATION_NONE (0). Confidence: high. */
uint32_t func_001092BC(uint32_t app_id, uint8_t *out_notification) {
    if (out_notification) *out_notification = 0;
    return RESULT_OK;
}

/* USA 0010AFF8  APT IPC AppletUtility 0x004B00C2 (u32 id, const void *in, u32 inSize, void *out,
 * u32 outSize, u32 *outAppletResult). Evidence: cmd[1]=r0, cmd[2]=r2, cmd[3]=stack0, input static
 * buffer r1, output static buffer r3 (TLS+0x180), *stack1 = cmd[2]. Native: no output, result 0.
 * Confidence: high. */
uint32_t func_0010AFF8(uint32_t id, const void *in, uint32_t in_size, void *out, uint32_t out_size,
                       uint32_t *out_result) {
    if (out && out_size) memset(out, 0, out_size);
    if (out_result) *out_result = 0;
    return RESULT_OK;
}

/* USA 0010B074  APT IPC SendParameter 0x000C0104 (u32 src, u32 dst, u32 cmd, const void *param,
 * u32 size, Handle h). Native: there is no receiving applet; the parameter is dropped.
 * Confidence: high. */
uint32_t func_0010B074(uint32_t src, uint32_t dst, uint32_t cmd, const void *param, uint32_t size,
                       uint32_t handle) {
    return RESULT_OK;
}

/* USA 0010B0CC  APT IPC CancelParameter 0x000F0100 (u8 checkSender, u32 sender, u8 checkReceiver,
 * u32 receiver, u8 *outCanceled). Native: nothing pending, nothing canceled. Confidence: high. */
uint32_t func_0010B0CC(uint32_t check_sender, uint32_t sender, uint32_t check_receiver,
                       uint32_t receiver, uint8_t *out_canceled) {
    if (out_canceled) *out_canceled = 0;
    return RESULT_OK;
}

/* USA 0010B128 / 0010B1B4  APT IPC GlanceParameter 0x000E0080 / ReceiveParameter 0x000D0080
 * (u32 *outSender, u32 appId, u32 *outCommand, void *buf, u32 bufSize, u32 *outSize,
 *  Handle *outHandle). Evidence: *r0 = cmd[2], *r2 = cmd[3], *stack1 = cmd[4],
 * *stack2 = cmd[6]; static receive buffer (stack0 << 14 | 2, r3). Native: no parameter is ever
 * sent to the application: sender 0, command APTCMD_NONE (0), size 0. Confidence: high. */
static uint32_t apt_no_parameter(uint32_t *out_sender, uint32_t *out_cmd, uint32_t *out_size,
                                 uint32_t *out_handle) {
    if (out_sender) *out_sender = 0;
    if (out_cmd) *out_cmd = 0;
    if (out_size) *out_size = 0;
    if (out_handle) *out_handle = 0;
    return RESULT_OK;
}
uint32_t func_0010B128(uint32_t *out_sender, uint32_t app_id, uint32_t *out_cmd, void *buf,
                       uint32_t buf_size, uint32_t *out_size, uint32_t *out_handle) {
    return apt_no_parameter(out_sender, out_cmd, out_size, out_handle);
}
uint32_t func_0010B1B4(uint32_t *out_sender, uint32_t app_id, uint32_t *out_cmd, void *buf,
                       uint32_t buf_size, uint32_t *out_size, uint32_t *out_handle) {
    return apt_no_parameter(out_sender, out_cmd, out_size, out_handle);
}

/* USA 0010B240  APT IPC 0x003F0040 (u32), result only. Confidence: medium (name unknown). */
uint32_t func_0010B240(uint32_t value) { return RESULT_OK; }

/* USA 003E5928 (EU 003E5908)  APT IPC 0x002C0044 (const void *buf, u32 size, Handle h) --
 * application-jump / deliver-arg preparation. Confidence: medium (name), high (signature). */
uint32_t func_003E5928(const void *buf, uint32_t size, uint32_t handle) { return RESULT_OK; }

/* USA 003E597C (EU 003E595C)  APT IPC GetAppletManInfo 0x00050040 (u8 pos, u8 *outPos,
 * u32 *outRequestedApp, u32 *outHomeMenuApp, u32 *outActiveApp). Native: the application
 * (0x300) is active and requested; the home-menu id is reported as 0x101 but is not running.
 * Confidence: medium. */
uint32_t func_003E597C(uint32_t pos, uint8_t *out_pos, uint32_t *out_requested, uint32_t *out_menu,
                       uint32_t *out_active) {
    if (out_pos) *out_pos = (uint8_t)pos;
    if (out_requested) *out_requested = 0x300;
    if (out_menu) *out_menu = 0x101;
    if (out_active) *out_active = 0x300;
    return RESULT_OK;
}

/* USA 003E59EC / 003E5A3C  APT IPC StartSystemApplet 0x001F0084 / StartLibraryApplet 0x001E0084
 * (u32 appId, const void *param, u32 size, Handle h). Confidence: high. */
uint32_t func_003E59EC(uint32_t app_id, const void *param, uint32_t size, uint32_t handle) {
    return RESULT_OK;
}
uint32_t func_003E5A3C(uint32_t app_id, const void *param, uint32_t size, uint32_t handle) {
    return RESULT_OK;
}

/* USA 003E5A8C  APT IPC 0x00400042 (const void *captureInfo, u32 size) -- send capture buffer
 * info to the applet. Confidence: medium (name), high (signature). */
uint32_t func_003E5A8C(const void *info, uint32_t size) { return RESULT_OK; }

/* USA 003E5AD8  APT IPC PrepareToStartNewestHomeMenu 0x002B0000 (). Confidence: high. */
uint32_t func_003E5AD8(void) { return RESULT_OK; }

/* USA 003E5B08 / 003E5B40  APT IPC PrepareToStartSystemApplet 0x00190040 /
 * PrepareToStartLibraryApplet 0x00180040 (u32 appId). Confidence: high. */
uint32_t func_003E5B08(uint32_t app_id) { return RESULT_OK; }
uint32_t func_003E5B40(uint32_t app_id) { return RESULT_OK; }

/* USA 003E426C (EU 003E424C)  software keyboard: start the swkbd library applet (appId 0x401)
 * and wait for it (u8 *outHomeRequested, SwkbdConfig *cfg, void *sharedMem, u32 sharedMemSize,
 * ...4 more unused here) -> 1 if the keyboard ran, 0 if the configuration was rejected.
 * Original: validate (003E3F30), Prepare/StartLibraryApplet, create the shared memory block,
 * save gsp/dsp state, wait for the applet's reply which overwrites the 0x400-byte config.
 * Native meaning: a keyboard that is confirmed at once with the default text. The config layout
 * is the CTR swkbd parameter block (cross-checked: the game reads the result at cfg+0x138 =
 * 0x674+0x138 = 0x7AC in USA 0019AAFC, checks filter flags at +0x18 and the shared-memory size at
 * +0x130, and sets max text length at +0x20 = 10 in USA 0019ACC8).
 * Text: ALBW_PLAYER_NAME (ASCII), else the initial text if one was given, else "Link".
 * Confidence: medium (layout from the public swkbd parameter format + the cross-checks above). */
enum {
    SWKBD_NUM_BUTTONS_M1 = 0x004, SWKBD_MAX_TEXT_LEN = 0x020, SWKBD_INITIAL_TEXT_OFFSET = 0x120,
    SWKBD_SHARED_MEMORY_SIZE = 0x130, SWKBD_RESULT = 0x138, SWKBD_TEXT_OFFSET = 0x144,
    SWKBD_TEXT_LENGTH = 0x148,
};

static int32_t rd_i32(const uint8_t *p) { int32_t v; memcpy(&v, p, 4); return v; }
static uint16_t rd_u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static void wr_i32(uint8_t *p, int32_t v) { memcpy(p, &v, 4); }

uint32_t func_003E426C(uint8_t *out_home_requested, uint8_t *cfg, uint8_t *shared_mem,
                       uint32_t shared_size, uint32_t arg4, uint32_t arg5, uint32_t arg6,
                       uint32_t arg7, int32_t arg8) {
    if (!cfg) return 0;
    if (out_home_requested) *out_home_requested = 0;
    int32_t buttons_m1 = rd_i32(cfg + SWKBD_NUM_BUTTONS_M1);
    uint32_t max_len = rd_u16(cfg + SWKBD_MAX_TEXT_LEN);
    if (max_len == 0 || max_len > 0x400) max_len = 0x400;

    char text[257];
    const char *name = getenv("ALBW_PLAYER_NAME");
    int32_t init_off = rd_i32(cfg + SWKBD_INITIAL_TEXT_OFFSET);
    text[0] = 0;
    if (name && *name) {
        snprintf(text, sizeof text, "%s", name);
    } else if (shared_mem && init_off >= 0 && (uint32_t)init_off + 2 <= shared_size) {
        size_t n = 0;  /* initial text is UTF-16; keep its ASCII subset */
        while (n + 1 < sizeof text && (uint32_t)init_off + 2 * n + 2 <= shared_size) {
            uint16_t c = rd_u16(shared_mem + init_off + 2 * n);
            if (!c) break;
            text[n++] = c < 0x80 ? (char)c : '?';
        }
        text[n] = 0;
    }
    if (!text[0]) snprintf(text, sizeof text, "Link");
    if (strlen(text) > max_len) text[max_len] = 0;

    size_t len = 0;
    if (shared_mem && shared_size >= 2) len = put_utf16(shared_mem, shared_size / 2, text);
    wr_i32(cfg + SWKBD_TEXT_OFFSET, 0);
    put_u16le(cfg + SWKBD_TEXT_LENGTH, (uint16_t)len);
    /* confirm with the right-most button: D0_CLICK (0), D1_CLICK1 (2), D2_CLICK2 (5) */
    wr_i32(cfg + SWKBD_RESULT, buttons_m1 <= 0 ? 0 : buttons_m1 == 1 ? 2 : 5);
    fprintf(stderr, "services: software keyboard -> \"%s\"\n", text);
    return 1;
}

/* Session 0x716780 (unidentified; the failure code C9405001 that USA 001093C0 checks belongs to
 * module 20 = CAM, so probably cam:u). Only used by the APT sleep path (001093AC/001093C0/
 * 00109430), which never runs natively. Native: the camera is inactive. */

/* USA 0010B278 / 0010B2F0  IPC 0x003C0000 / 0x003B0000 (u8 *out). Confidence: low (name),
 * high (signature). */
uint32_t func_0010B278(uint8_t *out) {
    if (out) *out = 0;
    return RESULT_OK;
}
uint32_t func_0010B2F0(uint8_t *out) {
    if (out) *out = 0;
    return RESULT_OK;
}

/* USA 0010B2B4 / 0010B32C  IPC 0x003D0040 / 0x00130040 (u8). Confidence: low (name),
 * high (signature). */
uint32_t func_0010B2B4(uint32_t value) { return RESULT_OK; }
uint32_t func_0010B32C(uint32_t value) { return RESULT_OK; }

/* ---------------------------------------------------------------------------------------------
 * NDM (network daemon manager). Native: there are no background network daemons.
 * ------------------------------------------------------------------------------------------- */

/* USA 0010478C  ndm IPC SuspendDaemons 0x00060040 (u32 mask). Called directly by 003D5710 and
 * via the nop entry 00104788. Confidence: high. */
uint32_t func_0010478C(uint32_t mask) { return RESULT_OK; }
/* USA 00104788  nop entry falling through into 0010478C. Confidence: high. */
uint32_t func_00104788(uint32_t mask) { return func_0010478C(mask); }

/* USA 001047C8  ndm IPC OverrideDefaultDaemons 0x00140040 (u32 mask). Confidence: high. */
uint32_t func_001047C8(uint32_t mask) { return RESULT_OK; }
/* USA 001047C4  nop entry falling through into 001047C8. Confidence: high. */
uint32_t func_001047C4(uint32_t mask) { return func_001047C8(mask); }

/* USA 003D56B8 (EU 003D5698)  ndm IPC ResumeDaemons 0x00070040 (u32 mask). Confidence: high. */
uint32_t func_003D56B8(uint32_t mask) { return RESULT_OK; }

/* USA 003D5730 (EU 003D5710)  ndm::Finalize: refcount at 0x715980, svc CloseHandle of the ndm:u
 * session at 0x722AB8. Native: nothing to close. () -> Result. Confidence: medium. */
uint32_t func_003D5730(void) { return RESULT_OK; }

/* ---------------------------------------------------------------------------------------------
 * PTM (power / pedometer). The game uses only the pedometer (Play Coins); the coin balance
 * itself lives in shared extdata (fs area).
 * ------------------------------------------------------------------------------------------- */

/* USA 001014EC  ptm::Finalize: svc CloseHandle of the ptm:u session (0x71613C). () -> Result.
 * Confidence: high. */
uint32_t func_001014EC(void) { return RESULT_OK; }

/* USA 003DB654 (EU 003DB634)  ptm IPC GetStepHistory 0x000B00C2 (u16 *steps, u32 hours,
 * u64 startTime as two words). Evidence: cmd[1]=r1, cmd[2..3]=r2:r3, buffer (r1*2 bytes) = r0.
 * Native: no steps were walked. Confidence: high. */
uint32_t func_003DB654(uint16_t *steps, uint32_t hours, uint32_t start_lo, uint32_t start_hi) {
    if (steps && hours) memset(steps, 0, (size_t)hours * 2);
    return RESULT_OK;
}

/* USA 003DB6A0 (EU 003DB680)  ptm IPC GetTotalStepCount 0x000C0000 (u32 *outSteps).
 * Confidence: high. */
uint32_t func_003DB6A0(uint32_t *out_steps) {
    if (out_steps) *out_steps = 0;
    return RESULT_OK;
}

/* ---------------------------------------------------------------------------------------------
 * CFG (system settings). All reads go through GetConfigInfoBlk2; the block table below is the
 * native system configuration.
 * ------------------------------------------------------------------------------------------- */

/* 3DS system language codes: 0 JP, 1 EN, 2 FR, 3 DE, 4 IT, 5 ES, 6 ZH, 7 KO, 8 NL, 9 PT, 10 RU,
 * 11 TW. The EU build handles 1-5 (USA 00297AD8). Override: ALBW_LANGUAGE=<code>. */
#define CFG_DEFAULT_LANGUAGE 1
/* SecureInfoGetRegion: 0 JPN, 1 USA, 2 EUR, 3 AUS, 4 CHN, 5 KOR, 6 TWN. The code/data come from
 * the EU ROM (eu_code.bin), so the console region is EUR. Override: ALBW_REGION=<code>. */
#define CFG_DEFAULT_REGION 2
#define CFG_COUNTRY_UK 110
#define CFG_USERNAME "Link"

/* language code from a name or number: "fr", "French", "fr_FR.UTF-8", "2"; -1 = not recognised */
static int lang_code(const char *s) {
    while (*s == ' ' || *s == '\t' || *s == '"') s++;
    if (*s >= '0' && *s <= '9') { int v = atoi(s); return v >= 0 && v <= 11 ? v : -1; }
    static const struct { const char *p; int code; } t[] = {
        {"en", 1}, {"fr", 2}, {"de", 3}, {"ge", 3}, {"it", 4}, {"es", 5}, {"sp", 5}, {"ja", 0}, {"jp", 0}, {"zh", 6}, {"ko", 7}, {"nl", 8}, {"pt", 9}, {"ru", 10}};
    for (unsigned i = 0; i < sizeof t / sizeof *t; i++)
        if ((s[0] | 32) == t[i].p[0] && (s[1] | 32) == t[i].p[1]) return t[i].code;
    return -1;
}

/* The game language, first found of: ALBW_LANGUAGE (power users), a `language <fr|de|it|es|en|0-11>` line in
 * layout.cfg (the config folder), the computer's own language (LC_ALL / LC_MESSAGES / LANG; the Windows loader
 * sets LANG from the user's language), English. The EU game has English, French, German, Italian and Spanish
 * text: any other language falls back to English. The text archive loads at start-up, so a change needs a restart. */
extern const char *layout_default_path(void);
static int cfg_language(void) {
    static int cached = -1;
    if (cached >= 0) return cached;
    int code = -1, forced = 0;
    const char *e = getenv("ALBW_LANGUAGE");
    if (e && *e) forced = (code = lang_code(e)) >= 0;
    if (code < 0) {
        FILE *f = fopen(layout_default_path(), "r");
        char line[256];
        while (f && fgets(line, sizeof line, f)) {
            char *h = strchr(line, '#');
            if (h) *h = 0;
            char key[32] = "", arg[64] = "";
            if (sscanf(line, "%31s %63s", key, arg) == 2 && !strcmp(key, "language")) { code = lang_code(arg); break; }
        }
        if (f) fclose(f);
    }
    if (code < 0) {
        const char *v = getenv("LC_ALL");
        if (!v || !*v) v = getenv("LC_MESSAGES");
        if (!v || !*v) v = getenv("LANG");
        if (v && *v) code = lang_code(v);
    }
    if (!forced && (code < 1 || code > 5)) code = CFG_DEFAULT_LANGUAGE;   /* the EU game ships 1-5 only */
    static int logged;
    if (!logged++) fprintf(stderr, "cfg: game language %d (%s)\n", code, code == 0 ? "Japanese" : code == 1 ? "English" : code == 2 ? "French" : code == 3 ? "German" : code == 4 ? "Italian" : code == 5 ? "Spanish" : "other");
    return cached = code;
}

static void cfg_block(uint32_t block, uint8_t *out, uint32_t size) {
    static int unknown_logged;
    memset(out, 0, size);
    switch (block) {
    case 0x000A0002:  /* system language (u8) */
        if (size >= 1) out[0] = (uint8_t)cfg_language();
        break;
    case 0x000A0000:  /* user name: UTF-16 [10] + NUL, then ng-word flag / version (0x1C bytes) */
        put_utf16(out, size / 2 < 11 ? size / 2 : 11, CFG_USERNAME);
        break;
    case 0x000A0001:  /* birthday: month, day */
        if (size >= 2) { out[0] = 1; out[1] = 1; }
        break;
    case 0x000B0000:  /* country info: [2] = state/province, [3] = country code */
        if (size >= 4) { out[2] = 0; out[3] = CFG_COUNTRY_UK; }
        break;
    case 0x000B0001:  /* country name, 16 languages x 64 UTF-16 units */
        for (uint32_t off = 0; off + 128 <= size; off += 128) put_utf16(out + off, 64, "United Kingdom");
        break;
    case 0x000B0002:  /* state name: none */
    case 0x000B0003:  /* coordinates: 0 */
        break;
    case 0x000C0000:  /* parental controls (0xC0 bytes): nothing restricted */
        break;
    case 0x000D0000:  /* EULA: low 16 bits = accepted version; accept every version */
        if (size >= 2) { out[0] = 0xFF; out[1] = 0xFF; }
        break;
    case 0x00070001:  /* sound output mode: 1 = stereo */
        if (size >= 1) out[0] = 1;
        break;
    case 0x00050005: { /* stereo camera / 3D settings, 8 floats (factory values) */
        static const float stereo[8] = {62.0f, 289.0f, 76.80000305f, 46.08000183f,
                                        10.0f, 5.0f, 55.58000183f, 21.56999969f};
        memcpy(out, stereo, size < sizeof stereo ? size : sizeof stereo);
        break;
    }
    case 0x00130000:  /* system flags read at start-up (00102470/001023F8): all clear */
        break;
    default:
        if (!unknown_logged) {
            unknown_logged = 1;
            fprintf(stderr, "services: cfg block %08X (%u bytes) not in the native table, "
                            "returning zeros (further unknown blocks not logged)\n", block, size);
        }
        break;
    }
}

/* USA 00108634  cfg IPC GetConfigInfoBlk2 0x00010082 (void *out, u32 size, u32 blockId)
 * -> Result. Evidence: EU 00108634: cmd[1] = r1 (size), cmd[2] = r2 (block id),
 * cmd[3] = (r1 << 4) | 0xC, cmd[4] = r0 (buffer); callers e.g. (buf, 4, 0x130000),
 * (buf, 0xC0, 0xC0000). Confidence: high. */
uint32_t func_00108634(void *out, uint32_t size, uint32_t block) {
    if (out && size) cfg_block(block, out, size);
    return RESULT_OK;
}
/* USA 00108630  nop entry falling through into 00108634 (all callers use this one).
 * Confidence: high. */
uint32_t func_00108630(void *out, uint32_t size, uint32_t block) { return func_00108634(out, size, block); }

/* USA 003D2564 (EU 003D2540)  cfg IPC SecureInfoGetRegion 0x00020000 (u8 *outRegion).
 * Used by 003D25A0/003D25A4 (game: 00297AD8, 003DB130). Confidence: high. */
uint32_t func_003D2564(uint8_t *out_region) {
    if (out_region) *out_region = (uint8_t)env_int("ALBW_REGION", CFG_DEFAULT_REGION, 0, 6);
    return RESULT_OK;
}

/* USA 001024E0  cfg::Finalize (cfg:u): refcount at 0x7158B0, svc CloseHandle of 0x716798.
 * (void) -> void (Ghidra shows 3 pass-through args). Native: nothing to close. The init side
 * (0010140C / 00104368) runs decompiled on top of the native srv. Confidence: high. */
void func_001024E0(void) {}

/* USA 00104418  cfg::Finalize dispatcher by session kind (0 = cfg:u -> close 0x716798,
 * 1 = cfg:s 0010846C, 2 = cfg:i 001084D4). (u32 kind) -> void. Confidence: high. */
void func_00104418(uint32_t kind) {}

/* ---------------------------------------------------------------------------------------------
 * CECD (StreetPass). Native meaning: the StreetPass daemon is idle and its storage is empty;
 * anything written is discarded (nobody to exchange with), every read reports "not found",
 * which the game handles as "no StreetPass data".
 * Each command exists twice: cecd:u (0x716784) and cecd:s (0x716788) variants.
 * ------------------------------------------------------------------------------------------- */

/* ReadMessage 0x00030104 (u32 programId, u8 isOutbox, const void *msgId, u32 msgIdSize,
 * u32 *outSize, void *buf, u32 bufSize). Evidence: cmd[1]=r0, cmd[2]=byte r1, cmd[3]=r3,
 * cmd[4]=stack2, msgId buffer r2, output buffer stack1, *stack0 = cmd[2].
 * USA 003D135C (cecd:u, EU 003D1338) / 003D1810 (cecd:s). Confidence: high. */
static uint32_t cecd_read_message(uint32_t *out_size) {
    if (out_size) *out_size = 0;
    return RESULT_CECD_NOT_FOUND;
}
uint32_t func_003D135C(uint32_t program_id, uint32_t is_outbox, const void *msg_id, uint32_t msg_id_size,
                       uint32_t *out_size, void *buf, uint32_t buf_size) {
    return cecd_read_message(out_size);
}
uint32_t func_003D1810(uint32_t program_id, uint32_t is_outbox, const void *msg_id, uint32_t msg_id_size,
                       uint32_t *out_size, void *buf, uint32_t buf_size) {
    return cecd_read_message(out_size);
}

/* GetCecdState 0x000E0000 (u32 *outState). Native: idle (0).
 * USA 003D13DC / 003D1890. Confidence: high. */
uint32_t func_003D13DC(uint32_t *out_state) {
    if (out_state) *out_state = 0;
    return RESULT_OK;
}
uint32_t func_003D1890(uint32_t *out_state) {
    if (out_state) *out_state = 0;
    return RESULT_OK;
}

/* OpenAndRead 0x00120104 (void *buf, u32 bufSize, u32 *outRead, u32 programId, u32 pathType,
 * u32 fileOption). Evidence: cmd[1]=r1, cmd[2]=r3, cmd[3..4]=stack0..1, buffer r0, *r2 = cmd[2].
 * USA 003D1418 / 003D18CC. Confidence: high. */
uint32_t func_003D1418(void *buf, uint32_t size, uint32_t *out_read, uint32_t program_id,
                       uint32_t path_type, uint32_t option) {
    if (out_read) *out_read = 0;
    return RESULT_CECD_NOT_FOUND;
}
uint32_t func_003D18CC(void *buf, uint32_t size, uint32_t *out_read, uint32_t program_id,
                       uint32_t path_type, uint32_t option) {
    if (out_read) *out_read = 0;
    return RESULT_CECD_NOT_FOUND;
}

/* OpenAndWrite 0x00110104 (const void *buf, u32 bufSize, u32 programId, u32 pathType,
 * u32 fileOption). USA 003D1488 / 003D193C. Confidence: high. */
uint32_t func_003D1488(const void *buf, uint32_t size, uint32_t program_id, uint32_t path_type,
                       uint32_t option) {
    return RESULT_OK;
}
uint32_t func_003D193C(const void *buf, uint32_t size, uint32_t program_id, uint32_t path_type,
                       uint32_t option) {
    return RESULT_OK;
}

/* WriteMessage 0x00070106 (u32 programId, u8 isOutbox, void *msgId, u32 msgIdSize,
 * const void *buf, u32 bufSize, ...). USA 003D14E0 / 003D1994. Confidence: high (result only). */
uint32_t func_003D14E0(uint32_t program_id, uint32_t is_outbox, void *msg_id, uint32_t msg_id_size,
                       const void *buf, uint32_t buf_size, uint32_t extra) {
    return RESULT_OK;
}
uint32_t func_003D1994(uint32_t program_id, uint32_t is_outbox, void *msg_id, uint32_t msg_id_size,
                       const void *buf, uint32_t buf_size, uint32_t extra) {
    return RESULT_OK;
}

/* GetChangeStateEventHandle 0x00100000 (Handle *outEvent): *r0 = cmd[3]. Native: no event
 * (its only waiter, 003CE91C, is replaced below). USA 003D1560 / 003D1A14. Confidence: high. */
uint32_t func_003D1560(uint32_t *out_event) {
    if (out_event) *out_event = 0;
    return RESULT_OK;
}
uint32_t func_003D1A14(uint32_t *out_event) {
    if (out_event) *out_event = 0;
    return RESULT_OK;
}

/* OpenRawFile 0x000100C2 (u32 programId, u32 pathType, u32 fileOption, u32 *outFileSize).
 * USA 003D159C / 003D1A50. Confidence: high. */
uint32_t func_003D159C(uint32_t program_id, uint32_t path_type, uint32_t option, uint32_t *out_size) {
    if (out_size) *out_size = 0;
    return RESULT_CECD_NOT_FOUND;
}
uint32_t func_003D1A50(uint32_t program_id, uint32_t path_type, uint32_t option, uint32_t *out_size) {
    if (out_size) *out_size = 0;
    return RESULT_CECD_NOT_FOUND;
}

/* Stop 0x000C0040 / Start 0x000B0040 (u32 command). USA 003D15EC, 003D1624 (cecd:u),
 * 003D1AA0, 003D1AD8 (cecd:s). Confidence: high. */
uint32_t func_003D15EC(uint32_t command) { return RESULT_OK; }
uint32_t func_003D1624(uint32_t command) { return RESULT_OK; }
uint32_t func_003D1AA0(uint32_t command) { return RESULT_OK; }
uint32_t func_003D1AD8(uint32_t command) { return RESULT_OK; }

/* Delete 0x00080102 (u32 programId, u32 pathType, u8 isOutbox, const void *msgId,
 * u32 msgIdSize). USA 003D165C / 003D1B10. Confidence: high (result only). */
uint32_t func_003D165C(uint32_t program_id, uint32_t path_type, uint32_t is_outbox, const void *msg_id,
                       uint32_t msg_id_size) {
    return RESULT_OK;
}
uint32_t func_003D1B10(uint32_t program_id, uint32_t path_type, uint32_t is_outbox, const void *msg_id,
                       uint32_t msg_id_size) {
    return RESULT_OK;
}

/* SetData 0x000900C2 (u32 programId, const void *buf, u32 size, u32 option).
 * USA 003D16B8 / 003D1B6C. Confidence: high (result only). */
uint32_t func_003D16B8(uint32_t program_id, const void *buf, uint32_t size, uint32_t option) {
    return RESULT_OK;
}
uint32_t func_003D1B6C(uint32_t program_id, const void *buf, uint32_t size, uint32_t option) {
    return RESULT_OK;
}

/* ReadData 0x000A00C4 (void *dest, u32 destSize, u32 infoType, const void *param,
 * u32 paramSize). Evidence: cmd[1]=r1, cmd[2]=r2, cmd[3]=stack0, param buffer r3, output buffer
 * r0. Native: no data (zero-filled). USA 003D1700 / 003D1BB4. Confidence: high (signature),
 * medium (meaning of the zero answer). */
uint32_t func_003D1700(void *dest, uint32_t dest_size, uint32_t info_type, const void *param,
                       uint32_t param_size) {
    if (dest && dest_size) memset(dest, 0, dest_size);
    return RESULT_OK;
}
uint32_t func_003D1BB4(void *dest, uint32_t dest_size, uint32_t info_type, const void *param,
                       uint32_t param_size) {
    if (dest && dest_size) memset(dest, 0, dest_size);
    return RESULT_OK;
}

/* USA 003CE91C (EU 003CE8F8)  cecd Start/Stop the daemon (int stop, int noWait) -> Result.
 * Original: lock, SuspendDaemons via ndm once, Start(0xB)/Stop(0xC), and unless noWait waits on
 * the change-state event (svc WaitSynchronization1, up to 60 polls) for the state to change,
 * then svc CloseHandle(event). Native: the daemon has no state to wait for. Game callers:
 * 004C63C4, 004C6494, 00563DE0, 00563E58, 00564044, 005640C4. Confidence: high. */
uint32_t func_003CE91C(int32_t stop, int32_t no_wait) { return RESULT_OK; }

/* USA 003D121C (EU 003D11F8)  cecd::Finalize: CloseHandle (00115ED4) of the cecd session.
 * Native: nothing to close. () -> Result. Confidence: medium. */
uint32_t func_003D121C(void) { return RESULT_OK; }

/* ---------------------------------------------------------------------------------------------
 * HID (input). The SDK readers copy the newest entry of the HID shared-memory rings; natively
 * the newest state is the platform layer's (plat_input(), updated by plat_poll()).
 * Layouts are the 3DS ones, as used by the decompiled callers (offsets checked against the
 * game's input object: USA 0048BEE8 / 0048BDDC).
 * ------------------------------------------------------------------------------------------- */

typedef struct {
    uint32_t hold, trigger, release;
    int16_t stick_x, stick_y;
} PadStatus;  /* nn::hid::PadStatus, 0x10 bytes (input object +0x15C0) */

typedef struct {
    uint32_t lifo;        /* +0x00 ring (set by 003D5684 from 003D55A8); unused natively */
    int32_t latest;       /* +0x04 */
    uint32_t prev_hold;   /* +0x08 hold of the previous read (trigger/release source) */
    int16_t clamp[18];    /* +0x0C stick clamp parameters (003D4CD4 / 003D4C40) */
    uint8_t first_read;   /* +0x30 1 until the first successful read */
} PadReader;

typedef struct {
    uint16_t x, y;
    uint8_t touch;
    uint8_t pad[3];
} TouchStatus;  /* nn::hid::TouchPanelStatus, 8 bytes (input object +0x15D4) */

typedef struct {
    int16_t x, y, z;
} AccelStatus;  /* nn::hid::AccelerometerStatus, 6 bytes (input object +0x15E0) */

typedef struct {
    uint32_t lifo;              /* +0x00 */
    int16_t play, sensitivity;  /* +0x04 / +0x06 (003D52C4: 0, 0x80) */
    int16_t last[3];            /* +0x08 last filtered sample */
} AccelReader;

_Static_assert(sizeof(PadStatus) == 0x10, "PadStatus layout");
_Static_assert(offsetof(PadReader, clamp) == 0x0C, "PadReader layout");
_Static_assert(offsetof(PadReader, first_read) == 0x30, "PadReader layout");
_Static_assert(sizeof(TouchStatus) == 8, "TouchStatus layout");
_Static_assert(offsetof(AccelReader, last) == 0x08, "AccelReader layout");

/* decompiled helpers (pure functions over the game's data) */
_Bool func_003D5638(void);                     /* HID suspended (state byte 0x7258E8+3 == 4) */
int func_00102AA8(void);                       /* APT initialised */
int func_0010939C(void);                       /* APT: application has the input focus */
void func_003D4978(int16_t *clamp);            /* sanitise the clamp parameters */
int16_t *func_003D4C40(int16_t *clamp, int16_t *out_x, int16_t *out_y, int x, int y);  /* clamp */

/* "the application does not own the input": the SDK readers return a neutral state then */
static bool hid_input_blocked(void) { return func_00102AA8() != 0 && func_0010939C() == 0; }

/* raw 3DS pad bits: no touch bit in the pad state (touch has its own reader) */
#define HID_PAD_MASK (~(uint32_t)KEY_TOUCH)

/* USA 00101434  hid::Initialize: srv GetServiceHandle("hid:USER") (001025B0), GetIPCHandles
 * (0x000A0000: shared memory + 5 events), map the shared memory. Called from 00100C94 and from
 * the game's input object ctor 0048BEE8 (Ghidra shows 3 pass-through args). Native: the
 * platform layer is the input device. Confidence: high. */
uint32_t func_00101434(void) { return RESULT_OK; }
/* USA 001025B0  hid init body (session global, service name, ...) -> Result. Confidence: high. */
uint32_t func_001025B0(uint32_t *session, const char *name) { return RESULT_OK; }

/* USA 0010144C / 001026D8  hid::Finalize: unmap shared memory (svc UnmapMemoryBlock), close the
 * event and session handles. Confidence: high. */
void func_0010144C(void) {}
void func_001026D8(uint32_t *session) {}

/* USA 00104710  hid IPC GetIPCHandles 0x000A0000 (Handle *sharedMem, Handle *ev0..ev4).
 * Native: no kernel objects. Confidence: high. */
uint32_t func_00104710(uint32_t *shm, uint32_t *e0, uint32_t *e1, uint32_t *e2, uint32_t *e3,
                       uint32_t *e4) {
    uint32_t *outs[6] = {shm, e0, e1, e2, e3, e4};
    for (int i = 0; i < 6; i++)
        if (outs[i]) *outs[i] = 0;
    return RESULT_OK;
}

/* USA 003D5608 (EU 003D55E8)  hid IPC EnableAccelerometer 0x00110000 (). Confidence: high. */
uint32_t func_003D5608(void) { return RESULT_OK; }

/* USA 003D3800 (EU 003D37DC)  PadReader::ReadLatest(PadReader *r, PadStatus *out) -> 1 if a
 * sample was read. Original: returns 0 while HID is suspended (003D5638); sanitises the clamp
 * (003D4978); copies the newest ring entry; clamps the stick in place (003D4C40); seeds
 * prev_hold on the first read; clears bit 13 of hold; trigger = hold & ~prev, release =
 * prev & ~hold; zeroes the status when APT says the application lost the input; prev = hold.
 * Native: identical, with the newest entry taken from plat_input() (same button bit layout,
 * circle pad -156..156 = the raw hardware range the clamp expects). Confidence: high. */
uint32_t func_003D3800(PadReader *r, PadStatus *out) {
    if (func_003D5638()) return 0;
    func_003D4978(r->clamp);
    const PlatInput *in = plat_input();
    out->hold = in->held & HID_PAD_MASK;
    out->stick_x = in->cpad_x;
    out->stick_y = in->cpad_y;
    func_003D4C40(r->clamp, &out->stick_x, &out->stick_y, out->stick_x, out->stick_y);
    if (r->first_read) {
        r->prev_hold = out->hold;
        r->first_read = 0;
    }
    out->hold &= ~0x2000u;
    out->trigger = (out->hold ^ r->prev_hold) & ~r->prev_hold;
    out->release = r->prev_hold & ~out->hold;
    if (hid_input_blocked()) memset(out, 0, sizeof *out);
    r->prev_hold = out->hold;
    return 1;
}

/* USA 003D48F4 (EU 003D48D4)  TouchPanelReader::ReadLatest(reader, TouchPanelStatus *out)
 * -> bool (a sample was read). Native: bottom-screen touch from plat_input() (0..319, 0..239).
 * Confidence: high. */
extern long g_plat_frames;
uint32_t func_003D48F4(void *reader, TouchStatus *out) {
    const PlatInput *in = plat_input();
    if (in->touch && !hid_input_blocked()) {
        out->x = (uint16_t)in->touch_x;
        out->y = (uint16_t)in->touch_y;
        out->touch = 1;
        static int tr = -1;
        if (tr < 0) tr = getenv("ALBW_TOUCH_TRACE") != NULL;
        if (tr) fprintf(stderr, "touch: f=%ld %d,%d\n", g_plat_frames, in->touch_x, in->touch_y);
    } else {
        out->x = 0;
        out->y = 0;
        out->touch = 0;
    }
    return 1;
}

/* USA 003D4D38 (EU 003D4D18)  AccelerometerReader::ReadLatest(AccelReader *r, AccelStatus *out)
 * -> bool. Original: newest ring entry, per-axis play/sensitivity filter (003D55B4) against the
 * previous sample (+8..+0xC), axis rotation (003D5148). Native: a PC has no accelerometer; report
 * the console lying still and flat (1 G = 512 units, see the 1/512 factor of 003D4DFC) along the
 * axis perpendicular to the lower screen. The filters are identity on a constant input and the
 * rotation is not applied. Confidence: medium (sign of the gravity axis unverified). */
#define ACCEL_1G 512
uint32_t func_003D4D38(AccelReader *r, AccelStatus *out) {
    out->x = 0;
    out->y = -ACCEL_1G;
    out->z = 0;
    r->last[0] = out->x;
    r->last[1] = out->y;
    r->last[2] = out->z;
    return 1;
}

/* USA 003D3904 (EU 003D38E0)  reader for the extra HID ring polled through 003D4220 (0x3C-byte
 * entries; the game's object at input+0x15F4 is null unless a device is attached; most likely
 * the expansion pad) (reader, out) -> bool. Native: no such device; no sample. The reader's
 * stored history is left untouched. Confidence: medium (device identity), high (no-sample
 * meaning). */
uint32_t func_003D3904(void *reader, void *out) { return 0; }
