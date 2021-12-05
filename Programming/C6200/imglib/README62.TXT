;* ======================================================================== *;
;*  TEXAS INSTRUMENTS, INC.                                                 *;
;*                                                                          *;
;*  C62xIMGLIB  DSP Signal Processing Library                               *;
;*                                                                          *;
;*      Release:        Version 1.04b                                       *;
;*      Snapshot date:  09-Oct-2003                                         *;
;*                                                                          *;
;*  This library contains proprietary intellectual property of Texas        *;
;*  Instruments, Inc.  The library and its source code are protected by     *;
;*  various copyrights, and portions may also be protected by patents or    *;
;*  other legal protections.                                                *;
;*                                                                          *;
;*  This software is licensed for use with Texas Instruments TMS320         *;
;*  family DSPs.  This license was provided to you prior to installing      *;
;*  the software.  You may review this license by consulting the file       *;
;*  TI_license.PDF which accompanies the files in this library.             *;
;* ------------------------------------------------------------------------ *;
;*          Copyright (C) 2003 Texas Instruments, Incorporated.             *;
;*                          All Rights Reserved.                            *;
;* ======================================================================== *;


This file contains a brief description of the Imaging Library and how
to use it.

Other Related Documentation Located in ..\DOCS folder:
    
    spru400b.pdf - Imaging API User Guide

Currently, the Imaging Library is provided as a source archive, as well
as a Little Endian C6000 object library with header files.

--------------------------------------------------------------------------------
Release Directory Structure

-----+-----lib
     |
     +-----include
     |
     +-----examples

lib      - contains the library files (.lib) and the source code (.src)
include  - contains all of the library header files
examples - contains examples

--------------------------------------------------------------------------------
Using the Imaging Library

There are really two ways the Imaging Library can be used, one is from
within Code Composer Studio and the other is by using the command line
tools. In both cases, it's a matter of including the library header
file, linking to the correct build of the library, and calling the
APIs.

-------------------------------------------------
Using the Imaging Library in Code Composer Studio

1. First you have to specify to the linker to link the Imaging Library
   into your application.  You do this by going to
   "Project->Options->Linker->Include Libraries" and then specify the
   Imaging Library, img62x.lib.

2. For each of your C files that you plan to use an Imaging Library
   kernel in, you must include the appropriate header file for each
   kernel.  For instance, to use the kernel "IMG_fdct_8x8()", you must
   #include <img_fdct_8x8.h> in that file.

3. Now you may use the Imaging Library API at will.


-----------------------------------------------------
Using the Imaging Library with the Command Line Tools

1. You have to tell the linker to link your application with the
   Imaging library.  You can do this by using the linker -l option in
   your linker command file or at the linker command line, i.e
   -limg62x.lib

2. For each of your C files that you plan to use an Imaging Library
   kernel in, you must include the appropriate header file for each
   kernel.  For instance, to use the kernel "IMG_fdct_8x8()", you must
   #include <img_fdct_8x8.h> in that file.


--------------------------------------------------------------------------------
How do I Re-Build the Library?

All of the Imaging Library source code is archived in the file img62x.src
located in the LIB folder.  This source archive may be rebuilt using
mk6x as follows:

    mk6x img62x.src -mv6200 -l img62x.lib

********************************************************************************
* End of README.TXT
********************************************************************************
