
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid_rdb_client_h
= "@(#)$Id: rdb_client.h,v 1.1 1995/11/23 04:30:04 jaw Exp jaw $";
#endif


extern int rdb_initialize(void);
extern int rdb_create_patient_db(void);

#define MRI_RDB_PORT	12345

