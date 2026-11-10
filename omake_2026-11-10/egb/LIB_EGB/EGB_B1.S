/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.data
	.text
/**********************************
  block operation  1 / 5
 **********************************/

/*
	EGB_clearScreen(char *gwork)  VRAM領域の消去
*/
	.align 16
	.globl EGB_clearScreen
EGB_clearScreen:
	push  %edi
	movb  $0x20,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_partClearScreen(char *gwork) ビューポート領域の消去
*/
	.align 16
	.globl EGB_partClearScreen
EGB_partClearScreen:
	push  %edi
	movb  $0x21,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

	.end

