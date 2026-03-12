# ROP write up
This exploit was provided by BitFriends on crackmes.one https://crackmes.one/crackme/5f3d7ed033c5d42a7c667d95

# Context
This is very exciting as it's my first time doing any type of buffer overflow myself from scratch. I'd like to thank BitFriends in advance for uploading a binary that serves as a low entry introduction. My I had to learn almost all of it as I only undestood buffer overflows from a theoretical standpoint. So, frankly, for most of this solve I had no clue what I was doing. Because of this, I decided to include some of my learning process in this write up, as for me otherwise it would have big nerrative jumps that wouldn't otherwise make sense.

# Recon
```
/Projects/RE/crackmes.one/rop
❯ file ./rop
./rop: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0, BuildID[sha1]=f950fa8cc1c799a905ae42eaade36fce8f60ac61, not stripped

~/Projects/RE/crackmes.one/rop
❯ checksec ./rop
[*] '/home/edoardo/Projects/RE/crackmes.one/rop/rop'
    Arch:       amd64-64-little
    RELRO:      Partial RELRO
    Stack:      No canary found
    NX:         NX enabled
    PIE:        No PIE (0x400000)
    Stripped:   No

/Projects/RE/crackmes.one/rop
❯ ROPgadget --binary ./rop | grep "pop rdi"
0x0000000000400613 : pop rdi ; ret
```

As the challenge it self is called 'rop' I start out searching "rop exploit" online, and learn it stands for Return Oriented Programming. Supposedly, an attacker using this technique would be able to hijack a programs control flow and execute instructions already present in the machines memory. These instructions are refered to as gadgets. This to me sounds like some type of buffer overflow so let's run it and see if it takes any type of input.
```
~/Projects/RE/crackmes.one/rop
❯ ./rop
Input: test
Hello World!
```
So it takes a stdin, and doesn't seem to do anything with it. Let's see the Assembly in Ghidra. This is a very simple program so I'll show the whole thing:
```
        00400537 55              PUSH       RBP

        00400538 48 89 e5        MOV        RBP,RSP

        0040053b ba 07 00        MOV        EDX,0x7
                 00 00
        00400540 48 8d 35        LEA        RSI,[s_Input:_00400634]                          = "Input: "
                 ed 00 00 00
        00400547 bf 01 00        MOV        EDI,0x1
                 00 00
        0040054c b8 00 00        MOV        EAX,0x0
                 00 00
        00400551 e8 da fe        CALL       <EXTERNAL>::write                                ssize_t write(int __fd, void * _
                 ff ff
        00400556 b8 00 00        MOV        EAX,0x0
                 00 00
        0040055b e8 22 00        CALL       input                                            undefined input()
                 00 00
        00400560 ba 0d 00        MOV        EDX,0xd
                 00 00
        00400565 48 8d 35        LEA        RSI,[s_Hello_World!_0040063c]                    = "Hello World!\n"
                 d0 00 00 00
        0040056c bf 01 00        MOV        EDI,0x1
                 00 00
        00400571 b8 00 00        MOV        EAX,0x0
                 00 00
        00400576 e8 b5 fe        CALL       <EXTERNAL>::write                                ssize_t write(int __fd, void * _
                 ff ff
        0040057b b8 00 00        MOV        EAX,0x0
                 00 00
        00400580 5d              POP        RBP
        00400581 c3              RET
```

All that happens here is write string, call `input()`, write string. Nothing here. Let's check `input()`.
```
        00400582 55              PUSH       RBP
        00400583 48 89 e5        MOV        RBP,RSP
        00400586 48 83 ec 40     SUB        RSP,0x40
        0040058a 48 8d 45 c0     LEA        RAX=>local_48,[RBP + -0x40]
        0040058e ba 80 02        MOV        EDX,0x280
                 00 00
        00400593 48 89 c6        MOV        RSI,RAX
        00400596 bf 00 00        MOV        EDI,0x0
                 00 00
        0040059b b8 00 00        MOV        EAX,0x0
                 00 00
        004005a0 e8 9b fe        CALL       <EXTERNAL>::read                                 ssize_t read(int __fd, void * __
                 ff ff
        004005a5 90              NOP
        004005a6 c9              LEAVE
        004005a7 c3              RET
```
