#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char *argv[])
{
		struct tm *t;
		time_t tt;

		tt = time(NULL);
		if(errno == EFAULT)
				perror("time\n");

		// GMT Time
		t = gmtime(&tt);	
		printf("gmt time   : %s", asctime(t));

		// Local Time
		t = localtime(&tt);
		printf("local time : %s", asctime(t));
		printf("local time : Hour:%d, Min:%d, Sec:%d\n",t->tm_hour,t->tm_min,t->tm_sec);

		return 0;
}
