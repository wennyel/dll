OBJECTS = main.o rc.o patchmem.o

CFLAGS := -m32 -Wall -O2 -ggdb -MMD -MP -fno-strict-aliasing -masm=intel
CFLAGS += -DHOOK_BUILD_DATE="\"Jan 22 2025 12:00 PM UTC\""
CFLAGS += -D_WIN32_WINNT=0x502
CFLAGS += -I"C:/Cliente_Cabal/gamehook_src/keystone/include"  # Include path para os headers

LDFLAGS += -L"C:/Cliente_Cabal/gamehook_src/keystone/bin" -lkeystone  # Path para a libkeystone.a

# Certifique-se de que o diretório de build existe
$(shell mkdir build 2>nul)

all: build/gamehook.dll

.PHONY: clean
clean:
	@del /q /f build\*.o build\*.d build\gamehook.dll
	@if exist build rmdir /q /s build

build/gamehook.dll: $(OBJECTS:%.o=build/%.o)
	gcc $(CFLAGS) -o $@ -shared -fPIC $(filter %.o,$^) $(LDFLAGS) -Wl,--subsystem,windows -static-libgcc

build/%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

build/%.o: %.cpp
	g++ -fno-exceptions $(CFLAGS) -c -o $@ $<

build/rc.o: gamehook.rc
	windres -i $< -o $@

-include $(OBJECTS:%.o=build/%.d)
