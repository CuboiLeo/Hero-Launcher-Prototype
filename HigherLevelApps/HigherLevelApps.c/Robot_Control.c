/**
 * @file Robot_Control.c
 * @author Leo Liu
 * @brief general robot control
 * @version 1.0
 * @date 2022-07-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "Robot_Control.h"

void Robot_Control_Start(void); 
void Robot_Control_Send(void);
void Robot_Control_Disabled(void);

Robot_Control_Func_t Robot_Control_Func = Robot_Control_Func_GroundInit;
#undef Robot_Control_Func_GroundInit

Robot_Mode_t Robot_Mode;

//The function name tells you what it does
void Robot_Control_Start(void)
{
	State_Machine_Func.Remote_Control_Update(); 
	
	Shooting_Func.Trigger_Get_Data(&Shooting);
	Shooting_Func.Shooting_Processing(&Shooting);
	
	Robot_Control_Func.Robot_Control_Send();
}

void Robot_Control_Send(void)
{
	M3508_Func.M3508_Fric_Wheel_Send_Data(M3508_Fric_Wheel[0].Output_Current,M3508_Fric_Wheel[1].Output_Current,M3508_Fric_Wheel[2].Output_Current);
}

void Robot_Control_Disabled(void)
{
	Shooting_Func.Shooting_Disabled();
}
