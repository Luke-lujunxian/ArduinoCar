/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/

int val;//�������val
#include <TimerOne.h> //�������ļ�
//��TCS3200��ɫ��������������������Arduino���ֶ˿�
#define S0    6   //�������ķ����Խǿ��TCS3002D�������������ķ���Ƶ��Խ�ߣ�
#define S1    5  //S0��S1����Ͼ�������ź�Ƶ�ʱ������ӣ���������Ϊ2%
				 //��������ΪTCS3200������OUT��������ź�Ƶ��������������Ƶ��֮��
#define S2     4   //S2��S3����Ͼ����ú졢�̡��������ֹ���ͨ���˲���
#define S3     3
#define OUT    2  //TCS3200��ɫ����������ź����ӵ�Arduino�ж�0���ţ������������ź��ж�
				  //���жϺ����м�¼TCS3200����źŵ��������
#define LED    7  //����TCS3200��ɫ�������Ƿ����LED��
float g_SF[3];     //��TCS3200����źŵ�������ת��ΪRGB��׼ֵ��RGB��������
int   g_count = 0;  // �����뷴���ǿ���ӦTCS3200��ɫ����������źŵ�������
// �������ڴ洢��1s��TCS3200����źŵ���������������RGB�������Ӿ���RGB��׼ֵ
int   g_array[3];
int   g_flag = 0;   // �˲���ģʽѡ��˳���־

// ��ʼ��TSC3200���������ŵ��������ģʽ
//����TCS3002D��������������Ƶ����������ź�Ƶ�ʵı�������Ϊ2%
void TSC_Init()
{
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(OUT, INPUT);
	pinMode(LED, OUTPUT);
	digitalWrite(S0, LOW);
	digitalWrite(S1, HIGH);
}

//ѡ���˲���ģʽ�������ú졢�̡��������ֹ���ͨ���˲���
void TSC_FilterColor(int Level01, int Level02)
{
	if (Level01 != 0)
		Level01 = HIGH;
	if (Level02 != 0)
		Level02 = HIGH;
	digitalWrite(S2, Level01);
	digitalWrite(S3, Level02);
}

//�жϺ���������TCS3200����źŵ�������
void TSC_Count()
{
	g_count++;
}

//��ʱ���жϺ�����ÿ1s�жϺ󣬰Ѹ�ʱ���ڵĺ졢�̡������ֹ���ͨ���˲���ʱ��
//TCS3200����ź���������ֱ�洢������g_array[3]����ӦԪ�ر�����
void TSC_Callback()
{
	switch (g_flag)
	{
	case 0:
		Serial.println("->WB Start");
		TSC_WB(LOW, LOW);   //ѡ���ú�ɫ����ͨ���˲�����ģʽ
		break;
	case 1:
		Serial.print("->Frequency R=");
		Serial.println(g_count);   //��ӡ1s�ڵĺ��ͨ���˲���ʱ��TCS3200������������
		g_array[0] = g_count;    //�洢1s�ڵĺ��ͨ���˲���ʱ��TCS3200������������
		TSC_WB(HIGH, HIGH);  //ѡ������ɫ����ͨ���˲�����ģʽ
		break;
	case 2:
		Serial.print("->Frequency G=");
		Serial.println(g_count);   //��ӡ1s�ڵ��̹�ͨ���˲���ʱ��TCS3200������������
		g_array[1] = g_count;    //�洢1s�ڵ��̹�ͨ���˲���ʱ��TCS3200������������
		TSC_WB(LOW, HIGH);  //ѡ������ɫ����ͨ���˲�����ģʽ
		break;

	case 3:
		Serial.print("->Frequency B=");
		Serial.println(g_count);   //��ӡ1s�ڵ�����ͨ���˲���ʱ��TCS3200������������
		Serial.println("->WB End");
		g_array[2] = g_count;     //�洢1s�ڵ�����ͨ���˲���ʱ��TCS3200������������
		TSC_WB(HIGH, LOW);   //ѡ�����˲�����ģʽ  
		break;
	default:
		g_count = 0;     //����ֵ����
		break;
	}
}
//���÷�����к졢�̡�����ɫ��ֱ�ͨ���˲���ʱ��δ������ݵı�־
//�ú�����TSC_Callback( )����
void TSC_WB(int Level0, int Level1)
{
	g_count = 0;   //����ֵ����
	g_flag++;     //����źż�����־
	TSC_FilterColor(Level0, Level1); //�˲���ģʽ
	Timer1.setPeriod(1000000);     //��������ź��������ʱ��1s
}

/*
�����Ľӿڵ���;
uint8_t ��; + R/W +Port = �˿���
*/
uint8_t FireReadPort = A0;
int average = 0;

void setup()
{
	Serial.begin(9600);//���ò�����Ϊ9600��
	pinMode(A0, INPUT);
	average = Sensorinitializer(A0);
	pinMode(13, INPUT);
	pinMode(1, OUTPUT);
	digitalWrite(1, HIGH);
	//pinMode(D1)
	
	TSC_Init();
	Serial.begin(9600); //��������ͨ��
	Timer1.initialize();   // defaulte is 1s
	Timer1.attachInterrupt(TSC_Callback); //���ö�ʱ��1���жϣ��жϵ��ú���ΪTSC_Callback()
	//����TCS3200����źŵ������ش����жϣ��жϵ��ú���ΪTSC_Count()
	attachInterrupt(0, TSC_Count, RISING);
	digitalWrite(LED, HIGH);//����LED��
	delay(4000); //��ʱ4s���Եȴ���������졢�̡�����ɫ��1s�ڵ�TCS3200����ź��������
	//ͨ����ƽ����ԣ�����õ���ɫ����RGBֵ255��1s����ɫ����������RGB��������
	g_SF[0] = 255.0 / g_array[0];     //��ɫ���������
	g_SF[1] = 255.0 / g_array[1];    //��ɫ���������
	g_SF[2] = 255.0 / g_array[2];    //��ɫ���������
	//��ӡ��ƽ���ĺ졢�̡�����ɫ��RGB��������
	Serial.println(g_SF[0], 5);
	Serial.println(g_SF[1], 5);
	Serial.println(g_SF[2], 5);
	//�졢�̡�����ɫ��ֱ��Ӧ��1s��TCS3200���������������Ӧ�ı������Ӿ���RGB��׼ֵ
	//��ӡ���������RGBֵ
	for (int i = 0; i < 3; i++)
		Serial.println(int(g_array[i] * g_SF[i]));
}
void loop()
{	
	g_flag = 0;
	//ÿ���һ�α�������RGB��ɫֵ��ʱ4s
	delay(4000);
	//��ӡ����������RGB��ɫֵ
	for (int i = 0; i < 3; i++)
		Serial.println(int(g_array[i] * g_SF[i]));

	/*
	����������
	if(analogRead(A0) - average >= 10 || analogRead(A0) - average <= -10)
		Serial.println("Pass");
	*/
	//Serial.println(analogRead(A0) - average);
	//Serial.println(analogRead(A0));

	/*
	if(digitalRead(13) == HIGH)
		Serial.println( 1 );
	else
	{
		Serial.println(0);
	}
	
	*/
	delay(100);
}

int Sensorinitializer(uint8_t port) {
 	unsigned int value = 0;
	for (int i = 0; i < 30; i++)
	{
		value += analogRead(port);
		delay(100);
	}
	return (value / 30);
}


