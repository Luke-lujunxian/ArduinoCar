/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/
#include <time.h>


/*
声明的接口的用途
uint8_t 用途 + R/W  = 端口名
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
	//声控启动
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
	声音传感器测试
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
	digitalWrite(echoPing, LOW); // 使发出发出超声波信号接口低电平2μs
	delayMicroseconds(2);
	digitalWrite(echoPing, HIGH); // 使发出发出超声波信号接口高电平10μs，这里是至少10μs
	delayMicroseconds(10);
	digitalWrite(echoPing, LOW); // 保持发出超声波信号接口低电平
	int distance = pulseIn(echoPong, HIGH); // 读出脉冲时间
	distance = distance / 58; // 将脉冲时间转化为距离（单位：厘米）
	Serial.println(distance); //输出距离值

	if (distance >= 50)
	{
		//如果距离小于50厘米返回数据
		return 50;
	}//如果距离小于50厘米小灯熄灭
	else
		return distance;
}


