/* in this memory layout private application processor instruction and data are merged */
OUTPUT_ARCH(riscv)
MEMORY
{
  PRI0 (rwx)    : ORIGIN = 0x1C000000, LENGTH = 0x28000 /* CV32E40 Instruction */
  PRI2 (rwx)    : ORIGIN = 0x1C028000, LENGTH = 0x8000	/* copr Instruction */
  PRI3 (rwx)    : ORIGIN = 0x1C030000, LENGTH = 0x8000	/* copr Data */
  L2_IPC (!rx)  : ORIGIN = 0x1C040000, LENGTH = 0x100
  L2_CV  (!rx)   	: ORIGIN = 0x1C040100, LENGTH = 0x32000
  L2_SHARE  (!rx)   	: ORIGIN = 0x1C072100, LENGTH = 0x5F00
}