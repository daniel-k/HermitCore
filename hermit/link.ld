OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH("i386:x86-64")
ENTRY(start)
phys = (2 << 20);
cores = 40;

SECTIONS
{
  kernel_start =  phys; 
  .mboot phys : AT(ADDR(.mboot)) {
    *(.mboot)
    . = ALIGN((1 << 12));
    *(.kmsg)
  }
  .text ALIGN(4096) : AT(ADDR(.text)) {
    *(.text)
    *(.text.startup)
  }
  .rodata ALIGN(4096) : AT(ADDR(.rodata)) {
    *(.rodata)
    *(.rodata.*)
  }
  .data ALIGN(4096) : AT(ADDR(.data)) {
    *(.data)
  }
  .bss ALIGN(4096) : AT(ADDR(.bss)) {
    bss_start = .;
    *(.bss)
  }
  bss_end = .;
  .percore ALIGN(4096) : AT(ADDR(.percore)) {
    percore_start = .;
    *(.percore)
    . = ALIGN(64);
    percore_end0 = .;
  }
  percore_end = percore_start + cores * SIZEOF(.percore);
  kernel_end = percore_end;
}
