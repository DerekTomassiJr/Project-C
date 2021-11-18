//Made By: Derek Tomassi
//Project C
int equals(char* input, char* comparison);

void main() {
    char input[80];
    char filename[7];
    char buffer[13312]; 
    int sectorsRead;  

    while(1) {                
        syscall(0, "SHELL>\r\n", 0, 0); 
        syscall(1, input, 0, 0);

        syscall(0, input, 0, 0);
        syscall(0, "\r\n", 0, 0);

        
        if (equals(input, "type", filename) == 1) {          
            syscall(3, filename, buffer, &sectorsRead);
            
            if (sectorsRead > 0) {
                syscall(0, buffer, 0, 0);
                syscall(0, "\r\n", 0, 0);        
            } else {
                syscall(0, "file not found\r\n", 0, 0);
            }
        } else if (equals(input, "exec", filename) == 1){
            syscall(4, filename, 0, 0);

        } else {
            syscall(0, "Bad Command\r\n", 0, 0);
        }
    }
}

//function to check if two strings are equal and returns 1 if they are equal
int equals(char* input, char* comparison, char* filename) {
    int i = 0;
    int j;
    
    //comapring command types
    while (comparison[i] != '\0') {
        if (input[i] != comparison[i]) {
            return 0;
        }
        i++;
    }
    
    //incrementing pointer
    i++;
    
    //reading filename into filename array
    for (j = 0; j < 6; j++) {
        filename[j] = input[i];
        i++;
    }
    
    //adding \0 to end of filename array
    filename[6] = '\0';
    
    return 1;
}
