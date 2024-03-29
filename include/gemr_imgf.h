
/* derived from: */
/*    @(#)imageFileOffsets.h 1.1 92/04/29 GE Medical Systems    */
/*
 * Copyright 1991 by General Electric Co.
 */
 

#define IMGF_MAGIC	0x494d4746	
#define IC_ASIS		0
#define IC_RECT		1
#define IC_PACKED	2
#define IC_COMPRESSED	3
#define IC_COMPACK	4

typedef struct VARTYP {
        unsigned long   length; /* length of the data */
        char *          data;   /* pointer to the data */
}  VARTYPE;


/* the first header -- the only part garunteed to be aligned reasonably */
struct GemrImgf_Img {
        int     img_magic;      /* magic number */
        int     img_hdr_length; /* length of total header in bytes and 
                                a byte displacement to the 'pixel data area' */
        int     img_width;      /* width (pixels) of image */
        int     img_height;     /* height (pixels) of image */
        int     img_depth;      /* depth (1, 8, 16, or 24 bits) of pixel */
        int     img_compress;   /* type of compression; see IC_* below */
        int     img_dwindow;    /* default window setting */
        int     img_dlevel;     /* default level setting */
        int     img_bgshade;    /* background shade to use for non-image */
        int     img_ovrflow;    /* overflow value */
        int     img_undflow;    /* underflow value */
        int     img_top_offset; /* number of blank lines at image top */
        int     img_bot_offset; /* number of blank lines at image bottom */
        short   img_version;    /* version of the header structure */
        u_short img_checksum;   /* 16 bit end_around_carry sum of pixels */
        int     img_p_id;       /* a byte disp to unique image identifier */
        int     img_l_id;       /* byte length of unique image identifier */
        int     img_p_unpack;   /* a byte disp to 'unpack control' */
        int     img_l_unpack;   /* byte length of 'unpack control' */
        int     img_p_compress; /* a byte disp to 'compression control' */
        int     img_l_compress; /* byte length of 'compression control' */
        int     img_p_histo;    /* a byte disp to 'histogram control' */
        int     img_l_histo;    /* byte length of 'histogram control' */
        int     img_p_text;     /* a byte disp to 'text plane data' */
        int     img_l_text;     /* byte length of 'text plane data' */
        int     img_p_graphics; /* a byte disp to 'graphics plane data' */
        int     img_l_graphics; /* byte length of 'graphics plane data' */
        int     img_p_dbHdr;    /* a byte disp to 'data base header data' */
        int     img_l_dbHdr;    /* byte length of 'data base header data' */
        int     img_levelOffset;/* value to add to stored Pixel Data values */
                                /* to get the correct presentation value */
        int     img_p_user;     /* byte displacement to user defined data */
        int     img_l_user;     /* byte length of user defined data */                  
        int     img_p_suite;    /* byte displacement to suite header data */
        int     img_l_suite;    /* byte length of suite defined data */                  
        int     img_p_exam;     /* byte displacement to exam header data */
        int     img_l_exam;     /* byte length of exam defined data */                  
        int     img_p_series;   /* byte displacement to series header data */
        int     img_l_series;   /* byte length of series defined data */                  
        int     img_p_image;    /* byte displacement to image header data */
        int     img_l_image;    /* byte length of image defined data */                  
};

/* offsets */

/*  ImageFile PixHdr definitions  */
 
#define _IH_img_magic   0x0     /* 0 - int - magic number */
#define _IH_img_hdr_length      0x4     /* 4 - int - a byte displacement to the <pixel data area> */
#define _IH_img_width   0x8     /* 8 - int - width (pixels) of image */
#define _IH_img_height  0xc     /* 12 - int - height (pixels) of image */
#define _IH_img_depth   0x10    /* 16 - int - depth (1, 8, 16, or 24 bits) of */
#define _IH_img_compress        0x14    /* 20 - int - type of compression; see IC_* below */
#define _IH_img_dwindow 0x18    /* 24 - int - default window setting */
#define _IH_img_dlevel  0x1c    /* 28 - int - default level setting */
#define _IH_img_bgshade 0x20    /* 32 - int - background shade to use for non-image */
#define _IH_img_ovrflow 0x24    /* 36 - int - overflow value */
#define _IH_img_undflow 0x28    /* 40 - int - underflow value */
#define _IH_img_top_offset      0x2c    /* 44 - int - number of blank lines at image top */
#define _IH_img_bot_offset      0x30    /* 48 - int - number of blank lines at image bottom */
#define _IH_img_version 0x34    /* 52 - short - version of the header structure */
#define _IH_img_checksum        0x36    /* 54 - u_short - 16 bit end_around_carry sum of pixels */
#define _IH_img_p_id    0x38    /* 56 - int - a byte disp to unique image identifier */
#define _IH_img_l_id    0x3c    /* 60 - int - byte length of unique image identifier */
#define _IH_img_p_unpack	0x40	/* 64 - int - a byte disp to <unpack control> */
#define _IH_img_l_unpack        0x44    /* 68 - int - byte length of <unpack control> */
#define _IH_img_p_compress      0x48    /* 72 - int - a byte disp to <compression control> */
#define _IH_img_l_compress      0x4c    /* 76 - int - byte length of <compression control> */
#define _IH_img_p_histo 0x50    /* 80 - int - a byte disp to <histogram control> */
#define _IH_img_l_histo 0x54    /* 84 - int - byte length of <histogram control> */
#define _IH_img_p_text  0x58    /* 88 - int - a byte disp to <text plane data> */
#define _IH_img_l_text  0x5c    /* 92 - int - byte length of <text plane data> */
#define _IH_img_p_graphics      0x60    /* 96 - int - a byte disp to <graphics plane data> */
#define _IH_img_l_graphics      0x64    /* 100 - int - byte length of <graphics plane data> */
#define _IH_img_p_dbHdr 0x68    /* 104 - int - a byte disp to <data base header data> */
#define _IH_img_l_dbHdr 0x6c    /* 108 - int - byte length of <data base header data> */
#define _IH_img_levelOffset     0x70    /* 112 - int - value to add to stored Pixel Data values */
#define _IH_img_p_user  0x74    /* 116 - int - byte displacement to user defined data */
#define _IH_img_l_user  0x78    /* 120 - int - byte length of user defined data */
#define _IH_img_p_suite 0x7c    /* 124 - int - byte displacement to <suite> header data */
#define _IH_img_l_suite 0x80    /* 128 - int - byte length of <suite> header data */
#define _IH_img_p_exam  0x84    /* 132 - int - byte displacement to <exam> header data */
#define _IH_img_l_exam  0x88    /* 136 - int - byte length of <exam> header data */
#define _IH_img_p_series        0x8c    /* 140 - int - byte displacement to <series> header data */
#define _IH_img_l_series        0x90    /* 144 - int - byte length of <series> header data */
#define _IH_img_p_image 0x94    /* 148 - int - byte displacement to <image> header data */
#define _IH_img_l_image 0x98    /* 152 - int - byte length of <image> header data */
#define _IH_sizeof      0x9c    /* 156 - sizeof - sizeof ImgHdr */
 
/*  ImgHst definitions  */
 
#define _HS_hs_version  0x0     /* 0 - int - version of the histogram structure */
#define _HS_hs_sd       0x4     /* 4 - float - standard deviation of pixel data value */
#define _HS_hs_mean     0x8     /* 8 - short - rounded mean pixel data value */
#define _HS_hs_min      0xa     /* 10 - short - minimum pixel data value */
#define _HS_hs_max      0xc     /* 12 - short - maximum pixel data value */
#define _HS_hs_first    0xe     /* 14 - short - first histogram entry used for <hs_sd> */
#define _HS_hs_region   0x10    /* 16 - short - region entries used for <hs_sd> */
#define _HS_hs_length   0x12    /* 18 - short - number of bins in the histogram area */
#define _HS_hs_bins     0x814   /* 2068 - u_short[1024] - histogram bins */
 
/*  SUITEDATATYPE definitions  */
 
#define _SU_su_id       0x0     /* 0 - char[4] - Suite ID */
#define _SU_su_uniq     0x4     /* 4 - short - Make Unique Flag */
#define _SU_su_diskid   0x6     /* 6 - char[1] - Disk ID */
#define _SU_prodid      0x7     /* 7 - char[13] - Product ID */
#define _SU_su_verscre  0x14    /* 20 - char[2] - Original Version of Record */
#define _SU_su_verscur  0x16    /* 22 - char[2] - Current Version of Record */
#define _SU_su_checksum 0x18    /* 24 - u_int - Suite Record Checksum */
#define _SU_su_padding  0x1c    /* 28 - char[85] - Spare Space */
#define _SU_sizeof      0x72    /* 114 - sizeof SUITEDATATYPE */
 
/*  EXAMDATATYPE definitions  */
 
#define _EX_ex_suid     0x0     /* 0 - char[4] - Suite ID for this Exam */
#define _EX_ex_uniq     0x4     /* 4 - short - Make-Unique Flag */
#define _EX_ex_diskid   0x6     /* 6 - char - Disk ID for this Exam */
#define _EX_ex_no       0x8     /* 8 - u_short - Exam Number */
#define _EX_hospname    0xa     /* 10 - char[33] - Hospital Name */
#define _EX_detect      0x2c    /* 44 - short - Detector Type */
#define _EX_numcells    0x2e    /* 46 - int - Number of cells in det */
#define _EX_zerocell    0x32    /* 50 - float - Cell number at theta */
#define _EX_cellspace   0x36    /* 54 - float - Cell spacing */
#define _EX_srctodet    0x3a    /* 58 - float - Distance from source to detector */
#define _EX_srctoiso    0x3e    /* 62 - float - Distance from source to iso */
#define _EX_tubetyp     0x42    /* 66 - short - Tube type */
#define _EX_dastyp      0x44    /* 68 - int - DAS type */
#define _EX_num_dcnk    0x46    /* 70 - short - Number of Decon Kernals */
#define _EX_dcn_len     0x48    /* 72 - short - Number of elements in a Decon Kernal */
#define _EX_dcn_density 0x4a    /* 74 - short - Decon Kernal density */
#define _EX_dcn_stepsize        0x4c    /* 76 - short - Decon Kernal stepsize */
#define _EX_dcn_shiftcnt        0x4e    /* 78 - short - Decon Kernal Shift Count */
#define _EX_magstrength 0x50    /* 80 - int - Magnet strength (in gauss) */
#define _EX_patid       0x54    /* 84 - char[13] - Patient ID for this Exam */
#define _EX_patname     0x61    /* 97 - char[25] - Patient Name */
#define _EX_patage      0x7a    /* 122 - short - Patient Age (years, months or days) */
#define _EX_patian      0x7c    /* 124 - short - Patient Age Notation */
#define _EX_patsex      0x7e    /* 126 - short - Patient Sex */
#define _EX_patweight   0x80    /* 128 - int - Patient Weight */
#define _EX_trauma      0x84    /* 132 - short - Trauma Flag */
#define _EX_hist        0x86    /* 134 - char[61] - Patient History */
#define _EX_reqnum      0xc3    /* 195 - char[13] - Requisition Number */
#define _EX_ex_datetime 0xd0    /* 208 - int - Exam date/time stamp */
#define _EX_refphy      0xd4    /* 212 - char[33] - Referring Physician */
#define _EX_diagrad     0xf5    /* 245 - char[33] - Diagnostician/Radiologist */
#define _EX_op  0x116   /* 278 - char[4] - Operator */
#define _EX_ex_desc     0x11a   /* 282 - char[23] - Exam Description */
#define _EX_ex_typ      0x131   /* 305 - char[3] - Exam Type */
#define _EX_ex_format   0x134   /* 308 - short - Exam Format */
#define _EX_firstaxtime 0x136   /* 310 - double - Start time(secs) of first axial in exam */
#define _EX_ex_sysid    0x13e   /* 318 - char[9] - Creator Suite and Host */
#define _EX_ex_lastmod  0x148   /* 328 - int - Date/Time of Last Change */
#define _EX_protocolflag        0x14c   /* 332 - short - Non-Zero indicates Protocol Exam */
#define _EX_ex_alloc_key        0x14e   /* 334 - char[13] - Process that allocated this record */
#define _EX_ex_delta_cnt        0x15c   /* 348 - int - Indicates number of updates to header */
#define _EX_ex_verscre  0x160   /* 352 - char[2] - Version - Created */
#define _EX_ex_verscur  0x162   /* 354 - char[2] - Version - Now */
#define _EX_ex_checksum 0x164   /* 356 - u_int - Exam Record Checksum */
#define _EX_ex_complete 0x168   /* 360 - int - Exam Complete Flag */
#define _EX_ex_seriesct 0x16c   /* 364 - int - Last Series Number Used */
#define _EX_ex_numarch  0x170   /* 368 - int - Number of Series Archived */
#define _EX_ex_numseries        0x174   /* 372 - int - Number of Series Existing */
#define _EX_ex_series   0x178   /* 376 - VARTYPE - Series Keys for this Exam */
#define _EX_ex_numunser 0x180   /* 384 - int - Number of Unstored Series */
#define _EX_ex_unseries 0x184   /* 388 - VARTYPE - Unstored Series Keys for this Exam */
#define _EX_ex_toarchcnt        0x18c   /* 396 - int - Number of Unarchived Series */
#define _EX_ex_toarchive        0x190   /* 400 - VARTYPE - Unarchived Series Keys for this Exam */
#define _EX_ex_prospcnt	0x198	/* 408 - int - Number of Prospective/Scout Series */
#define _EX_ex_prosp	0x19c	/* 412 - VARTYPE - Prospective/Scout Series Keys for this Exam */
#define _EX_ex_modelnum 0x1a4   /* 420 - int - Last Model Number used */
#define _EX_ex_modelcnt 0x1a8   /* 424 - int - Number of ThreeD Models */
#define _EX_ex_models   0x1ac   /* 428 - VARTYPE - ThreeD Model Keys for Exam */
#define _EX_ex_stat     0x1b4   /* 436 - short - Patient Status */
#define _EX_uniq_sys_id 0x1b6   /* 438 - char[16] - Unique System ID */
#define _EX_service_id  0x1c6   /* 454 - char[16] - Unique Service ID */
#define _EX_mobile_loc  0x1d6   /* 470 - char[4] - Mobile Location Number */
#define _EX_ex_padding  0x1da   /* 474 - char[550] - Spare Space */
#define _EX_sizeof      0x400   /* 1024 - sizeof EXAMDATATYPE */
 
/*  SERIESDATATYPE definitions  */
 
#define _SE_se_suid     0x0     /* 0 - char[4] - Suite ID for this Series */
#define _SE_se_uniq     0x4     /* 4 - short - The Make-Unique Flag */
#define _SE_se_diskid   0x6     /* 6 - char[1] - Disk ID for this Series */
#define _SE_se_exno     0x8     /* 8 - u_short - Exam Number */
#define _SE_se_no       0xa     /* 10 - short - Series Number */
#define _SE_se_datetime 0xc     /* 12 - int - Allocation Series Data/Time stamp */
#define _SE_se_actual_dt        0x10    /* 16 - int - Actual Series Data/Time stamp */
#define _SE_se_desc     0x14    /* 20 - char[30] - Series Description */
#define _SE_pr_sysid    0x32    /* 50 - char[9] - Primary Receiver Suite and Host */
#define _SE_pansysid    0x3b    /* 59 - char[9] - Archiver Suite and Host */
#define _SE_se_typ      0x44    /* 68 - short - Series Type */
#define _SE_se_source   0x46    /* 70 - short - Series from which prescribed */
#define _SE_se_plane    0x48    /* 72 - short - Most-like Plane (for L/S) */
#define _SE_scan_type   0x4a    /* 74 - short - Scout or Axial (for CT) */
#define _SE_position    0x4c    /* 76 - int - Patient Position */
#define _SE_entry       0x50    /* 80 - int - Patient Entry */
#define _SE_anref       0x54    /* 84 - char[3] - Anatomical reference */
#define _SE_lmhor       0x58    /* 88 - float - Horizontal Landmark */
#define _SE_prtcl       0x5c    /* 92 - char[25] - Scan Protocol Name */
#define _SE_se_contrast 0x76    /* 118 - short - Non-zero if > 0 image used contrast(L/S) */
#define _SE_start_ras   0x78    /* 120 - char[1] - RAS letter for first scan location (L/S) */
#define _SE_start_loc   0x7a    /* 122 - float - First scan location (L/S) */
#define _SE_end_ras     0x7e    /* 126 - char[1] - RAS letter for last scan location (L/S) */
#define _SE_end_loc     0x80    /* 128 - float - Last scan location (L/S) */
#define _SE_se_pseq     0x84    /* 132 - short - Last Pulse Sequence Used (L/S) */
#define _SE_se_sortorder        0x86    /* 134 - short - Image Sort Order (L/S) */
#define _SE_se_lndmrkcnt        0x88    /* 136 - int - Landmark Counter */
#define _SE_se_nacq     0x8c    /* 140 - short - Number of Acquisitions */
#define _SE_xbasest     0x8e    /* 142 - short - Starting number for baselines */
#define _SE_xbaseend    0x90    /* 144 - short - Ending number for baselines */
#define _SE_xenhst      0x92    /* 146 - short - Starting number for enhanced scans */
#define _SE_xenhend     0x94    /* 148 - short - Ending number for enhanced scans */
#define _SE_se_lastmod  0x96    /* 150 - int - Date/Time of Last Change */
#define _SE_se_alloc_key        0x9a    /* 154 - char[13] - Process that allocated this record */
#define _SE_se_delta_cnt        0xa8    /* 168 - int - Indicates number of updates to header */
#define _SE_se_verscre  0xac    /* 172 - char[2] - Genesis Version - Created */
#define _SE_se_verscur  0xae    /* 174 - char[2] - Genesis Version - Now */
#define _SE_se_pds_a    0xb0    /* 176 - float - PixelData size - as stored */
#define _SE_se_pds_c    0xb4    /* 180 - float - PixelData size - Compressed */
#define _SE_se_pds_u    0xb8    /* 184 - float - PixelData size - UnCompressed */
#define _SE_se_checksum 0xbc    /* 188 - u_int - Series Record checksum */
#define _SE_se_complete 0xc0    /* 192 - int - Series Complete Flag */
#define _SE_se_numarch  0xc4    /* 196 - int - Number of Images Archived */
#define _SE_se_imagect  0xc8    /* 200 - int - Last Image Number Used */
#define _SE_se_numimages        0xcc    /* 204 - int - Number of Images Existing */
#define _SE_se_images   0xd0    /* 208 - VARTYPE - Image Keys for this Series */
#define _SE_se_numunimg 0xd8    /* 216 - int - Number of Unstored Images */
#define _SE_se_unimages 0xdc    /* 220 - VARTYPE - Unstored Image Keys for this Series */
#define _SE_se_toarchcnt        0xe4    /* 228 - int - Number of Unarchived Images */
#define _SE_se_toarchive        0xe8    /* 232 - VARTYPE - Unarchived Image Keys for this Series */
#define _SE_echo1_alpha 0xf0    /* 240 - float - Echo 1 Alpha Value */
#define _SE_echo1_beta  0xf4    /* 244 - float - Echo 1 Beta Value */
#define _SE_echo1_window        0xf8    /* 248 - u_short - Echo 1 Window Value */
#define _SE_echo1_level 0xfa    /* 250 - short - Echo 1 Level Value */
#define _SE_echo2_alpha 0xfc    /* 252 - float - Echo 2 Alpha Value */
#define _SE_echo2_beta  0x100   /* 256 - float - Echo 2 Beta Value */
#define _SE_echo2_window        0x104   /* 260 - u_short - Echo 2 Window Value */
#define _SE_echo2_level 0x106   /* 262 - short - Echo 2 Level Value */
#define _SE_echo3_alpha 0x108   /* 264 - float - Echo 3 Alpha Value */
#define _SE_echo3_beta  0x10c   /* 268 - float - Echo 3 Beta Value */
#define _SE_echo3_window        0x110   /* 272 - u_short - Echo 3 Window Value */
#define _SE_echo3_level 0x112   /* 274 - short - Echo 3 Level Value */
#define _SE_echo4_alpha 0x114   /* 276 - float - Echo 4 Alpha Value */
#define _SE_echo4_beta  0x118   /* 280 - float - Echo 4 Beta Value */
#define _SE_echo4_window        0x11c   /* 284 - u_short - Echo 4 Window Value */
#define _SE_echo4_level 0x11e   /* 286 - short - Echo 4 Level Value */
#define _SE_echo5_alpha 0x120   /* 288 - float - Echo 5 Alpha Value */
#define _SE_echo5_beta  0x124   /* 292 - float - Echo 5 Beta Value */
#define _SE_echo5_window        0x128   /* 296 - u_short - Echo 5 Window Value */
#define _SE_echo5_level 0x12a   /* 298 - short - Echo 5 Level Value */
#define _SE_echo6_alpha 0x12c   /* 300 - float - Echo 6 Alpha Value */
#define _SE_echo6_beta  0x130   /* 304 - float - Echo 6 Beta Value */
#define _SE_echo6_window        0x134   /* 308 - u_short - Echo 6 Window Value */
#define _SE_echo6_level 0x136   /* 310 - short - Echo 6 Level Value */
#define _SE_echo7_alpha 0x138   /* 312 - float - Echo 7 Alpha Value */
#define _SE_echo7_beta  0x13c   /* 316 - float - Echo 7 Beta Value */
#define _SE_echo7_window        0x140   /* 320 - u_short - Echo 7 Window Value */
#define _SE_echo7_level 0x142   /* 322 - short - Echo 7 Level Value */
#define _SE_echo8_alpha 0x144   /* 324 - float - Echo 8 Alpha Value */
#define _SE_echo8_beta  0x148   /* 328 - float - Echo 8 Beta Value */
#define _SE_echo8_window        0x14c   /* 332 - u_short - Echo 8 Window Value */
#define _SE_echo8_level 0x14e   /* 334 - short - Echo 8 Level Value */
#define _SE_se_padding  0x150   /* 336 - char[684] - Spare Space */
#define _SE_sizeof 0x3fc        /* 1020 - sizeof - sizeof SERIESDATATYPE */
 
/*  MRIMAGEDATATYPE definitions  */
 
#define _MR_im_suid     0x0     /* 0 - char[4] - Suite id for this image */
#define _MR_im_uniq     0x4     /* 4 - short - The Make-Unique Flag */
#define _MR_im_diskid   0x6     /* 6 - char[1] - Disk ID for this Image */
#define _MR_im_exno     0x8     /* 8 - u_short - Exam number for this image */
#define _MR_im_seno     0xa     /* 10 - short - Series Number for this image */
#define _MR_im_no       0xc     /* 12 - short - Image Number */
#define _MR_im_datetime 0xe     /* 14 - int - Allocation Image date/time stamp */
#define _MR_im_actual_dt        0x12    /* 18 - int - Actual Image date/time stamp */
#define _MR_sctime      0x16    /* 22 - float - Duration of scan (secs) */
#define _MR_slthick     0x1a    /* 26 - float - Slice Thickness (mm) */
#define _MR_imatrix_X   0x1e    /* 30 - short - Image matrix size - X */
#define _MR_imatrix_Y   0x20    /* 32 - short - Image matrix size - Y */
#define _MR_dfov        0x22    /* 34 - float - Display field of view - X (mm) */
#define _MR_dfov_rect   0x26    /* 38 - float - Display field of view - Y (if different) */
#define _MR_dim_X       0x2a    /* 42 - float - Image dimension - X */
#define _MR_dim_Y       0x2e    /* 46 - float - Image dimension - Y */
#define _MR_pixsize_X   0x32    /* 50 - float - Image pixel size - X */
#define _MR_pixsize_Y   0x36    /* 54 - float - Image pixel size - Y */
#define _MR_pdid        0x3a    /* 58 - char[14] - Pixel Data ID */
#define _MR_contrastIV  0x48    /* 72 - char[17] - IV Contrast Agent */
#define _MR_contrastOral        0x59    /* 89 - char[17] - Oral Contrast Agent */
#define _MR_contmode    0x6a    /* 106 - short - Image Contrast Mode */
#define _MR_serrx       0x6c    /* 108 - short - Series from which prescribed */
#define _MR_imgrx       0x6e    /* 110 - short - Image from which prescribed */
#define _MR_screenformat        0x70    /* 112 - short - Screen Format(8/16 bit) */
#define _MR_plane       0x72    /* 114 - short - Plane Type */
#define _MR_scanspacing 0x74    /* 116 - float - Spacing between scans (mm?) */
#define _MR_im_compress 0x78    /* 120 - short - Image compression type for allocation */
#define _MR_im_scouttype        0x7a    /* 122 - short - Scout Type (AP or lateral) */#define _MR_loc_ras     0x7c    /* 124 - char[] - RAS letter of image location */
#define _MR_loc 0x7e    /* 126 - float - Image location */
#define _MR_ctr_R       0x82    /* 130 - float - Center R coord of plane image */
#define _MR_ctr_A       0x86    /* 134 - float - Center A coord of plane image */
#define _MR_ctr_S       0x8a    /* 138 - float - Center S coord of plane image */
#define _MR_norm_R      0x8e    /* 142 - float - Normal R coord */
#define _MR_norm_A      0x92    /* 146 - float - Normal A coord */
#define _MR_norm_S      0x96    /* 150 - float - Normal S coord */
#define _MR_tlhc_R      0x9a    /* 154 - float - R Coord of Top Left Hand Corner */
#define _MR_tlhc_A      0x9e    /* 158 - float - A Coord of Top Left Hand Corner */
#define _MR_tlhc_S      0xa2    /* 162 - float - S Coord of Top Left Hand Corner */
#define _MR_trhc_R      0xa6    /* 166 - float - R Coord of Top Right Hand Corner */
#define _MR_trhc_A      0xaa    /* 170 - float - A Coord of Top Right Hand Corner */
#define _MR_trhc_S      0xae    /* 174 - float - S Coord of Top Right Hand Corner */
#define _MR_brhc_R      0xb2    /* 178 - float - R Coord of Bottom Right Hand Corner */
#define _MR_brhc_A      0xb6    /* 182 - float - A Coord of Bottom Right Hand Corner */
#define _MR_brhc_S      0xba    /* 186 - float - S Coord of Bottom Right Hand Corner */
#define _MR_forimgrev   0xbe    /* 190 - char[4] - Foreign Image Revision */
#define _MR_tr  0xc2    /* 194 - int - Pulse repetition time(usec) */
#define _MR_ti  0xc6    /* 198 - int - Pulse inversion time(usec) */
#define _MR_te  0xca    /* 202 - int - Pulse echo time(usec) */
#define _MR_te2 0xce    /* 206 - int - Second echo echo (usec) */
#define _MR_numecho     0xd2    /* 210 - short - Number of echoes */
#define _MR_echonum     0xd4    /* 212 - short - Echo Number */
#define _MR_tbldlta     0xd6    /* 214 - float - Table Delta */
#define _MR_nex 0xda    /* 218 - float - Number of Excitations */
#define _MR_contig      0xde    /* 222 - short - Continuous Slices Flag */
#define _MR_hrtrate     0xe0    /* 224 - short - Cardiac Heart Rate (bpm) */
#define _MR_tdel        0xe2    /* 226 - int - Delay time after trigger (msec) */
#define _MR_saravg      0xe6    /* 230 - float - Average SAR */
#define _MR_sarpeak     0xea    /* 234 - float - Peak SAR */
#define _MR_monsar      0xee    /* 238 - short - Monitor SAR flag */
#define _MR_trgwindow   0xf0    /* 240 - short - Trigger window (% of R-R interval) */
#define _MR_reptime     0xf2    /* 242 - float - Cardiac repetition time */
#define _MR_imgpcyc     0xf6    /* 246 - short - Images per cardiac cycle */
#define _MR_xmtgain     0xf8    /* 248 - short - Actual Transmit Gain (.1 db) */
#define _MR_rcvgain1    0xfa    /* 250 - short - Actual Receive Gain Analog (.1 db) */
#define _MR_rcvgain2    0xfc    /* 252 - short - Actual Receive Gain Digital (.1 db) */
#define _MR_mr_flip     0xfe    /* 254 - short - Flip Angle for GRASS scans (deg.) */
#define _MR_mindat      0x100   /* 256 - int - Minimum Delay after Trigger (uSec) */
#define _MR_cphase      0x104   /* 260 - short - Total Cardiac Phase prescribed */
#define _MR_swappf      0x106   /* 262 - short - Swap Phase/Frequency Axis */
#define _MR_pauseint    0x108   /* 264 - short - Pause Interval (slices) */
#define _MR_pausetime   0x10a   /* 266 - float - Pause Time */
#define _MR_obplane     0x10e   /* 270 - int - Oblique Plane */
#define _MR_slocfov     0x112   /* 274 - int - Slice Offsets on Freq axis */
#define _MR_xmtfreq     0x116   /* 278 - int - Center Frequency (0.1 Hz) */
#define _MR_autoxmtfreq 0x11a   /* 282 - int - Auto Center Frequency (0.1 Hz) */
#define _MR_autoxmtgain 0x11e   /* 286 - short - Auto Transmit Gain (0.1 dB) */
#define _MR_prescan_r1  0x120   /* 288 - short - PreScan R1 - Analog */
#define _MR_prescan_r2  0x122   /* 290 - short - PreScan R2 - Digital */
#define _MR_user_bitmap 0x124   /* 292 - int - Bitmap defining user CVs */
#define _MR_cenfreq     0x128   /* 296 - short - Center Frequency Method */
#define _MR_imode       0x12a   /* 298 - short - Imaging Mode */
#define _MR_iopt        0x12c   /* 300 - int - Imaging Options */
#define _MR_pseq        0x130   /* 304 - short - Pulse Sequence */
#define _MR_pseqmode    0x132   /* 306 - short - Pulse Sequence Mode */
#define _MR_psdname     0x134   /* 308 - char[33] - Pulse Sequence Name */
#define _MR_psd_datetime        0x156   /* 342 - int - PSD Creation Date and Time */
#define _MR_psd_iname   0x15a   /* 346 - char[13] - PSD name from inside PSD */
#define _MR_ctyp        0x168   /* 360 - short - Coil Type */
#define _MR_cname       0x16a   /* 362 - char[17] - Coil Name */
#define _MR_surfctyp    0x17c   /* 380 - short - Surface Coil Type */
#define _MR_surfcext    0x17e   /* 382 - short - Extremity Coil Flag */
#define _MR_rawrunnum   0x180   /* 384 - int - RawData Run Number */
#define _MR_cal_fldstr  0x184   /* 388 - u_int - Calibrated Field Strength (x10 uGauss) */
#define _MR_supp_tech   0x188   /* 392 - short - SAT fat/water/none */
#define _MR_vbw 0x18a   /* 394 - float - Variable Bandwidth (Hz) */
#define _MR_slquant     0x18e   /* 398 - short - Number of slices in this scan group */
#define _MR_gpre        0x190   /* 400 - short - Graphically prescribed */
#define _MR_intr_del    0x192   /* 402 - int - Interimage/interloc delay (uSec) */
#define _MR_user0       0x196   /* 406 - float - User Variable 0 */
#define _MR_user1       0x19a   /* 410 - float - User Variable 1 */
#define _MR_user2       0x19e   /* 414 - float - User Variable 2 */
#define _MR_user3       0x1a2   /* 418 - float - User Variable 3 */
#define _MR_user4       0x1a6   /* 422 - float - User Variable 4 */
#define _MR_user5       0x1aa   /* 426 - float - User Variable 5 */
#define _MR_user6       0x1ae   /* 430 - float - User Variable 6 */
#define _MR_user7       0x1b2   /* 434 - float - User Variable 7 */
#define _MR_user8       0x1b6   /* 438 - float - User Variable 8 */
#define _MR_user9       0x1ba   /* 442 - float - User Variable 9 */
#define _MR_user10      0x1be   /* 446 - float - User Variable 10 */
#define _MR_user11      0x1c2   /* 450 - float - User Variable 11 */
#define _MR_user12      0x1c6   /* 454 - float - User Variable 12 */
#define _MR_user13      0x1ca   /* 458 - float - User Variable 13 */
#define _MR_user14      0x1ce   /* 462 - float - User Variable 14 */
#define _MR_user15      0x1d2   /* 466 - float - User Variable 15 */
#define _MR_user16      0x1d6   /* 470 - float - User Variable 16 */
#define _MR_user17      0x1da   /* 474 - float - User Variable 17 */
#define _MR_user18      0x1de   /* 478 - float - User Variable 18 */
#define _MR_user19      0x1e2   /* 482 - float - User Variable 19 */
#define _MR_user20      0x1e6   /* 486 - float - User Variable 20 */
#define _MR_user21      0x1ea   /* 490 - float - User Variable 21 */
#define _MR_user22      0x1ee   /* 494 - float - User Variable 22 */
#define _MR_user23      0x1f2   /* 498 - float - Projection Angle */
#define _MR_user24      0x1f6   /* 502 - float - Concat Sat Type Flag */
#define _MR_im_alloc_key        0x1fa   /* 506 - char[13] - */
#define _MR_im_lastmod  0x208   /* 520 - int - Date/Time of Last Change */
#define _MR_im_verscre  0x20c   /* 524 - char[2] - Genesis Version - Created */
#define _MR_im_verscur  0x20e   /* 526 - char[2] - Genesis Version - Now */
#define _MR_im_pds_a    0x210   /* 528 - int - PixelData size - as stored */
#define _MR_im_pds_c    0x214   /* 532 - int - PixelData size - Compressed */
#define _MR_im_pds_u    0x218   /* 536 - int - PixelData size - UnCompressed */
#define _MR_im_checksum 0x21c   /* 540 - u_int - AcqRecon record checksum */
#define _MR_im_archived 0x220   /* 544 - int - Image Archive Flag */
#define _MR_im_complete 0x224   /* 548 - int - Image Complete Flag */
#define _MR_satbits     0x228   /* 552 - short - Bitmap of SAT selections */
#define _MR_scic        0x22a   /* 554 - short - Surface Coil Intensity Correction Flag */
#define _MR_satxloc1    0x22c   /* 556 - short - R-side SAT pulse loc rel to lndmrk */
#define _MR_satxloc2    0x22e   /* 558 - short - L-side SAT pulse loc rel to lndmrk */
#define _MR_satyloc1    0x230   /* 560 - short - A-side SAT pulse loc rel to lndmrk */
#define _MR_satyloc2    0x232   /* 562 - short - P-side SAT pulse loc rel to lndmrk */
#define _MR_satzloc1    0x234   /* 564 - short - S-side SAT pulse loc rel to lndmrk */
#define _MR_satzloc2    0x236   /* 566 - short - I-side SAT pulse loc rel to lndmrk */
#define _MR_satxthick   0x238   /* 568 - short - Thickness of X-axis SAT pulse */
#define _MR_satythick   0x23a   /* 570 - short - Thickness of Y-axis SAT pulse */
#define _MR_satzthick   0x23c   /* 572 - short - Thickness of Z-axis SAT pulse */
#define _MR_flax        0x23e   /* 574 - short - Phase contrast flow axis */
#define _MR_venc        0x240   /* 576 - short - Phase contrast velocity encoding */
#define _MR_thk_disclmr 0x242   /* 578 - short - Slice Thickness */
#define _MR_ps_flag     0x244   /* 580 - short - Auto/Manual Prescan flag */
#define _MR_ps_status   0x246   /* 582 - short - Bitmap of changed values */
#define _MR_image_type  0x248   /* 584 - short - Magnitude, Phase, Imaginary, or Real */
#define _MR_vas_collapse        0x24a   /* 586 - short - Collapse Image */
#define _MR_user23n     0x24c   /* 588 - float - User Variable 23 */
#define _MR_user24n     0x250   /* 592 - float - User Variable 24 */
#define _MR_proj_alg    0x254   /* 596 - short - Projection Algorithm */
#define _MR_proj_name   0x256   /* 598 - char[13] - Projection Algorithm Name */
#define _MR_x_axis_rot  0x264   /* 612 - float - X Axis Rotation */
#define _MR_y_axis_rot  0x268   /* 616 - float - Y Axis Rotation */
#define _MR_z_axis_rot  0x26c   /* 620 - float - Z Axis Rotation */
#define _MR_thresh_min1 0x270   /* 624 - int - Lower Range of Pixels 1 */
#define _MR_thresh_max1 0x274   /* 628 - int - Upper Range of Pixels 1 */
#define _MR_thresh_min2 0x278   /* 632 - int - Lower Range of Pixels 2 */
#define _MR_thresh_max2 0x27c   /* 636 - int - Upper Range of Pixels 2 */
#define _MR_echo_trn_len        0x280   /* 640 - short - Echo Train Length for Fast Spin Echo */
#define _MR_frac_echo   0x282   /* 642 - short - Fractional Echo - Effective TE Flag */
#define _MR_prep_pulse  0x284   /* 644 - short - Preporatory Pulse Option */
#define _MR_cphasenum   0x286   /* 646 - short - Cardiac Phase Number */
#define _MR_var_echo    0x288   /* 648 - short - Variable Echo Flag */
#define _MR_ref_img     0x28a   /* 650 - char[1] - Reference Image Field */
#define _MR_sum_img     0x28b   /* 651 - char[1] - Summary Image Field */
#define _MR_img_window  0x28c   /* 652 - u_short - Window Value */
#define _MR_img_level   0x28e   /* 654 - short - Level Value */
#define _MR_slop_int_1  0x290   /* 656 - int - Integer Slop Field 1 */
#define _MR_slop_int_2  0x294   /* 660 - int - Integer Slop Field 2 */
#define _MR_slop_int_3  0x298   /* 664 - int - Integer Slop Field 3 */
#define _MR_slop_int_4  0x29c   /* 668 - int - Integer Slop Field 4 */
#define _MR_slop_int_5  0x2a0   /* 672 - int - Integer Slop Field 5 */
#define _MR_slop_float_1        0x2a4   /* 676 - float - Float Slop Field 1 */
#define _MR_slop_float_2        0x2a8   /* 680 - float - Float Slop Field 2 */
#define _MR_slop_float_3        0x2ac   /* 684 - float - Float Slop Field 3 */
#define _MR_slop_float_4        0x2b0   /* 688 - float - Float Slop Field 4 */
#define _MR_slop_float_5        0x2b4   /* 692 - float - Float Slop Field 5 */
#define _MR_slop_str_1  0x2b8   /* 696 - char[16] - String Slop Field 1 */
#define _MR_slop_str_2  0x2c8   /* 712 - char[16] - String Slop Field 2 */
#define _MR_mr_padding  0x2d8   /* 728 - char[294] - Spare Space */
#define _MR_MRIMAGEDATATYPE     0x3fe   /* 1022 - sizeof - sizeof MRIMAGEDATATYPE */
 
struct GEMR_Header {
	int tag;
	struct	{
		void	*im;	/* main img hdr */
		void	*su;	/* suite header */
		void	*ex;	/* exam header */
		void	*se;	/* series header */
		void	*mr;	/* mr image header */
		void	*up;	/* unpack table */
	} p;
	union {
		char	raw[128];
		union {
			short	s[28*256];
			char	c[128 * 1024];	// extra buffer 
		} ge4x;
		struct	GemrImgf_Img	ge5x;
	} u;

	int read_header(FILE*);

	char * get_str_se_date(void);
	char * get_str_st_date(void);
	char * get_str_se_desc(void);
	char * get_str_st_desc(void);
	char * get_str_pt_name(void);
	char * get_str_hosp(void);
	char * get_str_se_protocol(void);
	
	int get_int_pt_age(void);
	int get_int_pt_sex(void);
	int get_int_pt_weight(void);
	int get_int_mr_xdim(void);
	int get_int_mr_ydim(void);

	float get_float_mr_thick(void);
	float get_float_mr_fov(void);
	float get_float_mr_psize(void);

};


