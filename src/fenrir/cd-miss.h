
// #include "cd-block-internal.h"

#define CMOK (0x0001) /* Command dispatch possible */
#define DRDY (0x0002) /* Data transfer preparations complete */
#define CSCT (0x0004) /* Finished reading 1 sector */
#define BFUL (0x0008) /* CD buffer full */
#define PEND (0x0010) /* CD playback completed */
#define DCHG (0x0020) /* Disc change or tray open */
#define ESEL (0x0040) /* Selector settings processing complete */
#define EHST (0x0080) /* Host I/O processing complete */
#define ECPY (0x0100) /* Duplication/move processing complete */
#define EFLS (0x0200) /* File system processing complete */
#define SCDQ (0x0400) /* Subcode Q update completed */
#define MPED (0x0800) /* MPEG-related processing complete */
#define MPCM (0x1000) /* MPEG action uncertain */
#define MPST (0x2000) /* MPEG interrupt status report */

/* CD-block */
#define DTR 0x0000UL
#define HIRQ 0x0008UL
#define HIRQ_MASK 0x000CUL
#define CR1 0x0018UL
#define CR2 0x001CUL
#define CR3 0x0020UL
#define CR4 0x0024UL