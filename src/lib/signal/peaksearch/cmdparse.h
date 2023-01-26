/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     QUIT = 258,
     PLOT = 259,
     PRINT = 260,
     DATA = 261,
     NORM = 262,
     SMOOTH = 263,
     LOAD = 264,
     TO = 265,
     RESCALE = 266,
     MARKED = 267,
     PEAKS = 268,
     PEAK = 269,
     ANNOTATE = 270,
     DRIVER = 271,
     ROTATED = 272,
     SORTED = 273,
     BY = 274,
     NUMBER = 275,
     HEIGHT = 276,
     AREA = 277,
     CALIBRATE = 278,
     USING = 279,
     CLEAR = 280,
     CALIBRATION = 281,
     CALIBRATED = 282,
     CHANNEL = 283,
     STRING = 284,
     NUM = 285
   };
#endif
#define QUIT 258
#define PLOT 259
#define PRINT 260
#define DATA 261
#define NORM 262
#define SMOOTH 263
#define LOAD 264
#define TO 265
#define RESCALE 266
#define MARKED 267
#define PEAKS 268
#define PEAK 269
#define ANNOTATE 270
#define DRIVER 271
#define ROTATED 272
#define SORTED 273
#define BY 274
#define NUMBER 275
#define HEIGHT 276
#define AREA 277
#define CALIBRATE 278
#define USING 279
#define CLEAR 280
#define CALIBRATION 281
#define CALIBRATED 282
#define CHANNEL 283
#define STRING 284
#define NUM 285




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 118 "cmdparse.y"
typedef union YYSTYPE {
	double num;
	char* str;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 102 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



