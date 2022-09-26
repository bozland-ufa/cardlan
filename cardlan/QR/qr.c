#include "apparel.h"

//QR code part
int uart0_fd = 0;
unsigned char Uart0_buff[1024];
unsigned char Uart0_buff_len;
unsigned char ur0RevFlag = 0;
unsigned char fgNoDisplayRemain = 0;
extern int uart4_fd;
extern int uart5_fd;


int speed_arr[] =
{
    B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
    B38400, B19200, B9600,  B4800, B2400, B1200,B600, B300,
};

int name_arr[] =
{
    115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
    38400, 19200, 9600, 4800, 2400,  1200,600, 300,
};

/*
*************************************************************************************************************
- Function name : int OpenDev (char *Dev)
- Function explanation : Set serial attribute
- Input parameter : Null
- Output parameter : Null
*************************************************************************************************************
*/
void set_speed (int fd, int speed)
{
    int i;
    int status;
    struct termios Opt;
    tcgetattr (fd, &Opt);
    for (i = 0; i < sizeof (speed_arr) / sizeof(int); i++)
    {
        if(speed == name_arr[i])
        {
            tcflush (fd, TCIOFLUSH);
            if(cfsetispeed (&Opt, speed_arr[i])==0)
            {
#if PrintDis
                printf("set_speed sucess ");
#endif
            }
            else
            {
            
#if PrintDis
                printf("set speed failed");
#endif
             return;
            }
            cfsetospeed (&Opt, speed_arr[i]);
            status = tcsetattr (fd, TCSANOW, &Opt);
            if (status != 0)
            {
                perror ("tcsetattr fd1");
                return;
            }
            tcflush (fd, TCIOFLUSH);
        }
    }
}

/*
*************************************************************************************************************
- Function name : int OpenDev (char *Dev)
- Function explanation : Set serial attribute
- Input parameter : Null
- Output parameter : Null
*************************************************************************************************************
*/
int set_Parity (int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr (fd, &options) != 0)
    {
        perror ("uart SetupSerial 1");
        return (-1);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf (stderr, "uart Unsupported data size\n");
        return (-1);
    }
    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;		/* Clear parity enable */
        options.c_iflag &= ~INPCK;		/* Enable parity checking */
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;		/* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;		/* Enable parity */
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;		/* Disnable parity checking */
        break;
    case 'S':
    case 's':					/*as no parity */
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf (stderr, "uart Unsupported parity\n");
        return (-1);
    }
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;

    case 2:
        options.c_cflag |= CSTOPB;
        break;

    default:
        fprintf (stderr, "uart Unsupported stop bits\n");
        return (-1);
    }
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush (fd, TCIFLUSH);
    options.c_cc[VTIME] = 50;
    options.c_cc[VMIN] = 0;				/* Update the options and do it NOW */

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~( ICRNL | IXON);
    options.c_oflag &= ~( ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    if (tcsetattr (fd, TCSANOW, &options) != 0)
    {
        perror ("uart SetupSerial 3");
        return (-1);
    }
    return (0);
}

/*
*************************************************************************************************************
- Function name : int OpenDev (char *Dev)
- Function explanation : Open driver
- Input parameter : Null
- Output parameter : Null
*************************************************************************************************************
*/
int OpenDev (char *Dev)
{
    int fd = open (Dev, O_RDWR );
    if (fd == -1)
    {
        perror("Uart_Readdata open()");
        return -1;
    }
    else
    {
        return fd;
    }
}

/*
*************************************************************************************************************
- Function name : int initialize (const char *com, int speed)
- Function explanation : Initialize the serial port driver
- Input parameter : Null
- Output parameter : Null
*************************************************************************************************************
*/
int initialize (const char *com, int speed)
{
    char serial[16] = "/dev/";
    strcat(serial, com);

    int fd = OpenDev (serial);
    if (fd > 0)
    {
        set_speed (fd, speed);
        if (set_Parity (fd, 8,1,'n') == -1)
        {
            close(fd);
            printf ("uart set_parity error\n");
        }
        return fd;
    }
    return -1;
}

/*
*************************************************************************************************************
- Function name : int ReadOrWriteFile (unsigned char RW_Type)
- Function explanation : Write file mode
- Input parameter : Null
- Output parameter : Null
*************************************************************************************************************
*/

int Uart_Printf(int fd,char *uartdata)
{
    int ret;
    ret= write(fd,uartdata,strlen(uartdata));

#if PrintDis
    printf("Uart_Printf == %02d\n",ret);
#endif

    return ret;
}


/*
*************************************************************************************************************
- Function name : void uninitialize (int fd)
- Function explanation : Close the serial port driver
- Input parameter : Null
- Output parameter : Null
*************************************************************************************************************
*/
void uninitialize (int fd)
{
    close (fd);
}

void InitUart(int *uartfd,char *uart,int speed)
{
int fd;

    fd = initialize(uart,speed);
    tcflush(fd,TCIFLUSH);
    Uart_Printf(fd,"Uart  test \n");
	*uartfd = fd;

#if PrintDis
    printf("Inint  Uart OK \n");
#endif

}

//QR code part


int read_uart0_data(int fd,char *rcv_buf,int *len)	//Each time data read from the serial port will have two-byte package tail
{
	int retval;
	fd_set rfds;
	struct timeval tv;
	int ret=0,pos=0;
	tv.tv_sec = 1;//set the rcv wait time 0
	tv.tv_usec = 0;//200000us = 0.3s 100000
	pos = 0;
	if ((fd < 0) ||( NULL == rcv_buf))
	{
		printf("Read Buffer is Nc\n");
		return -1;
	}
	
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		retval = select(fd+1,&rfds,NULL,NULL,&tv);
		if(retval ==-1)
		{
			perror("select()");
			return -1;
		}
		else if(retval)
		{
			ret= read(fd, rcv_buf+pos, 128); 
			//ret= read(fd, rcv_buf+pos, 22); //20
			
			if(ret>0)
			{
				printf("Read Buffer = %d\n", ret);
				pos += ret;
				*len = pos;
				//if (pos >= 20)	 //20
				//	return 0;
			}
			else
				return -1;
		}
		else	  //timeout
		{
				return 1;
		}
	}
	return 0;
}

void test_qr()
{
	int i, iLen=0;
	char rx_string[640] = {0};
#if 1
	InitUart(&uart0_fd, "ttyC0", 115200);
#else
	uart0_fd = initializegps("ttyC2", 115200);
	tcflush (uart0_fd, TCIFLUSH);
#endif
	printf("welcome use ttyC0 = %d\n", uart0_fd);
	
#if 1
	//16 4d 0d 54 52 47 4D 4F 44 33 2E
	rx_string[0] = 0x16;
	rx_string[1] = 0x4d;
	rx_string[2] = 0x0d;
	rx_string[3] = 0x54;
	rx_string[4] = 0x52;
	rx_string[5] = 0x47;
	rx_string[6] = 0x4d;
	rx_string[7] = 0x4f;
	rx_string[8] = 0x44;
	rx_string[9] = 0x33;
	rx_string[10] = 0x2e;
	write(uart0_fd, rx_string, 11); //auto mode
#endif
	
	while (1)
	{
		iLen = 0;
		signal(SIGPIPE, SIG_IGN);  //Turn off SIGPIPE signal, anti-crash
		memset(rx_string, 0, sizeof(rx_string));
		read_uart0_data(uart0_fd, rx_string, &iLen);
	#if 1
		printf("iLENGTH = %d\n", iLen);
		printf("starts ....\n");
		for (i=0; i<iLen; i++)
			printf("0x%02x ", rx_string[i]);
		printf("\n uart2 end ....\n");
	#endif
		
		//if (iLen >= 20)
		if(iLen>0)
		{		
			if (iLen == 20)
			{
				printf("recive numbers is equale to 20\n");
				for (i=0; i<18; i++)
				{
					if ((rx_string[i] < '0') || (rx_string[i] > '9'))
						break;
				}
				
				if ((i == 18) && (rx_string[18] == 0x0d) && (rx_string[19] == 0x0a))
				{
					memset(Uart0_buff, 0, sizeof(Uart0_buff));
					memcpy(Uart0_buff, rx_string, 18);
					Uart0_buff_len=18;
					ur0RevFlag = 1;
				}
			}
			else
			{
				printf("recvie numbers is not equale to 20\n");				
				if((rx_string[iLen-2]==0x0d) && (rx_string[iLen-1]==0x0a))
				{
					memset(Uart0_buff, 0, sizeof(Uart0_buff));
					memcpy(Uart0_buff, rx_string, iLen-2);
					Uart0_buff_len=iLen-2;
					ur0RevFlag = 1;
				}
			}
		}
		
		usleep(200000);
	}

	if (uart0_fd) 
		close(uart0_fd);

}



