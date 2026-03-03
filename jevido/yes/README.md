All credit for this binary goes to the writer and compiler Jevido https://github.com/jevido

First things first;
```
/Projects/Reverse_Engineering/jevido_bins
❯ checksec file yes

  _____ _    _ ______ _____ _  __ _____ ______ _____
 / ____| |  | |  ____/ ____| |/ // ____|  ____/ ____|
| |    | |__| | |__ | |    | ' /| (___ | |__ | |
| |    |  __  |  __|| |    |  <  \___ \|  __|| |
| |____| |  | | |___| |____| . \ ____) | |___| |____
 \_____|_|  |_|______\_____|_|\_\_____/|______\_____|

RELRO           Stack Canary      CFI               NX            PIE             RPATH      RUNPATH      Symbols         FORTIFY    Fortified   Fortifiable      Name                            
No RELRO        Canary Found      Unknown           NX enabled    PIE Disabled    No RPATH   No RUNPATH   1010 symbols    Yes        2           15               yes

~/Projects/Reverse_Engineering/jevido_bins
❯ file yes
yes: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0, BuildID[sha1]=db04d9fbd1eaf9f5deaf68dde4e71e03ac240201, not stripped
```
So firsrt off, this binary was about ~100MB, which is incredibly large. Also on first glance it seems to be different then Jevido's other bin; sii. This one is x86_64, rather then just 64, is also dinamically linked instead of statically. So, I am just going to assume I won't be able to use my experience from the other one for this one.

Starting at .text, there is a __libc_start_main right there, so this is likely just a C program. Since we've established it's dynamically linked, we don't just get main right here
```
        ...
        02adfb13 4c 8d 05        LEA        R8,[0x40d3770]
                 56 3c 5f 01
        02adfb1a 48 8d 0d        LEA        RCX,[0x40d3700]
                 df 3b 5f 01
        02adfb21 48 8d 3d        LEA        RDI,[0x2adfbf0]
                 c8 00 00 00
        02adfb28 ff 15 c2        CALL       qword ptr [-><EXTERNAL>::__libc_start_main]      = 0658c000
                 46 9c 03
        ...
```
So checking RDI, `0x2adfbf0` is likely going to be our main.
