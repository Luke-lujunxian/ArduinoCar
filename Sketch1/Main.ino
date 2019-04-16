/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/
#include <time.h>
#include <MsTimer2-1.1.0/MsTimer2.h>


/*
声明的接口的用途
uint8_t 用途 + R/W  = 端口名
*/
uint8_t fireReadFront = A0;
uint8_t fireReadLeft = A1;
uint8_t fireReadRight = A2;
uint8_t soundRead = A3;
uint8_t traceReadFront = A4; 

uint8_t echoPing = 0;
uint8_t echoPong = 1;
uint8_t traceLeft = 2;
uint8_t traceRight = 3;

uint8_t transportRead = 4;
uint8_t obstacleReadLeft = 5;
uint8_t obstacleReadFront = 6;
uint8_t motor1;
uint8_t motor2;

const int FORWARD = 0, LEFT = 1, RIGHT = 2;
const int WHITE, BLACK;
int fireNoice, soundNoice, black;

int pointPass = 0,linePass=0;//已经过的点数
int taskList[4] = { 1,2,3,4 };

const int timeFor360 = 10;//旋转一圈需要的秒数
const int traceError = 200;//前循迹头容差

void setup()
{
	fireNoice = SensorInitializer(fireReadFront);
	soundNoice = SensorInitializer(soundRead);
	black = SensorInitializer(traceReadFront);
	pinMode(echoPong, INPUT);
	pinMode(echoPing, OUTPUT);
	//声控启动
	while (true) {
		if (analogRead(soundRead) - soundNoice >= 10 || analogRead(soundRead) - soundNoice <= -10) {
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
	if (analogRead(traceReadFront) < black - traceError) {
		taskSelect();

		pointPass++;
	}
	
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

bool timeout = false;

int taskSelect() {
	MsTimer2::set(10, timeOut);
	switch (taskList[pointPass]){
		case 0: {
			//灭火
			MsTimer2::start();

			while (!isFire(fireReadFront)) {
				if (isFire(fireReadLeft))
				{
					//左转
					while (!isFire(fireReadFront)) {
						if (timeout)
							goto reset;
					}
					/*停止*/
				}
				else if (isFire(fireReadRight))
				{
					//右转
					while (!isFire(fireReadFront)) { 
						if (timeout)
							goto reset;
					}
					/*停止*/
				}
				else {
					//右转
					while (!isFire(fireReadFront)) {
						if (timeout)
							goto reset;
					}

				}
			}

			//开始灭火

			MsTimer2::stop();
		reset:
			//车辆复位
			break;
		}
		case 1: {
			//投掷
			break;
		}
		case 2: {
			//打击
		}
		default:
		
			break;
		}
		out:Serial.println("Time Out!");
}

bool isFire(uint8_t port) {
	if (analogRead(port) - fireNoice > 10 || analogRead(port) - fireNoice < -10)
		return true;
	else
		return false;
}

void timeOut() {
	timeout = true;
}

void reset(int h) {//h表示之前小车向左还是向右
	int trace,movedirection;
	if (h == RIGHT) {
		trace = traceRight;
		movedirection = LEFT;
	}
	else {
		trace = traceLeft;
		movedirection = RIGHT;
	}
	while(linePass){
		int state1=digitalRead(trace);
		move(movedirection);
		if (digitalRead(trace) - state1 == WHITE - BLACK) 
			--linePass;
	}
}
void move(int h) {
	
}


