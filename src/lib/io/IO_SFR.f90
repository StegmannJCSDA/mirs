!$Id:$
!-----------------------------------------------------------------------------------------------
! Name:         IO_SFR
! 
! Type:         F90 module
!
! Description:  This module is dedicated to the I/O of the SFR products.
!
!
! Modules needed:
!       - misc
!       - ErrorHandling
!
! Subroutines contained:
!       - ReadHdrSFR
!       - ReadSFR
!
! Data type included:
!       - SFR_type
!
! 
! History:
!       10-25-2014    Dr. Tanvir Islam, 
!                     NOAA/NESDIS/STAR & CIRA/CSU    Original Coder 
!
!-----------------------------------------------------------------------------------------------

MODULE IO_SFR
  USE misc
  USE ErrorHandling
  IMPLICIT NONE
  PRIVATE
  !---Publicly available subroutine
  PUBLIC :: ReadHdrSFR,ReadSFR,ReadSFR2
  !---Publicly available data/type definitions
  PUBLIC :: SFR_type,SFR_type2
  !---Declaration sections
  TYPE   :: SFR_type
    !---Positioning Data
    REAL                                       :: blat       !Latitude
    REAL                                       :: blon       !Longitude
    INTEGER                                    :: jday       !Day 
    REAL                                       :: sec        !UTC time
    !---SFR
    REAL                                       :: sfrr       !Snow Fall Rate
  END TYPE SFR_type
  !
  !---added three variables. 
  !
  TYPE   :: SFR_type2 
    !---Positioning Data
    REAL                                       :: blat       !Latitude
    REAL                                       :: blon       !Longitude
    INTEGER                                    :: jday       !Day 
    REAL                                       :: sec        !UTC time
    !---SFR
    REAL                                       :: sfrr       !Snow Fall Rate
    REAL                                       :: prob       !probability of snow
    INTEGER                                    :: flag0      !non-convergence flag, 1=not converged
    INTEGER                                    :: flag1      !1: SFR is too small (<0.05)
  END TYPE SFR_type2

CONTAINS


!===============================================================
! Name:         ReadHdrSFR
!
!
! Type:         Subroutine
!
!
! Description:  Opens a geophysical SFR file.
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - iu                 O              Unit number after opening
!       - InputFile          I              Name of the input file
!
!
! Modules needed:
!       - None
!
!
! History:
!       10-25-2014      Dr. Tanvir Islam, CIRA @ NOAA/NESDIS/STAR
!
!===============================================================

  SUBROUTINE ReadHdrSFR(iu,InputFile)
    CHARACTER(LEN=*)      :: InputFile
    INTEGER               :: iu
    !---Open file 
    iu=get_lun()
    OPEN(iu,file=InputFile,access='STREAM',form='unformatted',convert='little_endian')
    RETURN
  END SUBROUTINE ReadHdrSFR


!===============================================================
! Name:         ReadSFR
!
!
! Type:         Subroutine
!
!
! Description:  Reads content of a geophysical SFR file (Scan/Pos mode).
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - iu                 I              Unit number
!       - SFR                I/O            Structure to be filled 
!                                           with content of file
!       - ierr               O              Error index when reading
!
!
! Modules needed:
!       - None
!
!
! History:
!       10-25-2014      Dr. Tanvir Islam, CIRA @ NOAA/NESDIS/STAR
!
!===============================================================

  SUBROUTINE ReadSFR(iu,SFR,ierr)
    TYPE(SFR_type)   :: SFR
    INTEGER          :: iu,ierr
    READ(iu,iostat=ierr,end=10)
    IF (ierr.ne.0) THEN
       CALL ErrHandl(WarningType,Warn_readInvalid,'SFR invalid.')
       RETURN
    ENDIF
    READ(iu,err=20) SFR%jday
    READ(iu,err=20) SFR%sec
    READ(iu,err=20) SFR%blat
    READ(iu,err=20) SFR%blon
    READ(iu,err=20) SFR%sfrr
    RETURN
10  ierr=Warn_EndOfFile
    CALL ErrHandl(WarningType,Warn_EndOfFile,'Sfr') 
    RETURN
20  ierr=Warn_readInvalid
    CALL ErrHandl(WarningType,Warn_readInvalid,'(ReadSFR)')
    RETURN
  END SUBROUTINE ReadSFR

!===============================================================
! Name:         ReadSFR2
!
!
! Type:         Subroutine
!
!
! Description:  Reads content of a geophysical SFR file (Scan/Pos mode).
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - iu                 I              Unit number
!       - SFR                I/O            Structure to be filled 
!                                           with content of file
!       - prob               I/O            Structure to be filled 
!                                           with content of file
!       - flag               I/O            Structure to be filled 
!                                           with content of file
!       - ierr               O              Error index when reading
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-08-2019      Shuyan Liu, CIRA @ NOAA/NESDIS/STAR
!
!
!===============================================================
!
  SUBROUTINE ReadSFR2(iu,SFR,ierr)
    TYPE(SFR_type2)   :: SFR
    INTEGER               :: iu,ierr
    INTEGER(KIND=4)       :: flag

    READ(iu,iostat=ierr,end=10)
    IF (ierr.ne.0) THEN
       CALL ErrHandl(WarningType,Warn_readInvalid,'SFR invalid.')
       RETURN
    ENDIF
    READ(iu,err=20) SFR%jday
    READ(iu,err=20) SFR%sec
    READ(iu,err=20) SFR%blat
    READ(iu,err=20) SFR%blon
    READ(iu,err=20) SFR%sfrr
    READ(iu,err=20) SFR%prob
    READ(iu,err=20) flag

    SFR%flag0  = IBITS(flag,0,1)
    SFR%flag1  = IBITS(flag,1,1)

    RETURN
10  ierr=Warn_EndOfFile
    CALL ErrHandl(WarningType,Warn_EndOfFile,'Sfr') 
    RETURN
20  ierr=Warn_readInvalid
    CALL ErrHandl(WarningType,Warn_readInvalid,'(ReadSFR2)')
    RETURN
  END SUBROUTINE ReadSFR2



END MODULE IO_SFR
