//Made By: Derek Tomassi
//Project C

void printString(char*);
char* readString(char*);
void readSector(char* buffer, int sector);
void readFile(char* filename, char* buffer, int* sectorsRead);
void executeProgram(char* name);
void terminate();
void handleInterrupt21(int ax, int bx, int cx, int dx);

void main() { 
    //Testing Step #1
    //char buffer[13312]; /*this it the maximum size of a file*/
    //int sectorsRead;
    //makeInterrupt21();
    //interrupt(0x21, 3, "tstpr1", buffer, &sectorsRead); /*read the file into buffer*/
    //if(sectorsRead > 0) {       
        //interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
    //} else {
        //interrupt(0x21, 0, "messag not found\n", 0, 0); /*no sectors read? then print an error*/
    //}

    //Testing Step #2
    //makeInterrupt21();
    //interrupt(0x21, 4, "tstpr1", 0, 0);

    //Testing Step #3
    //makeInterrupt21();
    //interrupt(0x21, 4, "tstpr3", 0, 0);

    //Testing Step #4, 5, 6
    makeInterrupt21();
    interrupt(0x21, 4, "shell", 0, 0);

	while(1); /*hang up*/
}

//Function takes in string and prints to the screen
void printString(char* chars) {
	int i = 0;
	while (chars[i] != 0x0) {
		interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
		i++;
	}
}

//function takes 80 character input and reads it in as input
char* readString(char* line) {
	int i = 0;
	char chars = interrupt(0x16, 0, 0, 0, 0);
	while(chars != 0xd) {
		line[i] = chars;
		i++;
		chars = interrupt(0x16, 0, 0, 0, 0);
		if (chars == 0x8 && i > 0) {
                        printString(chars);
                } else if (i < 1) {
                        printString("Array Out of Bounds Error!");
                }

	}

	line[i] = 0xa;
	line[i+1] = 0x0;
	return line;
}

//function takes in 512 character array and sector number to read desired disk sector
void readSector(char* buffer, int sector) {
	int ah = 2;
	int al = 1;
	int ax = ah * 256 + al;

	int ch = 0;
	int cl = sector + 1;
	int cx = ch * 256 + cl;

	int dh = 0;
	int dl = 0x80;
	int dx = dh * 256 + dl;

	interrupt(0x13, ax, buffer, cx, dx);
}

//function that searches the directory to find a file and reads the file into a buffer
void readFile(char* filename, char* buffer, int* sectorsRead) {
	char dir[512];	
    int entry;
	int ltr;
	int s;
    int i;
	int sector;
    int foundthefile = 1;
	readSector(dir, 2);
    
	for (entry = 0; entry <= 512; entry += 32) {
        foundthefile = 1;		
        for (ltr = 0; ltr < 6; ltr++) {		
            if (filename[ltr] != dir[ltr + entry]) {
                foundthefile = 0;
			}
		}
		if (foundthefile == 1) {
			break;
		}
	}

	if (foundthefile == 0) {
		*sectorsRead = 0;
		return;
	}
    
	for (s = 6; s < 26; s++) {
		sector = dir[entry + s];
		if (sector == 0) {
			break;
		}

		readSector(buffer, sector);
		buffer += 512;
		*sectorsRead = *sectorsRead + 1;
	}
}

void executeProgram(char* name) {
    char buffer[13312];
    int sectorsRead;
    int memOffset = 0x0;
    int i = 0;
    
    //Step #1: reading file into buffer
    interrupt(0x21, 3, name, buffer, &sectorsRead);
    
    //Step #2: putting buffer into memory
    if (sectorsRead > 0) {        
        for(i = 0; i < 13312; i++) {
        putInMemory(0x2000, memOffset, buffer[i]);
        memOffset++;
        }
    } else {
        printString("program not found");
    }    

    
    //Step #3: calling launchProgram(int segment)
    launchProgram(0x2000);
}

void terminate() {
    char shellname[6];

    shellname[0] = 's';
    shellname[1] = 'h';
    shellname[2] = 'e';
    shellname[3] = 'l';
    shellname[4] = 'l';
    shellname[5] = '\0';

    executeProgram(shellname);
}

//Interrupt function that will run  printString, readString, and readSector functions
void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if (ax == 0) {
		printString(bx);
	} else if (ax == 1) {
		readString(bx);
	} else if (ax == 2) {
		readSector(bx, cx);
	} else if (ax == 3) {
        readFile(bx, cx, dx);
    } else if (ax == 4) {
        executeProgram(bx);     
    } else if (ax == 5) {
        terminate();
    } else if (ax >= 6) {
		printString("ERROR INTERRUPT 21 INCORRECT INPUT");
	}
}
