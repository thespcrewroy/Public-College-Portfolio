<h1 align="center">SLEUTH KIT (TSK) CHEATSHEET</h1>

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#for-fat/fat32-&-general-fs-investigation)

## For FAT/FAT32 & GENERAL FS INVESTIGATION
- Always work with READ-ONLY on copies
- **Record**: Hashes, Offsets, and Tool Versions

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#legend)
## Legend
- **```img```**: whole-disk image (MBR/GPT present)
- **```START```**: filesystem start sector (from mmls/sfdisk; often 2048)
- **```SECSZ```**: logical sector size (assume 512 unless known otherwise)
- **```<inode>```**: metadata address (from fls/ifind/istat) <br>

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#find-volume-layout)
## Find Volume Layout (Partitions & Offsets)
- **```mmls img```**: partition table
- **```sfdisk -l img```**: alternative view
- **```parted -s img unit s print```**: another option

<br>

> [!TIP]\
> ➤ For ```mmls img```, note each of the ```START``` sectors per partition.

> [!WARNING]\
> ➤ TSK commands use ```-o START``` in SECTORS (not bytes).

> [!NOTE]\
> ➤ OS mount uses the byte offset: offset = ```START``` * ```SECSZ```.

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#filesystem-metadata)

## Filesystem Metadata (sanity check)
- **```fsstat -f fat -o START img | head -40```**: show filesystem metadata (bytes/sector, sectors/cluster, label, FAT type, etc)
- **```dd if=img bs=512 skip=START count=1 status=none | xxd -l 128```**: look for 'FAT' text hints (FAT16/FAT32)
- **```dd if=img bs=512 skip=START count=1 status=none | tail -c 2 | xxd -p```**: expect 55aa at end of VBR

<br>

> [!TIP]\
> ➤ Use ```fsstate``` as a sanity check.

> [!TIP]\
> ➤ Use ```dd``` for boot sector spot checks to provide context.

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#directory-and-deleted-directory-listings)
## Directory and Deleted Directory Listings
- **```fls -o START img /```**: root listing
- **```fls -o START -r img / > files.txt```**: recursive listing → file (good for grep/search)
- **```fls -o START -rd img /```**: deleted only listing (method 1)
- **```grep '^\*' files.txt```**: deleted only listing (method 2)
- **```ifind -f fat -o START -n NAME.EXT img```**: find by exact matching name in this file system

> [!NOTE]\
> ➤ Deleted entries prefixed with __'\*'__.

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#allocated-or-deleted-metadata-and-extraction)

## Allocated or Intact Deleted File Metadata Extraction
- **```istat -o START img <inode>```**: show metadata (MAC times, size, cluster runs)
- **```icat -o START img <inode> > recovered.bin```**: extracts file metadata by inode from a disk image
- **```file recovered.bin```**: identify file type of ```recovered.bin``` to determine steps for conversion from a binary file to a human readable file
- **```strings -n 8 recovered.bin | head```**: quickly preview readable text fragments embedded inside the binary
- **```mkdir out_all && tsk_recover -o START img out_all```**: bulk recovery of allocated files
- **```mkdir out_unalloc && tsk_recover -o START -e img out_unalloc```**: bulk recovery carved from unallocated

> [!IMPORTANT]\
> ➤ ```icat```, ```file```, and ```strings``` are run in sequence to extract file metadata by inode.

[![-----------------------------------------------------](https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/aqua.png)](#unallocated-and-file-slack)

## Unallocated and File Slack

- **```blkls -f fat -b 512 -o START img > unalloc.dd```**: extracts all unallocated clusters (deleted files, slack space, etc) for carving
- **```mmcat img part_num > unalloc.dd```**: reads and outputs all data from a given partition, creating a standalone filesystem image

# Display slack space

fcat -s filename -o START img

> [!NOTE]\
> ➤ Allows you to view the inside of a file system


---

6. Disk Gaps / Unallocated Outside Any Filesystem (mmcat & dd)

---

# View layout with mmls; note slots labeled “Unallocated”, “Primary Table (#0)”, or partitions.

# A) mmcat by slot number (preferred for disk-level unallocated)

mmls img
mmcat -i raw img 00 > primary_region.bin # slot 00: MBR + post-MBR gap
mmcat -i raw img <slot_of_unallocated> > disk_unalloc.bin
binwalk disk_unalloc.bin
strings -n 8 disk_unalloc.bin | head

# B) dd by sector math (example with one partition)

# Example numbers from mmls:

# P1_START=2048, P1_SIZE=24576 → tail starts at 2048+24576 = 26624

TAIL_START=$((2048 + 24576))
dd if=img bs=512 skip=$TAIL_START of=disk_tail.bin status=none
binwalk disk_tail.bin

---

7. Mounting for a Quick Peek (NOT for deleted/slack)

---

# With losetup exposing partition devices (handy across tools)

LOOP=$(sudo losetup --show -Pf --read-only img)
sudo mount -o ro ${LOOP}p1 /mnt/p

# ... browse ...

sudo umount /mnt/p; sudo losetup -d "$LOOP"

---

8. MBR / Partition Table Inspection

---

# Sector 0 (MBR). 55aa = boot sector trailer (not a FAT signature).

dd if=img bs=512 count=1 status=none | xxd -g1 | sed -n '1,16p'
dd if=img bs=1 skip=446 count=64 status=none | xxd -g1 # 4×16-byte partition entries
dd if=img bs=512 count=1 status=none | tail -c 2 | xxd -p # expect 55aa

# Decode entries (start LBA, size). Use mmls for a friendlier view.

---

9. Autopsy (GUI) Quick Path

---

New Case → Add Data Source (Disk Image) → minimal ingest (File Type, Carving)
Views:
• Data Sources → Deleted Files
• Results → Carved Files
• Views → File Types; per-file Hex/Strings/Metadata
Export:
• Right-click file → Extract
• Note inode/paths/timestamps for reports

---

11. Common Pitfalls & Quick Fixes

---

- “-o START” uses SECTORS, not bytes. Don’t pass byte offsets to TSK.
- Pointing fsstat/fls at LBA 0 (MBR) ⇒ “cannot determine FS type.”
- blkls only works INSIDE a filesystem; use mmcat/dd for disk-level unallocated.
- Autopsy for discovery/screenshots; CLI for reproducible procedures.

---

12. Useful Commands

---

a) Image / Volume-system (whole-disk, partitions)

- img_stat — Show basic info about an image file (size, sectors, type).
- img_cat — Dump raw image bytes to stdout.
- mmls — List the partition table / volume system layout (MBR/GPT), start/size of each partition.
- mmstat — Summary of the volume system (type, number of partitions).
- mmcat — Extract a partition/volume (by slot number from mmls) to stdout.

b) Filesystem (generic across FAT/NTFS/ext…)

- fsstat — Filesystem metadata summary (block/cluster sizes, counts, label, etc.).
- fls — List directory contents (can recurse; shows deleted entries with \*).
- ffind — Given a metadata address (inode/MFT), find file path(s) that reference it.
- tsk_recover — Recover files from a filesystem into a local directory (allocated; -e for carved from unallocated).

c) Data-unit / block helpers (inside a filesystem)

- blkls — Dump unallocated data units (and with -s, file slack) from a filesystem.
- blkcat — Output the contents of a given block/cluster (by number).
- blkstat — Show allocation/status info for a specific block/cluster.
- blkcalc — Convert between byte/sector/block addresses for a filesystem.

d) inode / file inspect

- ifind — Map name → inode, or block/sector → owning inode (with -d for data block).
- istat — Show detailed metadata for a file (inode/MFT record): MAC times, blocks/runs.
- ils — List inodes/MFT records (allocated & unallocated; use -e for only unallocated).
- icat — Extract file content by inode/MFT (works even when filename is deleted).
- fcat — Extract file content by path (where supported).

e) Journals / special structures

- jls — List entries in a filesystem journal (e.g., ext3/4).
- jcat — Dump raw journal contents.
