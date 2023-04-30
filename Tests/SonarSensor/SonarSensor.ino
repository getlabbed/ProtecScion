#include <Arduino.h>

#define TRIG A1
#define ECHO A2
#define BUFFER_SIZE 5
#define MAX_SPEED_CHANGE 0.100 // m/ms, assuming a maximum speed change of 15 m/s
#define MIN_SPEED_CHANGE 0.001 // m/ms, filter out noise when the object is not moving

double averageSpeed = 0;
int averageCount = 0;

long duration;
int distance, prevDistance;
unsigned long prevTime, startTime;
int distanceBuffer[BUFFER_SIZE];
int bufferIndex = 0;

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
  startTime = prevTime = millis();
  memset(distanceBuffer, 0, BUFFER_SIZE * sizeof(int));
}

int readDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2;
}

int calculateAverageDistance() {
  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += distanceBuffer[i];
  }
  return sum / BUFFER_SIZE;
}

void updateBuffer(int value) {
  distanceBuffer[bufferIndex % BUFFER_SIZE] = value;
  bufferIndex++;
}

bool isValidDistance(int newDistance, int prevDistance, unsigned long elapsedTime) {
  float change = abs(newDistance - prevDistance);
  float maxChange = MAX_SPEED_CHANGE * elapsedTime;
  float minChange = MIN_SPEED_CHANGE * elapsedTime;
  return change <= maxChange && change >= minChange;
}

void loop() {
  averageCount = 0;
  averageSpeed = 0;
  while(averageCount <= 10){
    int newDistance = readDistance();

    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - prevTime;

    if (isValidDistance(newDistance, prevDistance, elapsedTime)) {
      updateBuffer(newDistance);
      distance = calculateAverageDistance();

      if (bufferIndex > BUFFER_SIZE) {
        float instantaneousSpeed = abs((float)(newDistance - prevDistance) / elapsedTime)*1000;

        averageSpeed += instantaneousSpeed;
        averageCount += 1;

        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.print("Instantaneous Speed: ");
        Serial.println(instantaneousSpeed);
      }

      prevDistance = newDistance;
      prevTime = currentTime;
    }

    delay(500); // Adjust the delay as needed to reduce noise
  }
  Serial.print("==== ==== ");
  int average = averageSpeed / averageCount;
  Serial.print(average);
  Serial.println(" ==== ====");
}