/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/

int val;//定义变量val

/*
声明的接口的用途
uint8_t 用途 + R/W +Port = 端口名
*/
uint8_t FireReadPort = A0;

void setup()
{
	Serial.begin(9600);//设置波特率为9600。
	pinMode(A0, INPUT);//设置数字13 口为输出接口。
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

