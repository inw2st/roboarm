#include <Servo.h>

// ----------------------------------------------------
// 1. 서보 객체 및 핀 정의
// ----------------------------------------------------
Servo myservoBase;      // 서보 1: Base (Turn)
Servo myservoUpperArm;  // 서보 2: Upper Arm
Servo myservoLowerArm;  // 서보 3: Lower Arm
Servo myservoClaw;      // 서보 4: Claw (집게)

// 초기 포즈 각도 설정
int posBase = 80;       
int posUpperArm = 60;   
int posLowerArm = 130;  
int posClaw = 0;        // Claw 초기 각도

// 오른쪽 조이스틱 핀
const int RIGHT_X_PIN = A2;
const int RIGHT_Y_PIN = A5;
const int RIGHT_KEY_PIN = 7; // Z 버튼

// 왼쪽 조이스틱 핀
const int LEFT_X_PIN = A3;
const int LEFT_Y_PIN = A4;
const int LEFT_KEY_PIN = 8;  // Z 버튼

// 조이스틱 값을 저장할 변수
int rightX, rightY, rightKey;
int leftX, leftY, leftKey;

// 토글 기능에 필요한 변수: 버튼 클릭 상태를 추적 (노이즈 방지)
bool isClawToggled = false; // 현재 토글 동작이 진행 중인지 확인

// Claw의 최소/최대 각도 상수로 정의
const int CLAW_CLOSED_POS = 2;   // 완전히 닫힌 각도
const int CLAW_OPEN_POS = 108;   // 완전히 열린 각도

// ----------------------------------------------------
// 2. Setup 함수: 초기화 및 서보 연결
// ----------------------------------------------------
void setup()
{
    // 서보 핀 연결
    myservoBase.attach(A1);
    myservoUpperArm.attach(A0);
    myservoLowerArm.attach(6);
    myservoClaw.attach(9);
    
    // 조이스틱 키(버튼) 핀 설정
    pinMode(RIGHT_KEY_PIN, INPUT_PULLUP); 
    pinMode(LEFT_KEY_PIN, INPUT_PULLUP);
    
    Serial.begin(9600);
    Serial.println("Robot Arm Control System Ready with Click Toggle.");

    // 초기 포즈 설정
    myservoBase.write(posBase);
    myservoUpperArm.write(posUpperArm);
    myservoLowerArm.write(posLowerArm);
    myservoClaw.write(posClaw); 
    
    delay(2000); 
}

// ----------------------------------------------------
// 3. Loop 함수: 지속적인 입력 감지 및 제어
// ----------------------------------------------------
void loop()
{
    // 1. 조이스틱 값 읽기
    rightX = analogRead(RIGHT_X_PIN);
    rightY = analogRead(RIGHT_Y_PIN);
    rightKey = digitalRead(RIGHT_KEY_PIN); // 눌리지 않음: HIGH, 눌림: LOW
    
    leftX = analogRead(LEFT_X_PIN);
    leftY = analogRead(LEFT_Y_PIN);
    leftKey = digitalRead(LEFT_KEY_PIN); // 눌리지 않음: HIGH, 눌림: LOW
    
    // 2. 서보 제어 함수 호출
    controlClaw();     // 집게 제어 (토글 기능 포함)
    controlBase();     // 턴 제어
    controlUpperArm(); // 상완 제어
    controlLowerArm(); // 하완 제어
}

// ----------------------------------------------------
// 4. 제어 로직 함수 구현
// ----------------------------------------------------

/**
 * @brief 서보 4 (집게/Claw) 제어 로직: X축 및 클릭 토글
 */
void controlClaw()
{
    // 현재 서보가 움직이고 있는지 확인 (X축 제어의 delay(5) 여부를 판단하는 변수)
    bool isOperating = false; 

    // =========================================================
    // A. 조이스틱 클릭(Z축) 토글 기능 처리
    // =========================================================
    // leftKey는 INPUT_PULLUP이므로, 눌렸을 때 (LOW)입니다.
    if (leftKey == LOW) { 
        if (!isClawToggled) { // 버튼을 누르는 순간 (한 번의 클릭)에만 실행
            
            int targetPos;
            
            // 1. 완전히 닫혀 있으면 (2도) -> 연다 (108도)
            if (posClaw <= CLAW_CLOSED_POS + 1) { 
                targetPos = CLAW_OPEN_POS;
            }
            // 2. 완전히 열려 있으면 (108도) -> 닫는다 (2도)
            else if (posClaw >= CLAW_OPEN_POS - 1) { 
                targetPos = CLAW_CLOSED_POS;
            }
            // 3. 애매하게 열려 있으면 -> 닫는다 (요구사항)
            else { 
                targetPos = CLAW_CLOSED_POS;
            }
            
            // 토글 목표 위치로 즉시 이동
            posClaw = targetPos;
            isClawToggled = true; 
            
            myservoClaw.write(posClaw);
            return; // 토글 클릭 시 X축 로직은 무시하고 바로 루프를 종료합니다.
        }
    } else {
        // 버튼에서 손을 떼면 isClawToggled 초기화 
        isClawToggled = false;
    }

    // =========================================================
    // B. 조이스틱 X축 제어 (점진적인 열기/닫기)
    // =========================================================
    
    // 왼쪽 조이스틱을 왼쪽으로 밀면 집게 열림 (값 > 1000)
    if (leftX > 1000) 
    {
        posClaw += 8; // 열림 속도
        if (posClaw > CLAW_OPEN_POS) {
            posClaw = CLAW_OPEN_POS; // 최대 각도 제한
        }
        isOperating = true; 
    }
    // 왼쪽 조이스틱을 오른쪽으로 밀면 집게 닫힘 (값 < 50)
    else if (leftX < 50) 
    {
        posClaw -= 2; // 닫힘 속도
        if (posClaw < CLAW_CLOSED_POS) {
            posClaw = CLAW_CLOSED_POS; // 최소 각도 제한
        }
        isOperating = true; 
    }
    
    // X축 조작에 따른 서보 명령 전송
    if (isOperating) {
        myservoClaw.write(posClaw);
        delay(5); // 동작 중 부드러움을 위한 딜레이
    }
}

/**
 * @brief 서보 1 (베이스/Turn) 제어 로직
 * @note 오른쪽 조이스틱 X축 사용
 */
void controlBase()
{
    // 오른쪽 조이스틱을 오른쪽으로 밀면 턴 오른쪽 (값 < 50)
    if (rightX < 50) 
    {
        posBase -= 1; 
        if (posBase < 1) {
            posBase = 1; // 최소 각도 제한
        }
    }
    // 오른쪽 조이스틱을 왼쪽으로 밀면 턴 왼쪽 (값 > 1000)
    else if (rightX > 1000) 
    {
        posBase += 1; 
        if (posBase > 180) {
            posBase = 180; // 최대 각도 제한
        }
    }

    myservoBase.write(posBase);
    if (rightX < 50 || rightX > 1000) {
        delay(5); // 동작 중 부드러움을 위한 딜레이
    }
}

/**
 * @brief 서보 2 (상완/Upper Arm) 제어 로직
 * @note 오른쪽 조이스틱 Y축 사용
 */
void controlUpperArm()
{
    // 오른쪽 조이스틱을 위로 밀면 상완 상승 (값 > 1000)
    if (rightY > 1000) 
    {
        posUpperArm -= 1;
        if (posUpperArm < 0) {
            posUpperArm = 0; // 최대 상승 각도 제한
        }
    }
    // 오른쪽 조이스틱을 아래로 밀면 상완 하강 (값 < 50)
    else if (rightY < 50) 
    {
        posUpperArm += 1;
        if (posUpperArm > 180) {
            posUpperArm = 180; // 최대 하강 각도 제한
        }
    }

    myservoUpperArm.write(posUpperArm);
    if (rightY > 1000 || rightY < 50) {
        delay(5); // 동작 중 부드러움을 위한 딜레이
    }
}

/**
 * @brief 서보 3 (하완/Lower Arm) 제어 로직
 * @note 왼쪽 조이스틱 Y축 사용
 */
void controlLowerArm()
{
    // 왼쪽 조이스틱을 위로 밀면 하완 펴기 (값 < 50)
    if (leftY < 50) 
    {
        posLowerArm += 1;
        if (posLowerArm > 180) {
            posLowerArm = 180; // 최대 펼침 각도 제한
        }
    }
    // 왼쪽 조이스틱을 아래로 밀면 하완 접기 (값 > 1000)
    else if (leftY > 1000) 
    {
        posLowerArm -= 1;
        if (posLowerArm < 35) {
            posLowerArm = 35; // 최대 접힘 각도 제한
        }
    }

    myservoLowerArm.write(posLowerArm);
    if (leftY < 50 || leftY > 1000) {
        delay(5); // 동작 중 부드러움을 위한 딜레이
    }
}