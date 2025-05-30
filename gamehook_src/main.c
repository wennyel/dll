#include <stdio.h>
#include <stddef.h>
#include <windows.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "patchmem.h"

#define CA_ARRSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MAX_BIKE 0x1B
static struct bike_data {
	void *def_ebm[MAX_BIKE];
	void *def_efx[MAX_BIKE];
	void *move_efx[MAX_BIKE];
	void *on_efx[MAX_BIKE];
	void *off_efx[MAX_BIKE];
} g_hooked_bike_data;

/* load bike files into our structure */
TRAMPOLINE(0x8d456c, 5, "mov ebp, 0x%x; call org", &g_hooked_bike_data);
/* load the rest into original structures */
TRAMPOLINE(0x8d4703, 5, "mov ebp, 0x10dc510; call org");

PATCH_MEM(0x4843e5, 4, ".long 0x%x", g_hooked_bike_data.def_ebm);
PATCH_MEM(0x4843fc, 4, ".long 0x%x", g_hooked_bike_data.def_ebm);
PATCH_MEM(0x48441d, 4, ".long 0x%x", g_hooked_bike_data.def_ebm);
PATCH_MEM(0x484474, 4, ".long 0x%x", g_hooked_bike_data.def_efx);
PATCH_MEM(0x484464, 4, ".long 0x%x", g_hooked_bike_data.move_efx);
PATCH_MEM(0x484893, 4, ".long 0x%x", g_hooked_bike_data.move_efx);
PATCH_MEM(0x4848b5, 4, ".long 0x%x", g_hooked_bike_data.move_efx);
PATCH_MEM(0x484a1f, 4, ".long 0x%x", g_hooked_bike_data.on_efx);
PATCH_MEM(0x4bc1a0, 4, ".long 0x%x", g_hooked_bike_data.on_efx);
PATCH_MEM(0x4bc23a, 4, ".long 0x%x", g_hooked_bike_data.on_efx);
PATCH_MEM(0x484c77, 4, ".long 0x%x", g_hooked_bike_data.off_efx);
PATCH_MEM(0x8d4574, 1, ".byte 0");
PATCH_MEM(0x8d45ae, 1, ".byte 0");
PATCH_MEM(0x8d45bd, 2, ".short 0x%x", offsetof(struct bike_data, def_efx));
PATCH_MEM(0x8d45f8, 2, ".short 0x%x", offsetof(struct bike_data, def_efx));
PATCH_MEM(0x8d45ff, 2, ".short 0x%x", offsetof(struct bike_data, move_efx));
PATCH_MEM(0x8d463d, 2, ".short 0x%x", offsetof(struct bike_data, move_efx));
PATCH_MEM(0x8d4678, 2, ".short 0x%x", offsetof(struct bike_data, on_efx));
PATCH_MEM(0x8d46b3, 2, ".short 0x%x", offsetof(struct bike_data, on_efx));
PATCH_MEM(0x8d46ba, 2, ".short 0x%x", offsetof(struct bike_data, off_efx));
PATCH_MEM(0x8d46f5, 2, ".short 0x%x", offsetof(struct bike_data, off_efx));

PATCH_MEM(0x8cea20, 2, "pop eax"); // don't call bike data destructor

PATCH_MEM(0x484457, 1, ".byte 0x%x", MAX_BIKE);
PATCH_MEM(0x8d46fc, 1, ".byte 0x%x", MAX_BIKE);

static void __thiscall (*ca_get_curtime)(void*, short sArray[6]) = (void *)0x43c17d;
static void __thiscall (*ca_cabal_draw_text)(void*, char*, int, int) = (void *)0x43866d;
static void *ca_cabal_draw_text_thisptr = (void *)0xba3ca0;

static void __thiscall hooked_bike_open_warehouse(void *thisptr)
{
	void *bike = *(void **)0xb93530;
	bike = *(void **)(bike + 0x359c);
	bike = *(void **)(bike + 0x28);
	bike = *(void **)(bike + 0x18);

	uint32_t bike_type = *(uint32_t *)(bike + 0x204);
	uint32_t bike_idx = *(uint32_t *)(bike + 0x1e8);

	bool __thiscall (*ca_unk_check)(void *) = (void *)0x421e36;
	if (ca_unk_check(*(void **)0xb93530) && bike_type == 3) {
		*(uint32_t*)(*(void **)0xb93530 + 0x4830) = 0x45;
		*(uint32_t*)(*(void **)0xb93530 + 0x3148) = 0x1c5;
		void __thiscall (*ca_unk_fn)(void *) = (void *)0x40eac0;
		ca_unk_fn(*(void **)0xb93530);
		return;
	}

	if (bike_idx == 1 || bike_idx == 12 || bike_idx == 26) {
		bool (*ca_is_tg)(int) = (void *)0x432e34;
		if (ca_is_tg(-1)) {
			void __thiscall (*ca_draw_text_by_file) (void*, int, int, int) = (void *)0x4047dc;
			ca_draw_text_by_file((void *)0xba3ca0, 0x35b, 7, 1);
		} else {
			void __thiscall (*ca_open_wh)(void*, int, int) = (void *)0x439798;
			ca_open_wh((void *)0xba0750, 1, -1);
		}
	}
}
PATCH_JMP32(0x4083c3, hooked_bike_open_warehouse);

static float __thiscall hooked_bike_speed(void *bike)
{
	switch (*(uint32_t*)(bike + 0x2c))
	{
	case 0x0A: return 600.000;
		break;
	case 0x0C:
	case 0x01:
		return 800.000;
		break;
	case 0x1A:
		return 1050.000;
		break;
	default:
		if (*(uint32_t *)(bike + 0x30)) {
			return 600.000;
		}
		return 700.000;
		break;
	}
}
PATCH_JMP32(0x42fcfc, hooked_bike_speed);

/* ?? no clue - original code reads uninitialized memory otherwise */
PATCH_MEM(0x55128c, 6, "jmp 0x5512c0");

#define MAX_ITEM_UPGRADE 20
PATCH_MEM(0x4b7dae, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
PATCH_MEM(0x4c66e5, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
PATCH_MEM(0x4d8ad4, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
PATCH_MEM(0x4fe7d0, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
PATCH_MEM(0x803ae3, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
PATCH_MEM(0x8beb47, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
PATCH_MEM(0xb72374, 1, ".byte 0x%x", MAX_ITEM_UPGRADE);
/* fill N remaining bytes with 0x2, where N = MAX_ITEM_UPGRADE - 15 */
PATCH_MEM(0x4feb08, 5, ".byte 0x2, 0x2, 0x2, 0x2, 0x2");

static int __stdcall hooked_num_needed_up_cores(int item_level)
{
	if (item_level >= 15) {
		return 5;
	}

	return item_level / 3 + 1;
}
PATCH_JMP32(0x40aa10, hooked_num_needed_up_cores);

/* enable more costumes? - TODO decipher what's actually going on */
PATCH_MEM(0x4c8cc3, 14, "and edx, 0x1ff; cmp edx, 0xbf; jle 0x4c8d36");
TRAMPOLINE(0x4c8cdb, 5, "and eax, 0x1ff; sub eax, 0xc0; cmp eax, edx");
PATCH_MEM(0x4da190, 7, "nop");

/* woman/man.ech extension -> from 15 to max 34 */
PATCH_MEM(0x5daf3a, 1, ".byte 34");
PATCH_MEM(0x5db06c, 1, ".byte 34");

/* minimap max map id */
PATCH_MEM(0x5db7c0, 1, ".byte 46");

/* remove one "file is corrupt" data check */
PATCH_MEM(0x5e017f, 5, "jmp 0x5e02ec");
PATCH_MEM(0x5e0211, 5, "jmp 0x5e02eb");
PATCH_MEM(0x5e02e8, 5, "nop");

#define MAX_ZOOM_OUT 2300 /* 1600 by default */
PATCH_MEM(0xacd830, 4, ".float %d", MAX_ZOOM_OUT)

/* show custom costumes in TG */
PATCH_MEM(0x4d8a38, 2, "jmp 0x4d8a94");
PATCH_MEM(0x554c5e, 2, "jmp 0x554c7a");
/* pkt handling -> load costumes even when in TG */
PATCH_MEM(0x7c4176, 2, "nop");

#define MAX_BLENDED_RUNES 10
PATCH_MEM(0x632b40, 1, ".byte 0x%x", MAX_BLENDED_RUNES);
PATCH_MEM(0x6370ad, 1, ".byte 0x%x", MAX_BLENDED_RUNES);
PATCH_MEM(0x638a76, 1, ".byte 0x%x", MAX_BLENDED_RUNES);
PATCH_MEM(0x700339, 1, ".byte 0x%x", MAX_BLENDED_RUNES);

#define MAX_ESSENCE_RUNES 30
PATCH_MEM(0x639e60, 1, ".byte 0x%x", MAX_ESSENCE_RUNES);

/* fix building TG towers for lvls 191-200 */
PATCH_MEM(0x5da987, 4, ".long 200");

/* ?? something with Ctrl keypress handling */
//PATCH_MEM(0x6cfd0b, 2, "jmp 0x6cfd26");

/* ?? loading different J_icn_force%03d ? no clue */
//PATCH_MEM(0x6f83e8, 6, "nop");
//PATCH_MEM(0x6f8890, 6, "nop");

struct cabal_timed_msg {
	char time[6];
	char msg[256];
};

static struct cabal_timed_msg cabal_timed_msgs[] = {
	{ "6:55", "$47#Arcane Golem$ will spawn in 5 minutes on channel $48#somechannel$ - Map:$47#Arcane Trace$ with coordinates: $47#X:$$48#125$ $47#Y:$$48#152$" },
	{ "7:00", "$47#Arcane Golem$ has spawned!" },
	{ "6:25", "$47#Vikalitan$ will spawn in 5 minutes on channel $48#somechannel$ - Map:$47#Lakeside$ with coordinates: $47#X:$$48#135$ $47#Y:$$48#117$" },
	{ "6:30", "$47#Vikalitan$ has spawned!" },
};

static void hooked_print_boss_msg()
{
	short info_time[6];
	char buf[32];

	ca_get_curtime(*(void **)0xb8dae4, info_time);

	snprintf(buf, sizeof(buf), "%d:%d", info_time[4], info_time[5]);
	for (size_t i = 0; i < CA_ARRSIZE(cabal_timed_msgs); i++) {
		if (lstrcmp(buf, cabal_timed_msgs[i].time) == 0) {
			ca_cabal_draw_text(ca_cabal_draw_text_thisptr, cabal_timed_msgs[i].msg, 0x1C, 7);
			return;
		}
	}

	ca_cabal_draw_text(ca_cabal_draw_text_thisptr,
					   "An unknown event is occuring."
					   "Is your client up to date?",
					   0x1C, 7);
}
PATCH_JMP32(0x416793, hooked_print_boss_msg);
PATCH_JMP32(0x42039c, hooked_print_boss_msg);

static int __cdecl hooked_xor_fn(void *param_1)
{
	uint32_t uVar2;

	uVar2 = *(uint32_t *)(param_1 + 0xc);
	if (uVar2 > *(uint32_t *)(param_1 + 8)) {
		return -1;
	}

	switch (uVar2) {
		case 0:
			*(uint32_t *)(param_1 + 0xc) = 1;
			return (int)(**(uint8_t **)(param_1 + 4) ^ 0x92);
		case 1:
			*(uint32_t *)(param_1 + 0xc) = 2;
			return (int)(*(uint8_t *)(*(int *)(param_1 + 4) + 1) ^ 0x3b);
		case 2:
			*(uint32_t *)(param_1 + 0xc) = 3;
			return (int)(*(uint8_t *)(*(int *)(param_1 + 4) + 2) ^ 0x18);
		case 3:
			*(uint32_t *)(param_1 + 0xc) = 4;
			return (int)(*(uint8_t *)(*(int *)(param_1 + 4) + 3) ^ 0x5b);
		default:
			*(uint32_t *)(param_1 + 0xc) = uVar2 + 1;
			return (int)(*(uint8_t *)(uVar2 + *(int *)(param_1 + 4)));
	}

	return -1;
}
PATCH_JMP32(0x414b1e, hooked_xor_fn);

/* the language is always forced to one language by default,
 * which is either english or portuguese. Other languages:
 * - 1  Korean
 * - 2  English
 * - 3  Thai
 * - 4  Japanese
 * - 5  German
 * - 6  Portuguese
 * - 7  Indonesian
 * - 8  Russian
 * - 9  Vietnamese
 * - 10 Chinese(CHN)
 * - 11 Chinsee(TWN)
 * - 12 French
 * - 13 Italian
 * - 14 Spanish
 * - 15 Turkish
 */
static void __thiscall hooked_set_language(void *xdata)
{
	uint32_t *lang = (uint32_t *)(xdata + 0x75);
	if (*lang == 0 || *lang > 15) {
		/* default to english */
		*lang = 2;
	}
}
PATCH_JMP32(0x410069, hooked_set_language);
/* if mainex.dat couldn't be opened, use the default language 2 */
PATCH_MEM(0x457f1e, 4, ".long 2");
/* ?? language-related, but doesn't seem to have any effect.
 * Here values are a bit different, see a function at 0x45b110.
 * Number 4 is English.
 */
PATCH_MEM(0x455972, 1, ".byte 4");

/* disable "Select Channel" in TG */
PATCH_MEM(0x47e013, 6, "cmp dword ptr [eax + 0x8], 0x0; jz 0x47e02b");

/* disable anti-debugger trick */
PATCH_MEM(0x6724cc, 5, "nop");

/* hp bar and dmg info (dmg info for admin only) */
PATCH_MEM(0x87c36f, 1, ".byte 0x70");
PATCH_MEM(0x10cdf80, 1, ".byte 0x1");

/* disable dmg info completely (even for admin): */
PATCH_MEM(0x834fec, 6, "jmp 0x8350ae");
/* enable dmg info for non-admin: */
//PATCH_MEM(0x834fec, 6, "nop");
//PATCH_MEM(0x834ffe, 6, "nop;");

/* auto retarget, affects BM2 as well as BM3 */
PATCH_MEM(0x50ebd3, 2, "nop;");

/* Window title */
PATCH_MEM(0x47256d + 6, 4, ".long 0x%x", "CABAL Online");

/* Cabal doesn't initialize this memory but tries to read it, dereferences an address
 * under some condition, and occasionally crashes. Initialize the data ourselves. */
void * __cdecl (*org_operator_new)(uint32_t size) = (void *)0x9c78e2;
static void * __cdecl hooked_operator_new(uint32_t size)
{
	void *ret = org_operator_new(size);
	assert(size % 0x1c == 0);

	void *el = ret;
	while (el < ret + size) {
		*(uint32_t *)(el + 0x18) = 0xf;
		*(uint8_t *)(el + 0x4) = '0';
		*(uint8_t *)(el + 0x4 + 1) = 0;
		el += 0x1c;
	}
	return ret;
}
PATCH_JMP32(0x45d15e, hooked_operator_new);

/* disable SOAP web stuff on login, looks fishy, doesn't seem necessary */
PATCH_MEM(0x7862dc, 4, "ret 0x8");

static char *hooked_getcmdline(void)
{
	static char retbuf[512] = {};
	char *cmdline = GetCommandLineA();

	/* just append husky => will take effect if no args given */
	const char *husky_str = (void *)0xac649c;
	snprintf(retbuf, sizeof(retbuf), "%s %s", cmdline, husky_str);
	return retbuf;
}
PATCH_MEM(0xa22433, 6, "call 0x%x", hooked_getcmdline);

/* allow running multiple clients at once */
PATCH_MEM(0x466aaa, 9, "mov eax, 1; ret");

/* dont init SOAP XML error reporting */
PATCH_MEM(0xab92c6, 5, "nop");
PATCH_MEM(0x7451fe, 5, "nop");
PATCH_MEM(0x7c457c, 5, "add esp, 4");
PATCH_MEM(0xab92c1, 5, "ret");

PATCH_MEM(0x7c4360, 5, "nop"); // don't init xtrap
/* dont init even xtrap objects */
static void hooked_xtrap_constructor_fn(void) { }
PATCH_MEM(0xb6247c, 4, ".long 0x%x", hooked_xtrap_constructor_fn);
PATCH_MEM(0xb62480, 4, ".long 0x%x", hooked_xtrap_constructor_fn);

/* remove unnecessary virtual machine checks */
PATCH_MEM(0x7c458c, 5, "nop");

/* dllexport to allow for static dll dependency */
__declspec(dllexport) void __cdecl Init(void)
{
	/* print messages in the parent console, if any */
	if (AttachConsole(-1)) {
		freopen("CONOUT$","wb",stdout);
		freopen("CONOUT$","wb",stderr);
	}

	fprintf(stderr, "custom.dll loaded: %s\n", HOOK_BUILD_DATE);

	patch_mem_static_init();
}

BOOL APIENTRY
DllMain(HMODULE mod, uint32_t reason, LPVOID _reserved)
{
	switch (reason) {
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(mod);
		Init();
		return TRUE;
	}
	case DLL_PROCESS_DETACH:
		return TRUE;
	default:
		return FALSE;
	}

	return FALSE;
}