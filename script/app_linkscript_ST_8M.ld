MEMORY
{
    ram(RW) : ORIGIN = 0x7e750000, LENGTH = 0x80000
}


SECTIONS
{
	. = 0x7e680000;

	.appRegTable :
	{	
		;PROVIDE_HIDDEN (__appRegTable_start__ = .);
		KEEP(*(.appRegTable))
		;PROVIDE_HIDDEN (__appRegTable_end__ = .);
	} >ram
	.text : 
	{ 
	    *(.text) 
	} >ram
	.rodata : 
	{
	    *(.rodata)
	} >ram
	.data : 
	{
		PROVIDE_HIDDEN (__data_load = LOADADDR(.data));
		PROVIDE_HIDDEN (__data_start = .);
		*(.data)
		PROVIDE_HIDDEN (_edata = .);
	} >ram
	.ARM.exidx :
	{
		PROVIDE_HIDDEN (__exidx_start = .);
		*(.ARM.exidx);
		PROVIDE_HIDDEN (__exidx_end = .);
	} >ram
	.bss :
	{
		PROVIDE_HIDDEN (__bss_start__ = .);
		*(.bss)
		PROVIDE_HIDDEN (__bss_end__ = .);
	} >ram

	end = 0x7e7D0000;
}