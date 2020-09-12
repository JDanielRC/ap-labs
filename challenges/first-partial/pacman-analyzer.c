#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

struct package{
    char name[150];
    char installDate[150];
    char lastUpdate[150];
    int upgradesAmount;
    char uninstallDate[150];
};

struct package packages[2000];
void analizeLog(char *logFile, char *report);
int getLines(FILE *file, char *buffer, size_t size);
int packageType(char* line);
char* getName(char* line);
char* getDate(char* line);


int main(int argc, char **argv) {

    if (argc != 5) {
	printf("Incorrect number of parameters, use: [-input logfile.txt -output desiredoutput.txt]\n");
    } else{
        analizeLog(argv[2], argv[4]);
    }
    return 0;
}

int getLines(FILE *file, char *buffer, size_t size){
    if (size == 0){
        return 0;
    }
    size_t currentSize = 0;
    int c;
    //leer caracter por caracter del archivo mientras no llegu
    while ((c = (char) getc(file)) != '\n' && currentSize < size){
        if (c == EOF){
            break;
        }
        //printf("caracter es: %c \n", c);
        buffer[currentSize] = (char) c;
        currentSize++;
        
    }
    if (currentSize == 0){
        return 0;
    }
    buffer[currentSize] ='\0';
    return currentSize;
}

char* getDate(char* line){
    int size = 0;
    for (size; line[size] != ']'; size++);
    //include the last ]
    size++;
    char *date = calloc(1, size);
    int cont = 0;
    for (int i = 0; i < size; i++, cont++){
        date[cont] = line[i];
    }
    return date;
}

char* getName(char* line){
    int cont = 0, startPoint = 0, size = 0;
    //go past the date and [content]
    for (int i = 0; i < 2; i++){
        for (startPoint; line[startPoint] != ']'; startPoint++);
        startPoint += 2;
    }
    
    //move past the 'installation/upgraded/removed' string
    for (startPoint; line[startPoint] != ' '; startPoint++);
    //move past the blank
    startPoint++;
    //find required size of string
    for (int j = startPoint + 1; line[j] != ' '; j++){
        size++;
    }
    //assign string
    char *name = calloc(1, size);
    for (int j = startPoint; line[j] != ' '; j++, cont++){
        name[cont] = line[j];
    }
    return name;
}

int packageType(char* line){
    int startPoint = 0;
    //installation, upgrade, and remove are always after two blank spaces in the line, so we only check that part
    for (int i = 0; i < 2; i++){
        for (startPoint; line[startPoint] != '\0'; startPoint++){ //ask teacher, had segmentatio fault before I changed this, I had line[startPOint] != ']'

            if (line[startPoint] == ']'){
                break;
            }
        }
        //move past the ']' and blank space
        startPoint += 2;
    }
    //installation
    if (line[startPoint] == 'i' && line[startPoint + 1] == 'n' && line[startPoint + 2] == 's' && line[startPoint + 3] == 't'){
        return 1;
    }
    //update
    if (line[startPoint] == 'u' && line[startPoint + 1] == 'p' && line[startPoint + 2] == 'g' && line[startPoint + 3] == 'r'){
        return 2;
    }
    //removal
    if (line[startPoint] == 'r' && line[startPoint + 1] == 'e' && line[startPoint + 2] == 'm' && line[startPoint + 3] == 'o'){
        return 3;
    }
    return 0;
}



void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    // Implement your solution here.
    char line[255];
    int c;
    
    FILE*  file;
    file = fopen(logFile, "r");
    if (file == NULL){
        printf("Error opening log file \n");
        return;
    }
    //create report file
    int writer = open(report, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (writer < 0) {
        perror("An error ocurred while creating/opening the report file"); 
        return;
    }
    //obtain all lines from pacman.txt
    
    int installed = 0, removed = 0, upgraded = 0, current = 0;
    while (c = getLines(file, line, 255) > 0){
        int n = packageType(line);
        if (n == 1){
            char* name = getName(line);
            char* date = getDate(line);
            strcpy(packages[current].name, name);
            strcpy(packages[current].installDate, date);
            packages[current].upgradesAmount = 0;
            strcpy(packages[current].uninstallDate, "-");
            current++;
            installed++;
        } else if (n == 2){
            char* name = getName(line);
            char* date = getDate(line);
            for (int i = 0; i < 1500; i++){
                if (strcmp(packages[i].name, name) == 0){
                    strcpy(packages[i].lastUpdate, date);
                    if (packages[i].upgradesAmount == 0){
                        upgraded++;
                    }
                    packages[i].upgradesAmount++;
                    break;
                }
            }
        } else if (n == 3){ 
            char* name = getName(line);
            char* date = getDate(line);
            for (int i = 0; i < 1500; i++){
                if (strcmp(packages[i].name, name) == 0){
                    strcpy(packages[i].uninstallDate, date);
                }
                break;
            }
            removed++;
        }
        
    } 
    //FIRST PART INFORMATION
    write(writer, "Pacman Packages Report\n", strlen("Pacman Packages Report\n"));
    write(writer,"----------------------\n",strlen("----------------------\n"));
    char aux[10];
    write(writer, "Installed packages : ", strlen("Installed packages : "));
    sprintf(aux, "%d\n", installed);
    write(writer, aux, strlen(aux));
    write(writer, "Upgraded packages : ",strlen("Upgraded packages : "));
    sprintf(aux, "%d\n", upgraded);
    write(writer, aux, strlen(aux));
    write(writer, "Removed packages : ",strlen("Removed packages : "));
    sprintf(aux, "%d\n", removed);
    write(writer, aux, strlen(aux));
    write(writer, "Current installed : \n",strlen("Current installed : "));
    sprintf(aux, "%d\n", (installed-removed));
    write(writer, aux, strlen(aux));

    //PACKAGE LIST
    write(writer, "\n\nList of packages\n", strlen("\n\nList of packages\n"));
    write(writer,"----------------------\n",strlen("----------------------\n"));
    for(int i = 0; i < 1500; i++){
        if(strcmp(packages[i].name, "") != 0){
            write(writer, "- Package name         : ",strlen("- Package name         : "));
            write(writer,packages[i].name, strlen(packages[i].name));
            write(writer, "\n   - Install date      : ",strlen("\n   - Install date      : "));
            write(writer,packages[i].installDate, strlen(packages[i].installDate));
            write(writer, "\n   - Last update date  : ",strlen("\n   - Last update date  : "));
            write(writer,packages[i].lastUpdate, strlen(packages[i].lastUpdate));
            write(writer, "\n   - How many updates  : ",strlen("\n   - How many updates  : "));
            sprintf(aux, "%d", packages[i].upgradesAmount);
            write(writer,aux, strlen(aux));
            write(writer, "\n   - Removal date      : ",strlen("\n   - Removal date      : "));
            write(writer,packages[i].uninstallDate, strlen(packages[i].uninstallDate));
            write(writer, "\n",strlen("\n"));
        } else if (strcmp(packages[i].name, "") == 0){
            break;
        }
    }

    //close writer
    if (close(writer) < 0)  
    { 
        perror("Error while trying to close the report file"); 
        exit(1); 
    } 

    printf("Report is generated at: [%s]\n", report);
}


