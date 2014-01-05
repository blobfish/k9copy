#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "2.3.8"

/* Big endian CPU - SPARC or PowerPC */
/* #undef ADM_BIG_ENDIAN */

/* Sparc workstations */
/* #undef ADM_SPARC */

/* AMR_NB */
/* #undef AMR_NB */

/* X86_64 AMD64 assembly */
/* #undef ARCH_64_BITS */

/* Enable PowerPC optim */
/* #undef ARCH_POWERPC */

/* AltiVec for libmpeg2 */
/* #undef ARCH_PPC */

/* post proc */
/* #undef ARCH_X86 */

/* X86_32 assembly */
/* #undef ARCH_X86_32 */

/* X86_64 AMD64 assembly */
/* #undef ARCH_X86_64 */

/* AltiVec for mpeg2enc */
/* #undef HAVE_ALTIVEC */

/* Enable AltiVec by default */
/* #undef HAVE_ALTIVEC_H */

/* Enable AltiVec by default */
/* #undef HAVE_BUILTIN_VECTOR */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H

/* Use malloc.h */
#define HAVE_MALLOC_H

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H


/* Big endian CPU - SPARC or PowerPC */
/* #undef WORDS_BIGENDIAN */

/* #undef ADM_CPU_PPC */
/* #undef ADM_CPU_X86 */
/* #undef ADM_CPU_X86_64 */

/* ffmpeg with swscale support */
/* #undef HAVE_SWSCALE */

/* opengl extensions available */
/* #undef HAVE_OPENGL */

/* #undef BACKLITE */

#endif
