// Pin definitions
#define TRIG_PIN 8
#define ECHO_PIN 13

#define LEFT_MOTOR_EN 10
#define LEFT_MOTOR_IN1 6
#define LEFT_MOTOR_IN2 7
#define LEFT_IR_PIN 12

#define RIGHT_MOTOR_EN 9
#define RIGHT_MOTOR_IN1 4
#define RIGHT_MOTOR_IN2 5
#define RIGHT_IR_PIN 11

float clipration = 1.19;
int currentSpeed = 0;
bool isMoving = true;
bool noobject = true;
String receivedString = "";

void setup() {
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LEFT_MOTOR_EN, OUTPUT);
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_EN, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  receiveSpeedLimit();  // Check for commands first
  
  checkObstacle();  // Check for obstacles
  
  if (isMoving && noobject) {
    followLine();
  }
}

void followLine() {
  int leftIR = digitalRead(LEFT_IR_PIN);
  int rightIR = digitalRead(RIGHT_IR_PIN);
  
  if (leftIR == LOW && rightIR == LOW) {
    // Move forward
    setMotorSpeeds(currentSpeed, currentSpeed);
  } else if (leftIR == HIGH && rightIR == LOW) {
    // Turn right
    setMotorSpeeds(0, currentSpeed);
  } else if (leftIR == LOW && rightIR == HIGH) {
    // Turn left
    setMotorSpeeds(currentSpeed, 0);
  } else {
    // Stop (both sensors on white)
    setMotorSpeeds(0, 0);
  }
}

void checkObstacle() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  
  if (distance <= 10) {
    Serial.println("There is an obstacle");
    setMotorSpeeds(0, 0);
    noobject = false;
  } else if (!noobject) { 
    
    noobject =true;
    Serial.println("Obstacle removed, resuming movement");
  }
}

void receiveSpeedLimit() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processReceivedData(receivedString);
      receivedString = "";
    } else {
      receivedString += c;
    }
  }
}

void processReceivedData(String data) {
  data.trim();
  
  if (data.startsWith("SL ")) {
    currentSpeed = data.substring(3).toInt();
    isMoving = true;  // Ensure the robot is set to move
    Serial.print("Speed set to: ");
    Serial.println(currentSpeed);
  } else if (data == "Stop" || data == "Red Light") {
    setMotorSpeeds(0, 0);
    isMoving = false;
    Serial.println("Stopped");
  } else if (data == "Green Light") {
    isMoving = true;
    Serial.println("Moving");
  }
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  if (!isMoving) {
    leftSpeed = 0;
    rightSpeed = 0;
  }
  
  leftSpeed = leftSpeed * clipration;
  leftSpeed = map(leftSpeed, 0, 304, 0, 255);
  analogWrite(LEFT_MOTOR_EN, leftSpeed);
  analogWrite(RIGHT_MOTOR_EN, rightSpeed);
  
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2,LOW);
  
  Serial.print("Motors set to - Left: ");
  Serial.print(leftSpeed);
  Serial.print(", Right: ");
  Serial.println(rightSpeed);
}
