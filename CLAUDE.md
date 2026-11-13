# Coding style

Write C-style C++. The project uses C++ for its quality-of-life features, not its paradigm. Many C++ features add cognitive overhead and do things behind your back, so code should stay plain and explicit: a reader should be able to see what it does from what is written.

## C++ features

Fine to use:
- Namespaces
- Function overloading
- The C++ standard library, when reinventing the wheel isn't worth it

Avoid everything else by default, especially features that run or decide things implicitly, such as:
- Constructors, destructors, and RAII
- Inheritance and virtual functions
- Operator overloading and implicit conversions
- Exceptions
- Heavy template use

If a C++ feature seems worth using and isn't listed as fine, ask first.

## Contain C++ usage

Keep C++ features from spreading. Using one in one place must not force the rest of the project to use C++ features too.
- Put C++ usage, including standard library types, inside the `.cpp` file that needs it, behind a C-like interface
- Headers expose plain structs and functions, not standard library types

## BASS

The library plays audio with BASS and BASS_FX (`externals/BASS/`). It is built for BASS only; there is no backend abstraction layer.
- Only `.cpp` files include `<bass/bass.h>` and `<bass/bass_fx.h>`. Public headers expose no BASS types or handles, so programs using the library never see BASS
- `externals/BASS/bass.chm` and `bass_fx.chm` are the BASS documentation
- The libraries are import libraries: a program needs `bass.dll` and `bass_fx.dll` (from `externals/BASS/lib/x64` or `x86`) next to its executable
- Windows x64 is the main target; x86 should keep building

## Project layout

Paths are relative to the project folder, `O2Jam/`:
- Header files go in `include/o2/`
- Implementation files go in `src/o2/`
- `include/` is on the include path, so include headers as `#include <o2/name.h>`
- Add new `.cpp` files to `O2Jam.vcxproj` so they get compiled
- `main.cpp` is a demo of the library; other projects use only `include/` and `src/`

The `O2Jam-Common`, `O2Jam-Bass` and `O2Jam-Test` projects are an older version of this library. Use them as a reference for how BASS was used, but don't change them or copy their style. They will be removed.

## Formatting

- When a function signature gets long, put one parameter per line, in the declaration and the definition:

```cpp
void setAttributes(
    Stream* stream,
    float volume,
    float pan
);
```

## Namespaces

Everything in these headers and their implementations goes in the `o2` namespace, so the code can be reused in later projects without name conflicts. Nested namespaces are fine, as long as they start with `o2`, such as `o2::jam`.

## Memory

- Use arenas wherever possible
- Keep `malloc` to a minimum and in as few places as possible, rather than scattered through the program
- Treat `new` the same as `malloc`, and remember that standard library containers allocate on their own
- BASS allocates its own memory for loaded audio; that's fine, but the library's own bookkeeping should not allocate after setup

## Performance and latency

- It must work inside games whose loop runs very fast (1000+ frames per second). Calls a game makes during play, like playing a hitsound or changing volume, must be cheap: no allocation and no file access
- Sound must start as soon as possible after it's played. Players judge timing by ear, so added delay between a call and the sound being heard is a bug
- Loading files is allowed to be slow; it happens outside gameplay

## Documentation

The library is documented in `docs/` (repository root): concept pages in `docs/concepts/`, and one page per function or struct in `docs/reference/` with the sections Parameters, Return value, Remarks, Example and See also.
- Any change to a public header updates the matching docs pages in the same pull request
- New functions and structs get a reference page and an entry in `docs/index.md`
- Propose header changes, with their docs, before implementing them

## Workflow

- One feature per branch, with a descriptive branch name, and one pull request per feature
- For changes to public headers, open the pull request with the headers and docs only, as a proposal; once it's approved, add the implementation as a new commit on the same pull request
- Ask clarifying questions before designing a feature
