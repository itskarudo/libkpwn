# LibKPWN - pwn the kernel

LibKPWN is an exploit development library for C, it is highly focused on kernel exploitation where C is most often the only option you have, since you're locked in an isolated minimal environemnt (qemu with busybox most often), it gives you a bunch of useful types, methods and objects that you would miss when pwntools isn't an option.

## Example usage

```c
#include <kpwn.h>
int main(void) {

    IO* io = io_new("/dev/ctf");

    io_write(io, b_mul(b("A"), 0x20));
    io_read(io, 0x20);
    uint64_t stack_leak = u64(io_read(io, 8));

    log_success("Leaked a stack address!\n");
    log_info("stack @ %p\n", stack_leak);

}
```

## Philosophy

When I'm writing an exploit, I want the language I'm using to not get in the way, I want it to actually help me get to the point as fast as possible.

That is definitely true with python, especially when combined with something like pwntools. but it absolutely is not the case when writing kernel exploits with C, I find myself often thinking about memory management and how big to make my buffers instead of actually writing the damn exploit, not to mention the struggles and hoops you have to go through in order to do something as basic as repeating an array of bytes or base64 encoding.

This is the sole reason behind this project, I want to make a library that can let me think about actually writing the exploit, that handles all the memory behind the scenes and gives me as high of an abstraction as C can provide so I can jump off a KPTI trampoline instead of a building.

## Memory management?

LibKPWN utilizes the [Boehm garbage collector](https://www.hboehm.info/gc/) behind the scenes, meaning there is no need to think about `free`'ing memory or be afraid that your program is allocating way too much.

If you wish to allocate a garbage-collected chunk of memory, you can do so by using the `GC_MALLOC` or `GC_MALLOC_ATOMIC` provided by the `gc/gc.h` header file.
