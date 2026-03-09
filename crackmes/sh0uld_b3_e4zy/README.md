# sh0uld_b3_e4zy write up
This binary ws provided on crackmes.one by cycrusader https://crackmes.one/crackme/65c795a3eef082e477ff6a7e

## Context
This binary was the second lowest rated challenge on the entire website, and this is in part because it just doesn't work. It doesn't even validate input. The comments also highlight this issue. But, since it has 0 write ups as of the time of writing, and, because the description of the binary was "Its easy to patch ;) try to reverse it!", I decided to patch it rather then solving it.

# Bugs
Opening the code in Ghidra, it's clear straight away that there is a bunch of dead code 
```
0010118a 48 89 45 f8     MOV        qword ptr [RBP + local_10],RAX
0010118e 31 c0           XOR        EAX,EAX
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

# Proposed solution

# Conclusion
