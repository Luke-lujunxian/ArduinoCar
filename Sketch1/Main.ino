/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/



/*
声明的接口的用途
uint8_t 用途 + R/W  = 端口名
*/
uint8_t FireReadFront = A0;
uint8_t FireReadLeft = A1;
uint8_t FireReadRight = A2;
uint8_t SoundRead = A3;
uint8_t TraceReadFront = A4;
uint8_t EchoRead = 0 ;
uint8_t EchoPing = 1;

int average = 0;

void setup()
{
	
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

int Sensorinitializer(uint8_t port) {
 	unsigned int value = 0;
	for (int i = 0; i < 30; i++)
	{
		value += analogRead(port);
		delay(100);
	}
	return (value / 30);
}


