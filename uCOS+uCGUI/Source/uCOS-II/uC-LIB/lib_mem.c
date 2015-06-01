/*
*********************************************************************************************************
*                                               uC/LIB
*                                       CUSTOM LIBRARY MODULES
*
*                          (c) Copyright 2004-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/LIB is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/LIB in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to
*               help you experience uC/LIB.  The fact that the source code is provided
*               does NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     STANDARD MEMORY OPERATIONS
*
* Filename      : lib_mem.c
* Version       : V1.24
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) NO compiler-supplied standard library functions are used in library or product software.
*
*                     (a) ALL standard library functions are implemented in the custom library modules :
*
*                         (1) \<Custom Library Directory>\lib*.*
*
*                         (2) \<Custom Library Directory>\Ports\<cpu>\<compiler>\lib*_a.*
*
*                               where
*                                       <Custom Library Directory>      directory path for custom library software
*                                       <cpu>                           directory name for specific processor (CPU)
*                                       <compiler>                      directory name for specific compiler
*
*                     (b) Product-specific library functions are implemented in individual products.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    LIB_MEM_MODULE
#include  <lib_mem.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                              Mem_Clr()
*
* Description : Clear data buffer (see Note #2).
*
* Argument(s) : pmem        Pointer to memory buffer to clear.
*
*               size        Number of data buffer octets to clear.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null clears allowed (i.e. 0-octet size).
*
*                   See also 'Mem_Set()  Note #1'.
*
*               (2) Clear data by setting each data octet to 0.
*********************************************************************************************************
*/

void  Mem_Clr (void        *pmem,
               CPU_SIZE_T   size)
{
    Mem_Set((void     *)pmem,
            (CPU_INT08U)0,                                      /* See Note #2.                                         */
            (CPU_SIZE_T)size);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              Mem_Set()
*
* Description : Fill data buffer with specified data octet.
*
* Argument(s) : pmem        Pointer to memory buffer to fill with specified data octet.
*
*               data_val    Data fill octet value.
*
*               size        Number of data buffer octets to fill.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null sets allowed (i.e. 0-octet size).
*
*               (2) For best CPU performance, optimized to fill data buffer using 'CPU_ALIGN'-sized data words.
*
*                   (a) Since many word-aligned processors REQUIRE that multi-octet words be accessed on 
*                       word-aligned addresses, 'CPU_ALIGN'd words MUST be accessed on 'CPU_ALIGN'd addresses.
*
*               (3) Modulo arithmetic is used to determine whether a memory buffer starts on a 'CPU_ALIGN'
*                   address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus, 
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   mem_align_modulo arithmetic operation.
*********************************************************************************************************
*/

void  Mem_Set (void        *pmem,
               CPU_INT08U   data_val,
               CPU_SIZE_T   size)
{
    CPU_SIZE_T   size_rem;
    CPU_ALIGN    data_align;
    CPU_ALIGN   *pmem_align;
    CPU_INT08U  *pmem_08;
    CPU_INT08U   mem_align_modulo;
    CPU_INT08U   i;


    if (size < 1) {                                             /* See Note #1.                                         */
        return;
    }
    if (pmem == (void *)0) {
        return;
    }


    data_align = 0;
    for (i = 0; i < sizeof(CPU_ALIGN); i++) {                   /* Fill each data_align octet with data val.            */
        data_align <<=  DEF_OCTET_NBR_BITS;
        data_align  |= (CPU_ALIGN)data_val;
    }

    size_rem         = (CPU_SIZE_T)size;
    mem_align_modulo = (CPU_INT08U)((CPU_ADDR)pmem % sizeof(CPU_ALIGN));    /* See Note #3.                             */

    pmem_08 = (CPU_INT08U *)pmem;
    if (mem_align_modulo != 0) {                                /* If leading octets avail,                   ...       */
        i = mem_align_modulo;
        while ((size_rem > 0) &&                                /* ... start mem buf fill with leading octets ...       */
               (i        < sizeof(CPU_ALIGN ))) {               /* ... until next CPU_ALIGN word boundary.              */
           *pmem_08++ = data_val;
            size_rem -= sizeof(CPU_INT08U);
            i++;
        }
    }

    pmem_align = (CPU_ALIGN *)pmem_08;                          /* See Note #2a.                                        */
    while (size_rem >= sizeof(CPU_ALIGN)) {                     /* While mem buf aligned on CPU_ALIGN word boundaries,  */
       *pmem_align++ = data_align;                              /* ... fill mem buf with    CPU_ALIGN-sized data.       */
        size_rem    -= sizeof(CPU_ALIGN);
    }

    pmem_08 = (CPU_INT08U *)pmem_align;
    while (size_rem > 0) {                                      /* Finish mem buf fill with trailing octets.            */
       *pmem_08++   = data_val;
        size_rem   -= sizeof(CPU_INT08U);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             Mem_Copy()
*
* Description : Copy data octets from one buffer to another buffer.
*
* Argument(s) : pdest       Pointer to destination memory buffer.
*
*               psrc        Pointer to source      memory buffer.
*
*               size        Number of data buffer octets to copy.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null copies allowed (i.e. 0-octet size).
*
*               (2) Memory buffers NOT checked for overlapping.
*
*               (3) For best CPU performance, optimized to fill data buffer using 'CPU_ALIGN'-sized data words.
*
*                   (a) Since many word-aligned processors REQUIRE that multi-octet words be accessed on 
*                       word-aligned addresses, 'CPU_ALIGN'd words MUST be accessed on 'CPU_ALIGN'd addresses.
*
*               (4) Modulo arithmetic is used to determine whether a memory buffer starts on a 'CPU_ALIGN'
*                   address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus, 
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   mem_align_modulo arithmetic operation.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((!defined(uC_CFG_OPTIMIZE_ASM_EN)) || \
     ((defined(uC_CFG_OPTIMIZE_ASM_EN)) && \
              (uC_CFG_OPTIMIZE_ASM_EN   != DEF_ENABLED)))
void  Mem_Copy (void        *pdest,
                void        *psrc,
                CPU_SIZE_T   size)
{
    CPU_SIZE_T    size_rem;
    CPU_ALIGN    *pmem_align_dest;
    CPU_ALIGN    *pmem_align_src;
    CPU_INT08U   *pmem_08_dest;
    CPU_INT08U   *pmem_08_src;
    CPU_INT08U    i;
    CPU_INT08U    mem_align_modulo_dest;
    CPU_INT08U    mem_align_modulo_src;
    CPU_BOOLEAN   mem_aligned;


    if (size < 1) {                                             /* See Note #1.                                         */
        return;
    }
    if (pdest == (void *)0) {
        return;
    }
    if (psrc  == (void *)0) {
        return;
    }


    size_rem              = (CPU_SIZE_T  )size;

    pmem_08_dest          = (CPU_INT08U *)pdest;
    pmem_08_src           = (CPU_INT08U *)psrc;
                                                                /* See Note #4.                                         */
    mem_align_modulo_dest = (CPU_INT08U  )((CPU_ADDR)pmem_08_dest % sizeof(CPU_ALIGN));
    mem_align_modulo_src  = (CPU_INT08U  )((CPU_ADDR)pmem_08_src  % sizeof(CPU_ALIGN));

    mem_aligned           = (mem_align_modulo_dest == mem_align_modulo_src) ? DEF_YES : DEF_NO;

    if (mem_aligned == DEF_YES) {                               /* If mem bufs' alignment offset equal, ...             */
                                                                /* ... optimize copy for mem buf alignment.             */
        if (mem_align_modulo_dest != 0) {                       /* If leading octets avail,                   ...       */
            i = mem_align_modulo_dest;
            while ((size_rem   >  0) &&                         /* ... start mem buf copy with leading octets ...       */
                   (i          <  sizeof(CPU_ALIGN ))) {        /* ... until next CPU_ALIGN word boundary.              */
               *pmem_08_dest++ = *pmem_08_src++;
                size_rem      -=  sizeof(CPU_INT08U);
                i++;
            }
        }

        pmem_align_dest = (CPU_ALIGN *)pmem_08_dest;            /* See Note #3a.                                        */
        pmem_align_src  = (CPU_ALIGN *)pmem_08_src;
        while (size_rem      >=  sizeof(CPU_ALIGN)) {           /* While mem bufs aligned on CPU_ALIGN word boundaries, */
           *pmem_align_dest++ = *pmem_align_src++;              /* ... copy psrc to pdest with CPU_ALIGN-sized words.   */
            size_rem         -=  sizeof(CPU_ALIGN);
        }

        pmem_08_dest = (CPU_INT08U *)pmem_align_dest;
        pmem_08_src  = (CPU_INT08U *)pmem_align_src;
    }

    while (size_rem > 0) {                                      /* For unaligned mem bufs or trailing octets, ...       */
       *pmem_08_dest++ = *pmem_08_src++;                        /* ... copy psrc to pdest by octets.                    */
        size_rem      -=  sizeof(CPU_INT08U);
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                              Mem_Cmp()
*
* Description : Verify that ALL data octets in two memory buffers are identical in sequence.
*
* Argument(s) : p1_mem      Pointer to first  memory buffer.
*
*               p2_mem      Pointer to second memory buffer.
*
*               size        Number of data buffer octets to compare.
*
* Return(s)   : DEF_YES, if 'size' number of data octets are identical in both memory buffers.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Null compares allowed (i.e. 0-octet size); 'DEF_YES' returned to indicate identical 
*                   null compare.
*
*               (2) Many memory buffer comparisons vary ONLY in the least significant octets -- e.g. 
*                   network address buffers.  Consequently, memory buffer comparison is more efficient 
*                   if the comparison starts from the end of the memory buffers which will abort sooner 
*                   on dissimilar memory buffers that vary only in the least significant octets.
*
*               (3) For best CPU performance, optimized to fill data buffer using 'CPU_ALIGN'-sized data words.
*
*                   (a) Since many word-aligned processors REQUIRE that multi-octet words be accessed on 
*                       word-aligned addresses, 'CPU_ALIGN'd words MUST be accessed on 'CPU_ALIGN'd addresses.
*
*               (4) Modulo arithmetic is used to determine whether a memory buffer starts on a 'CPU_ALIGN'
*                   address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus, 
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   mem_align_modulo arithmetic operation.
********************************************************************************************************
*/
/*$PAGE*/
CPU_BOOLEAN  Mem_Cmp (void        *p1_mem,
                      void        *p2_mem,
                      CPU_SIZE_T   size)
{
    CPU_SIZE_T    size_rem;
    CPU_ALIGN    *p1_mem_align;
    CPU_ALIGN    *p2_mem_align;
    CPU_INT08U   *p1_mem_08;
    CPU_INT08U   *p2_mem_08;
    CPU_INT08U    i;
    CPU_INT08U    mem_align_modulo_1;
    CPU_INT08U    mem_align_modulo_2;
    CPU_BOOLEAN   mem_aligned;
    CPU_BOOLEAN   mem_cmp;


    if (size < 1) {                                             /* See Note #1.                                         */
        return (DEF_YES);
    }
    if (p1_mem == (void *)0) {
        return (DEF_NO);
    }
    if (p2_mem == (void *)0) {
        return (DEF_NO);
    }


    mem_cmp            =  DEF_YES;
    size_rem           =  size;
                                                                /* Start @ end of mem bufs (see Note #2).               */
    p1_mem_08          = (CPU_INT08U *)p1_mem + size;
    p2_mem_08          = (CPU_INT08U *)p2_mem + size;
                                                                /* See Note #4.                                         */
    mem_align_modulo_1 = (CPU_INT08U  )((CPU_ADDR)p1_mem_08 % sizeof(CPU_ALIGN));
    mem_align_modulo_2 = (CPU_INT08U  )((CPU_ADDR)p2_mem_08 % sizeof(CPU_ALIGN));

    mem_aligned        = (mem_align_modulo_1 == mem_align_modulo_2) ? DEF_YES : DEF_NO;

    if (mem_aligned == DEF_YES) {                               /* If mem bufs' alignment offset equal, ...             */
                                                                /* ... optimize cmp for mem buf alignment.              */
        if (mem_align_modulo_1 != 0) {                          /* If trailing octets avail,                  ...       */
            i = mem_align_modulo_1;
            while ((mem_cmp == DEF_YES) &&                      /* ... cmp mem bufs while identical &         ...       */
                   (size_rem > 0)       &&                      /* ... start mem buf cmp with trailing octets ...       */
                   (i        > 0)) {                            /* ... until next CPU_ALIGN word boundary.              */
                p1_mem_08--;
                p2_mem_08--;
                if (*p1_mem_08 != *p2_mem_08) {                 /* If ANY data octet(s) NOT identical, cmp fails.       */
                     mem_cmp = DEF_NO;
                }
                size_rem -= sizeof(CPU_INT08U);
                i--;
            }
        }

        if (mem_cmp == DEF_YES) {                               /* If cmp still identical, cmp aligned mem bufs.        */
            p1_mem_align = (CPU_ALIGN *)p1_mem_08;              /* See Note #3a.                                        */
            p2_mem_align = (CPU_ALIGN *)p2_mem_08;

            while ((mem_cmp  == DEF_YES) &&                     /* Cmp mem bufs while identical & ...                   */
                   (size_rem >= sizeof(CPU_ALIGN))) {           /* ... mem bufs aligned on CPU_ALIGN word boundaries.   */
                p1_mem_align--;
                p2_mem_align--;
                if (*p1_mem_align != *p2_mem_align) {           /* If ANY data octet(s) NOT identical, cmp fails.       */
                     mem_cmp = DEF_NO;
                }
                size_rem -= sizeof(CPU_ALIGN);
            }

            p1_mem_08 = (CPU_INT08U *)p1_mem_align;
            p2_mem_08 = (CPU_INT08U *)p2_mem_align;
        }
    }

    while ((mem_cmp == DEF_YES) &&                              /* Cmp mem bufs while identical ...                     */
           (size_rem > 0)) {                                    /* ... for unaligned mem bufs or trailing octets.       */
        p1_mem_08--;
        p2_mem_08--;
        if (*p1_mem_08 != *p2_mem_08) {                         /* If ANY data octet(s) NOT identical, cmp fails.       */
             mem_cmp = DEF_NO;
        }
        size_rem -= sizeof(CPU_INT08U);
    }

    return (mem_cmp);
}

