//
//  myBMPLib.h
//  jpeg_compression
//
//  Created by Joanna Huang on 2024/1/6.
//

#ifndef myBMPLib_h
#define myBMPLib_h

#ifndef INCLUDE_myBMPLib
#define INCLUDE_myBMPLib

/*construct a structure of BMP header*/
typedef struct _BMP_header {
    char identifier[2]; // 0x0000
    unsigned int filesize; // 0x0002
    unsigned short reserved; // 0x0006
    unsigned short reserved2;
    unsigned int bitmap_dataoffset; // 0x000A
    
    unsigned int bitmap_headersize; // 0x000E
    unsigned int width; // 0x0012
    unsigned int height; // 0x0016
    unsigned short planes; // 0x001A
    unsigned short bits_perpixel; // 0x001C
    unsigned int compression; // 0x001E
    unsigned int bitmap_datasize; // 0x0022
    unsigned int hresolution; // 0x0026
    unsigned int vresolution; // 0x002A
    unsigned int usedcolors; // 0x002E
    unsigned int importantcolors; // 0x0032
} BMP_header;


/* structure for BMP file */
typedef struct _BMP_file{
    char _filename[1024];
    BMP_header header;
    int H;
    int W;
    unsigned char **B;
    unsigned char **G;
    unsigned char **R;
} BMP_file;

typedef struct _Block_8x8{
    double F_Y[8][8];
    double F_Cb[8][8];
    double F_Cr[8][8];
    short qF_Y[8][8];
    short qF_Cb[8][8];
    short qF_Cr[8][8];
    float eF_Y[8][8];
    float eF_Cb[8][8];
    float eF_Cr[8][8];
    short z_Y[64];
    short z_Cb[64];
    short z_Cr[64];
    short rle_value_Y[64];
    unsigned char rle_zeros_Y[64];      // 連續零的數量不超過8bits範圍
    short rle_value_Cb[64]; 
    unsigned char rle_zeros_Cb[64];
    short rle_value_Cr[64];
    unsigned char rle_zeros_Cr[64];
}Block;

typedef struct _YCbCr_pixel{
    int H;
    int W;
    double **Y;
    double **Cb;
    double **Cr;
    int num_blocks_vertical;
    int num_blocks_horizontal;
}YCbCr_pixel;

typedef struct _SQNR_data{
    double freq_coef_power_Y;
    double freq_coef_power_Cb;
    double freq_coef_power_Cr;
    double error_power_Y;
    double error_power_Cb;
    double error_power_Cr;
    double sqnr_Y[8][8];
    double sqnr_Cb[8][8];
    double sqnr_Cr[8][8];
}DCT_SQNR;

/*  Demo 0  */
// load BMP given with file name (fn) 
int BMP_file_load_fn(char *fn, BMP_file *p_bmp);
// save BMP given with file name (fn)
int BMP_file_save_fn(char *fn, BMP_file *p_bmp);
// save BMP as text file 
int BMP_save_text(BMP_file *p_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim);
int load_rgb_2_bmp(char *R_file, char *G_file, char *B_file, char *dim_file, BMP_file *p_bmp);
int set_BMP_header(BMP_file *p_bmp);
int free_bmp(BMP_file *p_bmp);

/*  Demo 1  */
int rgb2ycbcr(BMP_file *p_bmp, YCbCr_pixel *p_ycbcr);
Block** allocateBlocks(YCbCr_pixel *p_ycbcr);
int free_blocks(YCbCr_pixel *p_ycbcr, Block **blocks);
int do_2dDCT(Block *b, const YCbCr_pixel *pixelycbcr, int i, int j);
/* Do DCT to Y, Cb, Cr in ycbcr and save to F_Y, F_Cb, F_Cr in blocks */
int block_2DDCT(YCbCr_pixel *ycbcr, Block **blocks);
int save_qt_file(char *qtY_file ,char *qtCb_file, char *qtCr_file);
int quantization(Block **blocks,const  YCbCr_pixel *p_ycbcr);
int save_qF(Block **blocks, const YCbCr_pixel *p_ycbcr, char *qF_Y_file, char *qF_Cb_file, char *qF_Cr_file);
int calculate_err_and_sqnr(YCbCr_pixel *p_ycbcr, DCT_SQNR *p_sqnr, Block **blocks);
int save_dim_qerr_sqnr_file(Block **blocks, YCbCr_pixel *p_ycbcr, char *dim_file, 
                            char *eF_Y_file, char *eF_Cb_file, char *eF_Cr_file, DCT_SQNR *p_sqnr);
int read_qt_dim_text(char *fn_Qt_Y, char *fn_Qt_Cb, char *fn_Qt_Cr, char *fn_dim, 
                    int std_qt_y[8][8], int std_qt_cb[8][8], int std_qt_cr[8][8], 
                    YCbCr_pixel *p_ycbcr);

int read_qf_raw_file(Block **blocks, YCbCr_pixel *p_ycbcr, char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr);
/* Dequantization qF_Y, qF_Cb, qF_Cr to F_Y, F_Cb, F_Cr by quantization table */
int dequantization_coef(YCbCr_pixel *p_ycbcr, Block **blocks, int std_qt_y[8][8], int std_qt_cb[8][8], int std_qt_cr[8][8]);
int allocateYCbCr(YCbCr_pixel *p_ycbcr);
int free_ycbcr(YCbCr_pixel *p_ycbcr);
int do_2dIDCT(const Block *b, YCbCr_pixel *pixelycbcr, int i, int j);
/* do Inverse DCT to F in every blocks and save to Y Cb Cr in p_ycbcr */
int block_2D_IDCT(const Block **blocks, YCbCr_pixel *p_ycbcr);
int allocatergb(BMP_file *p_bmp,  YCbCr_pixel *p_ycbcr);
int ycbcr2rgb(BMP_file *p_bmp, YCbCr_pixel *p_ycbcr);
int compute_sqnr(BMP_file *p_Kimbmp, BMP_file *p_qresbmp);
int read_err_file(Block **blocks, YCbCr_pixel *p_ycbcr, char *fn_eF_Y, char *fn_eF_Cb, char *fn_eF_Cr);
int add_quant_err(Block **blocks, YCbCr_pixel *p_ycbcr);

/* Demo 2 */
int save_q_table(void);
int dc_DPCM(Block **blocks, YCbCr_pixel *p_ycbcr);
int zigzag(Block **blocks, YCbCr_pixel *p_ycbcr);
int rle(Block **blocks, YCbCr_pixel *p_ycbcr);
int save_rle_ascii(Block **b, YCbCr_pixel *p_ycbcr);
int save_rle_bin(Block **b, YCbCr_pixel *p_ycbcr);
int cal_compression_ratio(Block **blocks, YCbCr_pixel *p_ycbcr);
int read_dim_(YCbCr_pixel *p_ycbcr, char *fn_rle_file, char *type);
void read_channel_in_b(FILE *fp, unsigned char run[64], short value[64]);
int read_rle_file(Block **b, YCbCr_pixel *p_ycbcr, char *fn_rle_file, char *type);
int decode_run_length(short *value, unsigned char *run, short *zigzag);
int rld(Block **b,  YCbCr_pixel *p_ycbcr);
void do_izigzag(short *z, short qF[8][8]);
int inverse_zigzag(Block **blocks, YCbCr_pixel *p_ycbcr);
int dc_DPC_D(Block **b, YCbCr_pixel *p_ycbcr);
void set_ycbcr_qt(int std_qt_y[8][8], int std_qt_cb[8][8], int std_qt_cr[8][8]);
#endif
#endif
