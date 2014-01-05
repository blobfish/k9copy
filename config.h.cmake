#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "${VERSION}"

/* Big endian CPU - SPARC or PowerPC */
#cmakedefine ADM_BIG_ENDIAN

/* Sparc workstations */
#cmakedefine ADM_SPARC

/* AMR_NB */
#cmakedefine AMR_NB

/* X86_64 AMD64 assembly */
#cmakedefine ARCH_64_BITS

/* Enable PowerPC optim */
#cmakedefine ARCH_POWERPC

/* AltiVec for libmpeg2 */
#cmakedefine ARCH_PPC

/* post proc */
#cmakedefine ARCH_X86

/* X86_32 assembly */
#cmakedefine ARCH_X86_32

/* X86_64 AMD64 assembly */
#cmakedefine ARCH_X86_64

/* AltiVec for mpeg2enc */
#cmakedefine HAVE_ALTIVEC

/* Enable AltiVec by default */
#cmakedefine HAVE_ALTIVEC_H

/* Enable AltiVec by default */
#cmakedefine HAVE_BUILTIN_VECTOR

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Use malloc.h */
#cmakedefine HAVE_MALLOC_H

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H


/* Big endian CPU - SPARC or PowerPC */
#cmakedefine WORDS_BIGENDIAN

#cmakedefine ADM_CPU_PPC
#cmakedefine ADM_CPU_X86
#cmakedefine ADM_CPU_X86_64

/* ffmpeg with swscale support */
#cmakedefine HAVE_SWSCALE

/* opengl extensions available */
#cmakedefine HAVE_OPENGL 

#cmakedefine BACKLITE

#endif
