# XORcist writeup
Thanks to S3c_Cult for publishing this binary https://crackmes.one/crackme/684f47bd2b84be7ea774390e

## Standard checks
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

## Ghidra analysis
It's stripped so first goal is to identify main and any other important functions. Main is likely just the first function being called before __libc_start_main, like usual, and it does indeed look very main-y.
Inside here I can see the code that told me the password was wrong right away:
```
if ((x == 0) || (y % 100 == -1)) {
  puts("Nuh Uh Nuh Uh.");
}
```
All symbols are stripped, so I'll have to rename everything that looks meaningful as I go backwards from here.
Var Y is a random number, so the second condition looks very weird. The X value is like Y also declared with rand(), but it gets overwriten as `x = (*a)(b);` and then of course get's compared to 0; So I'll rename it again to `true_if_valid`. This seems to be the only part of the actual if statement input might control, so if my suspicion is correct, I need to find where stdin get's used, and where the XOR encoding is done, if any.

Going back up again, user input is likely here
```
puts("What\'s the password?");
fgets(b,0x20,stdin);
```
So I'll rename b to `user_input` for now. The a variable comes from a stripped function that only really does a strcpy(), call another stripped function, and call strcmp(). Now this looks juicy! So let's go ahead with the assumtion a is 'password related' and call it `password`.
This means our if statement and logic look like this now:
```
true_if_valid = (*password)(user_input);
if ((true_if_valid == 0) || (rand_value % 100 == -1)) {
  puts("Nuh Uh Nuh Uh.");
}
```
Looks like that would make sense, now let's keep going deeper into that strcmp function and see what that stripped function inside of it does. It really only has a for loop, so I'll go right ahead and rename the only local variable to `i`.
```
undefined4 i;

for (i = 0; *(char *)(param_1 + i) != '\0'; i = i + 1) {
  *(byte *)(param_1 + i) = *(byte *)(param_1 + i) ^ 0xa9;
}
return;
```
I am not very familiar yet with XOR encoding practices but this looks very promising, and I suspect this might be it. After reading more about it online we need to find an 'encryption key', Which the only thing here that looks like that is `0xA9`. So let's remember that for now. Something here must get 'XORred' against `0xA9` for all of these assumtions about the logic to make sense. So let's go back up and see what `param_1` is.

## Chasing the password
Chasing `param_1` back up into our parent function, it looks like the following:
```
strcpy(local_28,&DAT_00102023);
XOR_encoding(local_28);
iVar1 = strcmp(param_1,local_28);
```
If this is not a rabbithole then This raw data `DAT_00102023` must lead us to something usefull.
```
                             DAT_00102023                                    XREF[4]:     string_cmp:00101364(*), 
                                                                                          string_cmp:0010136b(*), 
                                                                                          string_cmp:0010136f(*), 
                                                                                          string_cmp:00101377(*)  
        00102023 dd              ??         DDh
        00102024 da              ??         DAh
        00102025 f6              ??         F6h
        00102026 d9              ??         D9h
        00102027 c4              ??         C4h
        00102028 c6              ??         C6h
        00102029 f6              ??         F6h
        0010202a ce              ??         CEh
        0010202b c7              ??         C7h
        0010202c ce              ??         CEh
        0010202d f6              ??         F6h
        0010202e c0              ??         C0h
        0010202f ca              ??         CAh
        00102030 c5              ??         C5h
        00102031 00              ??         00h
```
So Ghidra doesn't really seem to know what to do with this, and I am tempted to go back up, because the code does also do a bunch of of more stuff, but when we went back from the final if statement, we didn't actually encounter any of it. So let's persue this. I need to see if unXORring `DD DA F6 D9 C4 C6 F6 CE C7 CE F6 C0 CA C5 00` against `0xA9` leads to anything that looks like a password. 

## Decoding the hex values
I need to see if the first byte `0xDD` is usefull. So, representing `0xDD` as binary, would be easy as D is only 2 steps back from 16 (I know, I do math weird), so that would be `1101` and `1101` for a binary value of `11011101` to represent `0xDD`. The next one is also easy because A is obviously `1010`, and 9 is just one step down from that; `1001`.

So with the binary value for `0xDD` being `11011101`, and `0xA9` being `10101001`, I can easily reverse the operation and get the password(Hopefully!). I've read up online and XOR is eXclusinve OR, so it's not just true if either value is true, but exclusively if only one of the is true, and only one. So `1 XOR 1 = 0`, where `1 OR 1 = 1`. But, `1 XOR 0 = 1`. Refreshing up on my logic gates, I can now chart the calculation out in a table:
| Bit Position | A | B | A XOR B |
|--------------|---|---|---------|
|      1       | 1 | 1 |    0    |
|      2       | 1 | 0 |    1    |
|      3       | 0 | 1 |    1    |
|      4       | 1 | 0 |    1    |
|      5       | 1 | 1 |    0    |
|      6       | 1 | 0 |    1    |
|      7       | 0 | 0 |    0    |
|      8       | 1 | 1 |    0    |

Our resulting binary is `01110100`, which looks promising, because single byte Unicode supported ASCII characters always start with a 0 to be backwards compatible, as back in the day ASCII was only 7 bits. Let's look it up! I like to use 
Cyberchef https://cyberchef.org/ for my encoding/decoding needs. and it recognizes the byte right away as the letter 't'. Now I don't want to do every byte by hand so looking online for a tool to automate the process. Using the tool from dcode https://www.dcode.fr/xor-cipher it results the raw data as `ts_pmo_gng_icl`. This doesn't sound like a password, but all the other signs point to that it should be so let's give it a go.

## Trying the decoded password
```
~/Projects/Reverse_Engineering/crackmes.one/XORcist
❯ ./xorcist
What's the password?
ts_pmo_gng_icl
Logged in as root.
```
Great! That was the password! 

## Conclusion
This binary took a standard input and compared it to a XOR encoded password that was hidden in the binary as raw data. To solve it, it was required to deconstruct the logical steps and perform the XOR operation in reverse.

Amazing bin, thanks again to S3c_cult for putting this together, was very fun. 
