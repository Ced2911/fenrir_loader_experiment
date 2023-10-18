OUTPUT_ARCH(m68k)

__DYNAMIC  =  0;

MEMORY
{
  ram (rwx) : ORIGIN = 0x0, LENGTH = 512K
}

PROVIDE (__stack_start = 0x80000 - 1K);
PROVIDE (__stack_end = 0x80000);

/*
 * stick everything in ram (of course)
 */
SECTIONS
{
/* .vecs : { vectors.obj } > ram1 */
  VECTORS : {
     __vectors = .;
     *(VECTORS)
  } > ram1
  .text :
  {
  PROVIDE (entry_point = .);
    *(.text)
    . = ALIGN(0x4);
     __CTOR_LIST__ = .;
    ___CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    ___DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    *(.dtors)
     LONG(0)
    __DTOR_END__ = .;
    *(.rodata)
    *(.gcc_except_table) 

    __INIT_SECTION__ = . ;
    LONG (0x4e560000)	/* linkw %fp,#0 */
    *(.init)
    SHORT (0x4e5e)	/* unlk %fp */
    SHORT (0x4e75)	/* rts */

    __FINI_SECTION__ = . ;
    LONG (0x4e560000)	/* linkw %fp,#0 */
    *(.fini)
    SHORT (0x4e5e)	/* unlk %fp */
    SHORT (0x4e75)	/* rts */

    _etext = .;
    *(.lit)
  } > ram2

  .data :
  {
    *(.shdata)
    *(.data)
    _edata = .;
  } > ram2

  .bss :
  {
    . = ALIGN(0x4);
    __bss_start = . ;
    *(.shbss)
    *(.bss)
    *(COMMON)
    __bss_end = .;
    _end =  ALIGN (0x8);
    __end = _end;
  } > ram2

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}