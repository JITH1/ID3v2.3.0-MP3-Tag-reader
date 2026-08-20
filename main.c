#include "All_type.h"
#include "view.h"
#include "edit.h"

int main(int argc ,char *argv[])
{

    ID3View_data ID3_data ;
    ID3Edit_data ID3_edit ;

    if(argc < 2)
    {
        printf(RED"\nInsufficient Number Of Arguments...!\n"RESET);
        return FAILED ;
    }
    else if(argc > 5)
    {
        printf(RED"\nToo Many Arguments...!\n"RESET);
        return FAILED;
    }

    if(strcmp(argv[1],"-v") == 0)
    {
        if(argc>3)
        {
            printf(RED"\nToo Many Arguments...!\n"RESET);
            return FAILED;     
        }

        if(validate_mp3_file(argv,&ID3_data))
        {
            printf(GREEN"MP3 File Validation Successfull...!\n"RESET);

            if(read_metadata(&ID3_data))
            {
                printf(GREEN"\nMetadata Read Successfully...!\n\n"RESET);
            }
        }
    }
    else if(strcmp(argv[1],"-e") == 0)
    {
        if(argc != 5)
        {
            printf(RED"\nInvalid Number Of Arguments...!\n"RESET);
            return FAILED ;
        }
        
        if(validate_edit_mp3(argv,&ID3_edit))
        {
            printf(GREEN"MP3 File Validation Successfull...!\n"RESET);

            if(edit_metadata(&ID3_edit))
            {
                printf(GREEN"\nMetadata Edited Successfully...!\n\n"RESET);
                return DONE ;
            }
            else
            {
                printf(RED"\nFailed to edit Metadata...!\n"RESET);
                return FAILED ;
            }
             
        }

    }
    else if(strcmp(argv[1],"-h") == 0)
    {
        if(argc != 2)
        {
            printf(RED"\nInvalid Number Of Arguments...!\n"RESET);
            return FAILED ;
        }

        display_help();
    }
    
    return 0;
}