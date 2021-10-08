#define TICK_INTERRUPT_PERIOD_MS	50 

#define MS_100	(100/TICK_INTERRUPT_PERIOD_MS)
#define MS_200	(2 * MS_100) 
#define MS_300	(3 * MS_100)
#define MS_400	(4 * MS_100)
#define MS_500	(5 * MS_100)
#define MS_600	(6 * MS_100)
#define MS_700	(7 * MS_700)
#define SEC_1	(10 * MS_100)

void InitTick(void);
unsigned int GetSystemTick(void);
