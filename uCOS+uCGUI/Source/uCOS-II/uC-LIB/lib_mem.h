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
* Filename      : lib_mem.h
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
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  LIB_MEM_MODULE_PRESENT
#define  LIB_MEM_MODULE_PRESENT


/*$PAGE*/
/*
*********************************************************************************************************
*                                            INCLUDE FILES
*
* Note(s) : (1) The following common software files are located in the following directories :
*
*               (a) \<Custom Library Directory>\lib*.*
*
*               (b) (1) \<CPU-Compiler Directory>\cpu_def.h
*
*                   (2) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
*
*                           where
*                                   <Custom Library Directory>      directory path for custom   library      software
*                                   <CPU-Compiler Directory>        directory path for common   CPU-compiler software
*                                   <cpu>                           directory name for specific processor (CPU)
*                                   <compiler>                      directory name for specific compiler
*
*           (2) Compiler MUST be configured to include the '\<Custom Library Directory>\uC-LIB\',
*               '\<CPU-Compiler Directory>\' directory, & the specific CPU-compiler directory as 
*               additional include path directories.
*
*           (3) NO compiler-supplied standard library functions SHOULD be used.
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <app_cfg.h>


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   LIB_MEM_MODULE
#define  LIB_MEM_EXT
#else
#define  LIB_MEM_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      MEMORY DATA VALUE MACRO'S
*
* Note(s) : (1) (a) Some variables & variable buffers to pass & receive data values MUST start on appropriate
*                   CPU word-aligned addresses.  This is required because most word-aligned processors are more
*                   efficient & may even REQUIRE that multi-octet words start on CPU word-aligned addresses.
*
*                   (1) For 16-bit word-aligned processors, this means that
*
*                           all 16- & 32-bit words MUST start on addresses that are multiples of 2 octets
*
*                   (2) For 32-bit word-aligned processors, this means that
*
*                           all 16-bit       words MUST start on addresses that are multiples of 2 octets
*                           all 32-bit       words MUST start on addresses that are multiples of 4 octets
*
*               (b) However, some data values macro's appropriately access data values from any CPU addresses,
*                   word-aligned or not.  Thus for processors that require data word alignment, data words can
*                   be accessed to/from any CPU address, word-aligned or not, without generating data-word-
*                   alignment exceptions/faults.
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                          MEM_VAL_GET_xxx()
*
* Description : Decode data values from any CPU memory address.
*
* Argument(s) : addr        Lowest CPU memory address of data value to decode (see Notes #2 & #3a).
*
* Return(s)   : Decoded data value from CPU memory address (see Notes #1 & #3b).
*
* Caller(s)   : various.
*
* Note(s)     : (1) Decode data values based on the values' data-word order in CPU memory :
*
*                       MEM_VAL_GET_xxx_BIG()           Decode big-   endian data values -- data words' most
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_GET_xxx_LITTLE()        Decode little-endian data values -- data words' least
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_GET_xxx()               Decode data values using CPU's native or configured 
*                                                           data-word order
*
*                   See also 'cpu.h  CPU WORD CONFIGURATION  Note #2'.
*
*               (2) CPU memory addresses/pointers NOT checked for NULL.
*
*               (3) (a) MEM_VAL_GET_xxx() macro's decode data values without regard to CPU word-aligned addresses.
*                       Thus for processors that require data word alignment, data words can be decoded from any
*                       CPU address, word-aligned or not, without generating data-word-alignment exceptions/faults.
*
*                   (b) However, any variable to receive the returned data value MUST start on an appropriate CPU
*                       word-aligned address.
*
*                   See also 'MEMORY DATA VALUE MACRO'S  Note #1'.
*
*               (4) MEM_VAL_COPY_GET_xxx() macro's are more efficient than MEM_VAL_GET_xxx() macro's & are 
*                   also independent of CPU data-word-alignment & SHOULD be used whenever possible.
*
*                   See also 'MEM_VAL_COPY_GET_xxx()  Note #4'.
*
*               (5) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
*                   linked since each 'cpu.h' SHOULD ensure that the CPU data-word-memory order configuration
*                   constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate data-word-memory order 
*                   value (see 'cpu.h  CPU WORD CONFIGURATION  Note #2').  The 'else'-conditional code is 
*                   included as an extra precaution in case 'cpu.h' is incorrectly configured.
*********************************************************************************************************
*/

#define  MEM_VAL_GET_INT08U_BIG(addr)         (((CPU_INT08U)(*(((CPU_INT08U *)(addr)) + 0))) << (0 * DEF_OCTET_NBR_BITS))

#define  MEM_VAL_GET_INT16U_BIG(addr)        ((((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 0))) << (1 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 1))) << (0 * DEF_OCTET_NBR_BITS)))

#define  MEM_VAL_GET_INT32U_BIG(addr)        ((((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 0))) << (3 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 1))) << (2 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 2))) << (1 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 3))) << (0 * DEF_OCTET_NBR_BITS)))



#define  MEM_VAL_GET_INT08U_LITTLE(addr)      (((CPU_INT08U)(*(((CPU_INT08U *)(addr)) + 0))) << (0 * DEF_OCTET_NBR_BITS))

#define  MEM_VAL_GET_INT16U_LITTLE(addr)     ((((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 0))) << (0 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT16U)(*(((CPU_INT08U *)(addr)) + 1))) << (1 * DEF_OCTET_NBR_BITS)))

#define  MEM_VAL_GET_INT32U_LITTLE(addr)     ((((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 0))) << (0 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 1))) << (1 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 2))) << (2 * DEF_OCTET_NBR_BITS)) + \
                                              (((CPU_INT32U)(*(((CPU_INT08U *)(addr)) + 3))) << (3 * DEF_OCTET_NBR_BITS)))



#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_GET_INT08U(addr)                MEM_VAL_GET_INT08U_BIG(addr)
#define  MEM_VAL_GET_INT16U(addr)                MEM_VAL_GET_INT16U_BIG(addr)
#define  MEM_VAL_GET_INT32U(addr)                MEM_VAL_GET_INT32U_BIG(addr)

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_GET_INT08U(addr)                MEM_VAL_GET_INT08U_LITTLE(addr)
#define  MEM_VAL_GET_INT16U(addr)                MEM_VAL_GET_INT16U_LITTLE(addr)
#define  MEM_VAL_GET_INT32U(addr)                MEM_VAL_GET_INT32U_LITTLE(addr)

#else                                                                           /* See Note #5.                         */

#error  "CPU_CFG_ENDIAN_TYPE  illegally #defined in 'cpu.h'      "
#error  "                     [See 'cpu.h  CONFIGURATION ERRORS']"

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          MEM_VAL_SET_xxx()
*
* Description : Encode data values to any CPU memory address.
*
* Argument(s) : addr        Lowest CPU memory address to encode data value (see Notes #2 & #3a).
*
*               val         Data value to encode (see Notes #1 & #3b).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Encode data values into CPU memory based on the values' data-word order :
*
*                       MEM_VAL_SET_xxx_BIG()           Encode big-   endian data values -- data words' most
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_SET_xxx_LITTLE()        Encode little-endian data values -- data words' least
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_SET_xxx()               Encode data values using CPU's native or configured 
*                                                           data-word order
*
*                   See also 'cpu.h  CPU WORD CONFIGURATION  Note #2'.
*
*               (2) CPU memory addresses/pointers NOT checked for NULL.
*
*               (3) (a) MEM_VAL_SET_xxx() macro's encode data values without regard to CPU word-aligned addresses.
*                       Thus for processors that require data word alignment, data words can be encoded to any
*                       CPU address, word-aligned or not, without generating data-word-alignment exceptions/faults.
*
*                   (b) However, 'val' data value to encode MUST start on an appropriate CPU word-aligned address.
*
*                   See also 'MEMORY DATA VALUE MACRO'S  Note #1'.
*
*               (4) MEM_VAL_COPY_SET_xxx() macro's are more efficient than MEM_VAL_SET_xxx() macro's & are 
*                   also independent of CPU data-word-alignment & SHOULD be used whenever possible.
*
*                   See also 'MEM_VAL_COPY_SET_xxx()  Note #4'.
*
*               (5) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
*                   linked since each 'cpu.h' SHOULD ensure that the CPU data-word-memory order configuration
*                   constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate data-word-memory order 
*                   value (see 'cpu.h  CPU WORD CONFIGURATION  Note #2').  The 'else'-conditional code is 
*                   included as an extra precaution in case 'cpu.h' is incorrectly configured.
*********************************************************************************************************
*/

#define  MEM_VAL_SET_INT08U_BIG(addr, val)         { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT08U)(val)) &       0xFF) >> (0 * DEF_OCTET_NBR_BITS))); }

#define  MEM_VAL_SET_INT16U_BIG(addr, val)         { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT16U)(val)) &     0xFF00) >> (1 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT16U)(val)) &     0x00FF) >> (0 * DEF_OCTET_NBR_BITS))); }

#define  MEM_VAL_SET_INT32U_BIG(addr, val)         { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0xFF000000) >> (3 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0x00FF0000) >> (2 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 2)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0x0000FF00) >> (1 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 3)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0x000000FF) >> (0 * DEF_OCTET_NBR_BITS))); }



#define  MEM_VAL_SET_INT08U_LITTLE(addr, val)      { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT08U)(val)) &       0xFF) >> (0 * DEF_OCTET_NBR_BITS))); }

#define  MEM_VAL_SET_INT16U_LITTLE(addr, val)      { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT16U)(val)) &     0x00FF) >> (0 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT16U)(val)) &     0xFF00) >> (1 * DEF_OCTET_NBR_BITS))); }

#define  MEM_VAL_SET_INT32U_LITTLE(addr, val)      { (*(((CPU_INT08U *)(addr)) + 0)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0x000000FF) >> (0 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 1)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0x0000FF00) >> (1 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 2)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0x00FF0000) >> (2 * DEF_OCTET_NBR_BITS))); \
                                                     (*(((CPU_INT08U *)(addr)) + 3)) = ((CPU_INT08U)((((CPU_INT32U)(val)) & 0xFF000000) >> (3 * DEF_OCTET_NBR_BITS))); }



#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  MEM_VAL_SET_INT08U(addr, val)           MEM_VAL_SET_INT08U_BIG(addr, val)
#define  MEM_VAL_SET_INT16U(addr, val)           MEM_VAL_SET_INT16U_BIG(addr, val)
#define  MEM_VAL_SET_INT32U(addr, val)           MEM_VAL_SET_INT32U_BIG(addr, val)

#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)

#define  MEM_VAL_SET_INT08U(addr, val)           MEM_VAL_SET_INT08U_LITTLE(addr, val)
#define  MEM_VAL_SET_INT16U(addr, val)           MEM_VAL_SET_INT16U_LITTLE(addr, val)
#define  MEM_VAL_SET_INT32U(addr, val)           MEM_VAL_SET_INT32U_LITTLE(addr, val)

#else                                                                           /* See Note #5.                         */

#error  "CPU_CFG_ENDIAN_TYPE  illegally #defined in 'cpu.h'      "
#error  "                     [See 'cpu.h  CONFIGURATION ERRORS']"

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       MEM_VAL_COPY_GET_xxx()
*
* Description : Copy & decode data values from any CPU memory address to any CPU memory address.
*
* Argument(s) : addr_dest       Lowest CPU memory address to copy/decode source address's data value 
*                                   (see Notes #2 & #3).
*
*               addr_src        Lowest CPU memory address of data value to copy/decode 
*                                   (see Notes #2 & #3).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Copy/decode data values based on the values' data-word order :
*
*                       MEM_VAL_COPY_GET_xxx_BIG()      Decode big-   endian data values -- data words' most
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_COPY_GET_xxx_LITTLE()   Decode little-endian data values -- data words' least
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_COPY_GET_xxx()          Decode data values using CPU's native or configured 
*                                                           data-word order
*
*                   See also 'cpu.h  CPU WORD CONFIGURATION  Note #2'.
*
*               (2) CPU memory addresses/pointers NOT checked for NULL.
*
*               (3) MEM_VAL_COPY_GET_xxx() macro's copy/decode data values without regard to CPU word-aligned 
*                   addresses.  Thus for processors that require data word alignment, data words can be copied/
*                   decoded to/from any CPU address, word-aligned or not, without generating data-word-alignment 
*                   exceptions/faults.
*
*               (4) MEM_VAL_COPY_GET_xxx() macro's are more efficient than MEM_VAL_GET_xxx() macro's & are 
*                   also independent of CPU data-word-alignment & SHOULD be used whenever possible.
*
*                   See also 'MEM_VAL_GET_xxx()  Note #4'.
*
*               (5) Since octet-order copy/conversion are inverse operations, memory data value gets/sets are
*                   inverse operations.
*
*                   See also 'MEM_VAL_COPY_SET_xxx()  Note #5'.
*
*               (6) The 'CPU_CFG_ENDIAN_TYPE' pre-processor 'else'-conditional code SHOULD never be compiled/
*                   linked since each 'cpu.h' SHOULD ensure that the CPU data-word-memory order configuration
*                   constant (CPU_CFG_ENDIAN_TYPE) is configured with an appropriate data-word-memory order 
*                   value (see 'cpu.h  CPU WORD CONFIGURATION  Note #2').  The 'else'-conditional code is 
*                   included as an extra precaution in case 'cpu.h' is incorrectly configured.
*********************************************************************************************************
*/
/*$PAGE*/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)


#define  MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)         { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)         { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); }

#define  MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)         { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 3)); }



#define  MEM_VAL_COPY_GET_INT08U_LITTLE(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_GET_INT16U_LITTLE(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_GET_INT32U_LITTLE(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 3)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }



#define  MEM_VAL_COPY_GET_INT08U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)
#define  MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)
#define  MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)




#elif   (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_LITTLE)


#define  MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)         { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)         { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)         { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 3)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }



#define  MEM_VAL_COPY_GET_INT08U_LITTLE(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_GET_INT16U_LITTLE(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); }

#define  MEM_VAL_COPY_GET_INT32U_LITTLE(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                                    (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 3)); }



#define  MEM_VAL_COPY_GET_INT08U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT08U_LITTLE(addr_dest, addr_src)
#define  MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT16U_LITTLE(addr_dest, addr_src)
#define  MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT32U_LITTLE(addr_dest, addr_src)




#else                                                                           /* See Note #6.                         */

#error  "CPU_CFG_ENDIAN_TYPE  illegally #defined in 'cpu.h'      "
#error  "                     [See 'cpu.h  CONFIGURATION ERRORS']"

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       MEM_VAL_COPY_SET_xxx()
*
* Description : Copy & encode data values from any CPU memory address to any CPU memory address.
*
* Argument(s) : addr_dest       Lowest CPU memory address to copy/encode source address's data value 
*                                   (see Notes #2 & #3).
*
*               addr_src        Lowest CPU memory address of data value to copy/encode 
*                                   (see Notes #2 & #3).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) Copy/encode data values based on the values' data-word order :
*
*                       MEM_VAL_COPY_SET_xxx_BIG()      Encode big-   endian data values -- data words' most
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_COPY_SET_xxx_LITTLE()   Encode little-endian data values -- data words' least
*                                                           significant octet @ lowest memory address
*                       MEM_VAL_COPY_SET_xxx()          Encode data values using CPU's native or configured 
*                                                           data-word order
*
*                   See also 'cpu.h  CPU WORD CONFIGURATION  Note #2'.
*
*               (2) CPU memory addresses/pointers NOT checked for NULL.
*
*               (3) MEM_VAL_COPY_SET_xxx() macro's copy/encode data values without regard to CPU word-aligned 
*                   addresses.  Thus for processors that require data word alignment, data words can be copied/
*                   encoded to/from any CPU address, word-aligned or not, without generating data-word-alignment 
*                   exceptions/faults.
*
*               (4) MEM_VAL_COPY_SET_xxx() macro's are more efficient than MEM_VAL_SET_xxx() macro's & are 
*                   also independent of CPU data-word-alignment & SHOULD be used whenever possible.
*
*                   See also 'MEM_VAL_SET_xxx()  Note #4'.
*
*               (5) Since octet-order copy/conversion are inverse operations, memory data value gets/sets
*                   are inverse operations.
*
*                   See also 'MEM_VAL_COPY_GET_xxx()  Note #5'.
*********************************************************************************************************
*/

                                                                                /* See Note #5.                         */
#define  MEM_VAL_COPY_SET_INT08U_BIG(addr_dest, addr_src)       MEM_VAL_COPY_GET_INT08U_BIG(addr_dest, addr_src)
#define  MEM_VAL_COPY_SET_INT16U_BIG(addr_dest, addr_src)       MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)
#define  MEM_VAL_COPY_SET_INT32U_BIG(addr_dest, addr_src)       MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)

#define  MEM_VAL_COPY_SET_INT08U_LITTLE(addr_dest, addr_src)    MEM_VAL_COPY_GET_INT08U_LITTLE(addr_dest, addr_src)
#define  MEM_VAL_COPY_SET_INT16U_LITTLE(addr_dest, addr_src)    MEM_VAL_COPY_GET_INT16U_LITTLE(addr_dest, addr_src)
#define  MEM_VAL_COPY_SET_INT32U_LITTLE(addr_dest, addr_src)    MEM_VAL_COPY_GET_INT32U_LITTLE(addr_dest, addr_src)


#define  MEM_VAL_COPY_SET_INT08U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT08U(addr_dest, addr_src)
#define  MEM_VAL_COPY_SET_INT16U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)
#define  MEM_VAL_COPY_SET_INT32U(addr_dest, addr_src)           MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)


/*$PAGE*/
/*
*********************************************************************************************************
*                                         MEM_VAL_COPY_xxx()
*
* Description : Copy data values from any CPU memory address to any CPU memory address.
*
* Argument(s) : addr_dest       Lowest CPU memory address to copy source address's data value
*                                   (see Notes #2 & #3).
*
*               addr_src        Lowest CPU memory address of data value to copy 
*                                   (see Notes #2 & #3).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) MEM_VAL_COPY_xxx() macro's copy data values based on CPU's native data-word order.
*
*                   See also 'cpu.h  CPU WORD CONFIGURATION  Note #2'.
*
*               (2) CPU memory addresses/pointers NOT checked for NULL.
*
*               (3) MEM_VAL_COPY_xxx() macro's copy data values without regard to CPU word-aligned addresses.
*                   Thus for processors that require data word alignment, data words can be copied to/from any 
*                   CPU address, word-aligned or not, without generating data-word-alignment exceptions/faults.
*********************************************************************************************************
*/

#define  MEM_VAL_COPY_08(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); }

#define  MEM_VAL_COPY_16(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                     (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); }

#define  MEM_VAL_COPY_32(addr_dest, addr_src)      { (*(((CPU_INT08U *)(addr_dest)) + 0)) = (*(((CPU_INT08U *)(addr_src)) + 0)); \
                                                     (*(((CPU_INT08U *)(addr_dest)) + 1)) = (*(((CPU_INT08U *)(addr_src)) + 1)); \
                                                     (*(((CPU_INT08U *)(addr_dest)) + 2)) = (*(((CPU_INT08U *)(addr_src)) + 2)); \
                                                     (*(((CPU_INT08U *)(addr_dest)) + 3)) = (*(((CPU_INT08U *)(addr_src)) + 3)); }


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         Mem_Clr (void        *pmem,
                      CPU_SIZE_T   size);

void         Mem_Set (void        *pmem,
                      CPU_INT08U   data_val,
                      CPU_SIZE_T   size);

void         Mem_Copy(void        *pdest,
                      void        *psrc,
                      CPU_SIZE_T   size);

CPU_BOOLEAN  Mem_Cmp (void        *p1_mem,
                      void        *p2_mem,
                      CPU_SIZE_T   size);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of lib mem module include.                       */

