#makefile for PDF editor
.PHONY: distclean clean pdfedit kernel xpdf kpdf-kde-3.3.2 utils qoutputdevices qsa qsa_configure qsa4 qsa4_configure gui

# includes basic building rules
# REL_ADDR has to be defined, because Makefile.rules refers 
# to the Makefile.flags
REL_ADDR=./
include $(REL_ADDR)Makefile.rules

# make PDF editor - source and documentation
# targets are determined during configure
all: kernel 

# XPdf
xpdf:
	cd $(XPDFROOT) && $(MAKE) staticlib

# PDFedit core development package for 3rd party code
pdfedit-core-dev:
	cd $(XPDFROOT) && $(MAKE) staticlib
	cd $(UTILSROOT) && $(MAKE) staticlib
	cd $(KERNELROOT) && $(MAKE) staticlib

# Utils
utils: xpdf
	cd $(UTILSROOT) && $(MAKE) staticlib

# Kernel
kernel: xpdf utils
	cd $(KERNELROOT) && $(MAKE) staticlib

# internal rule for development which should be used when too many
# changes were introduced
dev-rebuild: dev-clean utils kernel tests pdfedit

# Cleanup
clean:
	cd $(KERNELROOT) && $(MAKE) clean || true
	cd $(UTILSROOT) && $(MAKE) clean
	cd $(XPDFROOT) && $(MAKE) clean || true

dev-clean:
	cd $(KERNELROOT) && $(MAKE) clean || true
	cd $(UTILSROOT) && $(MAKE) clean


# Dist Cleanup
distclean: 
	cd $(KERNELROOT) && $(MAKE) distclean || true
	cd $(UTILSROOT) && $(MAKE) distclean || true
	cd $(XPDFROOT) && $(MAKE) distclean || true
