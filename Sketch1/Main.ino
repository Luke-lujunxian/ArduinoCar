/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/

int val;//�������val

/*
�����Ľӿڵ���;
uint8_t ��; + R/W +Port = �˿���
*/
uint8_t FireReadPort = A0;

void setup()
{
	Serial.begin(9600);//���ò�����Ϊ9600��
	pinMode(A0, INPUT);//��������13 ��Ϊ����ӿڡ�
	Sensorinitializer(FireReadPort);
}
void loop()
{
	Serial.println(analogRead(A0));
}

int Sensorinitializer(uint8_t port) {
	int value = 0;
	for (int i = 0; i < 3000; i++)
	{
		value += analogRead(port);
		delay(1);
	}
	return value / 3000;
}

