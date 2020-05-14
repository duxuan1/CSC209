/**
 * Read and parse a wave file
 *
 **/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *param; // -fin, -fout or -pan
char *value; // char of milliseconds
int mseconds; // int of milliseconds 
char line[256]; // input line
int lineNumber = 1; // current line number
double channel = 0.0;
double sample_rate = 0.0;
double data_size = 0.0;
double sampleTotal = 0.0;
double curr; // current pointer on bytes
double start; // start pointer on bytes
double curr2; // current pointer on bytes

// calculate hex bytes to integer value
int getValue(char *line, int length){
	char *reversed = malloc(sizeof(char) * 10);
	int pointer = 0;
	for (int i = length - 1; i >= 0; i -= 3) {
		reversed[pointer] = line[i];
		reversed[pointer + 1] = line[i + 1];
		pointer += 2;
	}
	int value = strtol(reversed, NULL, 16);
	free(reversed);
	return value;
}

// transfer int value to hex representation to line, for fin and fout 
void modifyByte(char *line, short value, double multiplier){
	int newValue;
	newValue = (int)(value * multiplier);
	// short to hex char
	if (newValue > 0){
		char res[5];
		sprintf(res, "%04x", newValue);
		line[0] = res[2];
		line[1] = res[3];
		line[3] = res[0];
		line[4] = res[1];
	} else {
		char res[9];
		sprintf(res, "%08x", newValue);
		char *subresult = &res[4];
		line[0] = subresult[2];
		line[1] = subresult[3];
		line[3] = subresult[0];
		line[4] = subresult[1];
	}
}

void modifyLineFin(char *line, int linePointer){
	while (curr < sampleTotal && linePointer < 57){
		int times = 0;
		while (times < channel) { 
			short byte;
			byte = getValue(&line[linePointer], 4);
			modifyByte(&line[linePointer], byte, curr / sampleTotal);
			times += 1;
			linePointer += 6;
		}
		curr += 1;
	}
}

void modifyLineFout(char *line, int linePointer){
	while (curr < start && linePointer < 57){
		int times = 0;
		while (times < channel) {
			linePointer += 6;
			times += 1;
		}
		curr += channel;
	}
	while (curr >= start && linePointer < 57 ){
		int times = 0;
		while (times < channel) {
			short byte;
			byte = getValue(&line[linePointer], 4);
			modifyByte(&line[linePointer], byte, (data_size - curr - channel)/ sampleTotal);
			times += 1;
			linePointer += 6;
		}
		curr += channel;
	}
}

void modifyLinePanLeft(char *line, int linePointer){
	while (curr2 < start && linePointer < 57){
		linePointer += 12;
		curr2 += channel;
	}
	while (curr2 >= start && linePointer < 57 ){
		short byte;
		byte = getValue(&line[linePointer], 4);
		if ((data_size/channel- curr2 - channel*2) > 0){
			modifyByte(&line[linePointer], byte, (data_size/channel- curr2 - channel*2)/ (sampleTotal*channel));
		} else {
			modifyByte(&line[linePointer], byte, 0/ (sampleTotal*channel));
		}
		linePointer += 12;
		curr2 += channel;
		
	}
}

void modifyLinePanRight(char *line, int linePointer){
	while (curr < sampleTotal && linePointer < 57){ 
		short byte;
		byte = getValue(&line[linePointer], 4);
		modifyByte(&line[linePointer], byte, curr / sampleTotal);
		linePointer += 12;
		curr += 1;
	}
}

void fin(){
	while(fgets(line, sizeof(line), stdin) != NULL){ 
		if (lineNumber == 2){
			channel = getValue(&line[28], 4);
			sample_rate = getValue(&line[34], 4);
			sampleTotal = sample_rate * mseconds/1000.0;
		} else if (lineNumber == 3) {
			data_size = getValue(&line[34], 10);
			modifyLineFin(&line[0], 46);
		} else if (lineNumber > 3){
			modifyLineFin(&line[0], 10);
		}
		lineNumber ++;
		printf("%s",line);
	}
}

void fout() {
	while(fgets(line, sizeof(line), stdin) != NULL){ 
		if (lineNumber == 2){
			channel = getValue(&line[28], 4);
			sample_rate = getValue(&line[34], 4);
			sampleTotal = sample_rate * mseconds/1000.0 * channel;
		} else if (lineNumber == 3) {
			data_size = getValue(&line[34], 10) / channel;
			start = (data_size - sampleTotal);
			modifyLineFout(&line[0], 46);
		} else if (lineNumber > 3){
			modifyLineFout(&line[0], 10);
		}
		lineNumber ++;
		printf("%s",line);
	}
}

void pan() {
	while(fgets(line, sizeof(line), stdin) != NULL){ 
		if (lineNumber == 2){
			channel = getValue(&line[28], 4);
			sample_rate = getValue(&line[34], 4);
			sampleTotal = sample_rate * mseconds/1000.0;
		} else if (lineNumber == 3) {
			data_size = getValue(&line[34], 10);
			modifyLinePanLeft(&line[0], 46);
			modifyLinePanRight(&line[0], 52);
		} else if (lineNumber > 3){
			modifyLinePanLeft(&line[0], 10);
			modifyLinePanRight(&line[0], 16);
		}
		lineNumber ++;
		printf("%s",line);
	}
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Error: Invalid command-line arguments\n.");
		return 0;
	}
	param = argv[1];
	value = argv[2];
	mseconds = strtol(value, NULL, 10);
	if (strcmp(param, "-fin") == 0) {
		fin();
	} else if (strcmp(param, "-fout") == 0) {
		fout();
	} else if (strcmp(param, "-pan") == 0) {
		pan();
	} else {
		printf("Error: Invalid command-line arguments\n.");
	}
  	return 0;
}