#include "m328_rfid.h"
#include "rc522.h"


/*******************************
*Port config
*1--SS  <----->PB0
*2--SCK <----->PB5
*3--MOSI<----->PB3
*4--MISO<----->PB4
*5--NA
*6--GND <----->GND
*7--RST <----->PB1
*8--VCC <----->VCC
************************************/



unsigned char CT[2];//
unsigned char SN[4]; //
uint8_t KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};

void ShowID(uint16_t x,uint16_t y, uint8_t *p, uint16_t charColor, uint16_t bkColor);	 //
void PutNum(uint16_t x,uint16_t y, int n1,uint8_t n0, uint16_t charColor, uint16_t bkColor);	//
void Store(uint8_t *p,uint8_t store,uint8_t cash);//
uint8_t ReadData(uint8_t   addr,uint8_t *pKey,uint8_t *pSnr,uint8_t *dataout);
uint8_t WriteData(uint8_t   addr,uint8_t *pKey,uint8_t *pSnr,uint8_t *datain);

void main2()
{	
	unsigned char status;
	uint8_t Data[16];
	uint8_t i;
	uint8_t k;//��д�������Դ���

	//RCC_Configuration(); //ARMʱ�ӳ�ʼ��
	//NVIC_Configuration();//�жϳ�ʼ��
	//GPIO_Configuration();//led��ʼ��
	//UART_Init(72,115200);//���ڳ�ʼ��
	printf("usart is ok\n");//����������ݣ���ʾ��Ƭ�������ڹ�������
	//LCD_Init();					//Һ������ʼ��
	//LCD_Clear(WHITE);			//Һ��������

	InitRc522();				//��ʼ����Ƶ��ģ��
#if 0
//	GPIOC->BSRR=0X06;//led0 on
	while(1)													 
	{		
		
		if(!(USART_RX_STA&0x80))//���ڷ��͵�����ȫ��������USART_RX_BUF�У������ж�ʵ�֣�ÿ��������ɺ�USART_RX_STA���λ��1
		{
			status = PcdRequest(PICC_REQALL,CT);/*ɨ�迨*/
			status = PcdAnticoll(SN);/*����ײ*/
			if (status==MI_OK)
			{
				GPIOC->BRR=0x06;   //����ledȫ������
				//ShowID(0,100,SN,BLUE,WHITE); //��Һ��������ʾ����ID��
				//printf("ID:%02x %02x %02x %02x\n",sn[0],sn[1],sn[2],sn[3]);//���Ϳ���
			}	
			else
			{
				GPIOC->BSRR=0X06;//ֻ����led0
			}
		}
		else
		{
			status=0x02;//���ó�ʼֵ
			k=5;		 //�����ظ����������¶���һ�β����ܳɹ�
			do
		    {
				status=PcdRequest(PICC_REQALL,CT);/*ɨ�迨*/
				k--;						      
		    }while(status!=MI_OK && k>0);

			status=0x02;//
			k=5;
			do
		    {
				status=PcdAnticoll(SN);/*����ײ*/
				k--;						      
		    }while(status!=MI_OK && k>0);

			status=0x02;//
			k=20;
			do
		    {
				status=PcdSelect(SN);//ѡ��
				k--;						      
		    }while(status!=MI_OK && k>0);

			if(status!=MI_OK)
			{
				printf("PcdSelect is wrong\n");
			}
			switch (USART_RX_BUF[0])
			{
				case 0xa0:  printf("ID:%02x %02x %02x %02x\n",SN[0],SN[1],SN[2],SN[3]);//���Ϳ���					  
							break;
				case 0xa1://�����ݣ�һ�㲻�ã����ڲ����Լ��Կ����������ʱʹ��
							status=ReadData(USART_RX_BUF[7],&USART_RX_BUF[1],SN,Data);
							if(status==MI_OK)
						    {
								printf("ReadData:");
								for(i=0;i<16;i++)
							    {
							      printf("%02x ",Data[i]);
							    }
								printf("\n\n");
							    PcdHalt();
							}
							else
							{
								printf("ReadData is wrong\n");
							}	
							break;
				case 0xa2://д���ݣ�һ�㲻��
							status=WriteData(USART_RX_BUF[7],&USART_RX_BUF[1],SN,&USART_RX_BUF[8]);
						    if(status!=MI_OK)
						    {
								printf("WriteData is wrong\n");						      
						    }
							else
							printf("WriteData is ok\n");
							break;
				case 0xa3://�޸�����
							break;
				case '+' ://����money
							Store(&USART_RX_BUF[1],1,1);
							break;
				case '-' ://����money
							Store(&USART_RX_BUF[1],0,1);
							break;
				case '=' ://�鿴money
							Store(&USART_RX_BUF[1],1,0);
							break;
				default:
							break;
			}
			USART_RX_STA=0;//���ڽ�����ɱ�־����
		}
	}
#endif	

}/*************************************
*�������ܣ���ʾ���Ŀ��ţ���ʮ��������ʾ
*������x��y ����
*		p ���ŵĵ�ַ
*		charcolor �ַ�����ɫ
*		bkcolor   ��������ɫ
***************************************/
void ShowID(uint16_t x,uint16_t y, uint8_t *p, uint16_t charColor, uint16_t bkColor)	 //��ʾ���Ŀ��ţ���ʮ��������ʾ
{
	uint8_t num[9];
	uint8_t i;

	for(i=0;i<4;i++)
	{
		num[i*2]=p[i]/16;
		num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
		num[i*2+1]=p[i]%16;
		num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
	}
	num[8]=0;

	DisplayString(x,y,"The Card ID is:",charColor,bkColor);
	DisplayString(x,y+16,num,charColor,bkColor);
}
/********************************
*�������ܣ���p��n����
*/
int power(uint8_t p,uint8_t n)
{
	int pow=1;
	uint8_t i;
	for(i=0;i<n;i++)
	{
		pow*=p;	
	}
	return pow;
}
/******************************************
*�������ܣ�ʵ�ֿ��ڽ��Ĳ�ѯ����ֵ�Լ��۷�
*������	p    --���
*		store--storeΪ1��Ϊ��ֵ��
*				storeΪ0��Ϊ�۷�
*		cash --cashΪ1����Ϊ��ֵ��۷�
*				cashΪ0�����ѯ���
*******************************************/
void Store(uint8_t *p,uint8_t store,uint8_t cash)
{
	uint8_t data[16];
	uint8_t len=0;
	uint8_t i=0;
	uint32_t dec=0;
	uint8_t status;
	uint8_t tmp;

	status=ReadData(0x04,KEY,SN,data);
	dec=data[11];
	dec<<=8;
	dec+=data[12];
	dec<<=8;
	dec+=data[13];
	dec<<=8;
	dec+=data[14];

	if(!cash)
	{
		//LCD_Fill(0,148,240,16,WHITE);
		//DisplayString(0,148,"The Balance is:",RED,WHITE);
		//PutNum(120,148,dec,data[15],RED,WHITE);
	}
	else
	{
		while(p[len]!='.')
		{
			len++;
		}

		if(store)
		{
			data[15]+=p[len+1]-'0';
			if(data[15]>9)
			{
				data[15]-=10;
				dec++;
			}
			while(len--)
			{
				dec+=(p[len]-'0')*power(10,i);
				i++;
					
			}
		}
		else
		{
			if(data[15]<p[len+1]-'0')
			{
				data[15]+=10;
				dec--;
			}
			data[15]-=p[len+1]-'0';
			
			while(len--)
			{
				if(dec<(p[len]-'0')*power(10,i))
				{
					//DisplayString(0,164,"You do not have enough money!",RED,WHITE);
					goto End;	
				}
				dec-=(p[len]-'0')*power(10,i);
				i++;
					
			}
		}
	
		//LCD_Fill(0,164,240,16,WHITE);
		//DisplayString(0,164,"The Balance is:",RED,WHITE);
		//PutNum(120,164,dec,data[15],RED,WHITE);
	
		data[14]=dec&0xff;
		data[13]=(dec>>8)&0xff;
		data[12]=(dec>>16)&0xff;
		data[11]=(dec>>24)&0xff;
	
End:	status=WriteData(0X04,KEY,SN,data);

	
	}
}

uint8_t ReadData(uint8_t   addr,uint8_t *pKey,uint8_t *pSnr,uint8_t *dataout)
{
	uint8_t status,k;
	status=0x02;//
	k=5;
	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
		//printf("AuthState is wrong\n");						      
    }while(status!=MI_OK && k>0);

	status=0x02;//
	k=5;
	do
    {
		status=PcdRead(addr,dataout);
		k--;
		//printf("ReadData is wrong\n");							      
    }while(status!=MI_OK && k>0);
	return status;
}
uint8_t WriteData(uint8_t   addr,uint8_t *pKey,uint8_t *pSnr,uint8_t *datain)
{
	uint8_t status,k;
	status=0x02;//
	k=5;
	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
		//printf("AuthState is wrong\n");						      
    }while(status!=MI_OK && k>0);

	status=0x02;//
	k=5;
	do
    {
		status=PcdWrite(addr,datain);
		k--;
		//printf("ReadData is wrong\n");							      
    }while(status!=MI_OK && k>0);
	return status;
}
void PutNum(uint16_t x,uint16_t y, int n1,uint8_t n0, uint16_t charColor, uint16_t bkColor)
{
	uint8_t tmp[13];
	uint8_t i;

	//LCD_SetRegion(0,0,239,319,FALSE);
	tmp[0]=n1/1000000000+'0';
	for(i=1;i<10;i++)
	{
		tmp[i]=n1/(1000000000/power(10,i))%10+'0';
	}
	tmp[10]='.';
	tmp[11]=n0+'0';
	tmp[12]=0;
	DisplayString(x,y,tmp,charColor,bkColor);


	
}
