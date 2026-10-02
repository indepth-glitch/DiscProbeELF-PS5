# PS5 Disc Fallback Probe EXPERIMENTAL

Standalone PS5 payload that tests the concrete optical-access alternatives
found in public PS5 homebrew research.

AND REMEMBER THIS IS FOR EXPERIMENTAL USE! I WOULD LIKE TO MAKE PEEPS WAIT FIRST UNTIL IT'S DONE,
MY DUMBASS WANTS TO FINISH THIS CODE!

No PS5SX2 integration. No disc dumping.

## Paths tested

### `/mnt/disc`

Tests whether Sony has mounted the inserted disc as a filesystem and whether
`SYSTEM.CNF` is directly visible.

### `/dev/cd0`

Tests:

- read-only open
- `CDIOCALLOW` if the SDK exposes `<sys/cdio.h>`
- TOC-header ioctl if the SDK exposes it
- ordinary 2048-byte LBA 16 read
- ISO-9660 `CD001`
- volume label

The payload NEVER sends `CDIOCEJECT`.

### `/dev/duid`

Tests only whether the node opens and returns bytes. The bytes themselves are
not displayed or saved.

## Why CAM/SCSI and SceBdSvc aren't hard-coded

Those are useful fallback directions, but guessing private/firmware-dependent
ABI definitions would make this test less trustworthy and could send invalid
commands to the optical device. This build first measures interfaces that
have concrete public PS5 evidence.

If `/dev/cd0` opens and CD ioctls work while block reads fail, preserve the
exact errno. That is the useful result for a subsequent CAM/SCSI-specific
probe.

## Build

    export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
    make clean
    make

Output:

    ps5-disc-fallback-probe.elf

## Test order

1. No disc.
2. Known-readable DVD.
3. Original DVD-based PS2 game.
4. User-authored/homebrew DVD-R.

Photograph the notifications, especially failures and errno values.
