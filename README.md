# DiscProbeELF-PS5
Experimental PS5 optical-disc research payload for testing PS2 DVD access through `/dev/cd0`, `/mnt/disc`, and related interfaces. Created to explore possible physical-disc support for PS5SX2.

An experimental PS5 payload project investigating whether original PlayStation 2 DVD-ROM discs and readable recordable DVD media can be accessed from the PS5 homebrew environment.

With PS5SX2 bringing PCSX2-based PlayStation 2 emulation to PS5, this project explores one additional possibility: using physical PS2 discs as a game source instead of requiring users to manually provide an image file.

The project currently focuses on research and hardware testing. It does not modify PS5SX2 and does not currently provide physical-disc support for PS5SX2.

Current Research

The test payload investigates several PS5 optical-disc interfaces:

/dev/cd0 optical-device access

/mnt/disc filesystem access

ISO9660 sector reading

SYSTEM.CNF detection

PS2 game serial identification

ISO volume-label detection

Optical-drive ioctl availability

Disc identity interfaces such as /dev/duid

Sustained optical read performance

Original PS2 DVD-ROM compatibility

DVD-R/DVD+R accessibility

The goal is to determine which interfaces are usable before implementing an emulator-specific physical-disc backend.

Long-Term Goal

If reliable PS2 DVD access can be demonstrated, a future implementation could potentially use the PS5 optical drive together with PCSX2's existing CDVD emulation architecture.

Possible approaches include:

Direct physical-disc sector access.

A privileged DiscBridge service between the optical drive and a sandboxed emulator.

Installing/caching disc data to PS5 storage for faster loading while retaining physical-disc presence as part of the experience.

These approaches are experimental and have not yet been demonstrated to work with PS5SX2.

Current Status

Experimental / Proof of Concept

The current payload is intended to answer a basic question:

Can an exploited PS5 reliably access the filesystem and/or sectors of an original DVD-based PlayStation 2 game?

Results from real hardware testing will determine whether further PS5SX2 integration is technically viable.

Important

This project is independent research and is not affiliated with Sony Interactive Entertainment, PCSX2, or the PS5SX2 developers.

The project is intended for interoperability research, homebrew development, and testing with media you are authorized to use.
