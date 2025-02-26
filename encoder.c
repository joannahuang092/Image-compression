//
//  encoder.c
//  jpeg_compression
//
//  Created by Joanna Huang on 2024/12/19.
//
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> 
#include "myBMPLib.h"

#define pi 3.1415926535


void usage(FILE *fp)
{
    fprintf(fp, "encoder 0 input.bmp R.txt G.txt B.txt dim.txt\n");
    fprintf(fp, "encoder 1 input.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw\n");
}

int demo0(char *fn_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim){
    BMP_file bmp;
    if( BMP_file_load_fn(fn_bmp, &bmp)!= 1 ) {
        exit(-1);
    }
    if ( BMP_save_text(&bmp, fn_R, fn_G, fn_B, fn_dim)!= 1 ){
        exit(1);
    }

    free_bmp(&bmp);
    return 1;
}


int demo1(char *fn_bmp, char *fn_Qt_Y, char *fn_Qt_Cb, char *fn_Qt_Cr, char *fn_dim, 
          char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr, 
          char *fn_eF_Y, char *fn_eF_Cb, char *fn_eF_Cr)
{
    BMP_file bmp;
    YCbCr_pixel ycbcr;
    DCT_SQNR sqnr;

    if( BMP_file_load_fn(fn_bmp, &bmp)!= 1 ) {
        printf("Error loading BMP file\n");
        exit(1);
    }
    if( rgb2ycbcr(&bmp, &ycbcr)!= 1 ) {
        printf("Error converting RGB to YCbCr\n");
        exit(1);
    }
    // Divide image into 8*8 blocks and do 2D-DCT
    Block **blocks = allocateBlocks(&ycbcr);
    if( block_2DDCT(&ycbcr, blocks) != 1){
        printf("Error doing 2DDCT to blocks\n");
        exit(1);
    }
    if( save_qt_file(fn_Qt_Y ,fn_Qt_Cb, fn_Qt_Cr) != 1){
        printf("Error saving quantization table.\n");
        exit(1);
    }
    
    // do quantization to all frequency value by quatization_table and calculate error
    if( quantization(blocks, &ycbcr) != 1){
        exit(1);
    }
    if( save_qF(blocks, &ycbcr, fn_qF_Y, fn_qF_Cb, fn_qF_Cr) != 1){
        exit(1);
    }

    if( calculate_err_and_sqnr(&ycbcr, &sqnr, blocks) != 1 ) {
        exit(1);
    }
    
    if( save_dim_qerr_sqnr_file(blocks, &ycbcr, fn_dim, fn_eF_Y, fn_eF_Cb, fn_eF_Cr, &sqnr) != 1 ) {
        exit(1);
    }
    
    free_blocks(&ycbcr, blocks);
    free_ycbcr(&ycbcr);
    free_bmp(&bmp);
    
    return 1;
}

int demo2(char *fn_bmp, char *file_name, char *fn_rle)
{
    BMP_file bmp;
    YCbCr_pixel ycbcr;

    if( BMP_file_load_fn(fn_bmp, &bmp)!= 1 ) {
        printf("Error loading BMP file\n");
        exit(1);
    }
    if( rgb2ycbcr(&bmp, &ycbcr)!= 1 ) {
        printf("Error converting RGB to YCbCr\n");
        exit(1);
    }
    
    // Divide image into 8*8 blocks and do 2D-DCT
    Block **blocks = allocateBlocks(&ycbcr);
    if( block_2DDCT(&ycbcr, blocks) != 1){
        printf("Error doing 2DDCT to blocks\n");
        exit(1);
    }
    // do quantization to all frequency value by quatization_table and calculate error
    if( quantization(blocks, &ycbcr) != 1){
        exit(1);
    }
    if( dc_DPCM(blocks, &ycbcr) != 1 ){
        exit(1);
    }
    if ( zigzag(blocks, &ycbcr) != 1){
        exit(1);
    }
    if( rle(blocks, &ycbcr) != 1){
        exit(1);
    }

    if( strcmp(file_name, "ascii") == 0 ){
        if( save_rle_ascii(blocks, &ycbcr) != 1 ){
            exit(1);
        }
    }
    else if( strcmp(file_name, "binary") == 0 ){
        if( save_rle_bin(blocks, &ycbcr) != 1 ){
            exit(1);
        }
        if (cal_compression_ratio(blocks, &ycbcr) != 1){
             exit(1);
        }
    }

    free_blocks(&ycbcr, blocks);
    free_ycbcr(&ycbcr);
    free_bmp(&bmp);
    
    return 1;
}


int main(int argc, char **argv)
{
    
    int option = 0;     
    BMP_file bmp;

    if ( argc <=2 ) {
        usage(stderr);
        exit(1);
    }

    option = atoi(argv[1]);

    switch(option)
    {
        case 0:
            demo0(argv[2], argv[3], argv[4], argv[5], argv[6]);
        break;

        case 1:
            demo1(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11], argv[12]);
        break;
        
        case 2:
            demo2(argv[2], argv[3], argv[4]);
        break;
            
        case 3:
            //demo3(argv[2], argv[3], argv[4], argv[5]);
        break;
        
    }
    return 0;
}


