/* 
 * File:   main.c
 * Author: Bram
 *
 * Created on 24 december 2014, 20:08
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>

#define MAXITERATIONQUANTITY 99
#define RESULTSDIRPATH  "sources\\"
#define SOURCEFILEPATH "sources\\varianten.txt"
#define PROCESSEDFILEPATH "sources\\processedfile.txt"
#define MAXTIMEPERIODQUANTITY 60
#define MAXFILEQUANTITY 600

#define TRUE 1
#define FALSE 0

struct file
{
    char filename[20];
    char period[20];
    char traffic[20];
};
struct datum
{
    float traveltime;
    int volume;
};

struct file FileNames[MAXFILEQUANTITY];

int closingerror;
int processingerror;
int eoferror;

int iteration;
int filecount;
int filequantity;

FILE *resultsfile;
FILE *processedfile;
FILE *sourcelist;

int ProcessData()
{
    //Create temp vars
    char buffer[100];
    int i;
    char c;
    char smallbuffer[6];

    int routequantity=0;


    //Skip 8 rows.
    for(i=0; i<8; i++)
    {
        c='\0';
        while(c!='\n') fscanf(resultsfile, "%c", &c);
    }
    
    //Init routequantity
    while(TRUE)
    {
        fgets(buffer, 100, resultsfile);
        sscanf(buffer, "%3c", smallbuffer);
        if(strcmp(smallbuffer, "No.") == 0)
        {
            routequantity++;
        }
        else
        {
            break;
        }
    }
    printf("Routequantity = %d\n", routequantity);
    //Skip 2 rows
    for(i=0; i<2; i++)
    {
        c='\0';
        while(c != '\n') fscanf(resultsfile, "%c", &c);
    }

    fscanf(resultsfile, " No.:;");
    
    //Create vars
    struct datum data[routequantity][MAXTIMEPERIODQUANTITY];
    int routecount;
    int timeperiodcount;
    int routenumber[routequantity];
    int timeperiodnumber[MAXTIMEPERIODQUANTITY];

    //Init routenumbers
    printf("Initializing Routes...\n");
    for(i=0; i<routequantity; i++)
    {
        fscanf(resultsfile,"%d;%*d;", &(routenumber[i]));
        //printf("%d ", routenumber[i]);
    }
    
    printf("Routes initialized.\n");
    
    for(timeperiodcount=0; timeperiodcount<MAXTIMEPERIODQUANTITY; timeperiodcount++)
    {
        fscanf(resultsfile, "%*[ \r\n]%d;", &(timeperiodnumber[timeperiodcount]));
        for(routecount=0; routecount<routequantity; routecount++)
        {
            eoferror = fscanf(resultsfile, " %f;", &((data[routecount][timeperiodcount]).traveltime));
            if(eoferror==EOF) break;
            eoferror = fscanf(resultsfile, " %d;", &((data[routecount][timeperiodcount]).volume));
            if(eoferror==EOF) break;
            
            fprintf(processedfile,"%-20s",FileNames[filecount].filename);
            fprintf(processedfile,"%-20s",FileNames[filecount].period);
            fprintf(processedfile,"%-20s",FileNames[filecount].traffic);
            fprintf(processedfile,"%02d%8s",iteration,"");
            fprintf(processedfile,"%-10d",timeperiodnumber[timeperiodcount]);
            fprintf(processedfile,"%-20d",routenumber[routecount]);
            fprintf(processedfile,"%-10.1f",data[routecount][timeperiodcount].traveltime);
            fprintf(processedfile,"%-10d\r\n",data[routecount][timeperiodcount].volume);

        }
        if(eoferror==EOF) break;
    }
        
    return 0;
}

int main()
{
    int i;
    //Het openen van het indexbestand
    printf("Opening " SOURCEFILEPATH "...\n");
    sourcelist = fopen(SOURCEFILEPATH, "r");
    if(sourcelist == NULL)
    {
        printf("Could you fucking not?\n"
               "(Could not open file.)\n");
        getchar();
        exit(0);
    }
    else
    {
        printf("File opened!\n");  
    }
    
    //Bestandsnamen en -eigenschappen initialiseren door index te lezen
    char c = '\0';
    while(c!='\n') fscanf(sourcelist, "%c", &c);
    for(i=0; eoferror!=EOF; i++)//i<filequantity
    {
        eoferror = fscanf(sourcelist, " %s", FileNames[i].filename);
        eoferror = fscanf(sourcelist, " %s", FileNames[i].period);
        eoferror = fscanf(sourcelist, " %s", FileNames[i].traffic);
    }
    filequantity=i-1;
    
    printf("File scanned,\n"
            "%d filenames found\n", filequantity);
    
    //Sluiten indexbestand
    printf("Closing " SOURCEFILEPATH "...\n");
    closingerror = fclose(sourcelist);
    if (closingerror != 0)
    {
        printf("Could not close file.\n");
        getchar();
        exit(0);
    }
    printf("File closed.\n");
    
    //Openen schrijfbestand, labels schrijven
    printf("Creating " PROCESSEDFILEPATH "...\n");
    processedfile = fopen(PROCESSEDFILEPATH, "w");
    if(processedfile == NULL)
    {
        printf("Could not create file,\n");
        getchar();
        exit(0);
    }
    printf("File created\n");
    fprintf(processedfile, "%-20s%-20s%-20s%-10s%-10s%-20s%-10s%-10s\r\n","Variantnaam","Periode","Verkeer","Iteratie","Time","Route","Reistijd","Volume");
    
    //Bestandsnamen initialiseren, bestanden openen en in ProcessData() douwen, bestanden sluiten
    char temppath[100];
    for(filecount=0; filecount<filequantity; filecount++)
    {
        for(iteration=1; iteration<=MAXITERATIONQUANTITY; iteration++)
        {
            sprintf(temppath, RESULTSDIRPATH "%s""_""%02d"".rsz", FileNames[filecount].filename, iteration);
            printf("Opening %s...\n", temppath);
            resultsfile = fopen(temppath, "r");
            usleep(100000);
            if(resultsfile == NULL)
            {
                printf("Could not open file,\n"
                        "%s not opened.\n", temppath);
                break;
            }
            else
            {
                printf("File opened!\n");
                
                printf("Processing %s...\n", temppath);
                processingerror=ProcessData();
                if(processingerror != 0)
                {
                    printf("Error during procressing,\n"
                            "%s not processed.\n", temppath);
                }
                else
                {
                    printf("File processed!\n");
                }
                
                printf("Closing %s...\n", temppath);
                closingerror = fclose(resultsfile);
                if(closingerror != 0)
                {
                    printf("Could not close file.\n");
                    getchar();
                    exit(0);
                }
                printf("File closed.\n");
            }
        }
    }
    printf("Happy end!!!!!\n");
    return (EXIT_SUCCESS); //Iedereen is blij, net als in een disneyfilm, behalve de bugs (de slechterikken)...
}
