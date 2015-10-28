//
//  FullToEvenOdd.c
//  FullToEvenOdd
//
//  Created by Hank Lee on 8/31/15.
//  Copyright (c) 2015 Hank Lee. All rights reserved.
//

#include <stdint.h>
#include <string.h>

void full_to_even_odd
(
    int width,
    int height,
    uint8_t *dst_top,
    uint8_t *dst_bot,
    const uint8_t *src
)
{
    int i;
    
    for (i = 0; i < height / 2; i++)
    {
        memcpy(dst_top, src, width);
        src += width; dst_top += width;
        
        memcpy(dst_bot, src, width);
        src += width; dst_bot += width;
    }
}
