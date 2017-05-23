#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rgb2uyvy.h"


typedef struct 
{  
    FLOAT r_table[256];  
    FLOAT g_table[256];  
    FLOAT b_table[256];  
}rgb2yuv_table_t;  
 
static rgb2yuv_table_t* create_rgb_y_table()  
{  
    rgb2yuv_table_t* p_table;  
    INT i;  
 
    p_table = (rgb2yuv_table_t*)malloc(sizeof(rgb2yuv_table_t));  
    if (!p_table)  
        return NULL;  
 
    for (i = 0; i <= 255; i++)  
    {  
        p_table->r_table[i] = 0.299f*i;  
        p_table->g_table[i] = 0.587f*i;  
        p_table->b_table[i] = 0.114f*i;  
    }  
 
    return p_table;  
}  
 
static rgb2yuv_table_t* create_rgb_u_table()  
{  
    rgb2yuv_table_t* p_table;  
    INT i;  
 
    p_table = (rgb2yuv_table_t*)malloc(sizeof(rgb2yuv_table_t));  
    if (!p_table)  
        return NULL;  
 
    for (i = 0; i <= 255; i++)  
    {  
        p_table->r_table[i] = (-0.169f)*i;  
        p_table->g_table[i] = (-0.331f)*i;  
        p_table->b_table[i] = 0.5f*i + 128;  
    }  
 
    return p_table;  
}  
 
static rgb2yuv_table_t* create_rgb_v_table()  
{  
    rgb2yuv_table_t* p_table;  
    INT i;  
 
    p_table = (rgb2yuv_table_t*)malloc(sizeof(rgb2yuv_table_t));  
    if (!p_table)  
        return NULL;  
 
    for (i = 0; i <= 255; i++)  
    {  
        p_table->r_table[i] = 0.5f*i;  
        p_table->g_table[i] = (-0.419f)*i;  
        p_table->b_table[i] = (-0.082f)*i + 128;  
    }  
 
    return p_table;  
}  
 
typedef struct   
{  
    INT i_width;  
    INT i_height;  
 
    INT i_rgb_pixel_byte;  
 
    rgb2yuv_table_t* p_y_table;  
    rgb2yuv_table_t* p_u_table;  
    rgb2yuv_table_t* p_v_table;  
 
} rgb_yuv_sys_t;  

void release_rgb_uyvy(HANDLE handle)  
{  
    rgb_yuv_sys_t* p_sys = (rgb_yuv_sys_t*)handle;  
 
    if (!p_sys)  
        return;  
 
    if (p_sys->p_y_table)  
        free(p_sys->p_y_table);  
 
    if (p_sys->p_u_table)  
        free(p_sys->p_u_table);  
 
    if (p_sys->p_v_table)  
        free(p_sys->p_v_table);  
 
    free(p_sys);  
}  
 
HANDLE create_rgb_uyvy(INT i_width, INT i_height, INT i_rgb_pixel_bits)  
{  
    rgb_yuv_sys_t* p_sys = (rgb_yuv_sys_t*)malloc(sizeof(rgb_yuv_sys_t));  
    if (!p_sys)  
        return NULL;  
 
    memset(p_sys,0,sizeof(rgb_yuv_sys_t));  
 
    p_sys->i_width = i_width;  
    p_sys->i_height = i_height;  
    p_sys->i_rgb_pixel_byte = i_rgb_pixel_bits / 8;  
 
    p_sys->p_y_table = create_rgb_y_table();  
    if (!p_sys->p_y_table)  
    {  
        release_rgb_uyvy(p_sys);   
        return NULL;  
    }  
 
    p_sys->p_u_table = create_rgb_u_table();  
    if (!p_sys->p_u_table)  
    {  
        release_rgb_uyvy(p_sys);   
        return NULL;  
    }  
 
    p_sys->p_v_table = create_rgb_v_table();  
    if (!p_sys->p_v_table)  
    {  
        release_rgb_uyvy(p_sys);   
        return NULL;  
    }  
 
    return (HANDLE)p_sys;  
}  
 
#define rgb_yuv_pixel(r, g, b, yuv, p_sys)\
    (yuv) = (BYTE)((p_sys)->r_table[(r)] + (p_sys)->g_table[(g)] + (p_sys)->b_table[(b)])  
 
void rgb_uyvy(HANDLE handle, BYTE* p_in_r, BYTE* p_in_g, BYTE* p_in_b, BYTE* p_out_uyuv)  
{  
    INT i_line;  
    INT i_pixel;  
 
    rgb_yuv_sys_t* p_table = (rgb_yuv_sys_t*)handle;  
 
    INT i_width    =  p_table->i_width;  
    INT i_height   = p_table->i_height;  
    INT i_rgb_pixel_byte = p_table->i_rgb_pixel_byte;  
 
    rgb2yuv_table_t* p_y_table = p_table->p_y_table;  
    rgb2yuv_table_t* p_u_table = p_table->p_u_table;  
    rgb2yuv_table_t* p_v_table = p_table->p_v_table;  
 
    BYTE* p_y = p_out_uyuv + 1;  
    BYTE* p_u = p_out_uyuv;  
    BYTE* p_v = p_out_uyuv + 2;  
 
    for (i_line = 0; i_line < i_height; i_line++)  
    {  
        for (i_pixel = 0; i_pixel < i_width; i_pixel++)  
        {  
            if ( (i_pixel+1)%2 )  
            {  
                rgb_yuv_pixel(*p_in_r, *p_in_g, *p_in_b, *p_y, p_y_table);  
                rgb_yuv_pixel(*p_in_r, *p_in_g, *p_in_b, *p_u, p_u_table);  
                rgb_yuv_pixel(*p_in_r, *p_in_g, *p_in_b, *p_v, p_v_table);  
                p_y += 2; p_u += 4; p_v += 4;  
            }  
            else 
            {  
                rgb_yuv_pixel(*p_in_r, *p_in_g, *p_in_b, *p_y, p_y_table);     
                p_y += 2;  
            }  
            p_in_r += i_rgb_pixel_byte; p_in_g += i_rgb_pixel_byte; p_in_b += i_rgb_pixel_byte;  
        }  
    }  
}
