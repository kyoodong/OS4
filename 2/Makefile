read_test: rw_lock_r rw_lock
	gcc reader_test.c rw_lock-r-test.o rw_lock.o -lpthread

rw_lock_r:
	gcc -c rw_lock-r-test.c

rw_lock:
	gcc -c rw_lock.c

debug: rw_lock_r_debug rw_lock_debug
	gcc reader_test.c rw_lock-r-test.o rw_lock.o -lpthread -g
	
rw_lock_r_debug:
	gcc -c rw_lock-r-test.c -g

rw_lock_debug:
	gcc -c rw_lock.c -g

write_test: wr_lock_r rw_lock
	gcc writer_test.c rw_lock-w-test.o rw_lock.o -lpthread

wr_lock_r:
	gcc -c rw_lock-w-test.c

