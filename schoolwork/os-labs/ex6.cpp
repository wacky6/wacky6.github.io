#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <list>
using std::list;

typedef unsigned char byte;
typedef uint32_t      u32;
typedef u32           uid_t;
typedef u32           mask_t;
#define SET_R(m) (m |= (1<<2))
#define SET_W(m) (m |= (1<<1))
#define SET_X(m) (m |= (1<<0))
#define HAS_R(m) (m & (1<2))
#define HAS_W(m) (m & (1<1))
#define HAS_X(m) (m & (1<0))

struct access_t {
	uid_t  owner;
	mask_t mask;
};

struct ufd_t {
	access_t access;
	char     name[256];
	u32      size;
	char     phys[256];
};

struct mfd_t {
	access_t access;
	u32      size;
	ufd_t    files[256];
};

struct afd_t {
	char    name[256];
	mask_t  mask;
	FILE*   f_ptr;
};

void UFD(ufd_t* ufd, uid_t owner, mask_t mask, u32 size,
         const char* name, const char* phys) { 
	ufd->access.owner = owner;                    
	ufd->access.mask  = mask;                     
	ufd->size         = size;                     
	strcpy(ufd->name, name);       
	strcpy(ufd->phys, phys);       
}

void MFD(mfd_t* mfd, uid_t owner, mask_t mask, u32 size) {
	mfd->access.owner = owner;             
	mfd->access.mask  = mask;              
	mfd->size         = size;              
}



void dir(const mfd_t& mfd) {
	printf("dir:\n");
	for (int i=0; i!=mfd.size; ++i)
		printf("  %u:%x %s => %s\n", mfd.files[i].access.owner,
		                             mfd.files[i].access.mask,
		                             mfd.files[i].name, 
							   	     mfd.files[i].phys);
}

void create(mfd_t& mfd, uid_t owner, mask_t access,
            const char* name, const char* phys) {
	UFD(&mfd.files[mfd.size], owner, access, 1, name, phys);
	mfd.size++;
}

void delete_f(mfd_t& mfd, const char* name) {
	u32 index=-1;
	for (u32 i=0; i!=mfd.size && index==-1; ++i)
		if (strcmp(mfd.files[i].name, name)==0)
			index=i;
	if (index==-1) return;
	for (u32 i=index+1; i<=mfd.size; ++i)
		mfd.files[i-1] = mfd.files[i];
	mfd.size--;
}

afd_t* open_f(mfd_t& mfd, const char* name, uid_t uid, mask_t access) {
	u32 index=-1;
	for (u32 i=0; i!=mfd.size && index==-1; ++i)
		if (strcmp(mfd.files[i].name, name)==0)
			index=i;
	if (index==-1) {
		puts("file not exist");
		return 0;	
	}
	ufd_t& ufd = mfd.files[index];
	if (ufd.access.owner != uid) {
		puts("not owner");
		return 0;
	}
	if (access != (ufd.access.mask & access)) {
		printf("no access, file=%x, request=%x\n", ufd.access.mask, access);
		return 0;
	}
	afd_t* afd = new afd_t();
	FILE* f = fopen(ufd.phys, "rw");
	if (f==0) {
		perror("fopen");
		return 0;
	}
	strcpy(afd->name, ufd.name);
	afd->mask  = access;
	afd->f_ptr = f;
	return afd;
}

void close_f(afd_t* afd) {
	fclose(afd->f_ptr);
	delete afd;
}

void read_f(afd_t* afd) {
	printf("File content: \n");
	char buf[65536];
	while (!feof(afd->f_ptr)) {
		strcpy(buf, "");
		fgets(buf, 65536, afd->f_ptr);
		printf("%s", buf);
	}
}

void write_f(afd_t* afd, const char* str) {
	fputs(str, afd->f_ptr);
}

int main() {
	// initialize vfs	
	mfd_t mfd;
	MFD(&mfd, 1, 7, 0);

	create(mfd, 1, 6, "passwd", "_passwd");
	dir(mfd);

	afd_t* f1;
	f1 = open_f(mfd, "passwd", 0, 4);  // not owner
	f1 = open_f(mfd, "passwd", 1, 4);
	read_f(f1);
	close_f(f1);

	f1 = open_f(mfd, "passwd", 1, 5);  // no access
		
}
