#ifndef __KEYBOARD__H__
#define __KEYBOARD__H__

#define kEYBOARD_PATH  "/dev/input/event0"

/*------------function: open keyboard device------------Parameter: Path Button device node------------Return value: successful return 0 error return -1*/
int Open_KeyBoard(char * path);/*------------function: gain button value------------Paramter: null------------Return value: successful return 0 error return -1*/
int Get_KeyCode(void);/*------------function: close keyboard device------------Pameter: null------------0 Return value: successful return 0 */
int CloseKeyboard(void);




#endif