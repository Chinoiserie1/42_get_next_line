*This project has been created as part of the 42 curriculum by jlucotte.*

# get_next_line

## Description

`get_next_line` is a C function that returns **one line at a time** read from a
file descriptor:

```c
char	*get_next_line(int fd);
```

Each call returns the next line of the file, stdin, pipe or anything else `fd`
points to. The returned line keeps its terminating `\n`, except for the last line
of a file that does not end with one. When there is nothing left to read, or an
error happens, the function returns `NULL`.

The goal of the project is to learn about:

- **static variables**: data that must survive from one call to the next
  (the bytes read after a `\n` belong to the next line);
- the low-level `read()` system call and its return values;
- careful dynamic memory management with `malloc` / `free` (no leaks);
- preprocessor macros passed at compile time (`-D BUFFER_SIZE=n`).

Constraints from the subject:

| Allowed            | Forbidden                          |
|--------------------|------------------------------------|
| `read`, `malloc`, `free` | libft, `lseek()`, global variables |

The function must read as little as possible on each call. It must never read the
whole file and then split it into lines.

**Bonus:** use a single static variable and handle several file descriptors at
the same time (for example fd 3, then 4, then 3 again) without mixing their
reading state. Bonus files are suffixed with `_bonus`.

### Files

| File                          | Content                                |
|-------------------------------|----------------------------------------|
| `get_next_line.c`             | the `get_next_line()` function         |
| `get_next_line_utils.c`       | helper functions                       |
| `get_next_line.h`             | prototype, includes, `BUFFER_SIZE` default |
| `get_next_line_bonus.c/.h`, `get_next_line_utils_bonus.c` | bonus version (multiple fds) |

## Instructions

### Compilation

The project is meant to be compiled together with your own `main`. The buffer
size used by `read()` is chosen at compile time with `-D BUFFER_SIZE=n`:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

`-D BUFFER_SIZE=42` works like writing `#define BUFFER_SIZE 42` at the top of
each compiled file. The project also compiles **without** this flag: the header
then provides a default value with `#ifndef BUFFER_SIZE`.

Bonus version:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c
```

To check the value the preprocessor substitutes:

```sh
cc -E -D BUFFER_SIZE=7 get_next_line.c | tail
```

### Usage example

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	fd = 0;
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	return (0);
}
```

```sh
./a.out file.txt          # read a file
./a.out < file.txt        # read from standard input
cat file.txt | ./a.out    # read from a pipe
```

### Testing

Test with very different buffer sizes and line sizes:

```sh
for n in 1 2 42 9999 10000000; do
	cc -Wall -Wextra -Werror -D BUFFER_SIZE=$n get_next_line.c get_next_line_utils.c main.c
	./a.out file.txt
done
```

Edge cases worth testing: empty file, file without a final `\n`, file made only of
`\n`, a very long line, an invalid fd (`-1`, a closed fd), stdin, and (bonus)
alternating between several fds.

Check memory with:

```sh
cc -g -fsanitize=address -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
leaks --atExit -- ./a.out file.txt     # macOS
valgrind --leak-check=full ./a.out file.txt   # Linux
```

## Technical choices

### Maximum size of one `read()` call

```c
ssize_t	read(int fd, void *buf, size_t nbyte);
```

- The return type is `ssize_t` (signed) so that it can return `-1` on error.
  POSIX leaves the result implementation-defined when `nbyte > SSIZE_MAX`.
- **macOS (XNU kernel):** `read()` fails with `EINVAL` when
  `nbyte > INT_MAX` (2 147 483 647 bytes). Tested: `INT_MAX` is accepted,
  `INT_MAX + 1` returns `-1` / `Invalid argument`.
- **Linux:** `read()` transfers at most `0x7ffff000` (2 147 479 552) bytes,
  which is `INT_MAX` rounded down to a page size (see NOTES in `man 2 read`).
- Both kernels cap the size near `INT_MAX` because a lot of kernel code
  counts bytes with 32-bit `int`s.
- `read()` may always return **fewer** bytes than requested (pipes,
  terminals, end of file). This is not an error.

### Stack size limit

A buffer declared as a local array lives on the **stack**, and the stack has a
limited size. `ulimit -s` shows it in **kilobytes (KiB)**:

| System | `ulimit -s` default | In bytes |
|--------|---------------------|----------|
| macOS  | 8176 KiB (8 MiB minus a 16 KiB guard area) | 8 372 224 |
| Linux  | 8192 KiB (8 MiB)    | 8 388 608 |

A `BUFFER_SIZE` close to or above this value (for example `10000000`) cannot be
stored on the stack: the program crashes (stack overflow) and there is no
return value to check.

Computing how much stack is left to bound the buffer is **not reliable**:

- `ulimit -s` is a **soft** limit. It can be changed (`ulimit -s <n>`) and
  differs between machines.
- The stack already holds the caller's frames (`main`, the evaluator's test
  program), whose size is unknown.
- Frame sizes depend on the compiler and its flags (`-O0`, `-O2`,
  `-fsanitize=address`).
- Bounding the buffer below `BUFFER_SIZE` would also mean not reading with the
  size requested by the evaluator.

So the rule followed here is: **a large buffer, or one whose size is chosen by
someone else, does not go on the stack.**

### Where a buffer lives

| Declaration | Memory area | Notes |
|-------------|-------------|-------|
| `char buf[BUFFER_SIZE];` inside a function | **stack** | crashes if `BUFFER_SIZE` is large |
| `char buf[n];` with `n` a variable (VLA) | **stack** | same problem, and forbidden by the Norm |
| `static char buf[BUFFER_SIZE];` | **static area** (`.bss`) | not on the stack, size fixed at compile time, reserved for the whole program |
| `char *buf = malloc(BUFFER_SIZE + 1);` | **heap** | limited by available memory; `malloc` returns `NULL` on failure, must be `free`d |

Stack and heap are separate areas: filling the stack does not prevent `malloc`
from working, but any further function call would overflow the stack.

| | Stack | Heap |
|---|---|---|
| How | local variable | `malloc` / `free` |
| Limit | ~8 MiB (`ulimit -s`) | available RAM / virtual memory |
| Freed | automatically when the function returns | manually with `free` |
| Too large | immediate crash | `malloc` returns `NULL`, can be handled |

Other values of `BUFFER_SIZE` that make no sense wherever the buffer lives:
`BUFFER_SIZE <= 0`, or a value above what `read()` accepts (see above).

The buffer used by `read()` is allocated on the **heap** (see
[Algorithm](#algorithm)).

### Line separator: the `LINE_END` macro

The character that ends a line is defined once, in `get_next_line.h`:

```c
# define LINE_END '\n'
```

The code uses `LINE_END` everywhere instead of writing `'\n'`.

**Why:** if lines ever have to end on another character, only this one line in
the header has to change, instead of every `'\n'` in the code. This avoids
forgetting one occurrence, which would make the program cut lines on two
different characters.

The name says what the character means for the algorithm: the end of a
**line**. The end of the **file** is a different thing: it is detected when
`read()` returns `0`.

## Algorithm

### Buffer memory: heap

The buffer passed to `read()` is allocated on the **heap** with `malloc`.

**Why:** the heap does not have the ~8 MiB limit of the stack (`ulimit -s`). A
large `BUFFER_SIZE` such as `10000000` therefore works, and if the allocation
fails, `malloc` returns `NULL`: the error is detected and `get_next_line`
returns `NULL` instead of crashing.

### Static variable: a pointer to what was already read

`read()` returns `BUFFER_SIZE` bytes, not a line: the bytes read after a `\n`
belong to the next line and must survive until the next call. A normal local
variable is lost when the function returns, so this state is kept in a
**`static` pointer** (`static char *`), which points to the already-read data
stored on the **heap**.

**Why a static pointer instead of a static array** (`static char buf[BUFFER_SIZE + 1]`),
which is not on the stack either:

- only the pointer (8 bytes) is static; the data is allocated with the size
  actually needed, instead of an array reserved for the whole program;
- with a huge `BUFFER_SIZE`, a failed `malloc` returns `NULL` and can be
  handled, while a huge static array is reserved when the program starts;
- the memory can be released with `free` once the file is fully read.

**Consequence:** the data pointed to by the static pointer must be freed at the
end of the file and on every error path, to avoid memory leaks.

### Two variables, two roles

| Variable | Role | Lifetime |
|----------|------|----------|
| read buffer (`malloc(BUFFER_SIZE + 1)`) | receives the bytes of **one** `read()` call | one call of `get_next_line` |
| static pointer (the "stash") | everything already read from `fd` but **not yet returned** | between calls |

`read()` itself does not use the static variable. The static variable is needed
because bytes read after a `\n` have left the file descriptor: `read()` will
never return them again, so they must be kept until the next call.

`BUFFER_SIZE` limits **one** `read()` call, not one line. A single call of
`get_next_line` can call `read()` as many times as needed.

### One call, step by step

1. **Check the arguments.** If `fd < 0` or `BUFFER_SIZE <= 0` (or too large for
   `read()`), return `NULL`.
2. **Look in the stash first.** If it already contains a `\n`, a full line is
   available: go to step 5 without calling `read()`.
3. **Reading loop.** Allocate the read buffer, then repeat:
   - call `read(fd, buffer, BUFFER_SIZE)` and keep its return value in a
     `ssize_t`;
   - `-1` → error: free the buffer and the stash, set the stash to `NULL`,
     return `NULL`;
   - `0` → end of file: stop the loop;
   - `> 0` → put `\0` right after the bytes actually read, then **append** the
     buffer to the end of the stash;
   - stop as soon as the stash contains a `\n`.

   The buffer is freed when the loop ends.
4. **End of file with an empty stash.** If the stash is `NULL` or empty, there is
   nothing left to return: free it, set it to `NULL`, return `NULL`.
5. **Cut the stash in two.**
   - the **line**: from the start of the stash up to and including the first
     `\n`, or the whole stash if there is no `\n` (last line of a file that does
     not end with `\n`). It is copied into a new allocation, which the caller
     frees;
   - the **rest**: everything after the `\n`. It becomes the new stash, in a new
     allocation, and the old stash is freed. If nothing remains, the stash is
     freed and set to `NULL`.
6. **Return the line.**

Example with `BUFFER_SIZE = 4` and the file `bonjour\nsalut`:

| Call | `read()` returns | Stash after reading | Returned | Stash kept |
|------|------------------|---------------------|----------|------------|
| 1 | `bonj`, `our\n` | `bonjour\n` | `bonjour\n` | `NULL` |
| 2 | `salu`, `t`, `0` | `salut` | `salut` | `NULL` |
| 3 | `0` | `NULL` | `NULL` | `NULL` |

### Growing the stash

A line can be very long, and its length is unknown in advance, so the stash can
not be a fixed-size array. `realloc` is not allowed, so each append:

1. allocates a new block of `stash length + bytes read + 1`;
2. copies the old stash, then the buffer, into it;
3. frees the old stash;
4. makes the static pointer point to the new block.

The first time, the stash is `NULL` and is treated as an empty string.

### Memory rules

- **`free` then `NULL`, always together.** Setting the stash to `NULL` without
  `free` leaks the memory. Calling `free` without setting it to `NULL` leaves a
  pointer to freed memory, which the next call would use (crash or double free).
- Every error path (`read()` returns `-1`, any `malloc` fails) frees the buffer
  and the stash, and returns `NULL`.
- At the end of a file, the stash is freed, so nothing is left allocated once
  `get_next_line` has returned `NULL`.

### Why it works for any `BUFFER_SIZE`

- The read buffer and the stash are both on the **heap**, so a large
  `BUFFER_SIZE` such as `10000000` does not overflow the stack, and a failed
  `malloc` is handled (see [Buffer memory: heap](#buffer-memory-heap)).
- With `BUFFER_SIZE = 1`, the loop simply runs once per character.
- With a `BUFFER_SIZE` larger than the line, the loop runs once and the extra
  bytes stay in the stash.

### Trade-offs

- **Read as little as possible:** the loop stops at the first `\n`, and step 2
  avoids calling `read()` at all when the stash already holds a line. The whole
  file is never read at once.
- **Cost of appending:** each append copies the whole stash. With a very small
  `BUFFER_SIZE` and a very long line, this is slow (the copy work grows with the
  square of the line length). A larger `BUFFER_SIZE` reduces the number of
  copies. This was kept because it is simple and only uses `read`, `malloc` and
  `free`.
- **One stash for one `fd`:** the mandatory part keeps a single static pointer,
  so reading several file descriptors in turn mixes their data. The bonus part
  addresses this.

### Bonus: one stash per file descriptor

The bonus keeps **one stash per file descriptor**, still with a single static
variable: an **array of pointers**, indexed by the file descriptor.

```c
static char	*storage[MAX_FD];
```

- `storage[fd]` is the stash of `fd`. The rest of the algorithm is unchanged:
  it works on `storage[fd]` instead of a single stash.
- A static array starts filled with `NULL`, so every file descriptor starts
  with an empty stash.
- An array counts as **one** static variable, as required by the subject.
- Only the pointers are reserved (`MAX_FD` × 8 bytes). The stashes themselves
  are still allocated on the heap, only for the file descriptors that are read,
  and freed at the end of each file.

**Why an array instead of a linked list:**

- **The Norm and the required files.** The subject allows only three bonus
  files, and the Norm allows at most five functions per file. A linked list
  needs extra functions (find a node, create it, remove it and reconnect the
  list, free it). The array needs none: the mandatory functions are reused
  almost as they are.
- **Direct access.** `storage[fd]` gives the stash immediately. A linked list
  has to be walked through until the node of `fd` is found.

**Drawback: a maximum file descriptor.** The array has a fixed size, `MAX_FD`,
so a file descriptor greater than or equal to `MAX_FD` has no place in it.
Using `storage[fd]` with such a value would write outside the array, which is
**undefined behaviour**. To avoid this, `get_next_line` checks the value first:
if `fd < 0` or `fd >= MAX_FD`, it returns `NULL`. Such a file descriptor can
therefore not be read, which only happens when a program opens more files than
`MAX_FD` at the same time. The number of files a process may open depends on
the system (see `ulimit -n`). A linked list would not have this limit.

## Resources

### Compiler and preprocessor (`-D BUFFER_SIZE`)

- [GCC – Preprocessor Options (`-D name=definition`)](https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html)
- [Clang – Command Line Reference](https://clang.llvm.org/docs/ClangCommandLineReference.html)
- [The GNU C Preprocessor manual](https://gcc.gnu.org/onlinedocs/cpp/)
- [cppreference – Conditional inclusion (`#ifdef`, `#ifndef`)](https://en.cppreference.com/w/c/preprocessor/conditional)
- [cppreference – Macro replacement (`#define`)](https://en.cppreference.com/w/c/preprocessor/replace)
- `man cc`

### C language

- [cppreference – Storage duration (`static`)](https://en.cppreference.com/w/c/language/storage_duration)
- [cppreference – Dynamic memory management](https://en.cppreference.com/w/c/memory)

### System calls

- `man 2 read`: return values, end of file, errors
- `man 2 open`, `man 2 close`
- `man 3 malloc`, `man 3 free`
- [Linux man page – read(2)](https://man7.org/linux/man-pages/man2/read.2.html)
- [Linux man page – open(2)](https://man7.org/linux/man-pages/man2/open.2.html)
- [POSIX – read()](https://pubs.opengroup.org/onlinepubs/9799919799/functions/read.html):
  behaviour when `nbyte > SSIZE_MAX`
- [POSIX – limits.h](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/limits.h.html):
  `SSIZE_MAX`, `INT_MAX`
- [Linux man page – getrlimit(2)](https://man7.org/linux/man-pages/man2/getrlimit.2.html):
  `RLIMIT_STACK`, the limit shown by `ulimit -s`
- `man limits.h`, `man ulimit` / `help ulimit` (bash)
- [Wikipedia – File descriptor](https://en.wikipedia.org/wiki/File_descriptor)

### Debugging and memory checking

- [Clang AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [Valgrind Quick Start Guide](https://valgrind.org/docs/manual/quick-start.html)
- `man leaks` (macOS)

### Use of AI

AI (Claude, by Anthropic) was used for:

- explaining the subject and the meaning of the `-D BUFFER_SIZE` compiler option;
- explaining the maximum size of a `read()` call and the default stack size
  limits on macOS and Linux;
- gathering the documentation links listed above;
- drafting the structure of this README;
- reviewing my first versions of the code and pointing out bugs (end of file
  not detected, out-of-bounds `\0`, `size_t` instead of `ssize_t`, leaks);
- asking questions about my algorithm to check edge cases, and comparing it
  with the tests of [gnlTester](https://github.com/Tripouille/gnlTester);
- writing up the [Algorithm](#algorithm) section in English from my
  explanation.

The algorithm is my own design, and AI was **not** used to write the
`get_next_line` code.
