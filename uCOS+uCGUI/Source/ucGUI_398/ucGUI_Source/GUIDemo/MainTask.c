/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : MainTask.c
Purpose     : Init GUI & call of GUI-demo
--------------------END-OF-HEADER-------------------------------------
*/

#include <includes.h>

#include "GUI.h"
#include "GUIDEMO.h"
#include "GRAPH.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "EDIT.h"
#include "SLIDER.h"
#include "PID.h"
#include "TEXT.h"

/*********************************************************************
*
*       Var
*/

const int ID_CURVE_BTN_SWTITCH_2_PARA = 0x200; 
const int ID_CURVE_BTN_CLEAR = 0x201;

const int ID_CFG_PID_BTN_SWTITCH_2_CURVE = 0x202;
const int ID_CFG_PID_BTN_CLEAR = 0x203;
const int ID_CFG_PID_EDT_PWM1 = 0x204;
const int ID_CFG_PID_EDT_PWM2 = 0x210;
const int ID_CFG_PID_EDT_PWM3 = 0x211;


const int ID_CFG_PID_TXT_PWM1 = 0x207;
const int ID_CFG_PID_TXT_PWM2 = 0x208;
const int ID_CFG_PID_TXT_PWM3 = 0x209;


const int ID_CFG_PID_EDT_AD1 = 0x205;
const int ID_CFG_PID_EDT_AD2 = 0x206;

typedef enum PID_Para_CFG_Flag 
{
	Kp = 0,
	Ki = 1,
	Kd = 2,
	E  = 3,
	Set= 4
} PID_Para_CFG_Flag;

PID_Para_CFG_Flag PID_Para_Edit_Flag = Ki;
float E_Percent[2];

static int PID_EDIT = GUI_ID_EDIT0;

extern u16 usValue;
extern PID value_PID[6];
static int CounterPID = 0;

int ClosePWM = 0;

	
GRAPH_DATA_Handle  hData,hData2;
GRAPH_SCALE_Handle hScale;


WM_HWIN hGraph;	
WM_HWIN  hWM_HBKWIN_CURVE,
				 hWM_HBKWIN_CFG_PID,
				 hWN_Dialog_PID_Frame;

BUTTON_Handle btn,btnClear,
              btn1,btnClear1, 
							BTN_Frame_A,BTN_Frame_B;
EDIT_Handle   Edit_PWM1,Edit_PWM2,Edit_PWM3;

#ifdef EDIT_AD	
EDIT_Handle   Edit_AD1,Edit_AD2;	
#endif

#ifdef PWM_TEXT		
TEXT_Handle   Text_PWM1,Text_PWM2,Text_PWM3;
#endif

char buf[6]; //!< Used to temp of the Button of AD value
char bufTextOrder[2];


int BK_Light = 200;
int PID_CFG_Flag = 1;


/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "PID Configeration", 0,                       0,  0,  400,240,0/*FRAMEWIN_CF_MOVEABLE*/,0},
    { BUTTON_CreateIndirect,    "0",                 GUI_ID_BUTTON0,          10, 76, 75, 31, 0,0},
    { BUTTON_CreateIndirect,    "1",                 GUI_ID_BUTTON1,          91, 76, 72, 31, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT0,            10, 41, 75, 29, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT1,            91, 41, 72, 29, 0,0},
    { BUTTON_CreateIndirect,    "2",                 GUI_ID_BUTTON2,          172,76, 75, 31, 0,0},
    { BUTTON_CreateIndirect,    "3",                 GUI_ID_BUTTON3,          10, 113,75, 31, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT2,            172,41, 75, 29, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT3,            253,41, 55, 29, 0,0},
    { BUTTON_CreateIndirect,    "4",                 GUI_ID_BUTTON4,          91, 113,72, 31, 0,0},
    { BUTTON_CreateIndirect,    "5",                 GUI_ID_BUTTON5,          172,113,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "6",                 GUI_ID_BUTTON6,          10, 150,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "7",                 GUI_ID_BUTTON7,          91, 148,72, 31, 0,0},
    { BUTTON_CreateIndirect,    "8",                 GUI_ID_BUTTON8,          172,150,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "9",                 GUI_ID_BUTTON9,          10, 185,75, 31, 0,0},
    { BUTTON_CreateIndirect,    ".",                 GUI_ID_BUTTON10,          91, 185,72, 31, 0,0},
    { BUTTON_CreateIndirect,    "<-",                GUI_ID_BUTTON11,          172,187,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "Kp",                GUI_ID_BUTTON12,          10, 11, 75, 23, 0,0},
    { BUTTON_CreateIndirect,    "Ki",                GUI_ID_BUTTON13,          91, 11, 72, 23, 0,0},
    { BUTTON_CreateIndirect,    "Kd",                GUI_ID_BUTTON14,          172,11, 75, 23, 0,0},
    { BUTTON_CreateIndirect,    "E",                 GUI_ID_BUTTON15,          253,11, 55, 23, 0,0},
    { BUTTON_CreateIndirect,    "OK",                GUI_ID_BUTTON16,         316,187,75, 34, 0,0},
    { SLIDER_CreateIndirect,     NULL,               GUI_ID_SLIDER0,          294,76, 91, 31, 0,0},
    { BUTTON_CreateIndirect,    "Set",               GUI_ID_BUTTON17,         314,12, 75, 23, 0,0},
    { EDIT_CreateIndirect,       NULL,               GUI_ID_EDIT4,            316,41, 69, 29, 0,0},
    { BUTTON_CreateIndirect,    "Next",          		 GUI_ID_BUTTON18,         316,113,75, 31, 0,0},
    { BUTTON_CreateIndirect,    "Prev",          		 GUI_ID_BUTTON19,         316,148,75, 31, 0,0},
		{ TEXT_CreateIndirect,      "0-Order",              GUI_ID_TEXT0,            258,85, 35, 12, 0,0}
};
	
/*********************************************************************
*
*       Function Declaration
*/


/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

void PaintDialog(WM_MESSAGE * pMsg)
{
  //  WM_HWIN hWin = pMsg->hWin;
}


/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/

void InitDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;

    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
	
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    FRAMEWIN_AddMaxButton(hWin, FRAMEWIN_BUTTON_RIGHT, 1);
    FRAMEWIN_AddMinButton(hWin, FRAMEWIN_BUTTON_RIGHT, 2);
		FRAMEWIN_SetTitleHeight(hWin,18);
	
		SLIDER_SetRange(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),5,250);
		SLIDER_SetValue(WM_GetDialogItem(hWin,GUI_ID_SLIDER0),200);
	
		EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT0),GUI_TA_VCENTER|GUI_TA_CENTER);
		EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT1),GUI_TA_VCENTER|GUI_TA_CENTER);
		EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT2),GUI_TA_VCENTER|GUI_TA_CENTER);
		EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT3),GUI_TA_VCENTER|GUI_TA_CENTER);
		EDIT_SetTextAlign(WM_GetDialogItem(hWin,GUI_ID_EDIT4),GUI_TA_VCENTER|GUI_TA_CENTER);	

}

/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbBk_Frame(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;						
            }
            break;
        case WM_NOTIFY_PARENT:
						GUI_DispStringAt("Frame",10,220);
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
    
						switch(NCode)
						{
								 case WM_NOTIFICATION_RELEASED: 
								 {																			
											if(Id == GUI_ID_BUTTON0)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'0');
											}
											if(Id == GUI_ID_BUTTON1)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'1');													
											}
											if(Id == GUI_ID_BUTTON2)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'2');													
											}
											if(Id == GUI_ID_BUTTON3)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'3');
											}
											if(Id == GUI_ID_BUTTON4)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'4');													
											}
											if(Id == GUI_ID_BUTTON5)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'5');													
											}
											if(Id == GUI_ID_BUTTON6)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'6');													
											}
											if(Id == GUI_ID_BUTTON7)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'7');													
											}
											if(Id == GUI_ID_BUTTON8)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'8');
											}
											if(Id == GUI_ID_BUTTON9)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'9');							
											}
											if(Id == GUI_ID_BUTTON10)
											{
													EDIT_AddKey(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT),'.');										
											}
											
											/* BackSpace */
											if(Id == GUI_ID_BUTTON11)
											{
												 //EDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_EDIT0),NULL);//清空输入框，挨个删除用下面两行
												 WM_SetFocus(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT));
												 GUI_SendKeyMsg(GUI_KEY_BACKSPACE,1);  
																				
												 GPIO_ResetBits(GPIOF,GPIO_Pin_7);												
											}
											
											/* PID Parameter Edit choese*/
											{
													if(Id == GUI_ID_BUTTON12 || Id == GUI_ID_EDIT0)
													{
														PID_Para_Edit_Flag = Kp;
														PID_EDIT = PID_Para_Edit_Flag + GUI_ID_EDIT0;
														
													}											
													if(Id == GUI_ID_BUTTON13 || Id == GUI_ID_EDIT1)
													{
														PID_Para_Edit_Flag = Ki;
														PID_EDIT = PID_Para_Edit_Flag + GUI_ID_EDIT0;												
													}
													if(Id == GUI_ID_BUTTON14 || Id == GUI_ID_EDIT2)
													{
														PID_Para_Edit_Flag = Kd;
														PID_EDIT = PID_Para_Edit_Flag + GUI_ID_EDIT0; //GUI_ID_EDIT2													
													}											
													if(Id == GUI_ID_BUTTON15 || Id == GUI_ID_EDIT3)
													{
														PID_Para_Edit_Flag = E;
														PID_EDIT = PID_Para_Edit_Flag + GUI_ID_EDIT0;																											
													}
													if(Id == GUI_ID_BUTTON17 || Id == GUI_ID_EDIT4)
													{
														PID_Para_Edit_Flag = Set;
														PID_EDIT = PID_Para_Edit_Flag + GUI_ID_EDIT0; //GUI_ID_EDIT2												
													}	
													
													/* OK Key */
													if(Id == GUI_ID_BUTTON16)
													{														
															/* Store the last array(5) */
															value_PID[CounterPID].Proportion = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT0) );
														 	value_PID[CounterPID].Integral   = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT1) );
															value_PID[CounterPID].Derivative = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT2) );
															value_PID[CounterPID].SetPoint 	 = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT4) );

															BK_Light = SLIDER_GetValue(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_SLIDER0));
															SetPWM(BK_Light,PWM_BACK_LIGHT);	
														
															//!< Not the PID configuration Period
															PID_CFG_Flag = 0;
															
															WM_DeleteWindow(hWN_Dialog_PID_Frame);
															
															WM_ShowWindow(hWM_HBKWIN_CURVE);
															WM_HideWindow(hWM_HBKWIN_CFG_PID);
													}																										
													
											}
											
											/* Up and down of the order of the PID Array */
											{
												if(Id == GUI_ID_BUTTON18)
												{
														value_PID[CounterPID].Proportion = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT0) );
														value_PID[CounterPID].Integral   = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT1) );
														value_PID[CounterPID].Derivative = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT2) );
														value_PID[CounterPID].SetPoint 	 = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT4) );
													
														/* Get the E_Percent */
														{
															if(0 == CounterPID)
															{
																E_Percent[0] = EDIT_GetFloatValue ( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3) );
																E_Percent[1] = 1 - E_Percent[0];
															 }
														}
														
														CounterPID++;
																																							
														if(6 == CounterPID)
														{
																CounterPID = 0;
														}
														
														itoa(CounterPID,bufTextOrder,10);												
														TEXT_SetText( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_TEXT0),bufTextOrder);
														
														if(1 == CounterPID)
														{
																EDIT_SetFloatValue ( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3), E_Percent[1]);
														}
												
														/* Set the same value to edit */
														if( 2 == CounterPID || 4 == CounterPID)
														{																																
																EDIT_SetFloatValue(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3),E_Percent[0]);
																EDIT_SetFloatValue(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT4),value_PID[0].SetPoint);
														}
														else if( 3 == CounterPID || 5 == CounterPID)
														{
																EDIT_SetFloatValue(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3),E_Percent[1]);
																EDIT_SetFloatValue(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT4),value_PID[1].SetPoint);
														}											
												}
												
												/** Previous button **/
												if(Id == GUI_ID_BUTTON19)
												{
														value_PID[CounterPID].Proportion = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT0) );
														value_PID[CounterPID].Integral   = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT1) );
														value_PID[CounterPID].Derivative = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT2) );
														value_PID[CounterPID].SetPoint 	 = EDIT_GetFloatValue( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT4) );
													
														CounterPID--;
																																																
														if(-1 == CounterPID)
														{
																CounterPID = 5;
														}
														
														itoa(CounterPID,bufTextOrder,10);												
														TEXT_SetText( WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_TEXT0),bufTextOrder);
												}
											}
											
											if(Id == GUI_ID_SLIDER0)
											{												
													BK_Light = SLIDER_GetValue(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_SLIDER0));
													SetPWM(BK_Light,PWM_BACK_LIGHT);																									
											}
									
								 }
									break;				
						}														
        default:
            WM_DefaultProc(pMsg);
    }
}


/*********************************************************************
*
*       _cbBk_CFG_PID
*/
static void _cbBk_CFG_PID(WM_MESSAGE * pMsg) 
{
	static unsigned int GirdFlag = 0;
	int NCode, Id;
  switch (pMsg->MsgId) {
  case WM_PAINT:
		GUI_SetBkColor(GUI_BLUE);
	  GUI_Clear();
    GUI_DispStringAt("PID Configeration", 50, 10);
  // GUIDEMO_DrawBk(1);
    break;
		
	case WM_TOUCH:
		GirdFlag++;
		GRAPH_SetGridVis(hGraph,GirdFlag%3);  //!< Display the  if touch the screen
		break;

	/*******************/
	case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
      NCode = pMsg->Data.v;               /* Notification code */
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:    /* React only if released */
          if (Id == ID_CFG_PID_BTN_SWTITCH_2_CURVE)  /* ID =210 btn Button */  
					{                   	
						PID_CFG_Flag = 1;
						//WM_DeleteWindow(hGraph);
												
						GPIO_ResetBits(GPIOF,GPIO_Pin_8);
						hWN_Dialog_PID_Frame = 
				  	GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbBk_Frame, hWM_HBKWIN_CFG_PID, 0, 0);	
						
						BUTTON_SetText(btn1,"Parameter");
						
						//FRAMEWIN_SetClientColor(WM_GetDialogItem(hWN_Dialog_PID_Frame,0),GUI_LIGHTBLUE);
						
						EDIT_SetMaxLen(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT0),7);
						EDIT_SetMaxLen(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT1),7);
						EDIT_SetMaxLen(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT2),7);
						EDIT_SetMaxLen(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3),7);
						EDIT_SetMaxLen(WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3),7);
						
						WM_SetFocus(WM_GetDialogItem(hWN_Dialog_PID_Frame,PID_EDIT));

						
						//!< Set the Edit to Float mode
						{
						
								EDIT_SetFloatMode(
																	WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT0),1.0,
																	0.0,999.999,3,/*Three bit after dot*/
																	0
																	);
								EDIT_SetFloatMode(
																	WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT1),0.0,
																	0.0,999.999,3,/*Three bit after dot*/
																	0
																	);
								EDIT_SetFloatMode(
																	WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT2),0.0,
																	0.0,999.999,3,/*Three bit after dot*/
																	0
																	);
								EDIT_SetFloatMode(
																	WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT3),0.0,
																	0.0,999.999,3,/*Three bit after dot*/
																	0
																	);
								EDIT_SetFloatMode(
																	WM_GetDialogItem(hWN_Dialog_PID_Frame,GUI_ID_EDIT4),0.0,
																	0.0,100.000,3,/*Three bit after dot*/
																	0
																	);																	
																	
						}
			
						
          } 
					
					if (Id == 203)  /* ID =210 btn Button */  
					{                   	
						
						BUTTON_SetText(btnClear1,"Back");
						GPIO_SetBits(GPIOF,GPIO_Pin_8);
						WM_ShowWindow(hWM_HBKWIN_CURVE);
						WM_HideWindow(hWM_HBKWIN_CFG_PID);
          } 
				
        break;      
        default:          
				{
				}
      }
	break;
	
  default:
    WM_DefaultProc(pMsg);
  }
}



/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) 
{
	static unsigned int GirdFlag = 0;
	int NCode, Id;
  switch (pMsg->MsgId) {
  case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTBLUE);
	  GUI_Clear();
    GUI_DispStringAt("Temperature Curve ", 80, 0);
  // GUIDEMO_DrawBk(1);
    break;
	case WM_TOUCH:
		GirdFlag++;
		GRAPH_SetGridVis(hGraph,GirdFlag%3);  //!< Display the  if touch the screen
		break;

	/*******************/
	case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
      NCode = pMsg->Data.v;               /* Notification code */
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:    /* React only if released */
          if (Id == ID_CURVE_BTN_SWTITCH_2_PARA)  /* ID =210 btn Button */  
					{                   	
							ClosePWM = ! ClosePWM;
						
							if(1 == ClosePWM)
							{
									BUTTON_SetText(btn,"Closed");
							}
							else
							{
									BUTTON_SetText(btn,"Running");
							}
							
          } 	
					if (Id == 202)  /* ID =210 btn Button */  
					{                   	
						BUTTON_SetText(btnClear,"PID_Set");
						GPIO_SetBits(GPIOF,GPIO_Pin_8);
						
						PID_CFG_Flag = 1;
						
						WM_HideWindow(hWM_HBKWIN_CURVE);
				  	WM_ShowWindow(hWM_HBKWIN_CFG_PID);

          } 
        break;      
        default:          
				{
				}
      }
	break;
	
  default:
    WM_DefaultProc(pMsg);
  }
}



/*********************************************************************
*
*       MainTask
*/
void MainTask(void) 
{
		
	GUI_Init();
	
	
	WM_SetDesktopColor(GUI_BLUE);	//!< SetDesktopColor
	
	 /* The First window to display the Temperature Curve */
	{
		hWM_HBKWIN_CURVE =
		WM_CreateWindow(0,0,400,240,WM_CF_SHOW | WM_CF_MEMDEV ,0,0);
		
		btn = 		 BUTTON_CreateAsChild(315, 10, 75, 35,hWM_HBKWIN_CURVE, ID_CURVE_BTN_SWTITCH_2_PARA, WM_CF_SHOW);
		btnClear = BUTTON_CreateAsChild(315, 60, 75, 35,hWM_HBKWIN_CURVE, 202, WM_CF_SHOW);
		
		/** PWM Edit **/
		{
			/* Edit to display the PWM1 */
				Edit_PWM1 = EDIT_CreateEx			 (370, 170, 25, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_EDT_PWM1,
																				 3);
				EDIT_SetDecMode(Edit_PWM1,0,0,255,0,0);
				EDIT_SetValue(Edit_PWM1,0);
				
				/* Edit to display the PWM2 */
				Edit_PWM2 = EDIT_CreateEx			 (370, 190, 25, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_EDT_PWM2,
																				 3);
				EDIT_SetDecMode(Edit_PWM2,0,0,255,0,0);
				EDIT_SetValue(Edit_PWM2,0);
				
					/* Edit to display the PWM3 */
				Edit_PWM3 = EDIT_CreateEx			 (370, 210, 25, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_EDT_PWM3,
																				 3);
				EDIT_SetDecMode(Edit_PWM3,0,0,255,0,0);
				EDIT_SetValue(Edit_PWM3,0);
		}


		
		
		// Set Callback function for background window
		WM_SetCallback(hWM_HBKWIN_CURVE, _cbBk);
	}
		
	{
		hWM_HBKWIN_CFG_PID =
		WM_CreateWindow(0,0,400,240,WM_CF_HIDE | WM_CF_MEMDEV ,0,0); //Add the | WM_CF_MEMDEV flah
		
	

		btn1 			= BUTTON_CreateAsChild(320, 30 , 60, 35,hWM_HBKWIN_CFG_PID, ID_CFG_PID_BTN_SWTITCH_2_CURVE, WM_CF_SHOW);
		btnClear1 = BUTTON_CreateAsChild(320, 75, 60, 35,hWM_HBKWIN_CFG_PID, 203, WM_CF_SHOW);
		BUTTON_SetText(btn1,"PID_Set");
		BUTTON_SetText(btnClear1,"Back");
		
		BUTTON_SetText(btn     ,"Running");
		BUTTON_SetText(btnClear,"PID_Set");
	

		WM_SetCallback(hWM_HBKWIN_CFG_PID, _cbBk_CFG_PID);
	}
	

	
	//!< Create a graph widget
	{
		hGraph = GRAPH_CreateEx(8, 10, 300, 220, hWM_HBKWIN_CURVE, WM_CF_SHOW | WM_CF_CONST_OUTLINE | WM_CF_MEMDEV_ON_REDRAW , 0, 100);
		//GRAPH_DATA_YT_MirrorX(hGraph , 1);
		
		//!< Create a YT_data 
		hData  = GRAPH_DATA_YT_Create(GUI_BLUE, 292,0, 0); // Set the	MaxItemNum to 0	
		hData2 = GRAPH_DATA_YT_Create(GUI_RED,292,0,0);
		
		GRAPH_AttachData(hGraph, hData); //!< Attach the data to graph
		GRAPH_AttachData(hGraph, hData2); //!< Attach the data to graph
		
	}
	
	WM_ShowWindow(hWM_HBKWIN_CURVE);
	WM_HideWindow(hWM_HBKWIN_CFG_PID);
/*
	WM_BringToTop(hWM_HBKWIN);
	WM_BringToBottom(hWM_HBKWIN_CFG_PID);
	WM_MoveTo(hWM_HBKWIN_CFG_PID,400,240);
	WM_ResizeWindow(hWM_HBKWIN_CFG_PID,1,1);
*/	
	#ifdef PWM_TEXT		
			/** PWM Text **/
		{
			/* Edit to display the PWM1 */
				Text_PWM1 = TEXT_CreateEx			 (310, 170, 30, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_TXT_PWM1,
																				 "PMW1");				
				/* Edit to display the PWM2 */
				Text_PWM2 = TEXT_CreateEx			 (310, 190, 30, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_TXT_PWM2,
																				 "PWM2");
					/* Edit to display the PWM3 */
				Text_PWM3 = TEXT_CreateEx			 (310, 210, 30, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_TXT_PWM3,
																				 "PWM3");
		}
#endif
		
#ifdef EDIT_AD		
	 {
				/* Edit to display the AD1 */
				Edit_AD1 = EDIT_CreateEx			 (350, 120, 35, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_EDT_AD1,
																				 5);
				EDIT_SetDecMode(Edit_AD1,0,0,4095,0,0);
				EDIT_SetValue(Edit_AD1,0);
				
				/* Edit to display the AD2 */
				Edit_AD2 = EDIT_CreateEx			 (350, 140, 35, 20,hWM_HBKWIN_CURVE, 
																				 WM_CF_SHOW,EDIT_CF_HCENTER | EDIT_CF_VCENTER,ID_CFG_PID_EDT_AD2,
																				 5);
				EDIT_SetDecMode(Edit_AD2,0,0,4095,0,0);
				EDIT_SetValue(Edit_AD2,0);
		 

				EDIT_SetFloatMode(
													Edit_AD1,1.0,
													0.0,99.9,1,/*1 bit after dot*/
													0
													);
				EDIT_SetFloatMode(
													Edit_AD2,0.0,
													0.0,99.9,1,/*1 bit after dot*/
													0
													);
	 }
#endif		

	//!< Graph configuration
	{
		hScale = GRAPH_SCALE_Create(28, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
		GRAPH_SCALE_SetTextColor(hScale,GUI_GREEN);
		GRAPH_SCALE_SetFactor(hScale,0.5);
		GRAPH_SCALE_SetNumDecs(hScale,1);
		GRAPH_AttachScale(hGraph, hScale);
		
		GRAPH_SetGridDistX(hGraph,0); // Hide the X Grid
		GRAPH_SetGridDistY(hGraph,20);	
		GRAPH_SetGridVis(hGraph,1);  //!< Display the Grid
		
		GRAPH_SetBorder(hGraph,30,5,5,5);  //!< Set the frame border 
		
		//GRAPH_SetGridFixedX(hGraph,1);
		//GRAPH_SetVSizeX(hGraph,500);  //加入此会导致死掉 程序卡死在获取此指针上面.
		//GRAPH_SetGridFixedY(hGraph,1);
		GRAPH_SetColor(hGraph,GUI_BLUE,GRAPH_CI_BORDER);
		GRAPH_SetColor(hGraph,GUI_WHITE,GRAPH_CI_BK);
		GRAPH_SetColor(hGraph,GUI_WHITE,GRAPH_CI_FRAME);

	}	
	/* Button Configuration */
	{
	
		BUTTON_SetBkColor(btn,BUTTON_CI_UNPRESSED,GUI_LIGHTBLUE);
		BUTTON_SetBkColor(btn,BUTTON_CI_PRESSED,GUI_RED);
			
		BUTTON_SetFocusColor(btn,GUI_YELLOW);	
	}
			
  while(1) 
	{
		GUI_Exec();	

	 	OSTimeDlyHMSM(0,0,0,50);
//#define DRAWPOINT
#ifdef DRAWPOINT
		
		itoa(usValue, buf, 10);
		BUTTON_SetText(btn,buf);

	//	GRAPH_DATA_YT_AddValue(hData,usValue/20);
#endif		
	}
}
