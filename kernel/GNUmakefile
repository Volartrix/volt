MAKEFLAGS += -rR
.SUFFIXES:

override OUTPUT := volt-kernel.elf

override USER_VARIABLE = $(if $(filter $(origin $(1)),default undefined),$(eval override $(1) := $(2)))

$(call USER_VARIABLE,KARCH,x86_64)

$(call USER_VARIABLE,DESTDIR,)

$(call USER_VARIABLE,PREFIX,/usr/local)

$(call USER_VARIABLE,KCC,cc)

$(call USER_VARIABLE,KLD,ld)

$(call USER_VARIABLE,KCFLAGS,-g -O2 -pipe)

$(call USER_VARIABLE,KCPPFLAGS,)

$(call USER_VARIABLE,KNASMFLAGS,-F dwarf -g)

$(call USER_VARIABLE,KLDFLAGS,)

ifneq ($(shell ( test '$(MAKECMDGOALS)' = clean || test '$(MAKECMDGOALS)' = distclean ); echo $$?),0)
    ifeq ($(shell ( ! test -d freestnd-c-hdrs-0bsd || ! test -f src/cc-runtime.c || ! test -f src/limine.h ); echo $$?),0)
        $(error Please run the ./get-deps script first)
    endif
endif

# Check if KCC is Clang.
override KCC_IS_CLANG := $(shell ! $(KCC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

# Internal C flags that should not be changed by the user.
override KCFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -nostdinc \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -D PRINTF_DISABLE_SUPPORT_FLOAT \
    -Wno-maybe-uninitialized \
    -Wno-unused-parameter


# Internal C preprocessor flags that should not be changed by the user.
override KCPPFLAGS := \
    -I src \
    -I src/include \
    -isystem freestnd-c-hdrs-0bsd \
    $(KCPPFLAGS) \
    -MMD \
    -MP

override KNASMFLAGS += \
   -Wall

ifeq ($(KCC_IS_CLANG),1)
    override KCC += \
        -target x86_64-unknown-none -fuse-ld=lld
endif
override KCFLAGS += \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel
override KLDFLAGS += \
    -m elf_x86_64
override KNASMFLAGS += \
    -f elf64


override KLDFLAGS += \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -gc-sections \
    -T linker-$(KARCH).ld

override CFILES := $(shell cd src && find -L * -type f -name '*.c' | LC_ALL=C sort)
override ASFILES := $(shell cd src && find -L * -type f -name '*.S' | LC_ALL=C sort)
override NASMFILES := $(shell cd src && find -L * -type f -name '*.asm' | LC_ALL=C sort)
override OBJ := $(addprefix obj-$(KARCH)/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o))
override OBJ += $(addprefix obj-$(KARCH)/,$(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj-$(KARCH)/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

.PHONY: all
all: bin-$(KARCH)/$(OUTPUT)

bin-$(KARCH)/$(OUTPUT): GNUmakefile linker-$(KARCH).ld $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(KLD) $(OBJ) $(KLDFLAGS) -o $@
	echo "\033[31m\033[1m[LINKING]\033[0m $(OUTPUT)"

-include $(HEADER_DEPS)

obj-$(KARCH)/%.c.o: src/%.c GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $< -o $@
	echo "\033[32m\033[1m[COMPILING]\033[0m $< -> $@"

obj-$(KARCH)/%.S.o: src/%.S GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $< -o $@
	echo "\033[32m\033[1m[COMPILING]\033[0m $< -> $@"

obj-$(KARCH)/%.asm.o: src/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(KNASMFLAGS) $< -o $@
	echo "\033[38;2;0;100;0m\033[1m[ASSEMBLING]\033[0m $< -> $@"

.PHONY: clean
clean:
	rm -rf bin-$(KARCH) obj-$(KARCH)
	echo "\033[34m\033[1m[INFO]\033[0m\033[0m cleaned kernel"

.PHONY: distclean
distclean:
	rm -rf bin-* obj-* freestnd-c-hdrs-0bsd src/cc-runtime.c src/limine.h
	echo "\033[34m\033[1m[INFO]\033[0m\033[0m distcleaned kernel"

.PHONY: install
install: all
	install -d "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
	install -m 644 bin-$(KARCH)/$(OUTPUT) "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/$(OUTPUT)-$(KARCH)"
	echo "\033[34m\033[1m[INFO]\033[0m\033[0m Installed"

.PHONY: uninstall
uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/$(OUTPUT)-$(KARCH)"
	-rmdir "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
	echo "\033[34m\033[1m[INFO]\033[0m\033[0m Uninstalled"
