/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/
#include <time.h>


/*
�����Ľӿڵ���;
uint8_t ��; + R/W  = �˿���
*/
uint8_t fireReadFront = A0;
uint8_t fireReadLeft = A1;
uint8_t fireReadRight = A2;
uint8_t soundRead = A3;
uint8_t traceReadFront = A4;
uint8_t echoPing = 0 ;
uint8_t echoPong = 1;

int fireNoice, soundNoice;


void setup()
{
	fireNoice = SensorInitializer(fireReadFront);
	soundNoice = SensorInitializer(soundRead);
	//��������
	while (true) {
		if (analogRead(A0) - soundNoice >= 10 || analogRead(A0) - soundNoice <= -10) {
			Serial.println("Pass");
			break;
		}		
	}
}
void loop()
{	
	

	/*
	��������������
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
	
}

int SensorInitializer(uint8_t port) {
 	unsigned int value = 0;
	for (int i = 0; i < 30; i++)
	{
		value += analogRead(port);
		delay(100);
	}
	return (value / 30);
}

int getDistance()//from CSDN
{
	digitalWrite(echoPing, LOW); // ʹ���������������źŽӿڵ͵�ƽ2��s
	delayMicroseconds(2);
	digitalWrite(echoPing, HIGH); // ʹ���������������źŽӿڸߵ�ƽ10��s������������10��s
	delayMicroseconds(10);
	digitalWrite(echoPing, LOW); // ���ַ����������źŽӿڵ͵�ƽ
	int distance = pulseIn(echoPong, HIGH); // ��������ʱ��
	distance = distance / 58; // ������ʱ��ת��Ϊ���루��λ�����ף�
	Serial.println(distance); //�������ֵ

	if (distance >= 50)
	{
		//�������С��50���׷�������
		return 50;
	}//�������С��50����С��Ϩ��
	else
		return distance;
}


