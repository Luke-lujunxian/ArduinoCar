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
uint8_t fireReadFront = A0;//����
uint8_t fireReadLeft = A1;
uint8_t fireReadRight = A4;
uint8_t soundRead = A5;
uint8_t traceReadFront = A2; 

uint8_t echoPing = soundRead;
uint8_t echoPong = 4;
uint8_t traceLeft = A3;//2
uint8_t traceRight = A0;
uint8_t LED = echoPong;//���ö˿�
uint8_t FAN = soundRead;

uint8_t obstacleReadRight = 13;//5ռ��
uint8_t obstacleReadFront = 12;//6ռ��
uint8_t ENA = 6;
uint8_t ENB = 5;
uint8_t N1 = 11;
uint8_t N2 = 9;
uint8_t N3 = 8;
uint8_t N4 = 7;
uint8_t hit = echoPong;

const int FORWARD = 0, LEFT = 1, RIGHT = 2, BACKWARD = 3, TURNLEFT = 4, TURNRIGHT = 5,STOP=6;
const int WHITE, BLACK;
int fireNoice, soundNoice, black_f, black_l, black_r;

int pointPass = 0,linePass=0;//�Ѿ����ĵ���
int taskList[4] = { -1,-1,-1,-1 };

const int timeFor360 = 10;//��תһȦ��Ҫ������
const int traceError = 200;//ǰѭ��ͷ�ݲ�
const int rotationspeed = 360 / timeFor360;
int transportpoint;//�ڼ���������ľ��
int angle;//ÿ��Ҫת�ĽǶ�
int speed;//С��FOWRAD�ٶ�Ϊ0.8ʱ���ٶ�

const float GLOBALSPEED = 0.8;

Servo a;

void setup()
{	
	pinMode(soundRead, INPUT);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	fireNoice = SensorInitializer(fireReadFront,30);
	soundNoice = SensorInitializer(soundRead,30);
	


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
	pinMode(ENA, OUTPUT);
	pinMode(ENB, OUTPUT);
	digitalWrite(N1, LOW);
	digitalWrite(N2, LOW);
	digitalWrite(N3,LOW);
	digitalWrite(N4, LOW);
	a.attach(10);

	black_f = SensorInitializer(traceReadFront, 1);
	//black_l = SensorInitializer(traceLeft, 1);
	//black_r = SensorInitializer(traceRight, 1);
	digitalWrite(LED, LOW);

	Serial.println("Ini complete");
	//��������
	while (true) {
		if (analogRead(soundRead) - soundNoice >= 10 || analogRead(soundRead) - soundNoice <= -10) {
			Serial.println("Pass");
			break;
		}		
	}
	//pinMode(FAN, OUTPUT);
}
void loop()
{	
	while (false) {
		Serial.print(analogRead(traceReadFront));
		Serial.print("\t");
		Serial.print(analogRead(traceLeft));
		Serial.print("\t");
		Serial.print(analogRead(traceRight));
		Serial.println("------------------");
		delay(100);
	}
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
	findtrace();

	//����
	//obsoleteAvoid();
	/*
	//��ɫ��
	if (analogRead(traceReadFront) < black_f - traceError) {
		move(STOP, 0);
		taskSelect();
		pointPass++;
		if (pointPass == 1) {
			move(TURNRIGHT, 0.8);
			while(!digitalRead(traceRight)){}
			move(TURNRIGHT, 0.5);
			while (!digitalRead(traceLeft)) {}
			move(STOP, 0);
			move(BACKWARD, 0.7);
			delay(3000);
			move(TURNRIGHT, 0.8);
			delay(2000);
			while (!digitalRead(traceRight)) {}
			move(TURNRIGHT, 0.5);
			while (!digitalRead(traceLeft)) {}
		}
		else if(pointPass == 4){
			move(STOP,0);
		}
	}
	*/
	
}

int SensorInitializer(uint8_t port , int ms100) {
 	unsigned int value = 0;
	for (int i = 0; i < ms100; i++)
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
			//reset(LEFT);
			break;
		}
		case 2: {
			//���
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
			//reset(LEFT);
			break;
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

void reset(int h/*,int time*/) {//h��ʾ֮ǰС������������
	/*int movedirection;
	if (h == LEFT) {
		movedirection = TURNRIGHT;
	}
	else {
		movedirection = TURNLEFT;
	}
	move(movedirection, 0.5);
	delay(time);
	*/
	
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
		digitalWrite(N1, LOW);
		digitalWrite(N2, HIGH);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(speedRate*255 * GLOBALSPEED));
		analogWrite(ENB, (int)(speedRate*255 * GLOBALSPEED));
		break;
	case RIGHT:
		digitalWrite(N1, LOW);
		digitalWrite(N2, HIGH);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(speedRate*255 * GLOBALSPEED));
		analogWrite(ENB, (int)(speedRate*255*2/3 * GLOBALSPEED));
		break;
	case LEFT:
		digitalWrite(N1, LOW);
		digitalWrite(N2, HIGH);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(speedRate*255*2/3 * GLOBALSPEED));
		analogWrite(ENB, (int)(speedRate*255 * GLOBALSPEED));
		break;
	case TURNRIGHT:
		digitalWrite(N1, LOW);
		digitalWrite(N2, HIGH);
		digitalWrite(N3, LOW);
		digitalWrite(N4, HIGH);
		analogWrite(ENA, (int)(speedRate*255 * GLOBALSPEED));
		analogWrite(ENB, (int)(speedRate*255 * GLOBALSPEED));
		break;
	case TURNLEFT:
		digitalWrite(N1, HIGH);
		digitalWrite(N2, LOW);
		digitalWrite(N3, HIGH);
		digitalWrite(N4, LOW);
		analogWrite(ENA, (int)(speedRate*255 * GLOBALSPEED));
		analogWrite(ENB, (int)(speedRate*255 * GLOBALSPEED));
		break;
	case BACKWARD:
		digitalWrite(N1, HIGH);
		digitalWrite(N2, LOW);
		digitalWrite(N3, LOW);
		digitalWrite(N4, HIGH);
		analogWrite(ENA, (int)(speedRate*255 * GLOBALSPEED));
		analogWrite(ENB, (int)(speedRate*255 * GLOBALSPEED));
		break;
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
	if (digitalRead(obstacleReadFront) && getDistance() < 10 && digitalRead(traceReadFront)> black_f - traceError) {
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
			if (analogRead(traceReadFront) < black_f - traceError) {
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
		move(FORWARD, 0.8);
		while (digitalRead(traceLeft) != BLACK && digitalRead(traceRight) != BLACK);
		for (int i = carlength / speed; i > 0; i--) {
			findtrace();
			delay(1000);
		}
		move(TURNRIGHT, 0.5);
		delay(100);
		while (true) {
			if (digitalRead(traceLeft) == BLACK && digitalRead(traceRight) == BLACK) {
				move(STOP, 0);
				return;
			}
		}
	}
	else if (transportpoint == 3) {
		move(TURNRIGHT, 0.5);
		delay(timeFor360 / 2);
		move(FORWARD, 0.8);
		delay(1000);
		while (true) { 
			findtrace(); 
			if (analogRead(traceReadFront) < BLACK - traceError)
				break;
		}
		move(TURNRIGHT, 0.5);
		delay((180-angle)/rotationspeed);
		move(FORWARD, 0.8);
		while (digitalRead(traceLeft) != BLACK && digitalRead(traceRight) != BLACK);
		for (int i = carlength / speed; i > 0; i--) {
			findtrace();
			delay(1000);
		}
		move(TURNRIGHT, 0.5);
		delay(timeFor360/2);
		if (digitalRead(traceLeft) == BLACK && digitalRead(traceRight) == BLACK)
			move(STOP, 0);
	}
}

void LEDlight(bool s) {//�����
	if (s) {
		pinMode(LED, OUTPUT);
		digitalWrite(LED, HIGH);
	}
	else {
		digitalWrite(LED, LOW);
		pinMode(LED, INPUT);
	}
	
	
	
}

void findtrace() {
	bool left, right;
	left = analogRead(traceLeft) >= 100;
	right = analogRead(traceRight) >= 100;
	if (!left && right) {
		move(TURNLEFT, 0.4);
	}
	else if (left && !right) {
		move(TURNRIGHT, 0.4);
	}
	else if (!left && !right) {
		move(FORWARD, 0.8);
	}
	else {
		//δ֪������
		move(BACKWARD, 0);
	}
}

