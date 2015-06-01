/*
*********************************************************************************************************
*                                               uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                          (c) Copyright 2004-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/CPU is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/CPU in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to
*               help you experience uC/CPU.  The fact that the source code is provided
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
*                                           CORE CPU MODULE
*
* Filename      : cpu_core.c
* Version       : V1.18
* Programmer(s) : SR
*                 ITJ
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define   CPU_CORE_MODULE
#include  <cpu_core.h>


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

#if (CPU_CFG_NAME_EN == DEF_ENABLED)                            /* ------------------ CPU NAME FNCTS ------------------ */
static  void  CPU_NameInit(void);
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                             CPU_Init()
*
* Description : (1) Initialize CPU module :
*
*                   (a) Initialize CPU name
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Your Product's Application.
*
*               This function is a CPU initialization function & MAY be called by application/
*               initialization function(s).
*
* Note(s)     : (2) CPU_Init() MUST be called BEFORE product's application calls any core CPU module
*                   function(s).
*********************************************************************************************************
*/

void  CPU_Init (void)
{
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameInit();
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            CPU_NameClr()
*
* Description : Clear CPU Name.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_NameInit(),
*               Application.
*
*               This function is a CPU module application interface (API) function & MAY be called by
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void  CPU_NameClr (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


    CPU_CRITICAL_ENTER();
    Mem_Clr((void     *)&CPU_Name[0],
            (CPU_SIZE_T) CPU_CFG_NAME_SIZE);
    CPU_CRITICAL_EXIT();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                            CPU_NameGet()
*
* Description : Get CPU name.
*
* Argument(s) : pname       Pointer to an ASCII character array that will receive the return CPU name
*                               ASCII string from this function (see Note #1).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               CPU_ERR_NONE                CPU name successfully returned.
*                               CPU_ERR_NULL_PTR            Argument 'pname' pass a NULL pointer.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a CPU module application interface (API) function & MAY be called by
*               application function(s).
*
* Note(s)     : (1) The size of the ASCII character array that will receive the return CPU name ASCII
*                   string :
*
*                   (a) MUST   be greater than or equal to the current CPU name's ASCII string size
*                           including the terminating NULL character;
*                   (b) SHOULD be greater than or equal to CPU_CFG_NAME_SIZE
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void  CPU_NameGet (CPU_CHAR  *pname,
                   CPU_ERR   *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


    if (pname == (CPU_CHAR *)0) {
       *perr = CPU_ERR_NULL_PTR;
        return;
    }

    CPU_CRITICAL_ENTER();
    Str_Copy((CPU_CHAR *) pname,
             (CPU_CHAR *)&CPU_Name[0]);
    CPU_CRITICAL_EXIT();

   *perr = CPU_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                            CPU_NameSet()
*
* Description : Set CPU name.
*
* Argument(s) : pname       Pointer to CPU name to set.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               CPU_ERR_NONE                CPU name successfully set.
*                               CPU_ERR_NULL_PTR            Argument 'pname' pass a NULL pointer.
*                               CPU_ERR_NAME_SIZE           Invalid CPU name size (see Note #1).
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a CPU module application interface (API) function & MAY be called by
*               application function(s).
*
* Note(s)     : (1) 'pname' ASCII string size, including the terminating NULL character, MUST be less
*                    than or equal to CPU_CFG_NAME_SIZE.
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void  CPU_NameSet (CPU_CHAR  *pname,
                   CPU_ERR   *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    CPU_SIZE_T  len;


    if (pname == (CPU_CHAR *)0) {
       *perr = CPU_ERR_NULL_PTR;
        return;
    }

    len  = Str_Len(pname);
    len += sizeof((CPU_CHAR)0);                                 /* Adjust name len to include NULL char (see Note #1).  */
    if (len <= CPU_CFG_NAME_SIZE) {                             /* If       cfg name len <= max cfg name size, ...      */
        CPU_CRITICAL_ENTER();
        Str_Copy((CPU_CHAR *)&CPU_Name[0],                      /* ... copy cfg name to CPU name.                       */
                 (CPU_CHAR *) pname);
        CPU_CRITICAL_EXIT();
       *perr = CPU_ERR_NONE;

    } else {
       *perr = CPU_ERR_NAME_SIZE;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           CPU_NameInit()
*
* Description : Initialize CPU Name.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
static  void  CPU_NameInit (void)
{
    CPU_NameClr();
}
#endif

