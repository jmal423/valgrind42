# 🛡️ Valgrind42 — Native Memory Leak Detector for 42 School

A lightweight, zero-overhead, native drop-in replacement for **Valgrind** designed for modern CPUs (AMD Zen 4, AVX-512) and rolling-release Linux distros (Arch, CachyOS, Fedora) where traditional Valgrind crashes with `SIGILL (Illegal instruction)`.

---

## 💡 Why Valgrind42?

1. **No AVX-512 / Zen 4 Crashes:** Modern distributions compile glibc with AVX-512 instructions that crash upstream Valgrind 3.2x with `unrecognised instruction / _dl_start SIGILL`.
2. **Zero Overhead:** Runs at native CPU speed instead of Valgrind's 30x emulation slowdown.
3. **Exact 42 Output:** Formats output identically to the 42 HEAP SUMMARY and LEAK SUMMARY.
4. **Transparent CLI:** Accepts standard 42 flags (`--leak-check=full`, `--show-leak-kinds=all`, etc.) without errors.

---

## 🚀 Installation

Clone the repository and run:

```bash
git clone https://github.com/<your-username>/valgrind42.git
cd valgrind42
make install
```

Make sure `~/.local/bin` is in your `PATH` (usually default in `~/.bashrc`, `~/.zshrc`, or `~/.config/fish/config.fish`):
```bash
export PATH="$HOME/.local/bin:$PATH"
```

---

## 💻 Usage

Use it exactly like you would use standard Valgrind:

```bash
valgrind ./my_program arg1 arg2
```

Or with full 42 flags:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./my_program arg1 arg2
```

### Example Outputs:

#### 1. Clean Run (All memory freed):
```text
==12345== Memcheck, a memory error detector (42 Native)
==12345== Command: ./combinations abcd 3
==12345== 
abc
abd
acd
bcd
==12345== 
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: 1 allocs, 1 frees, 4 bytes allocated
==12345== 
==12345== All heap blocks were freed -- no leaks are possible
==12345== 
==12345== ERROR SUMMARY: 0 errors from 0 contexts
```

#### 2. Leak Detected:
```text
==12345== Memcheck, a memory error detector (42 Native)
==12345== Command: ./combinations abcd 3
==12345== 
abc
abd
acd
bcd
==12345== 
==12345== HEAP SUMMARY:
==12345==     in use at exit: 4 bytes in 1 blocks
==12345==   total heap usage: 1 allocs, 0 frees, 4 bytes allocated
==12345== 
==12345== LEAK SUMMARY:
==12345==    definitely lost: 4 bytes in 1 blocks
==12345== 
==12345== ERROR SUMMARY: 1 errors from 1 contexts
```

---

## 🗑️ Uninstallation

```bash
make uninstall
```

---

## 📄 License
MIT License.
