#include <Arduino.h>

#define TRIG A1
#define ECHO A2
#define BUFFER_SIZE 5
#define MAX_SPEED_CHANGE 5.00 // m/ms, assuming a maximum speed change of 15 m/s

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
  float maxChange = MAX_SPEED_CHANGE * elapsedTime;
  return abs(newDistance - prevDistance) <= maxChange;
}

void loop() {
  int newDistance = readDistance();

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - prevTime;

  if (isValidDistance(newDistance, prevDistance, elapsedTime)) {
    updateBuffer(newDistance);
    distance = calculateAverageDistance();

    if (bufferIndex > BUFFER_SIZE) {
      if (bufferIndex % 10 == 0) { // Calculate speed every 10 iterations
        float instantaneousSpeed = (float)(newDistance - prevDistance) / elapsedTime;

        Serial.print("Distance: ");
        Serial.println(distance);
        Serial.print("Instantaneous Speed: ");
        Serial.println(instantaneousSpeed);
      }
    }

    prevDistance = newDistance;
    prevTime = currentTime;
  }

  delay(10); // Reduced delay to 10 milliseconds
}