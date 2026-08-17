#ifndef type_H
#define type_H

#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>
#include<ctype.h>

#define RED    "\033[1;31m"
#define YELLOW "\033[33m"
#define GREEN  "\033[1;32m"
#define RESET  "\033[0m"

typedef enum 
{
    FAILED = 0,
    DONE
}state;

#endif