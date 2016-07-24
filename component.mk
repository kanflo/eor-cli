# Component makefile for extras/cli

# expected anyone using bmp driver includes it as 'cli/cli.h'
INC_DIRS += $(cli_ROOT)

# args for passing into compile rule generation
cli_SRC_DIR =  $(cli_ROOT)

$(eval $(call component_compile_rules,cli))
