#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){

    if (argc == 1){
        fprintf(stderr, "usage: ./linecount input_file\n");
    }

    //opens the input file
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL){
        return 1;
    }

    //stores the number of lines
    int ln_count = 0;

    //line holds each line before it is printed
    char line[128]; 

    //goes through the file line by line counting and printing the lines.
    while(!feof(fp)){
        fgets(line, 30, fp);
        ln_count++;
        printf("%d:%s", ln_count, line);
    }

    //closes the file
    fclose(fp);
    
    //prints the result.
    printf("%d lines.\n", ln_count);

    return 0;
}

