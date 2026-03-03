```
First things first;
~/Projects/Reverse_Engineering/jevido_bins
❯ checksec file sii

  _____ _    _ ______ _____ _  __ _____ ______ _____
 / ____| |  | |  ____/ ____| |/ // ____|  ____/ ____|
| |    | |__| | |__ | |    | ' /| (___ | |__ | |
| |    |  __  |  __|| |    |  <  \___ \|  __|| |
| |____| |  | | |___| |____| . \ ____) | |___| |____
 \_____|_|  |_|______\_____|_|\_\_____/|______\_____|

RELRO           Stack Canary      CFI               NX            PIE             RPATH      RUNPATH      Symbols         FORTIFY    Fortified   Fortifiable      Name                            
No RELRO        Canary Found      Unknown           NX enabled    PIE Disabled    No RPATH   No RUNPATH   3552 symbols    N/A         0           0                sii                             

~/Projects/Reverse_Engineering/jevido_bins
❯ file sii
sii: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, with debug_info, not stripped
```
Nothing out of the ordinary so far, apart from being purely 64, rather then x86_64.
One of the first things showing up in Ghidra is `fs.File.stdin`
<img width="390" height="35" alt="image" src="https://github.com/user-attachments/assets/fea062d9-9db8-40ab-a7a2-da2bde2eea5b" />
After looking this up it seems to belong to Zig, which upon looking up appears to be a general-purpose programming language attemting to impove on C. This seems to make sense so far. This little piece of code Ghidra decompiles with a for loop in front of it.
<img width="268" height="90" alt="image" src="https://github.com/user-attachments/assets/115bc4e8-0f36-40a5-b7f6-4558f3c02877" />
Since it's 4kb in bytes, I'll assume for now this is likely a buffer. Maybe even added by the compiler and not actually user code.
This code patterns seems to continue and flow in pseudo code:
```
fs.File.stdin
fs.File.reader
fs.File.stdout
fs.File.writer
```
I am not super familiar with Zig, but it seems just like a very standard program that takes a user input and prints it back out.
```
~/Projects/Reverse_Engineering/jevido_bins 8s
❯ ./sii
What's your name? Edoardo
Wassup Edoardo!
```
And there it is. I didn't find the extra text, so let's search for that in Ghidra.
```
                             __anon_22466 (0119c1ec+1)
        0119c1ec 09 57 68        ds         "\tWhat's your name? "
                 61 74 27 
                 73 20 79 

                             __anon_22492
        0119c235 57 61 73        ds         "Wassup {s}!\n"
                 73 75 70 
                 20 7b 73 
```
it's honestly not completely clear to me how and where this text then get's called in the user code to be printed out, but here they are. This seems to be all there is, short but sweet, nice to see a less popular language!
