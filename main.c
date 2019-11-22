//
//  main.c
//  FullToEvenOdd
//
//  Splits a full YUV into even lines (Top) and odd lines (Bottom)
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
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include "FullToEvenOdd.h"


#define MAX_WIDTH   3840
#define MAX_HEIGHT  2160


static uint8_t img[MAX_WIDTH * MAX_HEIGHT * 3 / 2];
static uint8_t buffer_top_y[MAX_WIDTH * MAX_HEIGHT / 2];
static uint8_t buffer_bot_y[MAX_WIDTH * MAX_HEIGHT / 2];
static uint8_t buffer_top_u[MAX_WIDTH * MAX_HEIGHT / 2 / 4];
static uint8_t buffer_bot_u[MAX_WIDTH * MAX_HEIGHT / 2 / 4];
static uint8_t buffer_top_v[MAX_WIDTH * MAX_HEIGHT / 2 / 4];
static uint8_t buffer_bot_v[MAX_WIDTH * MAX_HEIGHT / 2 / 4];


int main(int argc, const char * argv[]) {
    int ifd;
    int ofd;
    
    ssize_t rd_sz;
    
    uint32_t width;
    uint32_t height;
    uint32_t wxh;

    uint8_t *src;
    
    char *cp;
    char output[256] = { 0 };

    uint32_t frame_cnt;
    struct timeval tv_start = { 0 };
    struct timeval tv_end = { 0 };
    struct timeval tv_interval = { 0 };


    if (argc < 4)
    {
        fprintf(stderr, "useage: %s [input_file] [width] [height]\n", argv[0]);
        
        return -1;
    }

    rd_sz   = 0;
    width   = 0;
    height  = 0;
    cp      = NULL;
    frame_cnt = 0;
    
    // get input file name from comand line
    ifd = open(argv[1], O_RDONLY);
    if (ifd < 0)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    
    // specify output file name
    cp = strrchr(argv[1], '.');
    strncpy(output, argv[1], cp - argv[1]);
    strcat(output, "i");
    strcat(output, cp);
    
    ofd = open(output,
                 O_WRONLY | O_CREAT | O_TRUNC,
                 S_IRUSR);
    
    width   = atoi(argv[2]);
    height  = atoi(argv[3]);
    
    wxh = width * height;
    
    fprintf(stderr, "Processing: ");

    gettimeofday(&tv_start, NULL);
    while (1)
    {
        rd_sz = read(ifd, img, wxh * 3 / 2);
        
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
            write(ofd, buffer_top_y, wxh / 2);
            write(ofd, buffer_top_u, wxh / 2 / 4);
            write(ofd, buffer_top_v, wxh / 2 / 4);

            // Output Bottom
            write(ofd, buffer_bot_y, wxh / 2);
            write(ofd, buffer_bot_u, wxh / 2 / 4);
            write(ofd, buffer_bot_v, wxh / 2 / 4);
 
            frame_cnt++;
        }
        else
        {
            break;
        }
        fputc('.', stderr);
        fflush(stderr);
    }   
    gettimeofday(&tv_end, NULL);
    timersub(&tv_end, &tv_start, &tv_interval);

    close(ifd);
    close(ofd);

    fprintf(stderr, "Done\n");
    fprintf(stderr, "Output file: %s\n", output);

    double interval_in_double = ((double) tv_interval.tv_sec * 1000000 + tv_interval.tv_usec) / 1000000;
    fprintf(stderr, "Performance: %f frames per second\n", frame_cnt / interval_in_double);
    
    return 0;
}
