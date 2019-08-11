#define MAX_COMPS_NUM 3
#define JPEG_SUSPENDED 0

struct jpeg_source_mgr{
    const char *next_input_byte; //next byte to read buffer
    size_t bytes_in_buffer; //of bytes remaining in buffer
};

/* sof component info  */
struct jpeg_component_info
{
    int component_id;
    int h_samp_factor;
    int v_samp_factor;
    int quant_tbl_no;
    
    int dc_tbl_no;
    int ac_tbl_no;
};
/* sof marker */
struct jpeg_sof_marker
{
    long data_precision;
    long image_width;
    int image_height;
    int num_components;
    struct jpeg_component_info comp_info[MAX_COMPS_NUM];
}

struct jpeg_decompress_struct
{
    FILE *file;
    struct jpeg_source_mgr *src;
    struct jpeg_sof_marker sofMarker; /* sof marker info */
};

typedef struct jpeg_decompress_struct * j_decompress_ptr
