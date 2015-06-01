/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : includes.h
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#define   DEF_FALSE 0
#define   DEF_TRUE  1

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>

#include    <ucos_ii.h>

#include    <cpu.h>
#include    <lib_def.h>
#include    <lib_mem.h>
#include    <lib_str.h>

#include    <stm32f10x_conf.h>
#include    <stm32f10x_lib.h>

#include    <app_cfg.h>
#include    <bsp.h>
#include    <GUI.h>
#include    <ili9320.h>
#include    <GUIDemo.h>

#include "SetPWM.h"
#include "AD.h"

#include "PID.h"
//#include "PWM.h"

//#define EDIT_AD
//#define PWM_TEXT

#if (uC_PROBE_OS_PLUGIN > 0)
#include    <os_probe.h>
#endif

#if (uC_PROBE_COM_MODULE > 0)
#include    <probe_com.h>

#if (PROBE_COM_METHOD_RS232 > 0)
#include    <probe_rs232.h>
#endif
#endif



#endif
