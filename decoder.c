//
//  decoder.c
//  jpeg_compression
//
//  Created by Joanna Huang on 2024/12/19.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "myBMPLib.h"
#include <stddef.h> 


void usage(FILE *fp)
{
   
    fprintf(fp, "decoder 0 ResKimberly.bmp R.txt G.txt B.txt dim.txt\n");
    fprintf(fp, "decoder 1 QResKimberly.bmp Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw");
    return;
}

int demo0(char *bmp_fn, char *R_file, char *G_file, char *B_file, char *dim_file){
    
    BMP_file bmp;
    
    if( load_rgb_2_bmp(R_file, G_file, B_file, dim_file, &bmp) != 1 ) {
        exit(1);
    }
    if( set_BMP_header(&bmp) != 1 ) {
        exit(1);
    }
    if( BMP_file_save_fn(bmp_fn, &bmp) != 1 ) {
        exit(1);
    }
    
    for (int j = 0; j < bmp.H; j++) {
        free(bmp.B[j]);
        free(bmp.G[j]);
        free(bmp.R[j]);
    }
    free(bmp.B);
    free(bmp.G);
    free(bmp.R);
    
    return 1;
}

//QResKimberly.bmp Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw
int demo1_a(char *qres_bmp_file, char *bmp_file, char *fn_Qt_Y, char *fn_Qt_Cb, char *fn_Qt_Cr, char *fn_dim, char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr)
{
    BMP_file qres_bmp;
    BMP_file Kim_bmp;
    YCbCr_pixel ycbcr;
    int std_qt_y[8][8];
    int std_qt_cb[8][8];
    int std_qt_cr[8][8];
    if( read_qt_dim_text(fn_Qt_Y, fn_Qt_Cb, fn_Qt_Cr, fn_dim, 
                        std_qt_y, std_qt_cb, std_qt_cr, &ycbcr) != 1 ) {
        exit(1);
    }
    Block **blocks = allocateBlocks(&ycbcr);
    
    if( read_qf_raw_file(blocks, &ycbcr, fn_qF_Y, fn_qF_Cb, fn_qF_Cr)!= 1 ) {
        exit(1);
    }
    if( dequantization_coef(&ycbcr, blocks, std_qt_y, std_qt_cb, std_qt_cr) != 1 ){
        exit(1);
    }
    if( allocateYCbCr(&ycbcr) != 1){
        exit(1);
    }

    if( block_2D_IDCT((const Block**)blocks, &ycbcr) != 1){
        exit(1);
    }

    if( allocatergb(&qres_bmp, &ycbcr) != 1){
        exit(1);
    }
    if( ycbcr2rgb(&qres_bmp, &ycbcr) != 1 ){
        exit(1);
    }
    if( set_BMP_header(&qres_bmp) != 1 ) {
        exit(1);
    }
    if( BMP_file_save_fn(qres_bmp_file, &qres_bmp) != 1 ) {
        exit(1);
    }
    if( BMP_file_load_fn(bmp_file, &Kim_bmp) != 1 ){
        exit(1);
    }
    if( compute_sqnr(&Kim_bmp, &qres_bmp) != 1 ){
        exit(1);
    }

    free_blocks(&ycbcr, blocks);
    free_ycbcr(&ycbcr);
    free_bmp(&qres_bmp);
    free_bmp(&Kim_bmp);
    return 1;
}

int demo1_b(char *res_bmp_file, char *fn_Qt_Y, char *fn_Qt_Cb, char *fn_Qt_Cr, char *fn_dim, char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr,  char *fn_eF_Y, char *fn_eF_Cb, char *fn_eF_Cr)
{
    BMP_file res_bmp;
    YCbCr_pixel ycbcr;
    int std_qt_y[8][8];
    int std_qt_cb[8][8];
    int std_qt_cr[8][8];
    if( read_qt_dim_text(fn_Qt_Y, fn_Qt_Cb, fn_Qt_Cr, fn_dim, 
                        std_qt_y, std_qt_cb, std_qt_cr, &ycbcr) != 1 ) {
        exit(1);
    }
    Block **blocks = allocateBlocks(&ycbcr);

    if( read_qf_raw_file(blocks, &ycbcr, fn_qF_Y, fn_qF_Cb, fn_qF_Cr)!= 1 ) {
        exit(1);
    }
    if( dequantization_coef(&ycbcr, blocks, std_qt_y, std_qt_cb, std_qt_cr) != 1 ){
        exit(1);
    }

    if (read_err_file(blocks, &ycbcr, fn_eF_Y, fn_eF_Cb, fn_eF_Cr) != 1){
        exit(1);
    }
    if (add_quant_err(blocks, &ycbcr) != 1){
        exit(1);
    }

    if( allocateYCbCr(&ycbcr) != 1){
        exit(1);
    }
    if( block_2D_IDCT((const Block**)blocks, &ycbcr) != 1){
        exit(1);
    }
    if( allocatergb(&res_bmp, &ycbcr) != 1){
        exit(1);
    }
    if( ycbcr2rgb(&res_bmp, &ycbcr) != 1 ){
        exit(1);
    }
    if( set_BMP_header(&res_bmp) != 1 ) {
        exit(1);
    }
    if( BMP_file_save_fn(res_bmp_file, &res_bmp) != 1 ) {
        exit(1);
    }

    free_blocks(&ycbcr, blocks);
    free_ycbcr(&ycbcr);
    free_bmp(&res_bmp);

    return 1;
}

int demo2(char *qres_bmp_file, char *input_file_type, char *fn_rle_file)
{
    BMP_file qres_bmp;
    YCbCr_pixel ycbcr;
    int std_qt_y[8][8];
    int std_qt_cb[8][8];
    int std_qt_cr[8][8];

    if( read_dim_(&ycbcr, fn_rle_file, input_file_type) != 1 ) {
            exit(1);
    }

    Block **blocks = allocateBlocks(&ycbcr);
    
    if( read_rle_file(blocks, &ycbcr, fn_rle_file, input_file_type) != 1 ) {
        exit(1);
    }

    if( rld(blocks, &ycbcr) != 1 ) {
        exit(1);
    }

    if( inverse_zigzag(blocks, &ycbcr) != 1 ) {
        exit(1);
    }

    if( dc_DPC_D(blocks, &ycbcr) != 1 ) {
        exit(1);
    }

    set_ycbcr_qt(std_qt_y, std_qt_cb, std_qt_cr);
    if( dequantization_coef(&ycbcr, blocks, std_qt_y, std_qt_cb, std_qt_cr) != 1 ){
        exit(1);
    }

    if( allocateYCbCr(&ycbcr) != 1){
        exit(1);
    }

    if( block_2D_IDCT((const Block**)blocks, &ycbcr) != 1){
        exit(1);
    }
    
    if( allocatergb(&qres_bmp, &ycbcr) != 1){
        exit(1);
    }
    if( ycbcr2rgb(&qres_bmp, &ycbcr) != 1 ){
        exit(1);
    }
    if( set_BMP_header(&qres_bmp) != 1 ) {
        exit(1);
    }

    if( BMP_file_save_fn(qres_bmp_file, &qres_bmp) != 1 ) {
        exit(1);
    }
    
    free_blocks(&ycbcr, blocks);
    free_ycbcr(&ycbcr);
    free_bmp(&qres_bmp);

    return 1;
}


int main(int argc, char **argv)
{

    if ( argc <= 2 ) {
        usage(stderr);
        exit(1);
    }
    
    int option = 0;
    BMP_file bmp; 

    option = atoi(argv[1]);

    switch(option)
    {
        case 0:
            demo0(argv[2], argv[3], argv[4], argv[5], argv[6]);
        break;

        case 1:
            if(argc == 11){
                demo1_a(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]);
            }
            if(argc == 13){
                demo1_b(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11], argv[12]);
            }
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
