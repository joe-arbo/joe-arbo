#include "scheduler.h"

TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t xHandle4 = NULL;


// the loop function runs over and over again forever
void loop() {}


static void testFunc1( void *pvParameters )
{
	(void) pvParameters;
	int i,a;
	for( i = 0; i < 10000; i++ )
	{
		a = 1 + i*i*i*i;
	}	
}

static void testFunc2( void *pvParameters )
{ 
	(void) pvParameters;	
	int i, a;	
	for(i = 0; i < 10000; i++ )
	{
		a = 1 + a * a * i;
	}	
}


static void set1Test1(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 100; i++)
    {
        delayMicroseconds(1000);
    }
}

static void set1Test2(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 200; i++)
    {
        delayMicroseconds(1000);
    }
}

static void set1Test3(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 150; i++)
    {
        delayMicroseconds(1000);
    }
}

static void set1Test4(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 300; i++)
    {
        delayMicroseconds(1000);
    }
}


static void set2Test1(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 100; i++)
    {
        delayMicroseconds(1000);
    }
}

static void set2Test2(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 150; i++)
    {
        delayMicroseconds(1000);
    }
}

static void set2Test3(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 200; i++)
    {
        delayMicroseconds(1000);
    }
}

static void set2Test4(void *pvParameters)
{
    (void)pvParameters;
    int i;
    for (i = 0; i < 150; i++)
    {
        delayMicroseconds(1000);
    }
}


int main( void )
{
	Serial.begin(9600);
	char c1 = 'a';
	char c2 = 'b';
	char c3 = 'c';
	char c4 = 'd';		

	vSchedulerInit();

	// vSchedulerPeriodicTaskCreate(testFunc1, "t1", configMINIMAL_STACK_SIZE, &c1, 1, &xHandle1, pdMS_TO_TICKS(0), pdMS_TO_TICKS(800), pdMS_TO_TICKS(100), pdMS_TO_TICKS(800));
	// vSchedulerPeriodicTaskCreate(testFunc2, "t2", configMINIMAL_STACK_SIZE, &c2, 2, &xHandle2, pdMS_TO_TICKS(0), pdMS_TO_TICKS(400), pdMS_TO_TICKS(400), pdMS_TO_TICKS(400));

	// Task set 1
    vSchedulerPeriodicTaskCreate(set1Test1, "t1", configMINIMAL_STACK_SIZE, &c1, 1, &xHandle1, pdMS_TO_TICKS(0), pdMS_TO_TICKS(400), pdMS_TO_TICKS(100), pdMS_TO_TICKS(400));
    vSchedulerPeriodicTaskCreate(set1Test2, "t2", configMINIMAL_STACK_SIZE, &c2, 2, &xHandle2, pdMS_TO_TICKS(0), pdMS_TO_TICKS(800), pdMS_TO_TICKS(200), pdMS_TO_TICKS(700));
    vSchedulerPeriodicTaskCreate(set1Test3, "t3", configMINIMAL_STACK_SIZE, &c3, 3, &xHandle3, pdMS_TO_TICKS(0), pdMS_TO_TICKS(1000), pdMS_TO_TICKS(150), pdMS_TO_TICKS(1000));
    vSchedulerPeriodicTaskCreate(set1Test4, "t4", configMINIMAL_STACK_SIZE, &c4, 4, &xHandle4, pdMS_TO_TICKS(0), pdMS_TO_TICKS(5000), pdMS_TO_TICKS(300), pdMS_TO_TICKS(5000));

    // Task set 2
    // vSchedulerPeriodicTaskCreate(set2Test1, "t1", configMINIMAL_STACK_SIZE, &c1, 1, &xHandle1, pdMS_TO_TICKS(0), pdMS_TO_TICKS(400), pdMS_TO_TICKS(100), pdMS_TO_TICKS(400));
    // vSchedulerPeriodicTaskCreate(set2Test2, "t2", configMINIMAL_STACK_SIZE, &c2, 2, &xHandle2, pdMS_TO_TICKS(0), pdMS_TO_TICKS(500), pdMS_TO_TICKS(150), pdMS_TO_TICKS(200));
    // vSchedulerPeriodicTaskCreate(set2Test3, "t3", configMINIMAL_STACK_SIZE, &c3, 3, &xHandle3, pdMS_TO_TICKS(0), pdMS_TO_TICKS(800), pdMS_TO_TICKS(200), pdMS_TO_TICKS(700));
    // vSchedulerPeriodicTaskCreate(set2Test4, "t4", configMINIMAL_STACK_SIZE, &c4, 4, &xHandle4, pdMS_TO_TICKS(0), pdMS_TO_TICKS(1000), pdMS_TO_TICKS(150), pdMS_TO_TICKS(1000));

	vSchedulerStart();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached. */
	
	for( ;; );
}

