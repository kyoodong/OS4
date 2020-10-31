#include "rw_lock.h"
#include <pthread.h>

void init_rwlock(struct rw_lock * rw)
{
	pthread_mutex_init(&rw->mutex, NULL);
	pthread_mutex_init(&rw->lock_mutex, NULL);
	pthread_cond_init(&rw->cond, NULL);

	rw->write_lock = 0;
	rw->read_lock = 0;
	rw->write_wait = 0;
	rw->read_wait = 0;
}

void r_lock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->lock_mutex);
  //	Write the code for aquiring read-write lock by the reader.
	if (rw->write_lock) {
		rw->read_wait++;
		pthread_cond_wait(&rw->cond, &rw->lock_mutex);

		if (rw->read_wait) {
			pthread_cond_signal(&rw->cond);
			rw->read_wait--;
		}
	}

	rw->read_lock++;
	if (rw->read_lock == 1) {
		pthread_mutex_unlock(&rw->lock_mutex);
		pthread_mutex_lock(&rw->mutex);
	} else {
		pthread_mutex_unlock(&rw->lock_mutex);
	}
}

void r_unlock(struct rw_lock * rw)
{
	if (rw->read_lock == 0) {
		return;
	}
  //	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&rw->lock_mutex);
	rw->read_lock--;

	if (rw->read_lock == 0)
		pthread_mutex_unlock(&rw->mutex);

	pthread_mutex_unlock(&rw->lock_mutex);
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&rw->lock_mutex);
	rw->write_lock++;
	pthread_mutex_unlock(&rw->lock_mutex);
	pthread_mutex_lock(&rw->mutex);
}

void w_unlock(struct rw_lock * rw)
{
	pthread_mutex_lock(&rw->lock_mutex);
  //	Write the code for releasing read-write lock by the writer.
	if (rw->write_lock == 0)
		return;
	
	rw->write_lock--;
	if (rw->write_lock == 0 && rw->read_wait) {
		pthread_cond_signal(&rw->cond);
	}
	pthread_mutex_unlock(&rw->mutex);
	pthread_mutex_unlock(&rw->lock_mutex);
}

