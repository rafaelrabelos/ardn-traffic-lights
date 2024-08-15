#include <Arduino.h>

#define GREEN_LED   8
#define YELLOW_LED  9
#define RED_LED     10

struct SignalData
{
  int pin;
  long waitedTimeMs;
  long waitTimeMs;
};

struct TrafficLeds
{
  SignalData closed;
  SignalData reduce;
  SignalData open;
  SignalData * active;
};

void updateTrafficLeds(TrafficLeds *signal);
bool mustWait(SignalData *led);
void shutdownLeds(TrafficLeds *led);

TrafficLeds * signal;

void setup() {

  TrafficLeds mySignal;

  mySignal.closed = {.pin = RED_LED, .waitedTimeMs=0, .waitTimeMs = 8000};
  mySignal.reduce = {.pin = YELLOW_LED, .waitedTimeMs=0, .waitTimeMs = 3000};
  mySignal.open = {.pin = GREEN_LED, .waitedTimeMs=0, .waitTimeMs = 8000};
  pinMode(mySignal.closed.pin, OUTPUT);
  pinMode(mySignal.reduce.pin, OUTPUT);
  pinMode(mySignal.open.pin, OUTPUT);

  signal = &mySignal;

}

void loop() {
  updateTrafficLeds(signal);
}

void updateTrafficLeds(TrafficLeds *signal) {

  if(signal->active->pin == 0){
    signal->active = &signal->reduce;

    shutdownLeds(signal);
  }

  if(signal->active == &signal->closed && mustWait(signal->active)){
    digitalWrite(signal->reduce.pin, LOW);
    digitalWrite(signal->open.pin, LOW);
  }
  if(signal->active == &signal->reduce && mustWait(signal->active)){
    digitalWrite(signal->closed.pin, LOW);
    digitalWrite(signal->open.pin, LOW);
  }
  if(signal->active == &signal->open && mustWait(signal->active)){
    digitalWrite(signal->reduce.pin, LOW);
    digitalWrite(signal->closed.pin, LOW);
  }

  if(!mustWait(signal->active)){
    signal->active->waitedTimeMs = 0;
    digitalWrite(signal->active->pin, LOW);

    if(signal->active == &signal->open){
      signal->active = &signal->reduce;
      return;
    }
    if(signal->active == &signal->reduce){
      signal->active = &signal->closed;
      return;
    }
    if(signal->active == &signal->closed){
      signal->active = &signal->open;
    }
  }

  digitalWrite(signal->active->pin, HIGH);
  signal->active->waitedTimeMs = signal->active->waitedTimeMs + 500;
  delay(500);
}

bool mustWait(SignalData * led){
  return led->waitedTimeMs < led->waitTimeMs;
}

void shutdownLeds(TrafficLeds *led){
  digitalWrite(led->closed.pin, LOW);
  digitalWrite(led->reduce.pin, LOW);
  digitalWrite(led->open.pin, LOW);
}