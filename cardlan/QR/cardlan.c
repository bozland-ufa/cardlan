#include "apparel.h"

void test_cardlan()
{
	if(Open_Frambuffer( "/dev/fb0")==MI_OK)
	{
		Set_Background(NULL,Color_red,0);
		usleep(20000);

        TextOut(0,10,  "system initialization....",GB2312_16);
        
    }


    Close_Frambuffer();
}
