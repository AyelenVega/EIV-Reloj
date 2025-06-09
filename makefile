MODULES = 
BOARD = edu-ciaa-nxp
MUJU = ./muju

OUT_DIR = ./build
DOC_DIR = $(OUT_DIR)/doc

include $(MUJU)/module/base/makefile

doc:
	@echo "Generando documentacion"
	@mkdir -p $(DOC_DIR)
	@doxygen Doxyfile


