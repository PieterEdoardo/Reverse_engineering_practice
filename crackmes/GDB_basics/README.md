# GDB basics
All credit for this binary and challenge go to Varsovie https://crackmes.one/crackme/645d3d4e33c5d43938913079

# Recon
The title of this challenge has `gdb`, so to preserve the intention of the creator I'll try solve this primarily using `gdb`.

```
~/Projects/RE/crackmes.one/GDB_basics
❯ file a.out 
a.out: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=07adab7654c353add9669995f1587e01439c24db, for GNU/Linux 3.2.0, not stripped

~/Projects/RE/crackmes.one/GDB_basics
❯ checksec file a.out

  _____ _    _ ______ _____ _  __ _____ ______ _____
 / ____| |  | |  ____/ ____| |/ // ____|  ____/ ____|
| |    | |__| | |__ | |    | ' /| (___ | |__ | |
| |    |  __  |  __|| |    |  <  \___ \|  __|| |
| |____| |  | | |___| |____| . \ ____) | |___| |____
 \_____|_|  |_|______\_____|_|\_\_____/|______\_____|

RELRO           Stack Canary      CFI               NX            PIE             RPATH      RUNPATH      Symbols         FORTIFY    Fortified   Fortifiable      Name                            
Partial RELRO   No Canary Found   Unknown           NX enabled    PIE Enabled     No RPATH   No RUNPATH   39 symbols      No         0           1                a.out                           

~/Projects/RE/crackmes.one/GDB_basics
❯ ./a.out 
Enter a number : 1337
I cannot provide you a flag !

~/Projects/RE/crackmes.one/GDB_basics
❯ strings a.out
/lib64/ld-linux-x86-64.so.2
__isoc99_scanf
puts
printf
__cxa_finalize
__libc_start_main
...
[]A\A]A^A_
Enter a number : 
There is the flag : I_LOVE_YOU
I cannot provide you a flag !
;*3$"
...
```
I cut out most of the `strings` output, but it seems that once the correct number(s) are given, it puts out `I_LOVE_YOU`.

# Debugging
As we've established during recon, this binary isn't stripped, so we know what main is. Look at it, it's rather small.

<img width="803" height="535" alt="image" src="https://github.com/user-attachments/assets/0ec3dd56-eb2c-4320-9313-9fd933e9b4db" />

I read a bunch of stuff on `gdb` and I learned some basic commands. After bringing up the assembly with `layout asm`, I started off by setting a breakpoint on main and ran the program to get the live addresses with `break main` and `run` respectively. Looking at the logic it goes through acouple steps:
1. Declaring a variable on the stack.
2. Iterating through a for loop and it multiplies a value each cycle.
3. Calling `printf` print a message.
4. Calling `__isoc99_scanf` to take user input.
5. `CMP` user input with stack value.
6. Printing message based on whether user input was correct.

Still looking at the screenshot from above, the `CMP` at `0x5555555551ad` seem to be where the secret value is compared. But, this value, `rbp-0x4`. This naming is based on the negative location on the stack from rbp, so the they have negative values, like `-0x4` or `-0xc`. So if `0x4` is compared to user input, it'll should be as easy as finding where it get's declared and read it straight from the register there. It get's declared at the top of the script, and then only changed during the for loop.
```
MOV        EAX, dword ptr [rbp-0x4]
IMUL       EAX, dword ptr [rbp-0x8]
MOV        dword ptr [rbp-0x4], EAX
ADD        dword ptr [rbp-0x8], 0x1
```
First `0x4`'s value is loaded into the accumulator register, then it's multiplied by `0x8`, then the register's value is stored back on the stack at `rbp-0x4`. Lastly it increments `rbp-0x8` by 1 before looping the whole thing. What this means, is that the script generates a high value as the secret number on the spot and it's not hardcoded stored anywhere. It *must* be readed out from the memory.

I want to try and set another breakpoint right after the loop to see what the end result is. First, though, let's also follow the value of `EAX`, this is possible in `gdb` with the command `display $eax`. After every step, it'll print what its current value is. Setting the breakpoint can be done, in this case, by typing the command `break *0x555555555181`. Now it'll set a breakpoint at this specific command. Normally the command `ni` will go to the next instruction, but the start of the script has a loop, so it'll be stuck in there for a while. So let's use the `next` command and go striaght to the second breakpoint. 

<img width="896" height="661" alt="image" src="https://github.com/user-attachments/assets/546021d7-ccc7-48ba-b116-4949ea84f868" />

BAM! It prints the value `219283456` right away.
```
~/Projects/RE/crackmes.one/GDB_basics
❯ ./a.out
Enter a number : 219283456
There is the flag : I_LOVE_YOU
```

# Conclusion
This binary was solved by setting a breakpoint at key points in the programs logic with a life debugger to read out the secret value that was calculated in runtime.

I loved this binary, thanks again to Varsovie for creating and uploading this one. It's the only one they have on the site at this moment. It forced me to learn something new, and there were no easy ways out. This challenge shows off a new skill, and learns you why this is usefull right away.

