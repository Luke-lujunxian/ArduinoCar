/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/
#include <time.h>
#include <MsTimer2-1.1.0/MsTimer2.h>


/*
�����Ľӿڵ���;
uint8_t ��; + R/W  = �˿���
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
uint8_t obstacleReadRight = 10;//5ռ��
uint8_t obstacleReadFront = 12;//6ռ��
uint8_t ENA = 6;
uint8_t ENB = 5;
uint8_t N1 = 11;
uint8_t N2 = 9;
uint8_t N3 = 8;
uint8_t N4 = 7;

const int FORWARD = 0, LEFT = 1, RIGHT = 2, BACKWARD = 3, TURNLEFT = 4, TURNRIGHT = 5;
const int WHITE, BLACK;
int fireNoice, soundNoice, black;

int pointPass = 0,linePass=0;//�Ѿ����ĵ���
int taskList[4] = { 1,2,3,4 };

const int timeFor360 = 10;//��תһȦ��Ҫ������
const int traceError = 200;//ǰѭ��ͷ�ݲ�

void setup()
{
	fireNoice = SensorInitializer(fireReadFront);
	soundNoice = SensorInitializer(soundRead);
	black = SensorInitializer(traceReadFront);
	pinMode(echoPong, INPUT);
	pinMode(echoPing, OUTPUT); 
	pinMode(transportRead, INPUT);
	pinMode(obstacleReadRight, INPUT);
	pinMode(obstacleReadFront, INPUT);
	//��������
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

	//ѭ��
	if (!digitalRead(traceLeft) && digitalRead(traceRight)) {
		move(RIGHT, 0.5);
	}
	else if (digitalRead(traceLeft) && !digitalRead(traceRight)) {
		move(LEFT, 0.5);
	}
	else if (!digitalRead(traceLeft) && !digitalRead(traceRight)) {
		//δ֪������
		move(BACKWARD, 0);
	}
	else {
		move(FORWARD, 0.8);
	}

	//����
	obsoleteAvoid();


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

bool timeout = false;

int taskSelect() {
	MsTimer2::set(10, timeOut);
	switch (taskList[pointPass]){
		case 0: {
			//���
			MsTimer2::start();

			while (!isFire(fireReadFront)) {
				if (isFire(fireReadLeft))
				{
					//��ת
					while (!isFire(fireReadFront)) {
						if (timeout)
							goto reset;
					}
					/*ֹͣ*/
				}
				else {
					//��ת
					while (!isFire(fireReadFront)) {
						if (timeout)
							goto reset;
					}

				}
			}

			//��ʼ���

			MsTimer2::stop();
		reset:
			//������λ
			break;
		}
		case 1: {
			//Ͷ��
			break;
		}
		case 2: {
			//���
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

void reset(int h) {//h��ʾ֮ǰС������������
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
		move(movedirection, 0.8);
		if (digitalRead(trace) - state1 == WHITE - BLACK) 
			--linePass;
	}
}
void move(int h, float speedRate) {
	/*
		
	*/
}

void obsoleteAvoid() {
	if (digitalRead(obstacleReadFront) && getDistance() < 10) {
		time_t temp = time(NULL);
		int timePass = 0;
		move(TURNLEFT, 0.8);
		while (true) {
			if (!digitalRead(obstacleReadFront) && digitalRead(obstacleReadRight))
				break;
		}
		short turningTime = difftime(time(NULL), temp);
		move(FORWARD, 0.8);
		while (true) {
			if (!digitalRead(obstacleReadRight))
				break;
		}
		short fowTime = difftime(time(NULL), temp) - turningTime;

		temp = time(NULL);
		while (difftime(time(NULL), temp) < turningTime * 2)
		{
			move(TURNRIGHT, 0.8);
		}
		while (difftime(time(NULL), temp) - turningTime * 2 < fowTime) {
			move(FORWARD, 0.8);
			if (analogRead(traceReadFront) < black - traceError) {
				move(TURNLEFT, 0.8);
				break;
			}
		}
		while (!digitalRead(traceRight) && !digitalRead(traceLeft))
		{
			if (digitalRead(traceRight) && !digitalRead(traceLeft))
				move(TURNLEFT, 0.5);
		}
		move(FORWARD, 0);
	}
}


