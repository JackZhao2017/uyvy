
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include </usr/lib/include/jpeglib.h>
#include "rgb2uyvy.h"
HANDLE  h_rgb_uyvy = NULL;  
int read_jpeg_file(const char *input_filename, const char *output_filename)
{
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;
        FILE *input_file;
        FILE *output_file;
        JSAMPARRAY buffer;
        int row_width;

        unsigned char *output_buffer;
        unsigned char *tmp = NULL;
        unsigned char *uyvy=NULL;

        cinfo.err = jpeg_std_error(&jerr);

        if ((input_file = fopen(input_filename, "rb")) == NULL) {
                fprintf(stderr, "can't open %s\n", input_filename);
                return -1;
        }

        if ((output_file = fopen(output_filename, "wb")) == NULL) {

                fprintf(stderr, "can't open %s\n", output_filename);
                return -1;
        }

        jpeg_create_decompress(&cinfo);
        /* Specify data source for decompression */
        jpeg_stdio_src(&cinfo, input_file);
        /* Read file header, set default decompression parameters */
        (void) jpeg_read_header(&cinfo, TRUE);
        /* Start decompressor */
        (void) jpeg_start_decompress(&cinfo);
        row_width = cinfo.output_width * cinfo.output_components;
        printf("output_width:%d output_components:%d output_scanline:%d\n",cinfo.output_width,cinfo.output_components,cinfo.output_scanline);

        buffer = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);

        // write_bmp_header(&cinfo, output_file);
        output_buffer = (unsigned char *)malloc(row_width * cinfo.output_height);
        memset(output_buffer, 0, row_width * cinfo.output_height);
        tmp = output_buffer;
        /* Process data */
        while (cinfo.output_scanline < cinfo.output_height) {
                (void) jpeg_read_scanlines(&cinfo, buffer, 1);
                memcpy(tmp, *buffer, row_width);
                tmp += row_width;
        }
        uyvy=(unsigned char *)malloc(cinfo.output_width*cinfo.output_height*2);
        memset(uyvy,0,cinfo.output_width*cinfo.output_height*2);

        rgb_uyvy(h_rgb_uyvy, output_buffer, output_buffer+1,output_buffer+2, uyvy);
        fwrite(uyvy,1,cinfo.output_width*cinfo.output_height*2,output_file);

        free(output_buffer);
        (void) jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        /* Close files, if we opened them */
        fclose(input_file);
        fclose(output_file);
        return 0;
}


int main(int argc,char **argv)
{
    INT     i_width;  
    INT     i_height;  
    BYTE*   p_rgb32;  
    BYTE*   p_uyvy;

    i_width=640;
    i_height=480;

    h_rgb_uyvy = create_rgb_uyvy(i_width, i_height,24);      
    if (!h_rgb_uyvy)  
    {  
        release_rgb_uyvy(h_rgb_uyvy);  
        return;  
    }  

    read_jpeg_file("win.jpg", "win.uyvy");

    return 0;
}