/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*  
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * This file is automatically generated by buildiface 
 * DO NOT EDIT
 */
#include "mpi_fortimpl.h"


/* Begin MPI profiling block */
#if defined(USE_WEAK_SYMBOLS) && !defined(USE_ONLY_MPI_NAMES) 
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak MPI_TYPE_CONTIGUOUS = PMPI_TYPE_CONTIGUOUS
#pragma weak mpi_type_contiguous__ = PMPI_TYPE_CONTIGUOUS
#pragma weak mpi_type_contiguous_ = PMPI_TYPE_CONTIGUOUS
#pragma weak mpi_type_contiguous = PMPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_TYPE_CONTIGUOUS = pmpi_type_contiguous__
#pragma weak mpi_type_contiguous__ = pmpi_type_contiguous__
#pragma weak mpi_type_contiguous_ = pmpi_type_contiguous__
#pragma weak mpi_type_contiguous = pmpi_type_contiguous__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_TYPE_CONTIGUOUS = pmpi_type_contiguous_
#pragma weak mpi_type_contiguous__ = pmpi_type_contiguous_
#pragma weak mpi_type_contiguous_ = pmpi_type_contiguous_
#pragma weak mpi_type_contiguous = pmpi_type_contiguous_
#else
#pragma weak MPI_TYPE_CONTIGUOUS = pmpi_type_contiguous
#pragma weak mpi_type_contiguous__ = pmpi_type_contiguous
#pragma weak mpi_type_contiguous_ = pmpi_type_contiguous
#pragma weak mpi_type_contiguous = pmpi_type_contiguous
#endif



#elif defined(HAVE_PRAGMA_WEAK)

#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak MPI_TYPE_CONTIGUOUS = PMPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_type_contiguous__ = pmpi_type_contiguous__
#elif !defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_type_contiguous = pmpi_type_contiguous
#else
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_type_contiguous_ = pmpi_type_contiguous_
#endif

#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#if defined(F77_NAME_UPPER)
#pragma _HP_SECONDARY_DEF PMPI_TYPE_CONTIGUOUS  MPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _HP_SECONDARY_DEF pmpi_type_contiguous__  mpi_type_contiguous__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _HP_SECONDARY_DEF pmpi_type_contiguous  mpi_type_contiguous
#else
#pragma _HP_SECONDARY_DEF pmpi_type_contiguous_  mpi_type_contiguous_
#endif

#elif defined(HAVE_PRAGMA_CRI_DUP)
#if defined(F77_NAME_UPPER)
#pragma _CRI duplicate MPI_TYPE_CONTIGUOUS as PMPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _CRI duplicate mpi_type_contiguous__ as pmpi_type_contiguous__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _CRI duplicate mpi_type_contiguous as pmpi_type_contiguous
#else
#pragma _CRI duplicate mpi_type_contiguous_ as pmpi_type_contiguous_
#endif

#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));

#else
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));

#endif
#endif /* HAVE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */
/* End MPI profiling block */


/* These definitions are used only for generating the Fortran wrappers */
#if defined(USE_WEAK_SYMBOLS) && defined(USE_ONLY_MPI_NAMES)
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak mpi_type_contiguous__ = MPI_TYPE_CONTIGUOUS
#pragma weak mpi_type_contiguous_ = MPI_TYPE_CONTIGUOUS
#pragma weak mpi_type_contiguous = MPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_TYPE_CONTIGUOUS = mpi_type_contiguous__
#pragma weak mpi_type_contiguous_ = mpi_type_contiguous__
#pragma weak mpi_type_contiguous = mpi_type_contiguous__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_TYPE_CONTIGUOUS = mpi_type_contiguous_
#pragma weak mpi_type_contiguous__ = mpi_type_contiguous_
#pragma weak mpi_type_contiguous = mpi_type_contiguous_
#else
#pragma weak MPI_TYPE_CONTIGUOUS = mpi_type_contiguous
#pragma weak mpi_type_contiguous__ = mpi_type_contiguous
#pragma weak mpi_type_contiguous_ = mpi_type_contiguous
#endif
#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_TYPE_CONTIGUOUS")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous_")));

#else
extern FORT_DLL_SPEC void FORT_CALL MPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif
#endif

#endif

/* Map the name to the correct form */
#ifndef MPICH_MPI_FROM_PMPI
#if defined(USE_WEAK_SYMBOLS)
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
/* Define the weak versions of the PMPI routine*/
#ifndef F77_NAME_UPPER
extern FORT_DLL_SPEC void FORT_CALL PMPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_2USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif

#if defined(F77_NAME_UPPER)
#pragma weak pmpi_type_contiguous__ = PMPI_TYPE_CONTIGUOUS
#pragma weak pmpi_type_contiguous_ = PMPI_TYPE_CONTIGUOUS
#pragma weak pmpi_type_contiguous = PMPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak PMPI_TYPE_CONTIGUOUS = pmpi_type_contiguous__
#pragma weak pmpi_type_contiguous_ = pmpi_type_contiguous__
#pragma weak pmpi_type_contiguous = pmpi_type_contiguous__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak PMPI_TYPE_CONTIGUOUS = pmpi_type_contiguous_
#pragma weak pmpi_type_contiguous__ = pmpi_type_contiguous_
#pragma weak pmpi_type_contiguous = pmpi_type_contiguous_
#else
#pragma weak PMPI_TYPE_CONTIGUOUS = pmpi_type_contiguous
#pragma weak pmpi_type_contiguous__ = pmpi_type_contiguous
#pragma weak pmpi_type_contiguous_ = pmpi_type_contiguous
#endif /* Test on name mapping */

#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_TYPE_CONTIGUOUS")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL PMPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL PMPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous_")));

#else
extern FORT_DLL_SPEC void FORT_CALL PMPI_TYPE_CONTIGUOUS( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_type_contiguous_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_type_contiguous")));

#endif /* Test on name mapping */
#endif /* HAVE_MULTIPLE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */

#ifdef F77_NAME_UPPER
#define mpi_type_contiguous_ PMPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_type_contiguous_ pmpi_type_contiguous__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_type_contiguous_ pmpi_type_contiguous
#else
#define mpi_type_contiguous_ pmpi_type_contiguous_
#endif /* Test on name mapping */

#ifdef F77_USE_PMPI
/* This defines the routine that we call, which must be the PMPI version
   since we're renaming the Fortran entry as the pmpi version.  The MPI name
   must be undefined first to prevent any conflicts with previous renamings. */
#undef MPI_Type_contiguous
#define MPI_Type_contiguous PMPI_Type_contiguous 
#endif

#else

#ifdef F77_NAME_UPPER
#define mpi_type_contiguous_ MPI_TYPE_CONTIGUOUS
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_type_contiguous_ mpi_type_contiguous__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_type_contiguous_ mpi_type_contiguous
/* Else leave name alone */
#endif


#endif /* MPICH_MPI_FROM_PMPI */

/* Prototypes for the Fortran interfaces */
#include "fproto.h"
FORT_DLL_SPEC void FORT_CALL mpi_type_contiguous_ ( MPI_Fint *v1, MPI_Fint *v2, MPI_Fint *v3, MPI_Fint *ierr ){
    *ierr = MPI_Type_contiguous( (int)*v1, (MPI_Datatype)(*v2), (MPI_Datatype *)(v3) );
}
