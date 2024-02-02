/**
 * @file Shooting_Control.c
 * @author Leo Liu
 * @brief control shooting
 * @version 1.0
 * @date 2022-07-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "Shooting_Control.h"

Shooting_t Shooting;
Ramp_Calc_t Fric_Wheel_Ramp;

void Shooting_Init(void);
void Turn_Friction_Wheel_On(void);
void Turn_Friction_Wheel_Off(void);
void Trigger_Get_Data(Shooting_t *Shooting);
void Shooting_Processing(Shooting_t *Shooting);
void Shooting_Disabled(void);

Shooting_Func_t Shooting_Func = Shooting_Func_GroundInit;
#undef Shooting_Func_GroundInit

void Shooting_Init(void)
{
	Shooting.Fric_Wheel.Turned_On = 0;
	Ramp_Calc_Func.Clear_Ramp(&Fric_Wheel_Ramp);
	Shooting.Fric_Wheel.Target_Speed = FRIC_SPEED_12;
}

void Trigger_Get_Data(Shooting_t *Shooting)
{
	if(State_Machine.Control_Source == Remote_Control)
	{
		//Dial wheel up for single fire
		if(DR16_Export_Data.Remote_Control.Dial_Wheel < -50)
		{
			Shooting->Type.Single_Fire_Flag = 1;
			Shooting->Type.Burst_Flag = 0;
		}
		
		//Dial wheel down for burst
		else if(DR16_Export_Data.Remote_Control.Dial_Wheel > 50)
		{
			Shooting->Type.Single_Fire_Flag = 0;
			Shooting->Type.Burst_Flag = 1;
		}
		
		//Return to middle for reset
		else
		{
			Shooting->Type.Single_Fire_Flag = 0;
			Shooting->Type.Burst_Flag = 0;
			Shooting->Type.Single_Fired_Flag = 0;
		}
	}
	
	//Left click for single fire, holding it for burst
	else if(State_Machine.Control_Source == Computer)
	{
		if(DR16_Export_Data.Mouse.Left_Click == 1)
		{
			DR16_Export_Data.Mouse.Click_Counter++;
			
			if(DR16_Export_Data.Mouse.Click_Counter < 10)
			{
				Shooting->Type.Single_Fire_Flag = 1;
				Shooting->Type.Burst_Flag = 0;
			}
			
			else if(DR16_Export_Data.Mouse.Click_Counter >= 10)
			{
				Shooting->Type.Single_Fire_Flag = 0;
				Shooting->Type.Burst_Flag = 1;
			}
		}
		
		else
		{
			DR16_Export_Data.Mouse.Click_Counter = 0;
			Shooting->Type.Single_Fire_Flag = 0;
			Shooting->Type.Burst_Flag = 0;
		}
	}
}

void Shooting_Processing(Shooting_t *Shooting)
{
	if(Shooting->Fric_Wheel.Turned_On)
		Turn_Friction_Wheel_On();
	else
		Turn_Friction_Wheel_Off();
}

void Turn_Friction_Wheel_On(void)
{
	//Slowly ramp up the target speed to protect the motor and allows reaction time for accidental triggering
	M3508_Fric_Wheel[0].Target_Speed = FRIC_LEFT_DIRECTION * Shooting.Fric_Wheel.Target_Speed;
	M3508_Fric_Wheel[1].Target_Speed = FRIC_RIGHT_DIRECTION * Shooting.Fric_Wheel.Target_Speed;
	M3508_Fric_Wheel[2].Target_Speed = FRIC_BOTTOM_DIRECTION * Shooting.Fric_Wheel.Target_Speed;
	
	M3508_Fric_Wheel[0].Output_Current = PID_Func.Positional_PID(&Fric_Wheel_PID, M3508_Fric_Wheel[0].Target_Speed, M3508_Fric_Wheel[0].Actual_Speed);
	M3508_Fric_Wheel[1].Output_Current = PID_Func.Positional_PID(&Fric_Wheel_PID, M3508_Fric_Wheel[1].Target_Speed, M3508_Fric_Wheel[1].Actual_Speed);
	M3508_Fric_Wheel[2].Output_Current = PID_Func.Positional_PID(&Fric_Wheel_PID, M3508_Fric_Wheel[2].Target_Speed, M3508_Fric_Wheel[2].Actual_Speed);
	
	if(Fric_Wheel_Ramp.Ramp_Finished_Flag)	
		Shooting.Fric_Wheel_Ready_Flag = 1;
}

void Turn_Friction_Wheel_Off(void)
{
	Shooting.Fric_Wheel_Ready_Flag = 0;
		
	M3508_Fric_Wheel[0].Output_Current = 0;
	M3508_Fric_Wheel[1].Output_Current = 0;
	M3508_Fric_Wheel[2].Output_Current = 0;
}

void Shooting_Disabled(void)
{
	Shooting.Fric_Wheel.Turned_On = 0;
	Ramp_Calc_Func.Clear_Ramp(&Fric_Wheel_Ramp);
	DR16_Export_Data.Mouse.Click_Counter = 0;
	Shooting.Type.Single_Fire_Flag = 0;
	Shooting.Type.Burst_Flag = 0;
}
