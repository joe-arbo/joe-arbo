#include <Arduino_FreeRTOS.h>

void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);

void setup() {
    Serial.begin(9600);

    // wait for serial port to connect.
    while(!Serial) {;}

    //Create Task 1
    xTaskCreate (Task1, "Task1", 128, NULL, 2, NULL);

    //Create Task 2
    xTaskCreate (Task2, "Task2", 128, NULL, 3, NULL);
    
    //Create Task 3
    xTaskCreate (Task3, "Task3", 128, NULL, 1, NULL);
}

void loop() {}

//Implement Task 1
void Task1(void *pvParameters) {
    (void) pvParameters;

    for(;;) {
        Serial.println("Task 1: Every 2 Seconds");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

//Implement Task 2
void Task2(void *pvParameters) {
    (void) pvParameters;

    for(;;) {
        Serial.println("Task 2: Every 3 Seconds");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

//Implement Task 3
void Task3(void *pvParameters) {
    (void) pvParameters;
    
    for(;;) {
        Serial.println("Task 3: Every 5 Seconds");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}