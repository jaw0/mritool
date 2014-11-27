
#ifndef _shmem_h
#define _shmem_h


class ShMem {
  private:
	static int sh_unique_key;
  protected:
	int shm_key;
	int shm_id;

	ShMem();
	~ShMem();

	void* shmalloc(int);	// create segment and allocate
	void shmfree(void*);	// detach memory
};


#endif  /* !_shmem_h */
