# Advanced Real-Time Systems

Below are brief descriptions of what each of the 3 projects entailed. These are only executable if you run them on an Arduino operating on FreeRTOS. There is nothing to graphically represent here so descriptions will lack images.

---

### [Project 1](./P1/)

This project was an introduction to the FreeRTOS library and how it interacts with Arduino. You'll find a `Blink_AnalogRead` folder which contains an Arduino implementation using FreeRTOS to make the light on the Arduino Mega blink. This simply proves that the Arduino is now operating on FreeRTOS and not a Serial loop.

`tasks.c` is a modified library file form FreeRTOS. It allowed me to set custom parameters for tasks being executed. This is the strength of using an open source real-time operating system, it allows the user to control how exactly the Arduino processes tasks and their execution.

`question3` contains an Arduino implementation of 3 tasks being run on FreeRTOS. It uses FreeRTOS's `xTaskCreate` function to interface with the OS and execute the tasks on the Arduino Mega hardware.

---

### [Project 2](./P2/)

---

### [Project 3](./P3/)