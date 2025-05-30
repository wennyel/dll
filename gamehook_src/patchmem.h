#ifndef CA_PATCHMEM_H
#define CA_PATCHMEM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write `num_bytes` from `buf` under a given address.
 * Memory write protection to affected pages will be temporarily turned off.
 */
void patch_mem(uintptr_t addr, const char *buf, unsigned num_bytes);

/**
 * Write u32 under a given address as little-endian bytes.
 * Memory write protection to affected pages will be temporarily turned off.
 */
void patch_mem_u32(uintptr_t addr, uint32_t u32);

/**
 * Write u16 under a given address as little-endian bytes.
 * Memory write protection to affected pages will be temporarily turned off.
 */
void patch_mem_u16(uintptr_t addr, uint16_t u16);

/**
 * Change target address of relative JMP or CALL instruction under given
 * address. If `addr` doesn't point to a relative address instruction, this
 * will abort.
 */
void patch_jmp32(uintptr_t addr, uintptr_t fn);

/**
 * Insert a code trampoline at given address. The trampoline will simply
 * call given `fn` with no arguments. PUSHAD and PUSHFD is used to backup
 * the CPU state.
 *
 * \param addr address to patch
 * \param replaced_bytes number of bytes to patch, must be at least 5.
 * May be more than 5, then the remainder is filled with NOP instructions.
 * \param fn function to call
 */
void trampoline_call(uintptr_t addr, unsigned replaced_bytes, void *fn);

/**
 * Patch given address `*orig_fn` - supposedly a function entry point - to
 * jump to given address `fn`. The original contents under `orig_fn` are
 * copied to a dynamically allocated, executable buffer, which is later
 * assigned to `*orig_fn`.
 *
 * This is meant to replace an entire function while still being able to
 * call the original. Since it's meant for replacing function entry points,
 * no PUSHAD/PUSHFD is performed.
 *
 * \param orig_fn pointer to the function to replace, will be updated with
 * a new executable buffer with the original program code.
 * \param replaced_bytes number of bytes to patch, must be at least 5.
 * May be more than 5, then the remainder is filled with NOP instructions.
 * \param fn function to jump to
 */
void trampoline_fn(void **orig_fn, unsigned replaced_bytes, void *fn);

/**
 * Insert a code trampoline at given address. The trampoline will simply
 * execute provided instructions. PUSHAD and PUSHFD is used to backup
 * the CPU state.
 *
 * \param addr address to patch
 * \param replaced_bytes number of bytes to patch, must be at least 5.
 * May be more than 5, then the remainder is filled with NOP instructions.
 * \param buf buffer with instructions, will be copied to executable memory
 * \param num_bytes size of buf in bytes
 */
void *trampoline(uintptr_t addr, unsigned replaced_bytes, const char *buf, unsigned num_bytes);

/**
 * \see trampoline_fn
 * This is a simplified variant for hooking WinAPI functions.
 *
 * \param orig_fn pointer to the function to replace, will be updated with
 * a new executable buffer with the original program code.
 * \param fn function to jump to
 */
void trampoline_winapi_fn(void **orig_fn, void *fn);

/**
 * Convert unsigned 32-bit number to native-endian 4-byte string.
 *
 * \param buf output buffer
 * \param u32 number to convert
 */
void u32_to_str(char *buf, uint32_t u32);

/* patch executable memory without making backups */
void _patch_mem_unsafe(uintptr_t addr, const char *buf, unsigned num_bytes);
void _patch_mem_u32_unsafe(uintptr_t addr, uint32_t u32);
void _patch_jmp32_unsafe(uintptr_t addr, uintptr_t fn);

/* internal functions */
void _trampoline_fn_static_add(void **orig_fn, int replaced_bytes, void *fn);
void _trampoline_static_add(uintptr_t addr, int replaced_bytes, const char *asm_fmt, ...);
void _patch_mem_static_add(uintptr_t addr, int replaced_bytes, const char *asm_fmt, ...);
#define _PATCH_JOIN2(a, b) a ## _ ## b
#define _PATCH_JOIN(a, b) _PATCH_JOIN2(a, b)
#define _PATCH_UNIQUENAME(str) _PATCH_JOIN(str, __LINE__)

/** Special value to call the original code before the trampoline was inserted. */
#define TRAMPOLINE_ORG "call org"

/** Statically-defined trampoline() call. */
#define TRAMPOLINE(addr_p, replaced_bytes_p, ...) \
static void __attribute__((constructor)) _PATCH_UNIQUENAME(init_trampoline)(void) { \
    _trampoline_static_add(addr_p, replaced_bytes_p, __VA_ARGS__); \
}

/** Statically-defined trampoline_fn() call. */
#define TRAMPOLINE_FN(fn_p, replaced_bytes_p, ...) \
static void __attribute__((constructor)) _PATCH_UNIQUENAME(init_trampoline)(void) { \
    _trampoline_fn_static_add((void **)fn_p, replaced_bytes_p, __VA_ARGS__); \
}

/** Statically-defined patch_mem() call. */
#define PATCH_MEM(addr_p, replaced_bytes_p, ...) \
static void __attribute__((constructor)) _PATCH_UNIQUENAME(init_patch_mem)(void) { \
    _patch_mem_static_add(addr_p, replaced_bytes_p, __VA_ARGS__); \
}

/** Statically-defined patch_jmp32() call. */
#define PATCH_JMP32(addr_p, fn_p) \
static void __attribute__((constructor)) _PATCH_UNIQUENAME(init_patch_jmp)(void) { \
    char tmp[16]; \
    if (*(unsigned char *)(uintptr_t)addr_p == 0xe8) { \
        _snprintf(tmp, sizeof(tmp), "call 0x%x", (uintptr_t)fn_p); \
    } else { \
        _snprintf(tmp, sizeof(tmp), "jmp 0x%x", (uintptr_t)fn_p); \
    } \
    _patch_mem_static_add(addr_p, 5, tmp); \
}

/**
 * Insert all trampolines/patches added by static constructors.
 */
void patch_mem_static_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CA_PATCHMEM_H */
