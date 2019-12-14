//Hex-Rom
//-Aminotreal
#include <stdio.h>	//printf, main()
//#include <stdlib.h>	//malloc
#include <time.h>	//for clock()

struct comparator{
	int ss;
	int mode;
};
struct layer{
	struct comparator sub;
	struct comparator inv;
};
int normalize(int *permutation, int *options, int types);
int FindSequence(int filled, int (*Array)[16], int *sequence);

int main(void){
	int numbers[] = { 119, 18, 93, 91, 58, 107, 111, 82, 127, 123, 0 , 0, 0, 0, 0, 0};
	//carried across from old program ^
	
	
	
	//int sub_sequence[16] = {1,1,1,1,1,0,0,1,1,1,0};//also both sequence if !dualoutput
	//int inv_sequence[16] = {0,0,1,1,1,1,1,0,0,0,0};
	int sub_sequence[16] = {1,0,0,0,1,1,1,0,1,1,0};
	int inv_sequence[16] = {1,1,1,1,1,0,0,1,1,1,0};
	
	clock_t start = clock();
	clock_t end = clock();
	clock_t total = clock();
	
	enum {//Settings
	size            = 11,	//How many ss values we care about of the output seqeunce
	depth           = 3,	//layers of memory
	invert_input    = 1,	//If inverted input is allowed
	do_binary       = 1,	//if a repeater is on the output(if output > 1 output = 1)
	dual_output     = 0,	//Both sequences or not
	ored2_output    = 0,	//Outputs Ored. Incompatible with !dual_output
	print_inv       = 0,	//prints only with print_inv inv in value (0 = disabled)
	invert_output   = 0,	//Required sequence = max(invert_output - sequence, 0)
	seq_from_number = 1,	//If you want to generate the sequence from the numbers
	seg1            = 0,
	seg2            = 4
	};	//End of settings
	
	if (depth == 0){
		printf("Error: Depth\n");
		exit(1);
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
	
	int Sub_Cypher[32][16] = {0}, Sub_used = 0;
	struct comparator Sub_Furnace[32];//30
	for (int mode = 0; mode <=1; ++mode){									//Pre-Generate comp1(sub comp) Cypher
		for (int subSS = 0; subSS < 16; ++subSS){
			int sequence[16];
			for (int input = 0; input < 16; ++input){
				int output = 0;
				if (mode) output = input >= subSS ? input : 0;				//comp1 compare mode
				else output = input >= subSS ? input - subSS : 0;			//comp1 subtract mode
				sequence[input] = output;
			}
			int found = FindSequence(Sub_used, Sub_Cypher, sequence);
			if (!found){
				for (int i = 0; i < 16; ++i)
					Sub_Cypher[Sub_used][i] = sequence[i];
				Sub_Furnace[Sub_used].ss = subSS;
				Sub_Furnace[Sub_used].mode = mode;
				++Sub_used;
			}
		}
	}
	
	int Inv_Cypher[32][16] = {0}, Inv_used = 0;
	struct comparator Inv_Furnace[32];//31
	for (int mode = 0; mode <=1; ++mode){									//Pre-Generate comp2(inv comp) Cypher
		for (int invSS = 0; invSS < 16; ++invSS){
			int sequence[16];
			for (int input = 0; input < 16; ++input){
				int output = 0;
				if (mode) output = invSS >= input ? invSS : 0;				//comp2 compare mode
				else output = invSS >= input ? invSS - input : 0;			//comp2 subtract mode
				sequence[input] = output;
			}
			int found = FindSequence(Inv_used, Inv_Cypher, sequence);
			if (!found){
				for (int i = 0; i < 16; ++i)
					Inv_Cypher[Inv_used][i] = sequence[i];
				Inv_Furnace[Inv_used].ss = invSS;
				Inv_Furnace[Inv_used].mode = mode;
				Inv_used++;
			}
		}
	}
	
	int Layer_Cypher[739][16], Layer_used = 0;
	struct layer Layer_Furnace[739];//739
	for (int comp1 = 0; comp1 < Sub_used; ++comp1){				//Pre-Generate layer Cypher
		for (int comp2 = 0; comp2 < Inv_used; ++comp2){
			int sequence[16];
			for (int input = 0; input < 16; ++input){
				int out1 = Sub_Cypher[comp1][input];
				int out2 = Inv_Cypher[comp2][input];
				sequence[input] = out1 > out2 ? out1 : out2;
			}
			int found = FindSequence(Layer_used, Layer_Cypher, sequence);
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
	
	enum {types = 3 + depth - 1};	//how many digits(types) in permutation
	
	int Memory_Layout[types] = {0}, Layout_used = 0;
	Memory_Layout[Layout_used++] = Inv_used - 1;//inv out
	Memory_Layout[Layout_used++] = Sub_used - 1;//sub_out
	Memory_Layout[Layout_used++] = invert_input ? (Inv_used-1) : 0;//inv in
	for (int layer = 1; layer < depth; ++layer)
		Memory_Layout[Layout_used++] =  Layer_used - 1;//layers past 1(if 1 only user sub_out and inv_out)
	
	int tested = 0;
	int millions = 0;
	for (int permutation[types] = {0}; normalize(permutation, Memory_Layout, types) ;++permutation[0]){//for each memory layout
		enum {inv_out, sub_out, inv_in, layers};
		//for (int i = 0; i < types; ++i)printf("%d ", permutation[i]);putchar('\n');			//Print permutation
		int fail = 0;
		for (int input = 0; input < size && !fail; ++input){//for each ss input simulate permutation
		
			int wire = input;
			if (permutation[inv_in])
				wire = Inv_Cypher[permutation[inv_in]][input];
			for (int layer = 0; layer < depth - 1; ++layer)
				wire = Layer_Cypher[permutation[layers + layer]][wire];
			int out1 = Sub_Cypher[permutation[sub_out]][wire];
			int out2 = Inv_Cypher[permutation[inv_out]][wire];
			int output = out1 > out2 ? out1 : out2;
			
			if (!dual_output){
				if (do_binary && output > 1) output = 1;
				if (output != sub_sequence[input])
					fail = 1;
			}
			else{
				if (ored2_output){
					if (out1 > 1) out2 = out1 - 1;
					if (out2 > 1) out1 = out2 - 1;
				}
				if (do_binary){
					if (out1) out1 = 1;
					if (out2) out2 = 1;
				}
				if (out1 != sub_sequence[input] || out2 != inv_sequence[input])
					fail = 1;
			}
		}
		if (!fail){
			if (Inv_Furnace[permutation[inv_in]].ss == print_inv || !print_inv){
				if (Inv_Furnace[permutation[inv_in]].ss == 12){
				printf("InvIN: %d",  Inv_Furnace[permutation[inv_in]].ss);
				printf(",%d  ",      Inv_Furnace[permutation[inv_in]].mode);
				for (int layer = 0; layer < depth - 1; ++layer){
					printf("Sub: %d", Layer_Furnace[permutation[layers + layer]].sub.ss);
					printf(",%d  ",   Layer_Furnace[permutation[layers + layer]].sub.mode);
					printf("Inv: %d", Layer_Furnace[permutation[layers + layer]].inv.ss);
					printf(",%d  ",   Layer_Furnace[permutation[layers + layer]].inv.mode);
				}
				printf("SubOUT: %d", Sub_Furnace[permutation[sub_out]].ss);
				printf(",%d  ",      Sub_Furnace[permutation[sub_out]].mode);
				printf("InvOUT: %d", Inv_Furnace[permutation[inv_out]].ss);
				printf(",%d\n",      Inv_Furnace[permutation[inv_out]].mode);
				}
			}
		}
		if (tested == 100000000){
			end = clock();
			tested = 0;
			//printf("time: %f\n", (float)(end - total)/CLOCKS_PER_SEC);//accumulative time
			printf("time: %f ", (float)(end - start)/CLOCKS_PER_SEC);//time since last
			printf("Millions: %d00\n", millions);
			++millions;
			start = clock();
		}
		tested++;
	}
	printf("Tested: %d%d\n", millions, tested);
	printf("\a"); //ding?
}
inline int normalize(int *permutation, int *options, int types){
	int i = 0;
	for (;permutation[i] > options[i] && i < (types - 1);){
		permutation[i] = 0;
		++permutation[++i];
	}
	//return ((unsigned)(permutation[i] - 1 - options[i]) & (1<<31)) >> 31;
	//return !(permutation[i] > options[i]);
	//return permutation[i] <= options[i];
	if (permutation[i] > options[i])
		return 0;
	else return 1;
}
int FindSequence(int filled, int (*Array)[16], int * restrict sequence){
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