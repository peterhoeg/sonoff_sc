#include "sample_data.h"
#include "TimerOne.h"
#include "global.h"

#include "dht11.h"
#define AD_NUMREADINGS (50)
#define NOISE_NUM (10)
#define DHT11_NUMREADINGS (5)
#define VOLTAGE (5.0)
#define DUSTPIN A1
#define LIGHTPIN A3
#define MICROPHONEPIN A2
#define DHT11PIN 6

sensorDev sensor_dev[4];

static void initPin(void) {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(2, INPUT_PULLUP);
}

static void initPwm(void) {
  Timer1.initialize(10000);
  Timer1.pwm(10, 999);
  Timer1.pwm(9, 991);
}

static void initData(void) {
  sensor_dev[0].total = 0;
  sensor_dev[0].pin = DUSTPIN;
  sensor_dev[0].level = 0;
  sensor_dev[1].total = 0;
  sensor_dev[1].pin = LIGHTPIN;
  sensor_dev[1].level = 0;
  sensor_dev[2].total = 0;
  sensor_dev[2].pin = MICROPHONEPIN;
  sensor_dev[2].level = 0;
  sensor_dev[3].temp_humi_total[0] = 0;
  sensor_dev[3].temp_humi_total[1] = 0;
  sensor_dev[3].pin = DHT11PIN;
}

void initDevice(void) {
  initPin();
  initPwm();
  initData();
}

void getTempHumi(void) {
  static uint8_t readIndex = 0;
  static int16_t DHT11_readings[2][DHT11_NUMREADINGS] = {0};
  if (getSensorData(DHT11PIN)) {
    sensor_dev[3].temp_humi_total[0] -= DHT11_readings[0][readIndex];
    sensor_dev[3].temp_humi_total[1] -= DHT11_readings[1][readIndex];
    DHT11_readings[0][readIndex] = dht_temperature;
    DHT11_readings[1][readIndex] = dht_humidity;
    sensor_dev[3].temp_humi_total[0] += DHT11_readings[0][readIndex];
    sensor_dev[3].temp_humi_total[1] += DHT11_readings[1][readIndex];
    sensor_dev[3].temp_humi_average[0] =
        sensor_dev[3].temp_humi_total[0] / DHT11_NUMREADINGS;
    sensor_dev[3].temp_humi_average[1] =
        sensor_dev[3].temp_humi_total[1] / DHT11_NUMREADINGS;
    readIndex = readIndex + 1;
    if (readIndex >= DHT11_NUMREADINGS) {
      readIndex = 0;
    }
  } else {
    Serial.println("it is error");
  }
}

void getAdcSensorValue(void) {
  static uint8_t readIndex = 0;
  static uint8_t noise_index = 0;
  static int16_t AD_readings[2][AD_NUMREADINGS] = {0};
  static int16_t noise_readings[NOISE_NUM] = {0};
  static int16_t noise_max = -1;
  static int16_t noise_min = 1025;
  int16_t nosie_value_temp = 0;
  for (uint8_t i = 0; i < 2; i++) {
    sensor_dev[i].total = sensor_dev[i].total - AD_readings[i][readIndex];
    AD_readings[i][readIndex] = analogRead(sensor_dev[i].pin);
    sensor_dev[i].total = AD_readings[i][readIndex] + sensor_dev[i].total;
    sensor_dev[i].average_value = sensor_dev[i].total / AD_NUMREADINGS;
    //        sensor_dev[i].voltage_value = sensor_dev[i].average_value *
    //        (VOLTAGE / 1023.0);
  }
  sensor_dev[1].level = (sensor_dev[1].average_value > 999)
                            ? (10)
                            : (sensor_dev[1].average_value / 100 + 1);
  sensor_dev[0].level = (sensor_dev[0].average_value > 799)
                            ? (10)
                            : (sensor_dev[0].average_value / 80 + 1);
  nosie_value_temp = analogRead(sensor_dev[2].pin);
  if (nosie_value_temp > noise_max) {
    noise_max = nosie_value_temp;
  }
  if (nosie_value_temp < noise_min) {
    noise_min = nosie_value_temp;
  }
  readIndex = readIndex + 1;
  if (readIndex >= AD_NUMREADINGS) {
    sensor_dev[2].total = sensor_dev[2].total - noise_readings[noise_index];
    noise_readings[noise_index] = noise_max - noise_min;
    sensor_dev[2].total = noise_readings[noise_index] + sensor_dev[2].total;
    sensor_dev[2].average_value = sensor_dev[2].total / NOISE_NUM;
    sensor_dev[2].level = (sensor_dev[2].average_value > 999)
                              ? (10)
                              : (sensor_dev[2].average_value / 100 + 1);
    //        sensor_dev[2].voltage_value = sensor_dev[2].average_value *
    //        (VOLTAGE / 1023.0);
    noise_index = noise_index + 1;
    readIndex = 0;
    noise_max = -1;
    noise_min = 1025;
  }
  if (noise_index >= NOISE_NUM) {
    noise_index = 0;
  }
}
