cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

zephyr_include_directories(
	../libscpi/inc/
)

zephyr_library_sources(
	../libscpi/src/error.c
	../libscpi/src/expression.c
	../libscpi/src/fifo.c
	../libscpi/src/fifo_private.h
	../libscpi/src/ieee488.c
	../libscpi/src/lexer.c
	../libscpi/src/lexer_private.h
	../libscpi/src/minimal.c
	../libscpi/src/parser.c
	../libscpi/src/parser_private.h
	../libscpi/src/units.c
	../libscpi/src/utils.c
	../libscpi/src/utils_private.h
)
