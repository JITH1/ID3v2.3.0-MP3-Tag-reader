#include "All_type.h"
#include "edit.h"

state validate_edit_mp3(char *argv[] , ID3Edit_data *ID3_edit)
{
    char *ptr = strrchr(argv[4],'.');

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

    ID3_edit->mp3_fname = argv[4];

    ID3_edit->fptr_mp3 = fopen(ID3_edit->mp3_fname,"rb");

    if(!ID3_edit->fptr_mp3)
    {
        printf(RED"\nCan't Open File...!\n"RESET);
        return FAILED ;
    }

    if(!select_editframe_type(ID3_edit,argv))
    {
        return FAILED ;
    }
    
    size_t read = fread(ID3_edit->ID3_format,1,3,ID3_edit->fptr_mp3);

    if(read != 3)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    ID3_edit->ID3_format[3] = '\0';

    read = fread(&ID3_edit->version,1,1,ID3_edit->fptr_mp3);

    if(read != 1)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    read = fread(&ID3_edit->revision,1,1,ID3_edit->fptr_mp3);

    if(read != 1)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    uint8_t buffer[10];

    read = snprintf(buffer,sizeof(buffer),"%sv2.%d.%d",ID3_edit->ID3_format,ID3_edit->version,ID3_edit->revision);

    if(strcmp(buffer,"ID3v2.3.0") == 0)
    {
        printf(GREEN"\nMP3 ID3 Version Supported...!\n"RESET);
        printf(GREEN"Version : %s",buffer);
    }
    else
    {
        printf(RED"\nUnsupported ID3 Version...!\n");
        printf("\n%s ID3 version is : %s\n",ID3_edit->mp3_fname,buffer);
        printf("\nThis project only support for ID3v2.3.0\n\n"RESET);
        return FAILED ;
    }

    fseek(ID3_edit->fptr_mp3,1,SEEK_CUR);

    uint8_t size[4];

    read = fread(size,1,4,ID3_edit->fptr_mp3);

    if(read != 4)
    {
        printf(RED"\nFailed to read Data...!\n"RESET);
        return FAILED;
    }

    ID3_edit->tag_Size = ((uint32_t)(size[0] & 0x7F) <<  21) | ((uint32_t)(size[1] & 0x7F) <<  14) | ((uint32_t)(size[2] & 0x7F) <<  7) | ((uint32_t)(size[3] & 0x7F)) ;

    printf(GREEN"\nSize of %s tag size : %u\n\n",ID3_edit->mp3_fname,ID3_edit->tag_Size);

    if(!(sizeof(ID3_edit->New_frame_data) > sizeof(argv[3])))
    {
        printf(RED"\nPlease Reduce the edit data size of increase data buffer...!"RESET);
        return FAILED ;
    }

    strcpy(ID3_edit->New_frame_data,argv[3]);

    printf(GREEN"New Frame Data For Editing : %s\n"RESET,ID3_edit->New_frame_data);

    ID3_edit->New_frame_size = strlen(ID3_edit->New_frame_data)+1;

    printf(GREEN"\nNew Frame Size for %s : %u\n",ID3_edit->frame_type,ID3_edit->New_frame_size);

    fclose(ID3_edit->fptr_mp3);

    return DONE ;
}

state select_editframe_type(ID3Edit_data *ID3_edit,char *argv[])
{

    if(strcmp(argv[2],"-t") == 0)
    {
        strcpy(ID3_edit->frame_ID,"TIT2");
        strcpy(ID3_edit->frame_type,"Title");
    }
    else if(strcmp(argv[2],"-a") == 0)
    {
        strcpy(ID3_edit->frame_type,"Artist");
        strcpy(ID3_edit->frame_ID,"TPE1");
    }
    else if(strcmp(argv[2],"-A") == 0)
    {
        strcpy(ID3_edit->frame_type,"Album");
        strcpy(ID3_edit->frame_ID,"TALB");
    }
    else if(strcmp(argv[2],"-y") == 0)
    {
        strcpy(ID3_edit->frame_type,"Year");
        strcpy(ID3_edit->frame_ID,"TYER");
    }
    else if(strcmp(argv[2],"-g") == 0)
    {
        strcpy(ID3_edit->frame_type,"Genre");
        strcpy(ID3_edit->frame_ID,"TCON");
    }
    else if(strcmp(argv[2],"-c") == 0)
    {
        strcpy(ID3_edit->frame_type,"Comments");
        strcpy(ID3_edit->frame_ID,"COMM");
    }
    else if(strcmp(argv[2],"-p") == 0)
    {
        strcpy(ID3_edit->frame_type,"Publisher");
        strcpy(ID3_edit->frame_ID,"TPUB");
    }
    else if(strcmp(argv[2],"-m") == 0)
    {
        strcpy(ID3_edit->frame_type,"Composer");
        strcpy(ID3_edit->frame_ID,"TCOM");
    }
    else if(strcmp(argv[2],"-b") == 0)
    {
        strcpy(ID3_edit->frame_type,"Band");
        strcpy(ID3_edit->frame_ID,"TPE2");
    }
    else if(strcmp(argv[2],"-o") == 0)
    {
        strcpy(ID3_edit->frame_type,"Original Artist");
        strcpy(ID3_edit->frame_ID,"TOPE");
    }
    else if(strcmp(argv[2],"-E") == 0)
    {
        strcpy(ID3_edit->frame_type,"Encoded By");
        strcpy(ID3_edit->frame_ID,"TENC");
    }
    else
    {
        printf(RED"\nPlease Enter a Valid Edit Flag ... View Help menu for supported edit flag...!\n"RESET);
        return FAILED ;
    }

    return DONE ;
    
}

state edit_metadata(ID3Edit_data *ID3_edit)
{
    printf(GREEN"\n# Required Frame to Edit : %s\n"RESET,ID3_edit->frame_type);

    ID3_edit->fptr_mp3 = fopen(ID3_edit->mp3_fname,"rb");

    if(!ID3_edit->fptr_mp3)
    {
        printf(RED"\nCan't open file %s\n"RESET,ID3_edit->mp3_fname);
        return FAILED ;
    }

    fseek(ID3_edit->fptr_mp3,10,SEEK_CUR);

    uint32_t offset = 0;
    uint8_t frame[5];

    size_t read , write ;

    while(offset < ID3_edit->tag_Size)
    {
        read = fread(frame,1,4,ID3_edit->fptr_mp3);

        if(read != 4)
        {
            printf(RED"\nCan't able to read data...!\n"RESET);
            return FAILED;
        }

        frame[read] = '\0';

        if(strcmp(frame,ID3_edit->frame_ID) == 0)
        {
            uint8_t size[4];
            read = fread(size,1,4,ID3_edit->fptr_mp3);

            if(read != 4)
            {
                printf(RED"\nCan't able to read data...!\n"RESET);
                return FAILED;
            }

            ID3_edit->frame_Size = ((uint32_t)size[0] << 24) | ((uint32_t)size[1] << 16) | ((uint32_t)size[2] << 8) | ((uint32_t)size[3]) ;

            printf(GREEN"\n# Frame %s found , Current size of the frame is %u\n"RESET,frame,ID3_edit->frame_Size);

            break ;
        }
        else
        {
            uint8_t size[4];

            read = fread(size,1,4,ID3_edit->fptr_mp3);

            if(read != 4)
            {
                printf(RED"\nCan't able to read data...!\n"RESET);
                return FAILED;
            }

            ID3_edit->frame_Size = ((uint32_t)size[0] << 24) | ((uint32_t)size[1] << 16) | ((uint32_t)size[2] << 8) | ((uint32_t)size[3]) ;
            
            if(ID3_edit->frame_Size == 0)
            {
                printf(RED"\nCan't Find the frame for edit...!\n"RESET);
                return FAILED ;  
            }

            fseek(ID3_edit->fptr_mp3,2,SEEK_CUR);
            fseek(ID3_edit->fptr_mp3,ID3_edit->frame_Size,SEEK_CUR);

        }

        offset += 10 + ID3_edit->frame_Size;
    }
    
    ID3_edit->new_tag_size = (ID3_edit->tag_Size - ID3_edit->frame_Size) + ID3_edit->New_frame_size ; 

    rewind(ID3_edit->fptr_mp3);

    ID3_edit->temp_fname  = "new.mp3" ;

    FILE *new_mp3_ptr = fopen(ID3_edit->temp_fname,"wb");

    if(!new_mp3_ptr)
    {
        printf(RED"\nCan't Make New File...!\n"RESET);
        return FAILED ;
    }
    
    uint8_t buffer[6];

    read = fread(buffer,1,6,ID3_edit->fptr_mp3);

    if(read != 6)
    {
        printf(RED"\nCan't able to read header data...!\n"RESET);
        return FAILED ;
    }

    write = fwrite(buffer,1,6,new_mp3_ptr);

    if(write != 6)
    {
        printf(RED"\nCan't able to write header...!\n"RESET);
    }

    uint8_t new_size_tag[4];

    new_size_tag[0] = (ID3_edit->new_tag_size >> 21U ) & 0x7F ;
    new_size_tag[1] = (ID3_edit->new_tag_size >> 14U ) & 0x7F ;
    new_size_tag[2] = (ID3_edit->new_tag_size >> 7U ) & 0x7F ;
    new_size_tag[3] = (ID3_edit->new_tag_size >> 0U ) & 0x7F ; 

    write = fwrite(new_size_tag,1,4,new_mp3_ptr);

    if(write != 4)
    {
        printf(RED"\nCan't able to write new tag size...!\n"RESET);
    }

    uint8_t frame_id[4];
    uint8_t frame_size[4];
    uint8_t frame_flag[2];
    char *frame_content ;

    uint32_t size_frame ;

    fseek(ID3_edit->fptr_mp3,10,SEEK_SET);

    uint32_t offset_track = 0;

    while( offset_track < ID3_edit->tag_Size )
    {
        read = fread(frame_id,1,4,ID3_edit->fptr_mp3);

        if(read != 4)
        {
            printf(RED"\nCan't able to read frame ID...!\n"RESET);
            return FAILED ;
        }

        frame_id[read] = '\0';

        if(frame_id[0] == '\0')
        {
            // padding byte starts 
            break ;
        }
        
        if(strcmp(frame_id,ID3_edit->frame_ID) == 0)
        {
            write = fwrite(frame_id,1,4,new_mp3_ptr);

            if(write != 4)
            {
                printf(RED"\nCan't able to write frame ID...!\n"RESET);
                return FAILED ;
            }

            frame_size[0] = (ID3_edit->New_frame_size >> 24U ) & 0xFF ;
            frame_size[1] = (ID3_edit->New_frame_size >> 16U ) & 0xFF ;
            frame_size[2] = (ID3_edit->New_frame_size >> 8U ) & 0xFF ;
            frame_size[3] = (ID3_edit->New_frame_size >> 0U ) & 0xFF ;

            write = fwrite(frame_size,1,4,new_mp3_ptr);

            if(write != 4)
            {
                printf(RED"\nCan't able to write frame size...!\n"RESET);
                return FAILED ;
            }

            read = fread(frame_size,1,4,ID3_edit->fptr_mp3);
          
            if(read != 4)
            {
                printf(RED"\nCan't able to read frame size of matched frame...!\n"RESET);
                return FAILED ;
            }

            size_frame = ((uint32_t)frame_size[0] << 24U ) | ((uint32_t)frame_size[1] << 16U ) | ((uint32_t)frame_size[2] << 8U ) | ((uint32_t)frame_size[3] << 0U ) ;

            read = fread(frame_flag,1,2,ID3_edit->fptr_mp3);

            if(read != 2)
            {
                printf(RED"\nCan't able to read flag...!\n"RESET);
                return FAILED ;
            }

            write = fwrite(frame_flag,1,2,new_mp3_ptr);

            if(write != 2)
            {
                printf(RED"\nCan't able to write frame flag...!\n"RESET);
                return FAILED ;
            }

            uint8_t encode_byte = 0x00;

            fwrite(&encode_byte,1,1,new_mp3_ptr);

            fwrite(ID3_edit->New_frame_data,1,ID3_edit->New_frame_size-1,new_mp3_ptr);
            
            fseek(ID3_edit->fptr_mp3,size_frame,SEEK_CUR);
            
            offset_track += (10 + size_frame) ;
        }
        else
        {
            read = fread(frame_size,1,4,ID3_edit->fptr_mp3);

            if(read != 4)
            {
                printf(RED"\nCan't able to read frame Size...!\n"RESET);
                return FAILED ;
            }

            size_frame = ((uint32_t)frame_size[0] << 24U ) | ((uint32_t)frame_size[1] << 16U ) | ((uint32_t)frame_size[2] << 8U ) | ((uint32_t)frame_size[3] << 0U ) ;  

            read = fread(frame_flag,1,2,ID3_edit->fptr_mp3);

            if(read != 2)
            {
                printf(RED"\nCan't able to read frame flag...!\n"RESET);
                return FAILED ;
            }

            frame_content = malloc(sizeof(char)*size_frame);

            read = fread(frame_content,1,size_frame,ID3_edit->fptr_mp3);

            write = fwrite(frame_id,1,4,new_mp3_ptr);

            if(write != 4)
            {
                printf(RED"\nCan't able to write frame ID...!\n"RESET);
                return FAILED ;
            }

            write = fwrite(frame_size,1,4,new_mp3_ptr);

            if(write != 4)
            {
                printf(RED"\nCan't able to write frame Size...!\n"RESET);
                return FAILED ;
            }

            write = fwrite(frame_flag,1,2,new_mp3_ptr);

            if(write != 2)
            {
                printf(RED"\nCan't able to write frame flag...!\n"RESET);
                return FAILED ;
            }

            write = fwrite(frame_content,1,size_frame,new_mp3_ptr);

            if(write != size_frame)
            {
                printf(RED"\nCan't able to write frame content...!\n"RESET);
                return FAILED ;
            }

            free(frame_content);
            
            offset_track += (10 + size_frame) ;

            printf("\n Copied %s\n",frame_id);

        }

    }

    uint32_t padding_bytes = ID3_edit->tag_Size - offset_track ;

    char *pad_bytes = calloc(padding_bytes,sizeof(char));
        
    if(!pad_bytes)
    {
        printf(RED"\nCan't Able to allocate memory...!\n"RESET);
        return FAILED ;
    }

    write  = fwrite(pad_bytes,1,padding_bytes,new_mp3_ptr);

    if(write != padding_bytes)
    {
        printf(RED"\nCan't able to copy padding bytes...!\n"RESET);
        return FAILED ;
    }
    
    free(pad_bytes);

    fseek(ID3_edit->fptr_mp3,ID3_edit->tag_Size+10,SEEK_SET);

    uint8_t audio_chunk ;

    while(fread(&audio_chunk,1,1,ID3_edit->fptr_mp3) == 1)
    {
        fwrite(&audio_chunk,1,1,new_mp3_ptr);
    }
    
    fclose(ID3_edit->fptr_mp3);
    fclose(new_mp3_ptr);

    remove(ID3_edit->mp3_fname);
    
    rename(ID3_edit->temp_fname,ID3_edit->mp3_fname);
    
    return DONE ;

}