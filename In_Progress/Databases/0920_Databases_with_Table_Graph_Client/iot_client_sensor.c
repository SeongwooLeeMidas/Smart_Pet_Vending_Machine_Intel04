#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <mysql/mysql.h>

#define BUF_SIZE 100
#define NAME_SIZE 30
#define ARR_CNT 10 

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE] = "[Default]";
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread, mysql_thread;
	void* thread_return;

	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	sprintf(name, "%s", argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	sprintf(msg, "[%s:PASSWD]", name);
	write(sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);


	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);
	return 0;
}


void* send_msg(void* arg)
{
	int* sock = (int*)arg;
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[NAME_SIZE + BUF_SIZE + 2];

	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n", stdout);
	while (1) {
		memset(msg, 0, sizeof(msg));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if (FD_ISSET(STDIN_FILENO, &newset))
		{
			fgets(msg, BUF_SIZE, stdin);
			if (!strncmp(msg, "quit\n", 5)) {
				*sock = -1;
				return NULL;
			}
			else if (msg[0] != '[')
			{
				strcat(name_msg, "[ALLMSG]");
				strcat(name_msg, msg);
			}
			else
				strcpy(name_msg, msg);
			if (write(*sock, name_msg, strlen(name_msg)) <= 0)
			{
				*sock = -1;
				return NULL;
			}
		}
		if (ret == 0)
		{
			if (*sock == -1)
				return NULL;
		}
	}
}

void* recv_msg(void* arg)
{
    MYSQL* conn;
    MYSQL_ROW sqlrow;
    MYSQL_RES* result;
    int res;
    char sql_cmd[200] = { 0 };
    char* host = "localhost";
    char* user = "iot";
    char* pass = "pwiot";
    char* dbname = "iotdb";

    int* sock = (int*)arg;
    int i;
    char* pToken;
    char* pArray[ARR_CNT] = { 0 };

    char name_msg[NAME_SIZE + BUF_SIZE + 1];
    int str_len;

	char breed[20];
    int item1, item2, item3, item4, item5, item6;

    conn = mysql_init(NULL);

    puts("MYSQL startup");
    if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
    {
        fprintf(stderr, "ERROR : %s[%d]\n", mysql_error(conn), mysql_errno(conn));
        exit(1);
    }
    else
        printf("Connection Successful!\n\n");

    while (1) {
        memset(name_msg, 0x0, sizeof(name_msg));

        if (*sock == -1) {
            fprintf(stderr, "ERROR: Invalid socket\n");
            return NULL;
        }

		printf("DEBUG: Before reading from socket\n");

        str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE);
        if (str_len <= 0)
        {
            *sock = -1;
            return NULL;
        }

        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
		printf("DEBUG: Received message: %s\n", name_msg);

        pToken = strtok(name_msg, "[:@]");
        i = 0;
        while (pToken != NULL && i < ARR_CNT)
        {
            pArray[i] = pToken;
            i++;
            pToken = strtok(NULL, "[:@]");
        }

        // INSERT 처리
        if (!strcmp(pArray[1], "DB") && (i == 9)) {
            item1 = atoi(pArray[3]);
            item2 = atoi(pArray[4]);
            item3 = atoi(pArray[5]);
            item4 = atoi(pArray[6]);
            item5 = atoi(pArray[7]);
            item6 = atoi(pArray[8]);

            sprintf(sql_cmd, "insert into dog_food_sales(name, date, time, breed, item1, item2, item3, item4, item5, item6) values(\"%s\",now(),now(),\"%s\",%d,%d,%d,%d,%d,%d)",
                    pArray[0], pArray[2], item1, item2, item3, item4, item5, item6);

            res = mysql_query(conn, sql_cmd);
            if (res) {
                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
                continue;
            }
            printf("Inserted %lu rows\n", (unsigned long)mysql_affected_rows(conn));
        }

        // DB_RECO 처리
        else if (!strcmp(pArray[1], "DB_RECO")) {
				printf("DEBUG: Entered DB_RECO logic\n");

				sprintf(sql_cmd, "SELECT SUM(item1) AS item1_count, SUM(item2) AS item2_count, "
								"SUM(item3) AS item3_count, SUM(item4) AS item4_count, "
								"SUM(item5) AS item5_count, SUM(item6) AS item6_count "
								"FROM dog_food_sales");

			printf("DEBUG: SQL Command for DB_RECO: %s\n", sql_cmd);

            res = mysql_query(conn, sql_cmd);
            if (res != 0) {
                fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
                return NULL;  // 오류 발생 시 루프 종료
            }

            result = mysql_store_result(conn);
            if (!result) {
                fprintf(stderr, "ERROR: mysql_store_result failed: %s[%d]\n", mysql_error(conn), mysql_errno(conn));
                return NULL;  // result가 NULL이면 처리 중단
            }

			if (mysql_num_rows(result) == 0) {
					fprintf(stderr, "ERROR: No rows found in the result\n");
					mysql_free_result(result);
					return NULL;
			}

			printf("DEBUG: mysql_store_result 성공\n");

			sqlrow = mysql_fetch_row(result);
			if (sqlrow == NULL) {
					fprintf(stderr, "ERROR: mysql_fetch_row returned NULL\n");
					mysql_free_result(result);
					return NULL;
			}

			int item_counts[6] = {0};
			for (int i = 0; i < 6; i++) {
					if (sqlrow[i] != NULL) {
							item_counts[i] = atoi(sqlrow[i]);
							printf("DEBUG: item_counts[%d] = %d\n", i, item_counts[i]);
					} else {
							fprintf(stderr, "ERROR: sqlrow[%d] is NULL\n", i);
							item_counts[i] = 0;
					}
			}

            mysql_free_result(result);

			int max1_idx = 0, max2_idx = 1;
			if (item_counts[1] > item_counts[0]) {
					max1_idx = 1;
					max2_idx = 0;
			}

			for (int i = 2; i < 6; i++) {
					if (item_counts[i] > item_counts[max1_idx]) {
							max2_idx = max1_idx;
							max1_idx = i;
					} else if (item_counts[i] > item_counts[max2_idx]) {
							max2_idx = i;
					}
			}

			printf("DEBUG: max1_idx = %d, max2_idx = %d\n", max1_idx + 1, max2_idx + 1);

            sprintf(name_msg, "[KJD_QT]DB_BREED@%d@%d\n", max1_idx + 1, max2_idx + 1);
            if (write(*sock, name_msg, strlen(name_msg)) <= 0) {
                fprintf(stderr, "ERROR: Failed to write to socket\n");
                *sock = -1;
                return NULL;
            }
        }
    }

    mysql_close(conn);
    return NULL;
}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
