#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
 

/*下面方式是通过位带操作方式读取IO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


#define key1  PEin(9) 
#define key2  PEin(10) 
#define key3  PEin(11) 
#define key4  PEin(12) 

void KEY_Init(void);
u8 KEY_Scan(u8 mode);

#endif
