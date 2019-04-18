/*
 Name:		Sketch1.ino
 Created:	2019/3/15 16:42:17
 Author:	Junxian Lu
*/
#include <time.h>
#include <MsTimer2-1.1.0/MsTimer2.h>
#include <Servo.h>


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
uint8_t LED = echoPong;//���ö˿�
uint8_t FAN = soundRead;

uint8_t obstacleReadRight = 4;//5ռ��
uint8_t obstacleReadFront = 12;//6ռ��
uint8_t ENA = 6;
uint8_t ENB = 5;
uint8_t N1 = 11;
uint8_t N2 = 9;
uint8_t N3 = 8;
uint8_t N4 = 7;
uint8_t hit = 1;

const int FORWARD = 0, LEFT = 1, RIGHT = 2, BACKWARD = 3, TURNLEFT = 4, TURNRIGHT = 5,STOP=6;
const int WHITE, BLACK;
int fireNoice, soundNoice, black;

int pointPass = 0,linePass=0;//�Ѿ����ĵ���
int taskList[4] = { 1,2,3,4 };

const int timeFor360 = 10;//��תһȦ��Ҫ������
const int traceError = 200;//ǰѭ��ͷ�ݲ�
const int rotationspeed = 360 / timeFor360;
int transportpoint;//�ڼ���������ľ��
int angle;//ÿ��Ҫת�ĽǶ�
int speed;//С��FOWRAD�ٶ�Ϊ0.8ʱ���ٶ�
Servo a;

void setup()
{
	pinMode(soundRead, INPUT);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	fireNoice = SensorInitializer(fireReadFront);
	soundNoice = SensorInitializer(soundRead);
	black = SensorInitializer(traceReadFront);
	digitalWrite(LED, LOW);


	pinMode(echoPong, INPUT);
	pinMode(echoPing, OUTPUT); 
	pinMode(obstacleReadRight, INPUT);
	pinMode(obstacleReadFront, INPUT);
	pinMode(fireReadFront, INPUT);
	pinMode(fireReadLeft, INPUT);
	pinMode(fireReadRight, INPUT);
	pinMode(traceLeft, INPUT);
	pinMode(traceRight, INPUT);
	pinMode(traceReadFront, INPUT);

	pinMode(N1, OUTPUT);
	pinMode(N2, OUTPUT);
	pinMode(N3, OUTPUT);
	pinMode(N4, OUTPUT);
	digitalWrite(N1, LOW);
	digitalWrite(N2, LOW);
	digitalWrite(N3,LOW);
	digitalWrite(N4, LOW);
	a.attach(10);
	Serial.println("Ini complete");
	//��������
	while (true) {
		if (analogRead(soundRead) - soundNoice >= 10 || analogRead(soundRead) - soundNoice <= -10) {
			Serial.println("Pass");
			break;
		}		
	}
	pinMode(FAN, OUTPUT);
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

	//��ɫ��
	if (analogRead(traceReadFront) < black - traceError) {
		move(STOP, 0);
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
	MsTimer2::set(12, timeOut);
	switch (taskList[pointPass]){
		case 0: {
			//���
			MsTimer2::start();
			int leftTime = 0;
			int movedirection;

			while (!isFire(fireReadFront)) {
				if (isFire(fireReadLeft))
				{
					move(TURNLEFT, 0.8);
					time_t temp = time(NULL);
					while (!isFire(fireReadFront)) {
						if (timeout) {
							leftTime = difftime(time(NULL), temp);
							goto reset;
						}		
					}
					leftTime = difftime(time(NULL), temp);
					/*ֹͣ*/
				}
				else {
					//��ת
					move(TURNRIGHT, 0.8);
					time_t temp = time(NULL);
					while (!isFire(fireReadFront)) {
						if (timeout) {
							leftTime -= difftime(time(NULL), temp);
							goto reset;
						}
					}
					leftTime -= difftime(time(NULL), temp);
				}
			}
			move(STOP, 0);
			//��ʼ���
			digitalWrite(LED, HIGH);
			delay(2000);
			digitalWrite(LED, LOW);
			digitalWrite(FAN, HIGH);
			while (isFire(fireReadFront)){}
			digitalWrite(FAN, LOW);
			MsTimer2::stop();
		reset:
			//��λ
			time_t temp = time(NULL);
			while (leftTime < 0 && difftime(time(NULL), temp) < -leftTime)
			{
				move(TURNLEFT, 0.8);
			}
			while (leftTime > 0 && difftime(time(NULL), temp) < leftTime)
			{
				move(TURNRIGHT, 0.8);
			}
			break;
		}
		case 1: {
			//Ͷ��
			linePass = 0;
			while (true) {
				if (getDistance() == 50)
					move(TURNLEFT, 0.5);
				if (traceLeft == BLACK) {
					delay(1);
					if (traceLeft == WHITE) {
						linePass++;
					}
				}
			}
			move(STOP, 0);
			int previousdistance = getDistance();
			move(FORWARD, 0.3);
			while (getDistance() > 5);
			move(STOP, 0);
			a.write(90);
			delay(100);
			a.write(0);
			move(BACKWARD, 0.3);
			while (getDistance() < previousdistance);
			move(STOP, 0);
			reset(LEFT);
			break;
		}
		case 2: {
			linePass = 0;
			while (true) {
				if (getDistance() == 50)
					move(TURNLEFT, 0.5);
				if (traceLeft == BLACK) {
					delay(1);
					if (traceLeft == WHITE) {
						linePass++;
					}
				}
			}
			move(STOP, 0);
			int previousdistance = getDistance();
			move(FORWARD, 0.3);
			while (getDistance() > 5);
			move(STOP, 0);
			for (int i = 0; i < 180; i += 30) {
				a.write(i);
				if (digitalRead(hit) == 1) break;
			}
			move(BACKWARD, 0.3);
			while (getDistance() < previousdistance);
			move(STOP, 0);
			reset(LEFT);
			break;
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
	switch (h)
	{
	case FORWARD:
		digitalWrite(N1, HIGH);
		digitalWrite(N2, LOW);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(map(speedRate,0, 1, 0, 255)));
		analogWrite(ENB, (int)(map(speedRate,0, 1, 0, 255)));
		break;
	case RIGHT:
		digitalWrite(N1, HIGH);
		digitalWrite(N2, LOW);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(map(speedRate,0, 1, 0, 255)));
		analogWrite(ENB, (int)(map(speedRate/2,0, 1, 0, 255)));
	case LEFT:
		digitalWrite(N1, HIGH);
		digitalWrite(N2, LOW);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(map(speedRate/2,0, 1, 0, 255)));
		analogWrite(ENB, (int)(map(speedRate,0, 1, 0, 255)));
	case TURNRIGHT:
		digitalWrite(N1, HIGH);
		digitalWrite(N2, LOW);
		digitalWrite(N3, LOW);
		digitalWrite(N4, HIGH);
		analogWrite(ENA, (int)(map(speedRate,0, 1, 0, 255)));
		analogWrite(ENB, (int)(map(speedRate,0, 1, 0, 255)));
	case TURNLEFT:
		digitalWrite(N1, LOW);
		digitalWrite(N2, HIGH);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(map(speedRate,0, 1, 0, 255)));
		analogWrite(ENB, (int)(map(speedRate,0, 1, 0, 255)));
	case BACKWARD:
		digitalWrite(N1, LOW);
		digitalWrite(N2, HIGH);
		digitalWrite(N3, LOW);
		digitalWrite(N4, HIGH);
		analogWrite(ENA, (int)(map(speedRate,0, 1, 0, 255)));
		analogWrite(ENB, (int)(map(speedRate,0, 1, 0, 255)));
	case STOP:
		digitalWrite(N1, LOW);
		digitalWrite(N2, LOW);
		digitalWrite(N3, LOW);
		digitalWrite(N4, LOW);
	default:
		break;
	}
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
		move(STOP, 0);
	}
}
void transport() {
	int carlength=0;//�������ĵ���ͷ�ľ���
	if (transportpoint == 2) {
		move(TURNRIGHT, 0.5);
		delay(angle / rotationspeed);
		for (int i = carlength / speed; i > 0; i--) {
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
			delay(1000);
		}
		move(TURNRIGHT, 0.5);
		delay(100);
		while (true) {
			if (digitalRead(traceLeft) == BLACK && digitalRead(traceRight) == BLACK) {
				for (int i = 4 / speed; i > 0; i--) {
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
						move(BACKWARD, 0.8);
					}
					delay(1000);
				}
				move(TURNLEFT, 0.5);
				delay(100);
				while (true) {
					if (digitalRead(traceLeft) == BLACK && digitalRead(traceRight) == BLACK) {
						move(STOP, 0);
						return;
					}
				}
			}
		}
	}
	else if (transportpoint == 3) {
		move(BACKWARD, 0.8);
		delay(carlength / speed);
		move(TURNRIGHT, 0.5);
		delay(100);
		while (true) {
			if (digitalRead(traceLeft) == BLACK && digitalRead(traceRight) == BLACK) {
				move(FORWARD, 0.8);
				while (true) {
					if (digitalRead(traceReadFront) != BLACK) {
						move(BACKWARD, 0.8);
						delay((4 + carlength) / speed);
						move(TURNRIGHT, 0.5);
						delay(100);
						while (true) {
							if (digitalRead(traceLeft) == BLACK && digitalRead(traceRight) == BLACK) {
								move(STOP, 0);
								return;
							}
						}
					}
				}
			}
		}
	}
}


