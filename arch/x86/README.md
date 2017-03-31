# HermitCore x86 platform code

This folder contains all x86 specific code.


## Modifying kernel arguments

The kernel arguments, i.e. variables defined in `kernel/entry.asm` are generated
from the description file `kernel_arguments.def`. The definition is C-like but
still not the same since it is only parsed by `generate_kernel_arguments.py`.

It will generate an ASM file that has all the definitions and will be included
by `entry.asm` and a C header that declares the struct `kernel_arguments` which
can be included via `#include <hermit/kernel_arguments.h>` throughout the whole
code base.

A typical definition in `kernel_arguments.def` may look like this:

```
// TYPE NAME = INITIAL_VALUE [// OPTIONAL_COMMENT]
uint32_t cpu_freq = 0	// CPU frequency
```

One-line comments starting with `//` and empty lines are allowed. A comment at
the end of definition will be treated as its description and included in the
generated header file for documentation.

The allowed types can be found and extended at the top of
`generate_kernel_arguments.py`. You have to supply the C type, it's size in
bytes and the statement to declare such a variable in NASM.

It may look like this:

```python
type_translation = {
#   C type              (ASM type, size)
    'uint32_t':             ('dd', 4),
    'uint64_t':             ('dq', 8)
}                
```
