//
//  main.c
//  FullToEvenOdd
//
//  Splits a full YUV into 2 YUVs: one contains even lines (Top), and the other contains odd lines (Bottom)
//
//  Created by Hank Lee on 8/31/15.
//  Copyright (c) 2015 Hank Lee. All rights reserved.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "FullToEvenOdd.h"

#define MAX_WIDTH   3840
#define MAX_HEIGHT  2160


typedef struct
{
    char name[256];
} string_t;


static uint8_t img[MAX_WIDTH * MAX_HEIGHT * 3 / 2];
static uint8_t buffer_top_y[MAX_WIDTH * MAX_HEIGHT / 2];
static uint8_t buffer_bot_y[MAX_WIDTH * MAX_HEIGHT / 2];
static uint8_t buffer_top_u[MAX_WIDTH * MAX_HEIGHT / 2 / 4];
static uint8_t buffer_bot_u[MAX_WIDTH * MAX_HEIGHT / 2 / 4];
static uint8_t buffer_top_v[MAX_WIDTH * MAX_HEIGHT / 2 / 4];
static uint8_t buffer_bot_v[MAX_WIDTH * MAX_HEIGHT / 2 / 4];

static string_t null;

int main(int argc, const char * argv[]) {
    int fd_rd;
    int fd_wr;
    
    ssize_t rd_sz;
    
    uint32_t width;
    uint32_t height;
    uint32_t wxh;

    uint8_t *src;
    
    char *cp;
    string_t output;
    

    if (argc < 4)
    {
        fprintf(stderr, "useage: %s [input_file] [width] [height]\n", argv[0]);
        
        return -1;
    }

    rd_sz   = 0;
    width   = 0;
    height  = 0;
    cp      = NULL;
    output  = null;
    
    // get input file name from comand line
    fd_rd = open(argv[1], O_RDONLY);
    
    // specify output file name
    cp = strchr(argv[1], '.');
    strncpy(output.name, argv[1], cp - argv[1]);
    strcat(output.name, "i");
    strcat(output.name, cp);
    
    fd_wr = open(output.name,
                 O_WRONLY | O_CREAT | O_TRUNC,
                 S_IRUSR);
    
    width   = atoi(argv[2]);
    height  = atoi(argv[3]);
    
    wxh = width * height;
    
    fprintf(stderr, "Processing: ");

    while (1)
    {
        rd_sz = read(fd_rd, img, wxh * 3 / 2);
        
        if (rd_sz == wxh * 3 / 2)
        {
            // Process Y
            src = img;
            
            full_to_even_odd(width, height, buffer_top_y, buffer_bot_y, src);
            
            // Process U
            src += wxh;
            
            full_to_even_odd(width / 2, height / 2, buffer_top_u, buffer_bot_u, src);
            
            // Process V
            src += wxh / 4;
            
            full_to_even_odd(width / 2, height / 2, buffer_top_v, buffer_bot_v, src);
            
            // Output Top
            write(fd_wr, buffer_top_y, wxh / 2);
            write(fd_wr, buffer_top_u, wxh / 2 / 4);
            write(fd_wr, buffer_top_v, wxh / 2 / 4);

            // Output Bottom
            write(fd_wr, buffer_bot_y, wxh / 2);
            write(fd_wr, buffer_bot_u, wxh / 2 / 4);
            write(fd_wr, buffer_bot_v, wxh / 2 / 4);
        }
        else
        {
            break;
        }
        fputc('.', stderr);
        fflush(stderr);
    }   

    close(fd_rd);
    close(fd_wr);

    fprintf(stderr, "Done\n");
    fprintf(stderr, "Output file: %s\n", output.name);
    
    return 0;
}
