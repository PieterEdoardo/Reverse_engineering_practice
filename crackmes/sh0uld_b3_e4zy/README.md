# sh0uld_b3_e4zy write up
This binary ws provided on crackmes.one by cycrusader https://crackmes.one/crackme/65c795a3eef082e477ff6a7e

## Context
This binary was the second lowest rated challenge on the entire website, and this is in part because it just doesn't work. It doesn't even validate input. The comments also highlight this issue. But, since it has 0 write ups as of the time of writing, and, because the description of the binary was "Its easy to patch ;) try to reverse it!", I decided to patch it rather then solving it.

# Bugs
Opening the code in Ghidra, it's clear straight away that there is a bunch of dead code.
```asm
00101190 c7 45 d0        MOV        dword ptr [RBP + local_38],0x61
         61 00 00 00
00101197 c7 45 d4        MOV        dword ptr [RBP + local_34],0x65
         65 00 00 00
0010119e c7 45 d8        MOV        dword ptr [RBP + local_30],0x69
         69 00 00 00
001011a5 c7 45 dc        MOV        dword ptr [RBP + local_2c],0x6f
         6f 00 00 00
001011ac c7 45 e0        MOV        dword ptr [RBP + local_28],0x75
         75 00 00 00
```
These 5 values get pushed on the stack, but they never actually get called after that, so the decompiler doesn't even use these. Ghidra gives me the following.
```c
undefined8 main(void)

{
  int iVar1;
  size_t sVar2;
  long in_FS_OFFSET;
  char local_15 [5];
  long local_10;
  
  local_10 = *(long *)(in_FS_OFFSET + 0x28);
  puts("MAX INPT 5 CHARS");
  printf("Enter Input: ");
  iVar1 = __isoc99_scanf(&DAT_00102023,local_15);
  if (iVar1 == 1) {
    sVar2 = strlen(local_15);
    if (5 < (int)sVar2) {
      puts("INVALID INPUT!");
    }
  }
  else {
    puts("INVALID INPUT!");
  }
  if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
    return 0;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}
```

Not only does the password not get retrieved from the stack, there is also no path to actually get the input correct. The only checks are if input succeeded, and if the input is long enough. On top of that if input is too long it causes a fat error.
```
~/Projects/RE/crackmes.one/sh0uld_b3_e4zy
❯ ./e4zy
MAX INPT 5 CHARS
Enter Input: 34354677
INVALID INPUT!
*** stack smashing detected ***: terminated
fish: Job 1, './e4zy' terminated by signal SIGABRT (Abort)
```
So these are the things that need fixing.
1. Actually retreiving and comparing the 'password'
2. Creating correct success and failure paths
3. Better input validation.

# Proposed solution



# Conclusion
