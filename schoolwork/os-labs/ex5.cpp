#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <vector>
#include <set>
using std::set;
using std::vector;

#define NUM_PAGES (16)
#define NUM_MEM   (4)
#define NUM_INS   (160)

#define u32 uint32_t

typedef struct {
	u32  counter;     // usage counter
	int  in_memory;   // true / false
} page;
page pages[NUM_PAGES];
long i,j,k;

void init_pages() {
	for (i=0; i!=NUM_PAGES; ++i) {
		pages[i].counter   = 0;
		pages[i].in_memory = 0;
	}
}

long pages_in_memory() {
	long num = 0;
	for (i=0; i!=NUM_PAGES; ++i)
		if (pages[i].in_memory) 
			++num;
	return num;
}

// record missed pages (num of page swap)
int miss;

void paging_opt(int req) {
	static vector<int> reqs = vector<int>();	
	if (req>=0) {
		reqs.push_back(req);
	}else{  // calc optimal situation
		set<int> pages;
		for (long i=reqs.size()-1; i>=0; --i) {
			if (pages.find(reqs[i])!=pages.end())
				continue; 
			if (pages.size() >= NUM_MEM) {
				++miss;
				pages.erase(reqs[i]);
			}
			pages.insert(reqs[i]);
		}
	}
}

void paging_fifo(int req) {
	static u32 time = 0;
	if (req<0) return;
	if (pages[req].in_memory) 
		return;
	if (pages_in_memory() < NUM_MEM) {
		pages[req].in_memory = 1;
		pages[req].counter   = ++time;
		return;
	}
	unsigned long earliest = NUM_INS;
	int pg = 0;
	for (i=0; i!=NUM_PAGES; ++i)
		if (pages[i].in_memory && pages[i].counter<earliest) {
			earliest = pages[i].counter;
			pg = i;
		}
	++miss;
	pages[pg].in_memory = 0;
	pages[req].in_memory = 1;
	pages[req].counter = ++time;
}

void paging_lru(int req) {
	if (req<0) return;
	for (i=0; i!=NUM_PAGES; ++i)
		pages[req].counter = pages[req].counter >> 1;

	pages[req].counter = pages[req].counter | (1<<31);
	if (pages[req].in_memory)
		return;
	if (pages_in_memory() < NUM_MEM) {
		pages[req].in_memory = 1;
		return;
	}
	int pg = 0;
	u32 lru = pages[req].counter;
	for (i=0; i!=NUM_PAGES; ++i)
		if (pages[i].in_memory && pages[i].counter<lru) {
			lru = pages[i].counter;
			pg = i;
		}
	++miss;
	pages[pg].in_memory  = 0;
	pages[req].in_memory = 1;
}


long get_page_number(long ins) {
	return ins / 10;
}

void simulate(const char* name, void(*algo)(int)) {
	int c = 0;
	int m = rand() % (NUM_INS-2);
	int m1, m2;

	miss = 0;
	init_pages();

	#define INS(n) {if ((n)<NUM_INS) {         \
					   algo((n)/10);           \
					   ++c;                    \
				   }}

	// (2)
	INS(m+1);
	INS(m+2);

	while (c<200) {
		// (3)
		m1 = rand() % (m);
		INS(m1+1);
		INS(m1+2);

		// (4)
		m2 = m+3 + rand() % (NUM_INS-m-3);
		INS(m2+1);
		INS(m2+2);
	}

	algo(-1);   // signal end of execution

	// print statistic
	printf("%s miss: %d\n", name, miss);

	#undef INS
}


int main() {
	time_t t = time(0);

	srand(t);
	simulate("opt ",  paging_opt);
	srand(t);
	simulate("fifo", paging_fifo);
	srand(t);
	simulate("lru ",  paging_lru);
}
