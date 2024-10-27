#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <signal.h>
#include <math.h>
#include <string.h>

short **uniValalloc(long width, long height){
	/* return short ptr to space (width) of short ptrs to space (height) of
	short int */
	short **universe_cell_vals = (short **) malloc(width * sizeof(short *));
	if (universe_cell_vals == NULL){
		fprintf(stderr, "ERROR: Could not allocate enough memory!!!\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	for (unsigned long i = 0; i < width; i++){
		*(universe_cell_vals + i) = (short *) malloc(height * sizeof(short));
		if (*(universe_cell_vals + i) == NULL){
			fprintf(stderr, "ERROR: Could not allocate enough memory!!!\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}
		for (unsigned long j = 0; j < height; j++){
			*(*(universe_cell_vals + i) + j) = 0;
		}
	}
	return universe_cell_vals;
}

/* with uni copy too */
void freeUnis(short **universe_cell_vals, long width, long height){
	for (long i = 0; i < width; i++){
		free(*(universe_cell_vals + i));
	}
	free(universe_cell_vals);
}

int interrupt_flag = 0;
void interruptHandle(int sig){
	interrupt_flag = 1;
}

void segFaulthandle(int sig){
	interrupt_flag = 2;
	fprintf(stderr, "ERROR: Segmentation Fault!!!\n");
	fflush(stderr);
}

short getCellstate(short **universe_cell_vals, long x, long y){
	return *(*(universe_cell_vals + x) + y);
}

void inputCellstate(short **universe_cell_vals, short value, long x, long y){
	*(*(universe_cell_vals + x) + y) = value;
}

/* NEEDS TO BE 100 X 100 OR IT WILL SEG FAULT */
void spawnGlider(short **universe_cell_vals){
	inputCellstate(universe_cell_vals, 1, 50, 50);
	inputCellstate(universe_cell_vals, 1, 51, 50);
	inputCellstate(universe_cell_vals, 1, 49, 49);
	inputCellstate(universe_cell_vals, 1, 50, 49);
	inputCellstate(universe_cell_vals, 1, 49, 51);
}

unsigned long numInrange(unsigned long num, unsigned long high_val, unsigned long low_val){
	return (num % ((high_val - low_val) + 1)) + low_val;
}

long high(long x, long y){
	if (x >= y){
		return x;
	}else{
		return y;
	}
}

long low(long x, long y){
	if (x <= y){
		return x;
	}else{
		return y;
	}
}

short randUni(short **universe_cell_vals, long width, long height){
	unsigned long rand_num_1, rand_num_2, rand_num_3, rand_num_4, rand_num_5, rand_num_6;
	getrandom(&rand_num_1, sizeof(unsigned long), 1);
	long coord_chk = numInrange(rand_num_1, width * height, 0);
	for (long x = 0; x < width; x++){
		for (long y = 0; y < height; y++){
			getrandom(&rand_num_2, sizeof(unsigned long), 1);
			long chk = numInrange(rand_num_2, width * height, 0);
			if (chk == coord_chk){
				inputCellstate(universe_cell_vals, 1, x, y);
				getrandom(&rand_num_3, sizeof(unsigned long), 1);
				getrandom(&rand_num_4, sizeof(unsigned long), 1);
				long corner_x = numInrange(rand_num_3, width - 1, 0);
				long corner_y = numInrange(rand_num_4, height - 1, 0);
				inputCellstate(universe_cell_vals, 1, corner_x, corner_y);
				getrandom(&rand_num_5, sizeof(unsigned long), 1);
				long coord_chk_2 = numInrange(rand_num_5, (long) (fabs(x - corner_x) * fabs(y - corner_y)), 0);
				long low_x, low_y, high_x, high_y;
				high_x = high(corner_x, x);
				high_y = high(corner_y, y);
				low_x = low(corner_x, x);
				low_y = low(corner_y, y);
				for (long i = low_x + 1; i < high_x; i++){
					for (long j = low_y + 1; j < high_y; j++){
						getrandom(&rand_num_6, sizeof(unsigned long), 1);
						long chk_2 = numInrange(rand_num_6, (long) (fabs(x - corner_x) * fabs(y - corner_y)), 0);
						if (chk_2 == coord_chk_2){
							inputCellstate(universe_cell_vals, 1, i, j);
						}
					}
				}
			}
		}
	}
}


short **uniValCp(short **universe_cell_vals, long width, long height){
	short **universe_cell_vals_cp = uniValalloc(width, height);
	for (long x = 0; x < width; x++){
		for (long y = 0; y < height; y++){
			inputCellstate(universe_cell_vals_cp, getCellstate(universe_cell_vals, x, y), x, y);
		}
	}
	return universe_cell_vals_cp;
}

short updateUni(short **universe_cell_vals, long width, long height, int sig_fault){
	short **universe_cell_vals_cp = uniValCp(universe_cell_vals, width, height);
	if (sig_fault != 0){
		freeUnis(universe_cell_vals_cp, width, height);
		return -1;
	}
	short n_count;
	long a, b;
	for (long x = 0; x < width; x++){
		for (long y = 0; y < height; y++){
			n_count = 0;
			a = x - 1;
			b = y + 1;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
				n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
			}
			a = x;
			b = y + 1;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			a = x + 1;
			b = y + 1;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			a = x - 1;
			b = y;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			a = x + 1;
			b = y;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			a = x - 1;
			b = y - 1;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			a = x;
			b = y - 1;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			a = x + 1;
			b = y - 1;
			if ((0 <= a) && (a < width) && (0 <= b) && (b < height)){
                                n_count = n_count + getCellstate(universe_cell_vals_cp, a, b);
                        }
			if (n_count == 3){
				inputCellstate(universe_cell_vals, 1, x, y);
			}else if ((n_count < 2) || (n_count > 3)){
				inputCellstate(universe_cell_vals, 0, x, y);
			}
		}
	}
	freeUnis(universe_cell_vals_cp, width, height);
}


long outputState(short **universe_cell_vals, long width, long height, unsigned long gen, char username[100]){
	char file_path[1000];
	sprintf(file_path, "/home/%s/.game-of-life-output/states/%lu.pbm", username, gen);
	FILE *pbmfile = fopen(file_path, "w");
	if (pbmfile == NULL){
		fprintf(stderr, "ERROR: Could not open %s\n", file_path);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	long live_cells = 0;
	fprintf(pbmfile, "P1\n%ld\n%ld\n", width, height);
	for (long y = (height - 1); y >= 0; y--){
		for (long x = 0; x < width; x++){
			short value = getCellstate(universe_cell_vals, x, y);
			if (value){
				live_cells++;
			}
			fprintf(pbmfile, "%d", value);
		}
		fprintf(pbmfile, "\n");
	}
	fclose(pbmfile);
	return live_cells;
}

void compileStates(unsigned long index, char username[100], FILE *gol_out_list){
	system("clear");
	char ffmpeg_out[1000]; 
	sprintf(ffmpeg_out, "ffmpeg -y -i ~/.game-of-life-output/states/%%d.pbm ~/.game-of-life-output/compiled-output/%lu.mp4", index);
	system(ffmpeg_out);
	char file_path[1000];
	sprintf(file_path, "/home/%s/.game-of-life-output/compiled-output/%lu.mp4", username, index);
	fprintf(gol_out_list, "file '%s'\n", file_path);
	system("clear");
}

void compileAll(void){
	system("rm -rf ~/.game-of-life-output/states");
	system("clear");
	system("ffmpeg -f concat -safe 0 -i /tmp/tmp-gol-out-list -c copy ~/.game-of-life-output/output.mp4");
	printf("\n\nWatch output at ~/.game-of-life-output/output.mp4\n\n");
}

void main(int argc, char **argv){
	signal(SIGINT, interruptHandle);
	signal(SIGSEGV, segFaulthandle);
	
	if ((argc < 4) || (argc > 4)){
		printf("Proper Usage: gol [WIDTH] [HEIGHT] [MAX GENERATIONS (number or 'inf')]\n\n");
		exit(EXIT_SUCCESS);
	}
	long width = strtol(*(argv + 1), NULL, 10);
	long height = strtol(*(argv + 2), NULL, 10);
	unsigned long gen;
	if (strcmp("inf",*(argv + 3)) != 0){
		gen = strtoul(*(argv + 3), NULL, 10);
	}else{
		gen = -1;
	}
	unsigned long state_reset_val = 500000000 / (5 + strlen(*(argv + 1)) + strlen(*(argv + 2)) + width + (width * height));
	
	char username[100];
	system("echo $USER > /tmp/tmp-username-for-gol-dont-touch-please");
	FILE *tmp = fopen("/tmp/tmp-username-for-gol-dont-touch-please", "r");
	FILE *gol_out_list = fopen("/tmp/tmp-gol-out-list", "w");
	if ((tmp == NULL) || (gol_out_list == NULL)){
		printf("Failed to open tmp files\n\n");
		exit(EXIT_FAILURE);
	}
	fscanf(tmp, "%s", username);
	fclose(tmp);

	system("clear");
	system("rm -rf ~/.game-of-life-output");
	system("mkdir ~/.game-of-life-output");
	system("mkdir ~/.game-of-life-output/compiled-output/");
	system("mkdir ~/.game-of-life-output/states/");
	short **universe = uniValalloc(width, height);
	unsigned long live_cells;
	/*spawnGlider(universe);*/
	unsigned long state_index = 0;
	unsigned long output_index = 0;
	for (unsigned long g = 0; g <= gen; g++){
		if (interrupt_flag == 1){
			freeUnis(universe, width, height);
			break;
		}else if (interrupt_flag == 2){
			freeUnis(universe, width, height);
			exit(EXIT_FAILURE);
		}
		live_cells = outputState(universe, width,  height, state_index, username);
		if (state_index == state_reset_val){
			state_index = 0;
			compileStates(output_index, username, gol_out_list);
			output_index++;
			system("rm -rf ~/.game-of-life-output/states");
			system("mkdir ~/.game-of-life-output/states");
		}else{
			state_index++;
		}
		updateUni(universe, width, height,  interrupt_flag);
		randUni(universe,  width,  height);
		printf("\r[Current G: %lu] [Live Cells: %lu]", g, live_cells);
		fflush(stdout);
	}
	if (interrupt_flag == 0){
		freeUnis(universe, width, height);
	}
	compileStates(output_index + 1, username, gol_out_list);
	fclose(gol_out_list);
	compileAll();
	exit(EXIT_SUCCESS);
}
