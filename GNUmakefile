MAKEFLAGS += -rR -slient
.SUFFIXES:
.SILENT: 

override USER_VARIABLE = $(if $(filter $(origin $(1)),default undefined),$(eval override $(1) := $(2)))

$(call USER_VARIABLE,KARCH,x86_64)

$(call USER_VARIABLE,QEMUFLAGS,-m 2G -monitor stdio)

override IMAGE_NAME := bin/volt-os-x64

.PHONY: all
all: $(IMAGE_NAME).iso

.PHONY: all-hdd
all-hdd: $(IMAGE_NAME).hdd

.PHONY: run
run: run-$(KARCH)

.PHONY: run-hdd
run-hdd: run-hdd-$(KARCH)

.PHONY: run-hdd-wsl
run-hdd-wsl: run-hdd-x86_64-wsl

.PHONY: run-wsl
run-wsl: run-x86_64-wsl

.PHONY: bin
bin:
	mkdir -p bin/ > /dev/null 2>&1

.PHONY: run-x86_64
run-x86_64: ovmf/ovmf-code-$(KARCH).fd ovmf/ovmf-vars-$(KARCH).fd $(IMAGE_NAME).iso
	echo "Running x64 ISO image in QEMU"
	qemu-system-$(KARCH) \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(KARCH).fd,readonly=on \
		-drive if=pflash,unit=1,format=raw,file=ovmf/ovmf-vars-$(KARCH).fd \
		-cdrom $(IMAGE_NAME).iso \
		$(QEMUFLAGS) 

.PHONY: run-hdd-x86_64
run-hdd-x86_64: ovmf/ovmf-code-$(KARCH).fd ovmf/ovmf-vars-$(KARCH).fd $(IMAGE_NAME).hdd
	echo "Running x64 HDD image in QEMU"
	qemu-system-$(KARCH) \
		-M pc \
		-hda $(IMAGE_NAME).hdd \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(KARCH).fd,readonly=on \
		-drive if=pflash,unit=1,format=raw,file=ovmf/ovmf-vars-$(KARCH).fd \
		$(QEMUFLAGS) 

.PHONY: run-x86_64-wsl
run-x86_64-wsl: ovmf/ovmf-code-$(KARCH).fd ovmf/ovmf-vars-$(KARCH).fd $(IMAGE_NAME).iso
	echo "Running x64 ISO image in WSL"
	qemu-system-$(KARCH).exe \
		-M pc \
		-cdrom $(IMAGE_NAME).iso \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(KARCH).fd,readonly=on \
		-drive if=pflash,unit=1,format=raw,file=ovmf/ovmf-vars-$(KARCH).fd \
		$(QEMUFLAGS) 

.PHONY: run-hdd-x86_64-wsl
run-hdd-x86_64-wsl: ovmf/ovmf-code-$(KARCH).fd ovmf/ovmf-vars-$(KARCH).fd $(IMAGE_NAME).hdd
	echo "Running x64 HDD image in WSL"
	qemu-system-$(KARCH).exe \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(KARCH).fd,readonly=on \
		-drive if=pflash,unit=1,format=raw,file=ovmf/ovmf-vars-$(KARCH).fd \
		-hda $(IMAGE_NAME).hdd \
		$(QEMUFLAGS) 

.PHONY: run-bios
run-bios: $(IMAGE_NAME).iso
	qemu-system-$(KARCH) \
		-M q35 \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		$(QEMUFLAGS) > /dev/null 2>&1

.PHONY: run-hdd-bios
run-hdd-bios: $(IMAGE_NAME).hdd
	qemu-system-$(KARCH) \
		-M q35 \
		-hda $(IMAGE_NAME).hdd \
		$(QEMUFLAGS) > /dev/null 2>&1

ovmf/ovmf-code-$(KARCH).fd:
	echo "Downloading ovmf-code.fd into ovmf/"
	mkdir -p ovmf > /dev/null 2>&1
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-$(KARCH).fd > /dev/null 2>&1

ovmf/ovmf-vars-$(KARCH).fd:
	echo "Downloading ovmf-vars.fd into ovmf/"
	mkdir -p ovmf > /dev/null 2>&1
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-vars-$(KARCH).fd > /dev/null 2>&1

limine/limine:
	echo "Downloading the limine bootloader"
	rm -rf limine > /dev/null 2>&1
	git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1 > /dev/null 2>&1
	$(MAKE) -C limine > /dev/null 2>&1

kernel-deps:
	echo "Getting kernel dependencies"
	./kernel/get-deps > /dev/null 2>&1
	touch kernel-deps > /dev/null 2>&1

.PHONY: kernel
kernel: kernel-deps
	echo "Making the kernel"
	$(MAKE) -C kernel

$(IMAGE_NAME).iso: limine/limine kernel bin
	echo "Creating ISO image"
	rm -rf iso_root > /dev/null 2>&1
	mkdir -p iso_root/boot > /dev/null 2>&1
	echo "Prepared directories"
	cp -v kernel/bin-$(KARCH)/volt-kernel.elf iso_root/boot/ > /dev/null 2>&1
	echo "Copied kernel binary"
	mkdir -p iso_root/boot/limine > /dev/null 2>&1
	cp -v limine.conf iso_root/boot/limine/ > /dev/null 2>&1
	mkdir -p iso_root/EFI/BOOT > /dev/null 2>&1
	cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/ > /dev/null 2>&1
	echo "Copied bootloader files"
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/ > /dev/null 2>&1
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/ > /dev/null 2>&1
	echo "Copied EFI executables"
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso > /dev/null 2>&1
	./limine/limine bios-install $(IMAGE_NAME).iso > /dev/null 2>&1
	rm -rf iso_root > /dev/null 2>&1
	echo "Created ISO image in $(IMAGE_NAME).iso"

$(IMAGE_NAME).hdd: limine/limine kernel bin
	echo "Creating HDD image"
	rm -f $(IMAGE_NAME).hdd > /dev/null 2>&1
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMAGE_NAME).hdd > /dev/null 2>&1
	echo "Created empty image $(IMAGE_NAME).hdd"
	sgdisk $(IMAGE_NAME).hdd -n 1:2048 -t 1:ef00 > /dev/null 2>&1
	echo "Formatted image $(IMAGE_NAME).hdd"
	./limine/limine bios-install $(IMAGE_NAME).hdd > /dev/null 2>&1
	echo "Installed limine BIOS"
	mformat -i $(IMAGE_NAME).hdd@@1M > /dev/null 2>&1
	echo "Formatting $(IMAGE_NAME).hdd"
	mmd -i $(IMAGE_NAME).hdd@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine > /dev/null 2>&1
	mcopy -i $(IMAGE_NAME).hdd@@1M kernel/bin-$(KARCH)/volt-kernel.elf ::/boot > /dev/null 2>&1
	mcopy -i $(IMAGE_NAME).hdd@@1M limine.conf ::/boot/limine > /dev/null 2>&1
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/limine-bios.sys ::/boot/limine > /dev/null 2>&1
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT > /dev/null 2>&1
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT > /dev/null 2>&1
	echo "Copied files to $(IMAGE_NAME).hdd"

.PHONY: clean
clean:
	$(MAKE) -C kernel clean
	rm -rf iso_root $(IMAGE_NAME).iso $(IMAGE_NAME).hdd bin > /dev/null 2>&1
	echo "Cleaned everything"

.PHONY: distclean
distclean:
	$(MAKE) -C kernel distclean
	rm -rf iso_root *.iso *.hdd kernel-deps limine ovmf bin > /dev/null 2>&1
	echo "Distcleaned everything"