#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
 

/*���淽ʽ��ͨ��λ��������ʽ��ȡIO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


#define key1  PEin(4) 
#define key2  PEin(3) 
#define key3  PEin(11) 
#define key4  PAin(0) 

void KEY_Init(void);
u8 KEY_Scan(u8 mode);

#endif
