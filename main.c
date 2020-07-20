//Hex-Rom
//-Aminotreal
#include <stdio.h>	//printf, main()
//#include <stdlib.h>	//malloc exit()
#include <time.h>	//for clock()

struct comparator{
	int ss;
	int mode;
};
struct layer{
	struct comparator sub;
	struct comparator inv;
};
struct furnaces{
	struct comparator *Sub_Furnace;//30
	struct comparator *Inv_Furnace;//31
	struct layer *Layer_Furnace;//739
};
void print_barrels(int depth, int *permutation, struct furnaces, int invert_input);
void fprint_barrels(int depth, int *permutation, struct furnaces, int invert_input, FILE *);
int normalize(int *permutation, int *options, int types);
int find_sequence(int filled, int (*Array)[16], int *sequence);

int main(void){
	int numbers[] = { 119, 18, 93, 91, 58, 107, 111, 82, 127, 123, 0 , 0, 0, 0, 0, 0};
	//carried across from old program ^
	
	FILE *file = NULL;
	
	
	//int sub_sequence[16] = {1,1,1,1,1,0,0,1,1,1,0};//also both sequence if !dualoutput
	//int inv_sequence[16] = {0,0,1,1,1,1,1,0,0,0,0};
	
	//Sub sequence is the main sequence if single output
	int sub_sequence[16] = {1,1,0,0,0,1,1,1,0,0,0,1,0,1,0,1};
	int inv_sequence[16] = {1,1,0,0,0,0,0,1,0,0,0,0,1,1,1,1};
	
	clock_t start = clock();
	clock_t end = clock();
	//clock_t total = clock();
	enum {//Settings
	size            = 16,	//First n sequence values defined (last can be anything)
	depth           = 3,	//How many layers of memory
	all_inv_in      = 0,	//If testing for all inv in
	invert_input    = 0,	//What value the input is inverted by(0 = no inv in)
	do_binary       = 1,	//If the sequences are hex or binary
	dual_output     = 0,	//Both sequences or not
	ored2_output    = 0,	//Outputs Ored. for dual sequence
	invert_output   = 0,	//subtract sequence value from this (don't let it go negative)
	seq_from_number = 0,	//If you want to generate the sequence from the numbers
	seg1            = 0,
	seg2            = 4,
	print_to_file 	= 0,	//if it should print to a file
	temp            = 0
	};	//End of settings
	
	if (print_to_file){
		file = fopen("Output.txt", "w");
	}
	
	if (seq_from_number){	//set sequences to numbers[] segment position seg1, seg2
		printf("fromnumber\n");
		for (int i = 0; i < size; ++i)
			sub_sequence[i] = numbers[i] & (1 << seg1) ? 1 : 0;
		for (int i = 0; i < size; ++i)
			inv_sequence[i] = numbers[i] & (1 << seg2) ? 1 : 0;
	}
	if (invert_output){	//inverts output by inverting the required sequence
		for (int i = 0; i < size; ++i){
			int temp0 = invert_output - sub_sequence[i];
			int temp1 = invert_output - inv_sequence[i];
			if (temp0 < 0) temp0 = 0;
			if (temp1 < 0) temp1 = 0;
			sub_sequence[i] = temp0;
			inv_sequence[i] = temp1;
		}
	}
	
	//Pre-Generate comp1(sub comp) Cypher
	int Sub_Cypher[32][16] = {0}, Sub_used = 0;
	struct comparator Sub_Furnace[32];//30
	for (int mode = 0; mode <=1; ++mode){
		for (int subSS = 0; subSS < 16; ++subSS){
			int sequence[16];
			for (int input = 0; input < 16; ++input){
				int output = 0;
				if (mode) output = input >= subSS ? input : 0;//comp1 compare mode
				else output = input >= subSS ? input - subSS : 0;//comp1 subtract mode
				sequence[input] = output;
			}
			int found = find_sequence(Sub_used, Sub_Cypher, sequence);
			if (!found){
				for (int i = 0; i < 16; ++i)
					Sub_Cypher[Sub_used][i] = sequence[i];
				Sub_Furnace[Sub_used].ss = subSS;
				Sub_Furnace[Sub_used].mode = mode;
				++Sub_used;
			}
		}
	}
	
	//Pre-Generate comp2(inv comp) Cypher
	int Inv_Cypher[32][16] = {0}, Inv_used = 0;
	struct comparator Inv_Furnace[32];//31
	for (int mode = 0; mode <=1; ++mode){
		for (int invSS = 0; invSS < 16; ++invSS){
			int sequence[16];
			for (int input = 0; input < 16; ++input){
				int output = 0;
				if (mode) output = invSS >= input ? invSS : 0;//comp2 compare mode
				else output = invSS >= input ? invSS - input : 0;//comp2 subtract mode
				sequence[input] = output;
			}
			int found = find_sequence(Inv_used, Inv_Cypher, sequence);
			if (!found){
				for (int i = 0; i < 16; ++i)
					Inv_Cypher[Inv_used][i] = sequence[i];
				Inv_Furnace[Inv_used].ss = invSS;
				Inv_Furnace[Inv_used].mode = mode;
				Inv_used++;
			}
		}
	}
	
	//Pre-Generate layer Cypher
	int Layer_Cypher[739][16], Layer_used = 0;
	struct layer Layer_Furnace[739];//739
	for (int comp1 = 0; comp1 < Sub_used; ++comp1){	
		for (int comp2 = 0; comp2 < Inv_used; ++comp2){
			int sequence[16];
			for (int input = 0; input < 16; ++input){
				int out1 = Sub_Cypher[comp1][input];
				int out2 = Inv_Cypher[comp2][input];
				sequence[input] = out1 > out2 ? out1 : out2;
			}
			int found = find_sequence(Layer_used, Layer_Cypher, sequence);
			if (!found){
				for (int i = 0; i < 16; ++i)
					Layer_Cypher[Layer_used][i] = sequence[i];
				Layer_Furnace[Layer_used].sub.ss = Sub_Furnace[comp1].ss;
				Layer_Furnace[Layer_used].sub.mode = Sub_Furnace[comp1].mode;
				Layer_Furnace[Layer_used].inv.ss = Inv_Furnace[comp2].ss;
				Layer_Furnace[Layer_used].inv.mode = Inv_Furnace[comp2].mode;
				Layer_used++;
			}
		}
	}
	struct furnaces Furnaces;
	Furnaces.Sub_Furnace = Sub_Furnace;
	Furnaces.Inv_Furnace = Inv_Furnace;
	Furnaces.Layer_Furnace = Layer_Furnace;
	
	enum {types = 3 + depth - 1};	//how many digits(types) in permutation
	
	enum {inv_out, sub_out, inv_in, layers};
	
	int Permutation_Max[types] = {0};
	Permutation_Max[inv_out] = Inv_used - 1;//inv out
	Permutation_Max[sub_out] = Sub_used - 1;//sub_out
	Permutation_Max[inv_in] = all_inv_in ? (Inv_used-1) : 0;//inv in
	for (int layer = 0; layer < depth - 1; ++layer)
		Permutation_Max[layers + layer] =  Layer_used - 1;//layers past 1
	
	int millions = 0;
	int tested = 0;
	for (int permutation[types] = {0}; normalize(permutation, Permutation_Max, types) ;++permutation[0]){//for each memory layout
		//for (int i = 0; i < types; ++i)printf("%d ", permutation[i]);putchar('\n');			//Print permutation
		int fail = 0;
		for (int input = 0; input < size && !fail; ++input){//for each ss input simulate permutation
			int wire = input;
			if (all_inv_in && permutation[inv_in])
				wire = Inv_Cypher[permutation[inv_in]][input];
			if (!all_inv_in && invert_input)
				wire = invert_input - input >= 0 ? invert_input - input : 0;
			for (int layer = 0; layer < depth - 1; ++layer)
				wire = Layer_Cypher[permutation[layers + layer]][wire];
			int out1 = Sub_Cypher[permutation[sub_out]][wire];
			int out2 = Inv_Cypher[permutation[inv_out]][wire];
			
			if (ored2_output){
				if (out1 > out2) out2 = out1 - 1;
				if (out2 > out1) out1 = out2 - 1;
			}
			int output = out1 > out2 ? out1 : out2;
			if (!dual_output){
				if (do_binary && output > 1){
					output = 1;
				}
				if (output != sub_sequence[input])
					fail = 1;
			}
			else{
				if (do_binary){
					if (out1) out1 = 1;
					if (out2) out2 = 1;
				}
				if (out1 != sub_sequence[input] || out2 != inv_sequence[input])
					fail = 1;
			}
		}
		if (!fail){
			print_barrels(depth, permutation, Furnaces, invert_input);
			if (print_to_file){
				fprint_barrels(depth, permutation, Furnaces, invert_input, file);
			}
		}
		if (tested == 100000000){
			tested = 0;
			end = clock();
			//printf("time: %f ", (float)(end - total)/CLOCKS_PER_SEC);//accumulative time
			printf("time: %f ", (float)(end - start)/CLOCKS_PER_SEC);//time since last
			printf("Millions: %d00\n", millions);
			start = clock();
			millions++;
		}
		tested++;
	}
	printf("Tested: %d%08d\n", millions, tested);
	printf("\a"); //ding?
	
	if (file) fclose(file);
}

inline int normalize(int *permutation, int *options, int types){
	int i = 0;
	for (;permutation[i] > options[i] && i < (types - 1);){
		permutation[i] = 0;
		++permutation[++i];
	}
	//return ((unsigned)(permutation[i] - 1 - options[i]) & (1<<31)) >> 31;
	//return !(permutation[i] > options[i]);
	return permutation[i] <= options[i];
}
int find_sequence(int filled, int (*Array)[16], int * restrict sequence){
	int found = 0;
	for (int i = 0; (i < filled) && (!found); ++i){
		int unique = 0;
		for (int num = 0; (num < 16 ) && (!unique); ++num){
			if (Array[i][num] != sequence[num])
				unique = 1;
		}
		if (!unique)
			found = 1;
	}
	return found;
}
void print_barrels(int depth, int *permutation, struct furnaces Furnaces, int invert_input){
	enum {inv_out, sub_out, inv_in, layers};
	struct comparator *Sub_Furnace = Furnaces.Sub_Furnace;
	struct comparator *Inv_Furnace = Furnaces.Inv_Furnace;
	struct layer *Layer_Furnace = Furnaces.Layer_Furnace;
	
	putchar(Inv_Furnace[permutation[inv_in]].mode ? '*' : ' ');
	if (permutation[inv_in])
		printf("%d ",  Inv_Furnace[permutation[inv_in]].ss);
	else
		printf("%d ",  invert_input);
	for (int layer = 0; layer < depth - 1; ++layer){
		putchar(Layer_Furnace[permutation[layers + layer]].sub.mode ? '*' : ' ');
		printf("%X,", Layer_Furnace[permutation[layers + layer]].sub.ss);
		putchar(Layer_Furnace[permutation[layers + layer]].inv.mode ? '*' : ' ');
		printf("%X ", Layer_Furnace[permutation[layers + layer]].inv.ss);
	}
	putchar(Sub_Furnace[permutation[sub_out]].mode ? '*' : ' ');
	printf("%X,", Sub_Furnace[permutation[sub_out]].ss);
	putchar(Inv_Furnace[permutation[inv_out]].mode ? '*' : ' ');
	printf("%X\n", Inv_Furnace[permutation[inv_out]].ss);
}
void fprint_barrels(int depth, int *permutation, struct furnaces Furnaces, int invert_input, FILE *file){
	enum {inv_out, sub_out, inv_in, layers};
	struct comparator *Sub_Furnace = Furnaces.Sub_Furnace;
	struct comparator *Inv_Furnace = Furnaces.Inv_Furnace;
	struct layer *Layer_Furnace = Furnaces.Layer_Furnace;
	
	putc(Inv_Furnace[permutation[inv_in]].mode ? '*' : ' ', file);
	if (permutation[inv_in])
		fprintf(file, "%d ",  Inv_Furnace[permutation[inv_in]].ss);
	else
		fprintf(file, "%d ",  invert_input);
	for (int layer = 0; layer < depth - 1; ++layer){
		putc(Layer_Furnace[permutation[layers + layer]].sub.mode ? '*' : ' ', file);
		fprintf(file, "%X,", Layer_Furnace[permutation[layers + layer]].sub.ss);
		putc(Layer_Furnace[permutation[layers + layer]].inv.mode ? '*' : ' ', file);
		fprintf(file, "%X ", Layer_Furnace[permutation[layers + layer]].inv.ss);
	}
	putc(Sub_Furnace[permutation[sub_out]].mode ? '*' : ' ', file);
	fprintf(file, "%X,", Sub_Furnace[permutation[sub_out]].ss);
	putc(Inv_Furnace[permutation[inv_out]].mode ? '*' : ' ', file);
	fprintf(file, "%X\n", Inv_Furnace[permutation[inv_out]].ss);
}