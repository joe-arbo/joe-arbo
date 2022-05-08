#include "scheduler.h"

TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;



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

int main( void )
{
	char c1 = 'a';
	char c2 = 'b';			

	vSchedulerInit();

	vSchedulerPeriodicTaskCreate(testFunc1, "t1", configMINIMAL_STACK_SIZE, &c1, 1, &xHandle1, pdMS_TO_TICKS(0), pdMS_TO_TICKS(800), pdMS_TO_TICKS(100), pdMS_TO_TICKS(800));
	vSchedulerPeriodicTaskCreate(testFunc2, "t2", configMINIMAL_STACK_SIZE, &c2, 2, &xHandle2, pdMS_TO_TICKS(0), pdMS_TO_TICKS(400), pdMS_TO_TICKS(400), pdMS_TO_TICKS(400));



	vSchedulerStart();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached. */
	
	for( ;; );
}

