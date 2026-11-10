OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
  /* RUN386やa.out環境の標準的な開始アドレス（環境に合わせて調整可） */
  . = 0x00000000;

  .text : {
    *(.text .text.*)
    *(.rodata .rodata.*)   /* .rodata を統合 */
    . = ALIGN(16);
  }

  .data : {
    *(.got .got.*)         /* ★エラーの原因だった .got を .data 内に吸収 */
    *(.data .data.*)
    . = ALIGN(16);
  }

  .bss : {
    /* スタートアップコードの &__bss_start がここを指すようになります */
    __bss_start = .;
    *(.bss .bss.*)
    *(COMMON)

    . = ALIGN(16);
  }

  /* ★ スタック領域を明示的に確保（例: 512KB） */
  . = ALIGN(16);
  _stack_bottom = .;
  . += 512 * 1024;    /* 512KB分のアドレスを進める */
  _stack_top = .;    /* スタックの初期値（上から下に伸びるため） */

  /* ★ スタックのさらに後ろをヒープの開始位置にする */
  . = ALIGN(16);
  /* スタートアップコードの &_end がここを指すようになります */
  _end = .;

  /* 現代の不要なメタデータセクションをすべて破棄 */
  /DISCARD/ : {
    *(.note.GNU-stack)
    *(.comment)
    *(.eh_frame)
  }
}
