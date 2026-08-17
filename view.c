#include "All_type.h"
#include "view.h"

state validate_mp3_file(char *argv[],ID3View_data *ID3_data)
{
    char *ptr = strrchr(argv[2],'.');

    if(!ptr)
    {
        printf(RED"\nThe given file does not contains .mp3 extention...!\n"RESET);
        return FAILED ;
    }

    if(!(strcmp(ptr,".mp3") == 0))
    {
        printf(RED"\nThe given file does not contains .mp3 extention...!\n"RESET);
        return FAILED ;
    }

    if(ptr[4]!='\0')
    {
        printf(RED"\nInvalid extention should contain .mp3 extention only...!\n"RESET);
        return FAILED ;
    }

    ID3_data->mp3_fname = argv[2];

    ID3_data->fptr_mp3 = fopen(ID3_data->mp3_fname,"rb");

    if(!ID3_data->fptr_mp3)
    {
        printf(RED"\nCan't Open File...!\n"RESET);
        return FAILED ;
    }

    size_t read = fread(ID3_data->ID3_format,1,3,ID3_data->fptr_mp3);

    if(read != 3)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    ID3_data->ID3_format[3] = '\0';

    read = fread(&ID3_data->version,1,1,ID3_data->fptr_mp3);

    if(read != 1)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    read = fread(&ID3_data->revision,1,1,ID3_data->fptr_mp3);

    if(read != 1)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    uint8_t buffer[10];

    read = snprintf(buffer,sizeof(buffer),"%sv2.%d.%d",ID3_data->ID3_format,ID3_data->version,ID3_data->revision);

    if(strcmp(buffer,"ID3v2.3.0") == 0)
    {
        printf(GREEN"\nMP3 ID3 Version Supported...!\n"RESET);
        printf(GREEN"Version : %s",buffer);
    }
    else
    {
        printf(RED"\nUnsupported ID3 Version...!\n");
        printf("\n%s ID3 version is : %s\n",ID3_data->mp3_fname,buffer);
        printf("\nThis project only support for ID3v2.3.0\n\n"RESET);
        return FAILED ;
    }

    fseek(ID3_data->fptr_mp3,1,SEEK_CUR);

    uint8_t size[4];

    read = fread(size,1,4,ID3_data->fptr_mp3);

    if(read != 4)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    ID3_data->tag_Size = ((uint32_t)(size[0] & 0x7F) <<  21) | ((uint32_t)(size[1] & 0x7F) <<  14) | ((uint32_t)(size[2] & 0x7F) <<  7) | ((uint32_t)(size[3] & 0x7F)) ;

    printf(GREEN"\nSize of %s tag size : %u\n\n",ID3_data->mp3_fname,ID3_data->tag_Size);

    return DONE ;
}

state read_metadata(ID3View_data *ID3_data)
{
    ID3_data->byte_processed = 0;
    
    size_t read ;

    printf(GREEN"\n\n+==================================================================+\n");
    printf("|                       MP3 TAG of %-32s|\n", ID3_data->mp3_fname);
    printf("+==================================================================+\n");
    printf("| %-18s | %-43s |\n", "FRAME TYPE", "FRAME DATA");
    printf("+--------------------+---------------------------------------------+\n"RESET);

    while(ID3_data->byte_processed < ID3_data->tag_Size)
    {
        read = fread(ID3_data->frame_ID,4,1,ID3_data->fptr_mp3);

        if(read != 1)
        {
            printf(RED"\nFailed to read Data...!\n"RESET);
            return FAILED;
        }

        ID3_data->frame_ID[4] = '\0';

        select_frame_type(ID3_data);
        
        uint8_t buffer[4];

        read = fread(buffer,1,4,ID3_data->fptr_mp3);

        if(read != 4)
        {
            printf(RED"\nFailed to read Data...!\n"RESET);
            return FAILED;
        }

        ID3_data->frame_Size = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | ((uint32_t)buffer[3]) ;

        if(ID3_data->frame_Size == 0 || (strcmp(ID3_data->frame_type,"No match") == 0))
        {
            break ;
        }

        fseek(ID3_data->fptr_mp3,2,SEEK_CUR);

        if(ID3_data->frame_Size < sizeof(ID3_data->frame_Data))
        {
            read = fread(ID3_data->frame_Data,1,ID3_data->frame_Size,ID3_data->fptr_mp3);
            ID3_data->frame_Data[read] = '\0' ;
            ID3_data->byte_processed += ID3_data->frame_Size + 10 ; 
            printf(GREEN"| %-18s | %-43s |\n"RESET,ID3_data->frame_type,ID3_data->frame_Data+1);
        }
        else
        {
            fseek(ID3_data->fptr_mp3,ID3_data->frame_Size,SEEK_CUR);
        }

    }
    
    printf(GREEN"+--------------------+---------------------------------------------+\n"RESET);

    fclose(ID3_data->fptr_mp3);

    return DONE ;

}

void select_frame_type(ID3View_data *ID3_data)
{

    if(strcmp(ID3_data->frame_ID,"TIT2") == 0)
    {
        strcpy(ID3_data->frame_type,"Title");
    }
    else if(strcmp(ID3_data->frame_ID,"TPE1") == 0)
    {
        strcpy(ID3_data->frame_type,"Artist");
    }
    else if(strcmp(ID3_data->frame_ID,"TALB") == 0)
    {
        strcpy(ID3_data->frame_type,"Album");
    }
    else if(strcmp(ID3_data->frame_ID,"TYER") == 0)
    {
        strcpy(ID3_data->frame_type,"Year");
    }
    else if(strcmp(ID3_data->frame_ID,"TCON") == 0)
    {
        strcpy(ID3_data->frame_type,"Genre");
    }
    else if(strcmp(ID3_data->frame_ID,"COMM") == 0)
    {
        strcpy(ID3_data->frame_type,"Comments");
    }
    else if(strcmp(ID3_data->frame_ID,"TPUB") == 0)
    {
        strcpy(ID3_data->frame_type,"Publisher");
    }
    else if(strcmp(ID3_data->frame_ID,"TCOM") == 0)
    {
        strcpy(ID3_data->frame_type,"Composer");
    }
    else if(strcmp(ID3_data->frame_ID,"TPE2") == 0)
    {
        strcpy(ID3_data->frame_type,"Band");
    }
    else if(strcmp(ID3_data->frame_ID,"TOPE") == 0)
    {
        strcpy(ID3_data->frame_type,"Original Artist");
    }
    else if(strcmp(ID3_data->frame_ID,"TENC") == 0)
    {
        strcpy(ID3_data->frame_type,"Encoded By");
    }
    else
    {
        strcpy(ID3_data->frame_type,"No match");
    }
    
}

void display_help()
{
    printf(YELLOW"\n\n+----------------------------------------------------------------------------+\n");
printf("|                         MP3 TAG READER - HELP                              |\n");
printf("+----------------------------------------------------------------------------+\n");

printf("| %-74s |\n", "SUPPORTED FORMAT:");
printf("| %-74s |\n", "Only ID3v2.3.0 MP3 files are supported.");
printf("| %-74s |\n", "ID3v2.2 and ID3v2.4 are not supported.");
printf("|                                                                            |\n");

printf("| %-74s |\n", "USAGE:");
printf("|   %-72s |\n", "./mp3_tag_reader -v <file.mp3>");
printf("|   %-72s |\n", "./mp3_tag_reader -e <tag> <value> <file.mp3>");
printf("|   %-72s |\n", "./mp3_tag_reader -h");
printf("|                                                                            |\n");

printf("| %-74s |\n", "OPTIONS:");
printf("|   %-6s %-65s |\n", "-v", "View MP3 tag information");
printf("|   %-6s %-65s |\n", "-e", "Edit MP3 tag information");
printf("|   %-6s %-65s |\n", "-h", "Display this help menu");
printf("|                                                                            |\n");

printf("| %-74s |\n", "SUPPORTED ID3v2.3.0 TAGS:");
printf("|                                                                            |\n");

printf("|   %-4s %-8s %-58s |\n", "-t", "TIT2", ": Title");
printf("|   %-4s %-8s %-58s |\n", "-a", "TPE1", ": Artist / Performer");
printf("|   %-4s %-8s %-58s |\n", "-A", "TALB", ": Album");
printf("|   %-4s %-8s %-58s |\n", "-y", "TYER", ": Year");
printf("|   %-4s %-8s %-58s |\n", "-g", "TCON", ": Genre / Content Type");
printf("|   %-4s %-8s %-58s |\n", "-c", "COMM", ": Comments");
printf("|   %-4s %-8s %-58s |\n", "-p", "TPUB", ": Publisher");
printf("|   %-4s %-8s %-58s |\n", "-m", "TCOM", ": Composer");
printf("|   %-4s %-8s %-58s |\n", "-b", "TPE2", ": Band / Orchestra / Accompaniment");
printf("|   %-4s %-8s %-58s |\n", "-o", "TOPE", ": Original Artist / Performer");
printf("|   %-4s %-8s %-58s |\n", "-E", "TENC", ": Encoded By");

printf("|                                                                            |\n");
printf("| %-74s |\n", "EXAMPLES:");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "View tags:");
printf("|   %-72s |\n", "./mp3_tag_reader -v song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Title (TIT2):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -t \"New Title\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Artist (TPE1):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -a \"Karthik\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Album (TALB):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -A \"Ayan(2009)\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Year (TYER):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -y \"2016\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Genre (TCON):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -g \"My3Songs.In\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Comments (COMM):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -c \"eng\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Publisher (TPUB):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -p \"SenSongsMp3.Co\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Composer (TCOM):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -m \"Harris Jayaraj\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Band / Orchestra (TPE2):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -b \"Suriya,Tamanna\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Original Artist (TOPE):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -o \"SenSongsMp3.Co\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Edit Encoded By (TENC):");
printf("|   %-72s |\n", "./mp3_tag_reader -e -E \"SenSongsMp3.Co\" song.mp3");
printf("|                                                                            |\n");

printf("|   %-72s |\n", "Display Help:");
printf("|   %-72s |\n", "./mp3_tag_reader -h");
printf("|                                                                            |\n");

printf("+----------------------------------------------------------------------------+\n\n\n"RESET);

}