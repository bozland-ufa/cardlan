#ifndef __NET__H__
#define __NET__H__


/*
------------Function: Use pppd dialing, connect 2g network
------------Parameter: Null
------------Return Value: Successfully returns 0, non-zero error
*/
int Gprs_Callpppd(void);

/*
------------Function: Close pppd dialing
------------Parameter: Null
------------Return Value: Successfully returns 0
*/
int Gprs_Killpppd(void);



#endif
