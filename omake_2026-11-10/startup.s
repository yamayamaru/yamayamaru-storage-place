                .text
                .global _start
                .extern _stack_top
                .extern ___startup

_start:
                movl            %esp, esp_backup
                leal            (_stack_top), %ebx
                sub             $64, %ebx
                and             $0xfffffff0, %ebx

                movl            %ebx, %esp

                movl            %esp, %ebp

                call            ___startup
                movb            %al, %bl

                movl            (esp_backup), %esp

                movb            $0x4c, %ah
                movb            %bl, %al
                int             $0x21
                ret

                .data

esp_backup:     .long           0

                .end
