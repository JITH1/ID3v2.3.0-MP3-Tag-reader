#ifndef EDIT_H
#define EDIT_H

#include "All_type.h"

typedef struct 
{
   FILE *fptr_mp3 ;
   char *mp3_fname ;

   uint8_t ID3_format[4];
   uint8_t version ;
   uint8_t revision ;
   uint32_t tag_Size ;

   uint8_t frame_ID[5] ;
   uint32_t frame_Size ;
   uint8_t frame_type[20];

   uint32_t New_frame_size ;
   uint8_t New_frame_data[100];
   uint32_t new_tag_size ;
   
   char *temp_fname ;

}ID3Edit_data;

state validate_edit_mp3(char *argv[] , ID3Edit_data *ID3_edit);
state edit_metadata(ID3Edit_data *ID3_edit);
state select_editframe_type(ID3Edit_data *ID3_edit,char *argv[]);

#endif

