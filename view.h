#ifndef VIEW_H
#define VIEW_H

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
   uint8_t  frame_Data[100];
   uint8_t frame_type[20];

   uint32_t byte_processed ;

}ID3View_data;

state validate_mp3_file(char *argv[] ,ID3View_data *ID3_data);
state read_metadata(ID3View_data *ID3_data);
void select_frame_type(ID3View_data *ID3_data);

void display_help();

#endif


