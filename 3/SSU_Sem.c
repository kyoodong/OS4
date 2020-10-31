#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
	s->count = value;
	pthread_mutex_init(&s->mutex, NULL);
	pthread_cond_init(&s->cond, NULL);
}

void SSU_Sem_down(SSU_Sem *s) {
	pthread_mutex_lock(&s->mutex);
	if (s->count == 0) {
		pthread_cond_wait(&s->cond, &s->mutex);
	}
	s->count--;
	pthread_mutex_unlock(&s->mutex);
}

void SSU_Sem_up(SSU_Sem *s) {
	pthread_mutex_lock(&s->mutex);
	s->count++;
	pthread_cond_signal(&s->cond);
	pthread_mutex_unlock(&s->mutex);
}

