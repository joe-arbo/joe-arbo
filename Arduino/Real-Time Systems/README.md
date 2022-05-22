# Advanced Real-Time Systems

Below are brief descriptions of what each of the 3 projects entailed. These are only executable if you run them on an Arduino operating on FreeRTOS. There is nothing to graphically represent here so descriptions will lack images.

---

### [Project 1](./P1/)

This project was an introduction to the FreeRTOS library and how it interacts with Arduino. You'll find a `Blink_AnalogRead` folder which contains an Arduino implementation using FreeRTOS to make the light on the Arduino Mega blink. This simply proves that the Arduino is now operating on FreeRTOS and not a Serial loop.

`tasks.c` is a modified library file form FreeRTOS. It allowed me to set custom parameters for tasks being executed. This is the strength of using an open-source real-time operating system, it allows the user to control how exactly the Arduino processes tasks and their execution.

`question3` contains an Arduino implementation of 3 tasks being run on FreeRTOS. It uses FreeRTOS's `xTaskCreate` function to interface with the OS and execute the tasks on the Arduino Mega hardware.

---

### [Project 2](./P2/)

Project 2 focused on gaining a better understanding of how FreeRTOS schedules tasks. Specifically, common OS scheduling protocols. `project2.ino` houses the driver code that creates the tasks being run, `scheduler.cpp` is what is used to take those tasks and schedule them accordingly. The scheduler handles task arrival time, execution time, met/missed deadlines, and what to do if a task set is not schedulable.

There are parts missing in `scheduler.cpp` that are completed in [Project 3](#project-3).

---

### [Project 3](./P3/)

Project 3 is a continuation of [Project 2](#project-2) in that it completely implements 2 different scheduling protocols (RM and DM). `P3.ino` is identical `project2.ino` with the addition of extra task sets. `rm_scheduler.cpp` and `dm_scheduler.cpp` house the modified code from `scheduler.cpp` but changes its functionality according to the rules of each respective protocol.