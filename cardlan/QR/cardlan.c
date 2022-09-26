#include "apparel.h"

void test_cardlan()
{
	if(Open_Frambuffer( "/dev/fb0")==MI_OK)
	{
        TextOut(0,10,  "system initialization....",GB2312_16);
    }

    Close_Frambuffer();
}
