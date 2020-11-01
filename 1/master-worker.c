#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size, item_to_consume;
int total_items, max_buf_size, num_workers, num_masters;

int *buffer;

pthread_mutex_t mutex;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
	{
			pthread_mutex_lock(&mutex);

			// 모든 아이템을 버퍼에 넣은 상황 -> 종료
      if(item_to_produce >= total_items) {
				pthread_mutex_unlock(&mutex);
				break;
      }

			// 모든 아이템을 다 넣진 못했지만 현재 버퍼가 가득 차 더 이상 넣을 수 없는 상황
			if (curr_buf_size >= max_buf_size) {
				pthread_mutex_unlock(&mutex);
				continue;
			}
 
			// 버퍼에 아이템을 넣고 로그 출력
      buffer[++curr_buf_size] = item_to_produce;
      print_produced(item_to_produce, thread_id);
      item_to_produce++;
			pthread_mutex_unlock(&mutex);
	}
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *generate_responds_loop(void *data) {
	int thread_id = *((int *) data);

	while (1) {
		pthread_mutex_lock(&mutex);

		// 모든 아이템을 소비한 상황 -> 종료
		if (item_to_consume >= total_items) {
			pthread_mutex_unlock(&mutex);
			break;
		}

		// 버퍼가 비어 소비할 아이템이 없는 상황 -> 대기
		if (curr_buf_size < 0) {
			pthread_mutex_unlock(&mutex);
			continue;
		}

		// 아이템을 하나 소비하고 로그 출력
		print_consumed(buffer[curr_buf_size], thread_id);
		buffer[curr_buf_size] = 0;
		item_to_consume++;
		curr_buf_size--;
		pthread_mutex_unlock(&mutex);
	}

	return 0;
}

int main(int argc, char *argv[])
{
  int *master_thread_id;
	int *worker_thread_id;
  pthread_t *master_thread, *worker_thread;
  item_to_produce = 0;
  curr_buf_size = -1;
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }

	pthread_mutex_init(&mutex, NULL);

	// 버퍼 생성
  buffer = (int *)malloc (sizeof(int) * max_buf_size);

  //create master producer threads
  master_thread_id = (int *)malloc(sizeof(int) * num_masters);
	worker_thread_id = (int *) malloc(sizeof(int) * num_workers);
  master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
	worker_thread = (pthread_t *) malloc(sizeof(pthread_t) * num_workers);

	// 스레드 아이디 지정
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

	// 마스터 스레드 생성
  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
	// 스레드 아이디 지정
	for (i = 0; i < num_workers; i++)
		worker_thread_id[i] = i;

  //create worker consumer threads
	for (i = 0; i < num_workers; i++)
		pthread_create(&worker_thread[i], NULL, generate_responds_loop, (void *) &worker_thread_id[i]);
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
    {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
    }

	for (i =  0; i < num_workers; i++) {
		pthread_join(worker_thread[i], NULL);
		printf("worker %d joined\n", i);
	}
  
  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(master_thread);
	free(worker_thread_id);
	free(worker_thread);

	pthread_mutex_destroy(&mutex);
  return 0;
}

