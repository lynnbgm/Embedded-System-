#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct CPU_info
{
	int cycles;
	int instructions;
	int exec_instrus;
	int zero;
	int hits;
	int LD_ST;
};

int find_register(char* registers_number){
	char R[3];
	if(registers_number[0] != '[')
		strncpy(R,registers_number,2);
	else
		strncpy(R,registers_number+1,2);
	//printf("%s",R);
	if(strcmp(R,"R1") == 0)
		return 0;
	else if(strcmp(R,"R2") == 0)
		return 1;
	else if(strcmp(R,"R3") == 0)
		return 2;
	else if(strcmp(R,"R4") == 0)
		return 3;
	else if(strcmp(R,"R5") == 0)
		return 4;
	else if(strcmp(R,"R6") == 0)
		return 5;
	else 
		return -1;
}

int main(int argc, char **argv){

	//Read and Phrase
    if(argc <= 1){
        printf("Error, no input file");
        return -1;
    }
	FILE *file;
	char line[30];
	char **Instrution_buff = malloc(100  * sizeof(char*));
	int i = 0;
	for(i = 0;i< 100;i++){
		Instrution_buff[i] = malloc(30* sizeof(char));
	}
	file = fopen(argv[1], "r");
	if(file == NULL){
		fputs("Input File not found!\n",stdout);
		return -1;
	}
	i = 0;
	while(fgets(line, sizeof line, file) != NULL){
		strcpy(Instrution_buff[i],line);
		i++;
	}


	//Initialization
	struct CPU_info infos = {0,i,0,0,0,0};
	int PC = 0;
	int8_t registers[6] = {0};
	int8_t memory[256] = {0};
	int memory_flag[256] = {0};
	char first_cmd[30];
	strcpy(first_cmd,Instrution_buff[0]);
	char *first_instr_addr = strtok(first_cmd, "\t");
	int first_address = atoi(first_instr_addr);


	//Execute Commands
	while(PC < i){
		char new_cmd[30];
		int j=0,dis = 0;
		for(j = 0; 0<=dis && dis <= 9;j++){
			if(Instrution_buff[PC][j] == '\t' || Instrution_buff[PC][j] == ' ' )
				continue;
			else
				dis = Instrution_buff[PC][j] - '0';
		}
		j--;
		strncpy(new_cmd,Instrution_buff[PC]+j,30 -j);
		char *cmd = strtok(new_cmd, " ");
		if(strcmp(cmd,"MOV") == 0){
			cmd = strtok(NULL," ");
			int reg_num = find_register(cmd);
			cmd = strtok(NULL," ");
			int8_t val =(int8_t) atoi(cmd);
			registers[reg_num] = val;
			PC++;
			infos.cycles++;
			infos.exec_instrus++;
		}
		else if(strcmp(cmd,"ADD") == 0){
			cmd = strtok(NULL," ");
			int reg_num1 = find_register(cmd);
			cmd = strtok(NULL," ");
			int reg_num2 = find_register(cmd);
			if(reg_num2 == -1){
				int8_t val =(int8_t) atoi(cmd);
				registers[reg_num1] +=val;
			}
			else{
				registers[reg_num1] += registers[reg_num2];
				
			}
			PC++;
			infos.cycles++;
			infos.exec_instrus++;
		}
		else if(strcmp(cmd,"CMP") == 0){
			cmd = strtok(NULL," ");
			int reg_num1 = find_register(cmd);
			cmd = strtok(NULL," ");
			int reg_num2 = find_register(cmd);
			if(registers[reg_num1] - registers[reg_num2] == 0)
				infos.zero = 1;
			else
				infos.zero = 0;
			PC++;
			infos.cycles++;
			infos.exec_instrus++;
		}
		else if(strcmp(cmd,"LD") == 0){
			cmd = strtok(NULL," ");
			int reg_num1 = find_register(cmd);
			cmd = strtok(NULL," ");
			int reg_num2 = find_register(cmd);
			int8_t address =  registers[reg_num2];
			registers[reg_num1] = memory[address];
			infos.LD_ST++;
			if(memory_flag[address] == 0){
				memory_flag[address] = 1;
				PC++;
				infos.cycles += 45;
				infos.exec_instrus++;
			}else{
				PC++;
				infos.cycles += 2;
				infos.exec_instrus++;
				infos.hits++;
			}

		}
		else if(strcmp(cmd,"ST") == 0){
			cmd = strtok(NULL," ");
			int reg_num1 = find_register(cmd);
			cmd = strtok(NULL," ");
			int reg_num2 = find_register(cmd);
			int8_t address =  registers[reg_num1];
			memory[address] = registers[reg_num2];
			infos.LD_ST++;
			if(memory_flag[address] == 0){
				memory_flag[address] = 1;
				PC++;
				infos.cycles += 45;
				infos.exec_instrus++;
			}else{
				PC++;
				infos.cycles += 2;
				infos.exec_instrus++;
				infos.hits++;
			}
		}
		else if(strcmp(cmd,"JE") == 0){
			cmd = strtok(NULL," ");
			int instr_address = atoi(cmd);
			if(infos.zero == 1){
				PC = instr_address - first_address;
				infos.cycles++;
				infos.exec_instrus++;
			}
			else{
				PC++;
				infos.cycles++;
				infos.exec_instrus++;
			}
		}
		else if(strcmp(cmd,"JMP") == 0){
			cmd = strtok(NULL," ");
			int instr_address = atoi(cmd);
			PC = instr_address - first_address;
			infos.cycles++;
			infos.exec_instrus++;
		}
		else{
			printf("Unknown instruction: %s\n",cmd);
			return -2;
		}

	}

	printf("Total number of instructions in the code: %d\n", infos.instructions);
	printf("Total number of executed instructions: %d\n", infos.exec_instrus);
	printf("Total number of clock cycles: %d\n", infos.cycles);
	printf("Number of hits to local memory: %d\n", infos.hits);
	printf("Total number of executed LD/ST instructions: %d\n", infos.LD_ST);

	free(Instrution_buff);

	return 0;
}