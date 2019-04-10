/********************************************************************************************************
**                                 Copyright (c)          MEI14
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F429ZI (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: 
**  Module Date: 2014-
**  Module Auth: 
**  Description: 
**  Version    : V1.0
**  Notes      : 
**-------------------------------------------------------------------------------------------------------
**  Version    : 
**  Modified By: 
**  Date       : 
**  Content    : 
**  Notes      : 
********************************************************************************************************/
/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "xbm.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2015-03-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
float xbm_asin(float x)
{
	float y;
	int invFlag = 0;
	if(x < 0)
	{
		x = -x;
		invFlag = 1;
	}
//	if(x<0.9f)
//	{
//		y = ((((( 2.367151670267223f*x - 5.007707733447045f)*x +  4.251681043536677f)*x - 1.511005895260145f)*x
//			+0.307915153002358f)*x + 0.978017055636465f)*x + 0.000333171279375f;
//	}else if(x < 1.0f)
//	{
//		y = ((((( 7813615.2190167f*x - 44192058.1066729f)*x +  104120992.9791892f)*x - 130810839.4518392f)*x
//			+92423821.5684885f)*x - 34820476.5983619f)*x + 5464945.9606194f;
//	}else
//	{
//		y = 1.57;
//	}
  if(x<0.9f)
	{
		y = ((((( 2.367151670267223f*x - 5.007707733447045f)*x +  4.251681043536677f)*x - 1.511005895260145f)*x
			+0.307915153002358f)*x + 0.978017055636465f)*x + 0.000333171279375f;
	}else if(x<0.99f)
  {
    y = ((((( 169418.006076744f*x - 927425.420298533f)*x +  2115026.990318988f)*x - 2572008.675396274f)*x
			+1759001.602170400f)*x - 641456.964349098f)*x + 97445.967822562f;
  }else if(x<0.999f)
  {
    y = ((((( 23841133503.0992f*x - 140942414468.9369f)*x +  347150818400.3403f)*x - 456001097788.1824f)*x
			+ 336905530125.0671f)*x - 132745769131.5239f)*x + 21791799361.6876f;
  }else
  {
    y = 1.4915;
  }
	if(invFlag == 1)
	{
		return -y;
	}else
	{
		return y;
	}
}

float xbm_atan(float x)
{
	float y;
	int invFlag = 0;
	if(x < 0.0f)
	{
		x = -x;
		invFlag = 1;
	}
	if(x < 1.0f)
	{
		y = (((-0.03825f*x*x + 0.14498f)*x*x - 0.32053f)*x*x + 0.99913f)*x;
	}else if(x < 3.0f)
	{
		y= ((0.03214001f*x - 0.28140609f)*x + 0.93934992f)*x + 0.09782516f;
	}else if(x < 20.0f)
	{
		y = ((((( -0.000000121964262f*x + 0.000009637007236f)*x - 0.000312307892459f)*x + 0.005353125313825f)*x
			-0.052153209590924f)*x + 0.287835099268528f)*x + 0.735982162169589f;
	}else if(x < 100.0f)
	{
		y = ((((( -0.000000000000992f*x + 0.000000000409265f)*x - 0.000000069742821f)*x + 0.000006342069404f)*x
			-0.000331071339679f)*x + 0.009885048057013f)*x + 1.414935610187774f;
	}else
	{
		y = 1.4915;
	}
	if(invFlag == 1)
	{
		return -y;
	}else
	{
		return y;
	}
}

float xbm_atan2(float x, float y)
{
  float temp = y/x;
  //求值
  temp = xbm_atan(temp);
  //换算象限
  if( (x < 0) && (y > 0) )
  {
    temp += PI;
  }else if( (x<0) && (y<0) )
  {
    temp += -PI;
  }
  return temp;  
}
//===========================================  End Of File  ===========================================//