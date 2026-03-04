hanks to S3c_Cult for publishign this binary https://crackmes.one/crackme/684f47bd2b84be7ea774390e

First off, running it.

```
~/Projects/Reverse_Engineering/crackmes.one/XORcist
❯ ./xorcist 
What's the password?
test
Nuh Uh Nuh Uh.
```

So pretty standard input/output password check bin so far. It's call XORcist, so it probably uses XOR encoding for the password.

```
~/Projects/Reverse_Engineering/crackmes.one/XORcist
❯ checksec file xorcist

  _____ _    _ ______ _____ _  __ _____ ______ _____
 / ____| |  | |  ____/ ____| |/ // ____|  ____/ ____|
| |    | |__| | |__ | |    | ' /| (___ | |__ | |
| |    |  __  |  __|| |    |  <  \___ \|  __|| |
| |____| |  | | |___| |____| . \ ____) | |___| |____
 \_____|_|  |_|______\_____|_|\_\_____/|______\_____|

RELRO           Stack Canary      CFI               NX            PIE             RPATH      RUNPATH      Symbols         FORTIFY    Fortified   Fortifiable      Name                            
Partial RELRO   Canary Found      NO SHSTK & NO IBT NX enabled    PIE Enabled     No RPATH   No RUNPATH   No Symbols      No         0           2                xorcist                         

~/Projects/Reverse_Engineering/crackmes.one/XORcist
❯ file xorcist 
xorcist: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=4e8fe8f22756696045e4960d6ab793299b555298, for GNU/Linux 4.4.0, stripped
```
