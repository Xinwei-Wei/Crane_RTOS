#include "menu.h"
#include "oled.h"

void ShowMenu(){
	OLED_ShowString(10,0,"durian1 ",12);
	OLED_ShowString(10,12,"durian2 ",12);
	OLED_ShowString(10,24,"durian3 ",12);
	OLED_ShowString(10,36,"durian4 ",12);
	OLED_ShowString(10,48,"durian5 ",12);
	OLED_ShowString(100,48,"back",12);
}

void ShowTask(u8 a){
	switch(a+1){
		case 1:{ OLED_ShowString(10,0,"durian11",12);
						 OLED_ShowString(10,12,"        ",12);
						 OLED_ShowString(10,24,"        ",12);
						 OLED_ShowString(10,36,"        ",12);
						 OLED_ShowString(10,48,"        ",12);
			       OLED_ShowString(100,48,"back",12);
						 break;	}
		case 2:{ OLED_ShowString(10,0,"durian21",12);
			       OLED_ShowString(10,12,"        ",12);
						 OLED_ShowString(10,24,"        ",12);
						 OLED_ShowString(10,36,"        ",12);
						 OLED_ShowString(10,48,"        ",12);
			       OLED_ShowString(100,48,"back",12);
						 break;	}
		case 3:{ OLED_ShowString(10,0,"durian31",12);
						 OLED_ShowString(10,12,"        ",12);
						 OLED_ShowString(10,24,"        ",12);
						 OLED_ShowString(10,36,"        ",12);
						 OLED_ShowString(10,48,"        ",12);
			       OLED_ShowString(100,48,"back",12);
						 break;	}
		case 4:{ OLED_ShowString(10,0,"durian41",12);
			       OLED_ShowString(10,12,"        ",12);
						 OLED_ShowString(10,24,"        ",12);
						 OLED_ShowString(10,36,"        ",12);
						 OLED_ShowString(10,48,"        ",12);
			       OLED_ShowString(100,48,"back",12);
						 break;	}
		case 5:{ OLED_ShowString(10,0,"durian51",12);
			       OLED_ShowString(10,12,"        ",12);
						 OLED_ShowString(10,24,"        ",12);
						 OLED_ShowString(10,36,"        ",12);
						 OLED_ShowString(10,48,"        ",12);
			       OLED_ShowString(100,48,"back",12);
						 break;	}
	}
}

void ShowSelect(char select){
	switch(select+1){
		case 1 :{ OLED_ShowChar(0,0,'*',12,1);
							OLED_ShowChar(0,12,' ',12,1);
							OLED_ShowChar(0,24,' ',12,1);
							OLED_ShowChar(0,36,' ',12,1);
							OLED_ShowChar(0,48,' ',12,1);
							OLED_ShowChar(88,48,' ',12,1);
							break;}
		case 2 :{ OLED_ShowChar(0,0,' ',12,1);
							OLED_ShowChar(0,12,'*',12,1);
							OLED_ShowChar(0,24,' ',12,1);
							OLED_ShowChar(0,36,' ',12,1);
							OLED_ShowChar(0,48,' ',12,1);
							OLED_ShowChar(88,48,' ',12,1);
							break;}
		case 3 :{ OLED_ShowChar(0,0,' ',12,1);
							OLED_ShowChar(0,12,' ',12,1);
							OLED_ShowChar(0,24,'*',12,1);
							OLED_ShowChar(0,36,' ',12,1);
							OLED_ShowChar(0,48,' ',12,1);
							OLED_ShowChar(88,48,' ',12,1);
							break;}
		case 4 :{ OLED_ShowChar(0,0,' ',12,1);
							OLED_ShowChar(0,12,' ',12,1);
							OLED_ShowChar(0,24,' ',12,1);
							OLED_ShowChar(0,36,'*',12,1);
							OLED_ShowChar(0,48,' ',12,1);
							OLED_ShowChar(88,48,' ',12,1);
							break;}
		case 5 :{ OLED_ShowChar(0,0,' ',12,1);
							OLED_ShowChar(0,12,' ',12,1);
							OLED_ShowChar(0,24,' ',12,1);
							OLED_ShowChar(0,36,' ',12,1);
							OLED_ShowChar(0,48,'*',12,1);
							OLED_ShowChar(88,48,' ',12,1);
							break;}
		case 6 :{ OLED_ShowChar(0,0,' ',12,1);
							OLED_ShowChar(0,12,' ',12,1);
							OLED_ShowChar(0,24,' ',12,1);
							OLED_ShowChar(0,36,' ',12,1);
							OLED_ShowChar(0,48,' ',12,1);
							OLED_ShowChar(88,48,'*',12,1);
							break;}
	}
}

