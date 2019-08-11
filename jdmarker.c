#include <stdio.h>
#include <jinclude.h>
#include <jpeglib.h>

#define INPUT_BYTE(cinfo, buff, action) \
        //do
        {\
         if (!(JFREAD(cinfo->file, buff, sizeof(char))))\
         { action; }\
        //}while(0)
#define INPUT_2BYTE(cinfo, buff, action)\
       // do
        {\
         if (!(JFREAD(cinfo->file, buff, 2 * sizeof(char))))\
         { action; }\
       // }while(0);
typedef enum{
    M_SOF0 = 0xc0,
    M_SOF1 = 0xc1,
    M_SOF2 = 0xc2,
    M_SOF3 = 0xc3,

    M_SOF5 = 0xc5,
    M_SOF6 = 0xc6,
    M_SOF7 = 0xc7,

    M_JPG = 0xc8,
    M_SOF9 = 0xc9,
    M_SOF10 = 0xca,
    M_SOF11 = 0xcb,

    M_SOF13 = 0xcd,
    M_SOF14= 0xce,
    M_SOF15 = 0xcf,

    M_DHT = 0xc4,
    M_DAC = 0xcc,

    M_RST0 = 0xd0,
    M_RST1 = 0xd1,
    M_RST2 = 0xd2,
    M_RST3 = 0xd3,
    M_RST4 = 0xd4,
    M_RST5 = 0xd5,
    M_RST6 = 0xd6,
    M_RST7 = 0xd7,

    M_SOI  = 0xd8,
    M_EOI  = 0xd9,
    M_SOS  = 0xda,
    M_DQT  = 0xdb,
    M_DNL  = 0xdc,
    M_DRI  = 0xdd,
    M_DHP  = 0xde,
    M_EXP  = 0xdf,

    M_APP0 = 0xe0,
    M_APP1 = 0xe1,
    M_APP2 = 0xe2,
    M_APP3 = 0xe3,
    M_APP4 = 0xe4,
    M_APP5 = 0xe5,
    M_APP6 = 0xe6,
    M_APP7 = 0xe7,
    M_APP8 = 0xe8,
    M_APP9 = 0xe9,
    M_APP10 = 0xea,
    M_APP11 = 0xeb,
    M_APP12 = 0xec,
    M_APP13 = 0xed,
    M_APP14 = 0xee,
    M_APP15 = 0xef,

    M_JPG0 = 0xf0,
    M_JPG8 = 0xf8,
    M_JPG13 = 0xfd,
    M_COM = 0xfe,

    M_TEM = 0x01,
    M_ERROR = 0x100
};

bool getSOI(j_decompress_ptr cinfo)
{
    if (cinfo->marker->sawSOI)
    {
        printf("more sois\n");
        return false;
    }

    return true;
}

bool getSOF(j_decompress_ptr cinfo, bool is_baseline bool is_prog, bool is_arith)
{
    int length = 0;
    int c = 0;
    int ci = 0;
    int i = 0;

    unsigned char buff[2] = 0;

    INPUT_BYTE(cinfo, buff, return false);
    cinfo->sofMarker.data_precision = buff[0];
    
    INPUT_2BYTE(cinfo, buff, return false);
    cinfo->sofMarker.image_height = (buff[0] << 8) | buff[1];

    INPUT_2BYTE(cinfo, buff, return false);
    cinfo->sofMarker.image_width = (buff[0] << 8) | buff[1];

    INPUT_BYTE(cinfo, buff, return false);
    cinfo->sofMarker.num_components = buff[0];

    printf("data_precisiona:%d, image_height:%d, image_width:%d,num_components:%d\n",
    cinfo->sofMarker.data_precision,
    cinfo->sofMarker.image_height,
    cinfo->sofMarker.num_components
    );
   
    if ((cinfo->image_height <= 0) || (cinfo->image_width <= 0) || (cinfo->num_components <= 0))
    {
        printf("sof data err\n");
        return false;
    }

	for (ci = 0; ci < cinfo->sofMarker.num_components && ci < MAX_COMPS_NUM; ci++)
	{
		INPUT_BYTE(cinfo, buff, return false);
		cinfo->sofMarker.comp_info[ci].component_id = buff[0];
		
		INPUT_BYTE(cinfo, buff return false);
		cinfo->sofMarker.comp_info[ci].h_samp_factor = (buff[0] >> 4) & 15;
		cinfo->sofMarker.comp_info[ci].v_samp_factor = buff[0] & 15;

        INPUT_BYTE(cinfo, buff, return false);
		cinfo->sofMarker.comp_info[ci].quant_tbl_no = buff[0];
	}
}

static int firstMarker(j_decompress_ptr cinfo)
{
	int c1 = 0;
	int c2 = 0;

	c1 = getc(cinfo->file);
	c2 = getc(cinfo->file);
	if (c1 != 0xFF || c2 != M_SOI)
	{
		return 0;
	}

	return c2;
}

static int nextMarker(j_decompress_ptr cinfo)
{
	int c = 0;
	int discarded_bytes = 0;
    
	/* find 0xFF, count and skip any non-0xFF */
	c = getc(cinfo->file);
	while (0xFF != c)
	{
		discarded_bytes++;
		c = getc(cinfo->file);
	}
    
	/* skip duplicate 0xFF */
	do
	{
		c = getc(cinfo->file);
	}while (0xFF == c);

	if (discarded_bytes != 0)
	{
		printf("garbage data fount in Jpeg file %d", discarded_bytes);
	}

	return c;
}

int readMarkers(j_decompress_ptr cinfo)
{
	for (;;)
	{
		if (cinfo->unread_marker == 0)
		{
			if (!cinfo->marker->sawSOI)
			{
				if (!firstMarker(cinfo))
				{
					return JPEG_SUSPENDED;
				}
			}
		}
		else
		{
			if (!next_marker(cinfo))
			{
				return JPEG_SUSPENDED;
			}
		}
	}

	switch(cinfo->unread_marker)
	{
		case M_SOI:
			{
				break;
			}
		case M_EOI:
		{
			cinfo->unread_marker = 0;
			return JPEG_REACHEN_EOI;
		}
		default:
		{
			break;
		}
	}

	cinfo->unsigned = 0;
}
