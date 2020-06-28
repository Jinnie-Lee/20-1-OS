#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct PAGE {
	int memory_location;
	int valid_bit;
};
struct PAGE page[200000];

struct MEMORY {
	int page_num;
	int reference_bit;
	int dirty_bit;
};

int page_fault_cnt = 0;
int disk_write_cnt = 0;
int disk_head_moving_dist = 0;

int main(int argc, char *argv[]) {

	int memory_size = atoi(argv[1]);

	FILE* input_file = fopen("a.txt", "r");
	struct MEMORY* memory = (struct MEMORY*)malloc(memory_size * sizeof(struct MEMORY));

	if (input_file == NULL) {
		printf("파일오픈실패");
		exit(1);
	}

	for (int i = 0; i < memory_size; i++) {
		memory[i].reference_bit = 0;
		memory[i].dirty_bit = 0;
	}
	for (int i = 0; i < 200000; i++) {
		page[i].valid_bit = 0;
	}					//initialization

	int pg, op;			//page number, operator
	int clock = -1;		//clock init
	int disk_head = 0;	//disk head position

	while (!feof(input_file)) {
		fscanf(input_file, "%d %d", &pg, &op);
		if (page[pg].valid_bit == 1) {		//the pg is in memory
			memory[page[pg].memory_location].reference_bit = 1;
			if (op == 1) memory[page[pg].memory_location].dirty_bit = 1;
		}
		else {								//the pg isn't in memory
			page_fault_cnt++;
			clock++;
			if (clock == memory_size) clock = 0;

			if (page_fault_cnt > memory_size) {		//full memory
				while (memory[clock].reference_bit != 0) {
					memory[clock].reference_bit = 0;
					clock++;
					if (clock == memory_size) clock = 0;

				}
				page[memory[clock].page_num].valid_bit = 0;
				if (memory[clock].dirty_bit == 1) {
					disk_head_moving_dist += abs(memory[clock].page_num / 1000 - disk_head);
					disk_head = memory[clock].page_num / 1000;
					disk_write_cnt++;
				}
			}
			disk_head_moving_dist += abs(pg / 1000 - disk_head);
			disk_head = pg / 1000;
			memory[clock].page_num = pg;
			memory[clock].dirty_bit = (op == 1) ? 1 : 0;
			memory[clock].reference_bit = 0;
			page[pg].memory_location = clock;
			page[pg].valid_bit = 1;
		}
	}
	free(memory);
	fclose(input_file);
	printf("%d\n%d\n%d\n", page_fault_cnt, disk_write_cnt, disk_head_moving_dist);
}