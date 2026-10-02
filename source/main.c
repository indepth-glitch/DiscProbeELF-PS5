#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "notify.h"

#define SECTOR 2048
#define PVD_LBA 16

/*
 * We intentionally do not hard-code unverified PS5 CAM/SCSI ioctl ABIs.
 * This probe exhausts the concrete, publicly evidenced interfaces first:
 * /mnt/disc, /dev/cd0, and /dev/duid.
 *
 * CD-specific ioctl constants come from the target SDK's sys/cdio.h when
 * available. They are used only for harmless media/status-style probes.
 */
#if __has_include(<sys/cdio.h>)
#include <sys/cdio.h>
#define HAVE_CDIO 1
#else
#define HAVE_CDIO 0
#endif

static void trim_label(char s[33])
{
    for (int i = 31; i >= 0; --i) {
        if (s[i] == ' ' || s[i] == '\0')
            s[i] = '\0';
        else
            break;
    }
}

static void test_mount(void)
{
    DIR *d = opendir("/mnt/disc");
    if (!d) {
        notify("[Fallback] /mnt/disc FAIL errno %d", errno);
        return;
    }

    notify("[Fallback] /mnt/disc PASS");
    closedir(d);

    int fd = open("/mnt/disc/SYSTEM.CNF", O_RDONLY);
    if (fd >= 0) {
        char b[256];
        ssize_t n = read(fd, b, sizeof(b)-1);
        close(fd);
        if (n > 0) {
            b[n] = 0;
            notify("[Fallback] mounted SYSTEM.CNF PASS");
        } else {
            notify("[Fallback] SYSTEM.CNF exists, read=%d", (int)n);
        }
    } else {
        notify("[Fallback] mounted SYSTEM.CNF absent errno %d", errno);
    }
}

static int test_cd0(void)
{
    unsigned char pvd[SECTOR];
    int fd = open("/dev/cd0", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        notify("[Fallback] /dev/cd0 OPEN FAIL errno %d", errno);
        return -1;
    }

    notify("[Fallback] /dev/cd0 OPEN PASS");

#if HAVE_CDIO
    /*
     * CDIOCALLOW is known to reach the PS5 optical device in public payload
     * work. It does not eject the disc; it only clears eject prevention.
     * We do NOT issue CDIOCEJECT in this probe.
     */
#ifdef CDIOCALLOW
    errno = 0;
    int ar = ioctl(fd, CDIOCALLOW);
    if (ar == 0)
        notify("[Fallback] CDIOCALLOW PASS");
    else
        notify("[Fallback] CDIOCALLOW FAIL errno %d", errno);
#endif

#ifdef CDIOREADTOCHEADER
    struct ioc_toc_header th;
    memset(&th, 0, sizeof(th));
    errno = 0;
    if (ioctl(fd, CDIOREADTOCHEADER, &th) == 0)
        notify("[Fallback] TOC ioctl PASS");
    else
        notify("[Fallback] TOC ioctl FAIL errno %d", errno);
#endif
#else
    notify("[Fallback] SDK has no sys/cdio.h");
#endif

    if (lseek(fd, (off_t)PVD_LBA * SECTOR, SEEK_SET) < 0) {
        notify("[Fallback] cd0 SEEK FAIL errno %d", errno);
        close(fd);
        return -2;
    }

    ssize_t n = read(fd, pvd, sizeof(pvd));
    if (n != sizeof(pvd)) {
        notify("[Fallback] cd0 READ FAIL n=%d errno %d", (int)n, errno);
        close(fd);
        return -3;
    }

    notify("[Fallback] cd0 BLOCK READ PASS");

    if (!memcmp(pvd + 1, "CD001", 5)) {
        char label[33];
        memcpy(label, pvd + 40, 32);
        label[32] = 0;
        trim_label(label);
        notify("[Fallback] ISO9660 PASS");
        notify("[Fallback] DISC: %s", label[0] ? label : "(blank)");
    } else {
        notify("[Fallback] LBA16 readable, CD001 absent");
    }

    close(fd);
    return 0;
}

static void test_duid(void)
{
    unsigned char buf[256];
    int fd = open("/dev/duid", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        notify("[Fallback] /dev/duid OPEN FAIL errno %d", errno);
        return;
    }

    notify("[Fallback] /dev/duid OPEN PASS");

    /*
     * Do not display the identifier itself. We only care whether the
     * interface is readable/present for future disc-presence experiments.
     */
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n > 0)
        notify("[Fallback] /dev/duid READ PASS (%d bytes)", (int)n);
    else if (n == 0)
        notify("[Fallback] /dev/duid zero-byte read");
    else
        notify("[Fallback] /dev/duid READ FAIL errno %d", errno);

    close(fd);
}

int main(void)
{
    notify("[Fallback] START");

    /* Alternative A: Sony-mounted disc filesystem. */
    test_mount();

    /* Alternative B/C: optical device block read + standard CD ioctls. */
    test_cd0();

    /* Alternative D: public research associates this node with disc ID. */
    test_duid();

    /*
     * CAM/SCSI and SceBdSvc are deliberately not guessed here. Their ABI
     * must be established from the target firmware/SDK before sending
     * low-level commands to the drive.
     */
    notify("[Fallback] CAM/SCSI: not guessed");
    notify("[Fallback] SceBdSvc: not guessed");
    notify("[Fallback] COMPLETE");
    return 0;
}
