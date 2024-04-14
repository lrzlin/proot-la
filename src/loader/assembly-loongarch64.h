/* -*- c-set-style: "K&R"; c-basic-offset: 8 -*-
 *
 * This file is part of PRoot.
 *
 * Copyright (C) 2015 STMicroelectronics
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

/* According to the LoongArch ABI, all registers have undefined values at
 * program startup except:
 *
 * - the instruction pointer (pc)
 * - the stack pointer (sp)
 * - the rtld_fini pointer (a0)
 */
#define BRANCH(stack_pointer, destination) do {			\
	asm volatile (						\
		"# Restore initial stack pointer.	\n\t"	\
		"move $sp, %0				\n\t"	\
		"					\n\t"	\
		"# Clear rtld_fini.		\n\t"	\
		"move $a0, $zero			\n\t"	\
		"					\n\t"	\
		"# Start the program.			\n\t"	\
		"jr %1				\n"		\
		: /* no output */				\
		: "r" (stack_pointer), "r" (destination)	\
		: "memory", "$sp", "$a0");			\
	__builtin_unreachable();				\
	} while (0)

#define PREPARE_ARGS_1(arg1_)				\
	register word_t arg1 asm("$a0") = arg1_;		\

#define PREPARE_ARGS_3(arg1_, arg2_, arg3_)		\
	PREPARE_ARGS_1(arg1_)				\
	register word_t arg2 asm("$a1") = arg2_;		\
	register word_t arg3 asm("$a2") = arg3_;		\

#define PREPARE_ARGS_4(arg1_, arg2_, arg3_, arg4_)	\
	PREPARE_ARGS_3(arg1_, arg2_, arg3_)		\
	register word_t arg4 asm("$a3") = arg4_;		\

#define PREPARE_ARGS_6(arg1_, arg2_, arg3_, arg4_, arg5_, arg6_)	\
	PREPARE_ARGS_4(arg1_, arg2_, arg3_, arg4_)			\
	register word_t arg5 asm("$a4") = arg5_;				\
	register word_t arg6 asm("$a5") = arg6_;

#define OUTPUT_CONTRAINTS_1			\
	"r" (arg1)

#define OUTPUT_CONTRAINTS_3			\
	OUTPUT_CONTRAINTS_1,			\
	"r" (arg2), "r" (arg3)

#define OUTPUT_CONTRAINTS_4			\
	OUTPUT_CONTRAINTS_3,			\
	"r" (arg4)

#define OUTPUT_CONTRAINTS_6				\
	OUTPUT_CONTRAINTS_4,				\
	"r" (arg5), "r" (arg6)

#define SYSCALL_CLOBBERLIST				\
		"memory", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8"

#define SYSCALL(number_, nb_args, args...)				\
	({								\
		register word_t number asm("$a7") = number_;		\
		register word_t result asm("$a0");			\
		PREPARE_ARGS_##nb_args(args)				\
			asm volatile (					\
				"syscall 0				\n\t"	\
				: "+r" (result)				\
				: "r" (number),				\
				OUTPUT_CONTRAINTS_##nb_args		\
				: SYSCALL_CLOBBERLIST);			\
			result;						\
	})

#define OPENAT	56
#define CLOSE	57
#define MMAP	222
#define EXECVE	221
#define EXIT	93
#define PRCTL   167
#define MPROTECT 226