ENTRY(Reset_Handler)

_estack = 0x20001800;

_Min_Leftover_RAM = 0x400;

MEMORY
{
    FLASH ( rx )    : ORIGIN = 0x08000000, LENGTH = 32K
    RAM  ( rwx )    : ORIGIN = 0x20000000, LENGTH = 6K
}

SECTIONS
{
  /* The vector table must go at the start of flash */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP (*(.vector_table))
    . = ALIGN(4);
  } >FLASH
  
  /* The 'text' section contains the main code; goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    . = ALIGN(4);
  } >FLASH
  
  /* Read-only data goes into FLASH*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH
  
  /* Variables go into RAM */
  _sidata = .;
  .data : AT(_sidata)
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM
  
  /* The 'bss' section is similar to the 'data' section,
   * but its space is initialized to all 0s at the
   * start of the program. */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >RAM
  
  /* Make sure we have enough room in RAM left for a decent stack */
  .dynamic_allocations :
  {
    . = ALIGN(4);
    _ssystem_ram = .;
    . = . + _Min_Leftover_RAM;
    . = ALIGN(4);
    _esystem_ram = .;
  } >RAM
}