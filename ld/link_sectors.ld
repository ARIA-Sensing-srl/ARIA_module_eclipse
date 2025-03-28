
OUTPUT_ARCH(riscv)
MEMORY
{
  PRI0 (rwx)    : ORIGIN = 0x1C000000, LENGTH = 0x18000 /* CV32E40 Instruction */
  PRI1 (rwx)    : ORIGIN = 0x1C018000, LENGTH = 0x10000	/* CV32E40 Data */
  PRI2 (rwx)    : ORIGIN = 0x1C028000, LENGTH = 0x8000	/* copr Instruction */
  PRI3 (rwx)    : ORIGIN = 0x1C030000, LENGTH = 0x8000	/* copr Data */
  L2_IPC (!rx)  : ORIGIN = 0x1C040000, LENGTH = 0x100
  L2_CV_HEAP  (!rx)   	: ORIGIN = 0x1C040100, LENGTH = 0x32000
  L2_SHARE  (!rx)   	: ORIGIN = 0x1C072100, LENGTH = 0x5F00
}