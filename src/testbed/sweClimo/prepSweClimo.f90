!$Id: $
!===============================================================
! Name:     prepSweClimo
!
!
! Type:         Main Program
!
!
! Description:
!       Program that accesses SWE gridded climatology
!       collocates the SWE values with Rad measurement file FOVs
!       and writes output to DEP format file for later use by vipp SWE algorithm
!
! Modules needed:
!       - Consts
!       - misc
!       - utils
!       - IO_Dep
!       - IO_MeasurData
!       - ErrorHandling
!
!
! History:
!       10-30-2014      C. Grassotti - NOAA/NESDIS/STAR
!
!===============================================================

Program prepSweClimo
  USE Consts
  USE misc  
  USE utils
  USE IO_Dep
  USE IO_MeasurData
  USE IO_Misc
  USE ErrorHandling
  IMPLICIT NONE
  !---INTRINSIC functions used
  INTRINSIC :: LOG,ABS,ANY,COS,COUNT,EXP,MAXVAL,MINVAL,TRIM

  INTEGER,            PARAMETER                     :: nqc=4
  INTEGER,            PARAMETER                     :: ncols_ease=721,nrows_ease=721
  INTEGER,            PARAMETER                     :: nmonths=12,nhem=2
  INTEGER                                           :: iu_list,nFiles,ifile
  INTEGER                                           :: nprofiles,iprof,iu,iuDep,ierr
  INTEGER                                           :: errReadingClimo,errReadingStdev
  TYPE(Dep_type)                                    :: Dep
  TYPE(MeasurData_type)                             :: Ym
  CHARACTER(LEN=250), DIMENSION(:), POINTER         :: RadMeasFiles,depFiles
  INTEGER,            DIMENSION(nqc)                :: qc
  !-- SWE Climatology-related variables
  REAL,DIMENSION(nrows_ease,ncols_ease,nmonths,nhem)   :: sweClimo,sweStdev
  REAL,DIMENSION(nrows_ease,ncols_ease,nmonths+1,nhem) :: sweClimo_p1,sweStdev_p1
  REAL,DIMENSION(nmonths+1)                            :: julianClimo=&
!       (/15.,45.,75.,105.,135.,165.,195.,225.,255.,285.,315.,345.,375./)
       (/15.,43.,74.,104.,135.,165.,196.,227.,257.,288.,318.,349.,380./)
  REAL                                              :: sweClimoColloc, sweStdevColloc
  REAL                                              :: sweClimoColloc1, sweClimoColloc2
  REAL                                              :: sweStdevColloc1, sweStdevColloc2
  REAL                                              :: w2, scanDay
  REAL                                              :: ir,ic
  INTEGER                                           :: imonth,iday,ir0,ic0,ihem,imon1,imon2
  !---Namelist data 
  INTEGER            :: nAlgors=DEFAULT_VALUE_INT
  INTEGER            :: sensor_id=DEFAULT_VALUE_INT
  INTEGER            :: nOrbits2Process=DEFAULT_VALUE_INT
  INTEGER            :: AlgSN=DEFAULT_VALUE_INT
  CHARACTER(LEN=250) :: ListRadMeasFiles=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: pathDep=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: sweClimoFile=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: sweStdevFile=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: LogFile=DEFAULT_VALUE_STR4

  NAMELIST /ContrlPrepSwe/ListRadMeasFiles,pathDep,LogFile,&
       sweClimoFile,sweStdevFile,sensor_id,nOrbits2Process,AlgSN

  !---Read control-data from namelist
  READ(*,NML=ContrlPrepSwe)
  !---Prepare Log file
  CALL OpenLogFile(Logfile)

  print *,'sweClimoFile=',sweClimoFile
  print *,'sweStdevFile=',sweStdevFile
  !---Read the file names of EC-FM- SDRs
  ! MXC call ReadList(iu_list,trim(ListRadMeasFiles),RadMeasFiles,nFiles,depFiles,pathDep,'DEPsweclimo')
  call ReadList(iu_list,trim(ListRadMeasFiles),RadMeasFiles,nFiles,depFiles,pathDep,'DEPswecl')
  IF (nfiles .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in prepSweClimo)') 
  nFiles=minval((/nFiles,nOrbits2Process/))

  !---Read SWE Climatology info
  call ReadSweClimo(sweClimoFile,sweClimo,ncols_ease,nrows_ease,nmonths,nhem,errReadingClimo)
  if(errReadingClimo .ne. 0)then
     print *,'ERROR: Reading SweClimo File'
     CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in prepSweClimo)')
  endif
!  print *,'errReadingClimo=',errReadingClimo
!  print *,'ncols_ease,nrows_ease,nmonths,nhem=',ncols_ease,nrows_ease,nmonths,nhem
!  print *,'minval(sweClimo),maxval(sweClimo)=',minval(sweClimo),maxval(sweClimo)
!  stop
  !---Read SWE Std Dev info
  call ReadSweStdev(sweStdevFile,sweStdev,ncols_ease,nrows_ease,nmonths,nhem,errReadingStdev)
  if(errReadingStdev .ne. 0)then
     print *,'ERROR: Reading SweStdev File'
     CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in prepSweClimo)')
  endif

  !---Copy climatology information to arrays with first month repeated 
  !---for interpolation purposes

  sweClimo_p1(1:nrows_ease,1:ncols_ease,1:nmonths,1:nhem)=sweClimo
  sweClimo_p1(1:nrows_ease,1:ncols_ease,nmonths+1,1:nhem)=&
       sweClimo(1:nrows_ease,1:ncols_ease,1,1:nhem)

  sweStdev_p1(1:nrows_ease,1:ncols_ease,1:nmonths,1:nhem)=sweStdev
  sweStdev_p1(1:nrows_ease,1:ncols_ease,nmonths+1,1:nhem)=&
       sweStdev(1:nrows_ease,1:ncols_ease,1,1:nhem)



  !------------------------------------------------------------------------------
  !  Read radiances (measurements), read SWE climatology, collocate to FMSDR and output 
  !------------------------------------------------------------------------------
  FileLoop: DO ifile=1,nFiles 
!  FileLoop: DO ifile=1,1
    write(*,*)
    write(*,*) 'prepSweClimo, ifile: ',ifile
    write(*,*) 'Input    FMSDR='//TRIM(RadMeasFiles(ifile))
    write(*,*) 'Output DEP(SweClim)='//TRIM(DepFiles(ifile))
    call ReadHdrMeasurmts(RadMeasFiles(ifile),iu,nProfiles,Ym)
    !---Populate DEP header from Scene items
    DEP%AlgSN      = AlgSN
    DEP%iTyp       = 0 ! DEP from a scene, not a retrieval
    DEP%nPosScan   = Ym%nPosScan 
    DEP%nScanLines = Ym%nScanLines

    !---Output header of DEP (derived products, will contain collocated SweClimo and SweStdev)
    CALL WriteHdrDEP(iuDep,depFiles(ifile),DEP,nProfiles)

    !---Loop over the profiles
    ProfLoop: DO iprof=1,nprofiles
       qc(:) = 0

       !---Read the measurement(s)
       call ReadMeasurmts(iu,Ym,ierr)
       IF (ierr.eq.Warn_EndOfFile)   EXIT  ProfLoop
       IF (ierr.eq.Warn_readInvalid) CYCLE ProfLoop
       IF (ierr.ne.0) CALL ErrHandl(ErrorType,Err_ReadingFile,'. Radiance file. (prepSweClimo)')

       !---QC for this profile
       IF (Ym%nqc .gt. 0) THEN
          Dep%qc(1:nqc) = Ym%qc(1)
       ELSE
          Dep%qc(1:nqc) = 0
       ENDIF
       !---Fill-in DEP structure (mostly missing, since sweClimo and sweStdev will be stored)
       DEP%lat        = Ym%lat
       DEP%lon        = Ym%lon
       DEP%ProfIndx   = iProf
       DEP%Angle      = Ym%Angle(1)
       DEP%RelAziAngle= Ym%RelAziAngle
       DEP%SolZenAngle= Ym%SolZenAngle
       DEP%node       = Ym%node
       DEP%scanDAY    = Ym%julDay
       DEP%scanYear   = Ym%Year
       DEP%scanUTC    = Ym%secs
       DEP%iscanPos   = Ym%iscanPos
       DEP%iscanLine  = Ym%iscanLine

       !---Make sure lat/lons are within expected range
       !---Otherwise, fill with missing values, write out, and cycle to next
       !---Output resultsscene
       IF((abs(Ym%lat) .gt. 90.) .or. (abs(Ym%lon) .gt. 360.) )THEN
          DEP%SWE=DEFAULT_VALUE_REAL
          DEP%SnowCover=DEFAULT_VALUE_REAL
          call WriteDEP(iuDep,Dep)
          CYCLE ProfLoop
       ENDIF

       !---Convert year and julian day to month and day
       call day_month(DEP%scanYear,imonth,iday,DEP%scanDay)
       
       !---Use month, day, and lat/lon information to find the corresponding point in the
       !---SWE climatology
       IF(DEP%lat .GE. 0.)THEN
          call ezlh_convert ('Nl', DEP%lat, DEP%lon, ir, ic, ierr)
       ELSE
          call ezlh_convert ('Sl', DEP%lat, DEP%lon, ir, ic, ierr)
       ENDIF
!       print *,'OUTPUT: EASE Grid: column, row=',ic,ir

       !---Extract the appropriate information from the SWE climatology
       !---Using Julian day, use linear interpolation in time from monthly data
       !---Pick nearest col and row (no interpolation)
       ihem=1 ! default N. Hem
       IF(DEP%lat .LE. 0.)ihem=2
       ic0=nint(ic)
       ir0=nint(ir)
!       print *,'INPUT: DEP%lat, DEP%lon=',DEP%lat, DEP%lon,&
!            ' OUTPUT: EASE Grid: column, row, ic0,ir0=',ic,ir,ic0,ir0
!       print *,'INPUT: DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay=',&
!            DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay,&
!            ' OUTPUT: EASE Grid: column, row,ic0,ir0,',&
!            'imonth,iday,ihem,sweClimoColloc=',&
!            ic,ir,ic0,ir0,imonth,iday,ihem,sweClimoColloc
!       write(99,*)'INPUT: DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay=',&
!            DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay,&
!            ' OUTPUT: EASE Grid: column, row,ic0,ir0,',&
!            'imonth,iday,ihem,sweClimoColloc=',&
!            ic,ir,ic0,ir0,imonth,iday,ihem,sweClimoColloc

!       print *,'imonth,iday,ihem=',imonth,iday,ihem
!       write(99,*)'imonth,iday,ihem,DEP%scanDay=',&
!            imonth,iday,ihem,DEP%scanDay

       ! Find nearest 2 months and calculate interpolation weights

       scanDay=DEP%scanDay
       IF(iday .lt. 15)THEN
          imon1=imonth-1
          imon2=imonth
          ! Special handling for first half of January
          if(imon1 .eq. 0)then
             imon1=12
             imon2=13
             scanDay=DEP%scanDay+365.
          endif
       ELSE
          imon1=imonth
          imon2=imonth+1
       ENDIF
       w2 = (scanDay-julianClimo(imon1))/&
            (julianClimo(imon2)-julianClimo(imon1))

       !--- SWE Mean
       !---Only apply in N. Hem (S. Hem missing)
       if(ihem .eq. 1)then
          sweClimoColloc1=sweClimo_p1(ir0,ic0,imon1,ihem)
          sweClimoColloc2=sweClimo_p1(ir0,ic0,imon2,ihem)
          if(sweClimoColloc1 .lt. 0. .or. sweClimoColloc2 .lt. 0.)then
             sweClimoColloc=DEFAULT_VALUE_REAL
          else
             sweClimoColloc=(1.-w2)*sweClimoColloc1+w2*sweClimoColloc2
             if(sweClimoColloc .lt. 0.)then
                sweClimoColloc=DEFAULT_VALUE_REAL
             else
                !---Convert from mm to cm
                sweClimoColloc=sweClimoColloc/10.
             endif
          endif
       endif
!       write(99,*)'scanDay,imonth,imon1,imon2,julianClimo(imon1),julianClimo(imon2),w2=',&
!            scanDay,imonth,imon1,imon2,julianClimo(imon1),julianClimo(imon2),w2
!       write(99,*)'sweClimoColloc1,sweClimoColloc2,sweClimoColloc=',&
!            sweClimoColloc1,sweClimoColloc2,sweClimoColloc


       !--- Std Dev SWE
       !--- For now, set to missing
       sweStdevColloc=DEFAULT_VALUE_REAL
       if(ihem .eq. 1)then
          sweStdevColloc1=sweStdev_p1(ir0,ic0,imon1,ihem)
          sweStdevColloc2=sweStdev_p1(ir0,ic0,imon2,ihem)
          if(sweStdevColloc1 .lt. 0. .or. sweStdevColloc2 .lt. 0.)then
             sweStdevColloc=DEFAULT_VALUE_REAL
          else
             sweStdevColloc=(1.-w2)*sweStdevColloc1+w2*sweStdevColloc2
             if(sweStdevColloc .lt. 0.)then
                sweStdevColloc=DEFAULT_VALUE_REAL
             else
                !---Convert from mm to cm
                sweStdevColloc=sweStdevColloc/10.
             endif
          endif
       endif



!       !--- SWE Mean
!       !---Only apply in N. Hem (S. Hem missing)
!       if(ihem .eq. 1)sweClimoColloc=sweClimo(ir0,ic0,imonth,ihem)
!       IF(sweClimoColloc .LT. 0.)THEN
!          sweClimoColloc=DEFAULT_VALUE_REAL
!       ELSE
!       !---Convert from mm to cm
!          sweClimoColloc=sweClimoColloc/10.
!       ENDIF

       !--- Std Dev SWE
       !--- For now, set to missing
       sweStdevColloc=DEFAULT_VALUE_REAL
       !---Only apply in N. Hem (S. Hem missing)
       if(ihem .eq. 1)sweStdevColloc=sweStdev(ir0,ic0,imonth,ihem)
       IF(sweStdevColloc .LT. 0.)THEN
          sweStdevColloc=DEFAULT_VALUE_REAL
       ELSE
       !---Convert from mm to cm
          sweStdevColloc=sweStdevColloc/10.
       ENDIF
!       print *,'INPUT: DEP%lat, DEP%lon=',DEP%lat, DEP%lon,&
!            ' OUTPUT: EASE Grid: column, row, ic0,ir0=',ic,ir,ic0,ir0
!       print *,'INPUT: DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay=',&
!            DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay,&
!            ' OUTPUT: EASE Grid: column, row,ic0,ir0,',&
!           'imonth,iday,ihem,sweClimoColloc,sweStdevColloc=',&
!            ic,ir,ic0,ir0,imonth,iday,ihem,sweClimoColloc,sweStdevColloc
!       write(99,*)'INPUT: DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay=',&
!            DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay,&
!            ' OUTPUT: EASE Grid: column, row,ic0,ir0,',&
!            'imonth,iday,ihem,sweClimoColloc,sweStdevColloc=',&
!            ic,ir,ic0,ir0,imonth,iday,ihem,sweClimoColloc,sweStdevColloc


       !---Copy to DEP structure
       DEP%SWE=sweClimoColloc
       DEP%SnowCover=sweStdevColloc
!       print *,'ihem,imonth,ic0,ir0,sweClimoColloc=',ihem,imonth,ic0,ir0,sweClimoColloc
!       write(99,*)'DEP%lat, DEP%lon,ihem,imonth,ic0,ir0,sweClimoColloc=',DEP%lat, DEP%lon,ihem,imonth,ic0,ir0,sweClimoColloc
       

       !---Output results
       call WriteDEP(iuDep,Dep)
    END DO ProfLoop
! test: coordinates at N Pole
!    call ezlh_convert ('Nl', 90., 0., ir, ic, ierr)
!    print *,'INPUT: lat, lon=',90.,0.
!    print *,'OUTPUT: EASE Grid: column, row=',ic,ir

    !---Close Input/output files
    close(iu)
    close(iuDep)
    !---Release meomory allocated in ReadHdrMeasurmts of src/lib/io/IO_MeasurData.f90
    DEALLOCATE(Ym%CentrFreq,Ym%Rad,Ym%qc,Ym%Tb,Ym%polar,Ym%angle,Ym%secant_view)
      
  END DO FileLoop

  !---Release memory
  DEALLOCATE(RadMeasFiles)
  DEALLOCATE(DepFiles)

  CALL CloseLogFile()


CONTAINS

!===============================================================
! Name:        ReadSweClimo
!
!
! Type:        Subroutine
!
!
! Description:  Reads in the global SWE mean climatology 
!               (stored on EASE grid, monthly, both hemispheres)
!               
!
!
! Arguments:
!
!           Name            Type           Description
!      ---------------------------------------------------
!       - sweClimoFile       I             SWE Climatology File
!       - sweClimo           O             Array containing SWE mean climatology
!       - ncols_ease         I             No. columns on 25 km Hemispheric EASE grid
!       - nrows_ease         I             No. rows on 25 km Hemispheric EASE grid
!       - nmonths            I             No. months stored in climatology file
!       - nhem               I             No. hemispheres stored in climatology file
!       - errReadingClimo    O             Error status returned
!
!
! Modules needed:
!       - None
!
!
! History:
!       10-30-2014      C. Grassotti, NOAA/NESDIS/STAR
!
!===============================================================

  SUBROUTINE ReadSweClimo(sweClimoFile,sweClimo,&
       ncols_ease,nrows_ease,nmonths,nhem,err)

    CHARACTER(LEN=*)                   :: sweClimoFile
    INTEGER :: ncols_ease,nrows_ease,nmonths,nhem,err
!    REAL, DIMENSION(ncols_ease,nrows_ease,nmonths,nhem) :: sweClimo
    REAL, DIMENSION(nrows_ease,ncols_ease,nmonths,nhem) :: sweClimo

    INTEGER :: iu,ncols,nrows,nm,nhm
    INTEGER :: icol,irow,imonth,ihem
    CHARACTER(LEN=250)                   :: dummy
    

    !---Open file SWE climatology
    err = 0
    iu=get_lun()
    OPEN(iu,file=sweClimoFile,form='formatted',status='old',err=10)
    !---Read header and dimension information
    READ(iu,'(a)',err=10)dummy
!    print *,'dummy=',dummy
    READ(iu,'(a)',err=10)dummy
!    print *,'dummy=',dummy
    READ(iu,'(4(1x,i3))',err=10)ncols,nrows,nm,nhm
    print *,'Reading SWE Climatology file'
    print *,'SWE Climatology grid: ncols,nrows,nm,nhm=',ncols,nrows,nm,nhm
    IF(ncols .ne. ncols_ease)THEN
       err=1
       WRITE(*,*)'ERROR: ncols,ncols_ease inconsistent=',ncols,ncols_ease
       RETURN
    ENDIF
    IF(nrows .ne. nrows_ease)THEN
       err=1
       WRITE(*,*)'ERROR: nrows,nrows_ease inconsistent=',nrows,nrows_ease
       RETURN
    ENDIF
    IF(nm .ne. nmonths)THEN
       err=1
       WRITE(*,*)'ERROR: nm,nmonths inconsistent=',nm,nmonths
       RETURN
    ENDIF
    IF(nhm .ne. nhem)THEN
       err=1
       WRITE(*,*)'ERROR: nhm,nhem inconsistent=',nhm,nhem
       RETURN
    ENDIF
    !---Read SWE climatology data
!    READ(iu,*,err=10)sweClimo(:,:,:,:)
!    print *,'reading sweClimo file'
    DO ihem=1,nhem
!       print *,'ihem: ',ihem
       DO imonth=1,nmonths
!          print *,'imonth: ',imonth
          DO icol=1,ncols_ease
             READ(iu,'(721(1x,f6.1))',err=10)sweClimo(:,icol,imonth,ihem)             
          ENDDO

          READ(iu,'(a)',err=10)dummy
!          print *,'err=',err
       ENDDO
    ENDDO

    WHERE(sweClimo .GE. -100. .AND. sweClimo .LT. 0.)
       sweClimo=0.
    ENDWHERE

    RETURN
    close(iu)
    RETURN
10  err=1
    RETURN

END SUBROUTINE ReadSweClimo

!===============================================================
! Name:        ReadSweStdev
!
!
! Type:        Subroutine
!
!
! Description:  Reads in the global SWE climatology standard deviation 
!               (stored on EASE grid, monthly, both hemispheres)
!               
!
!
! Arguments:
!
!           Name            Type           Description
!      ---------------------------------------------------
!       - sweStdevFile       I             SWE Std Dev Climatology File
!       - sweStdev           O             Array containing SWE mean climatology
!       - ncols_ease         I             No. columns on 25 km Hemispheric EASE grid
!       - nrows_ease         I             No. rows on 25 km Hemispheric EASE grid
!       - nmonths            I             No. months stored in climatology file
!       - nhem               I             No. hemispheres stored in climatology file
!       - errReadingStdev    O             Error status returned
!
!
! Modules needed:
!       - None
!
!
! History:
!       10-30-2014      C. Grassotti, NOAA/NESDIS/STAR
!
!===============================================================

  SUBROUTINE ReadSweStdev(sweStdevFile,sweStdev,&
       ncols_ease,nrows_ease,nmonths,nhem,err)

    CHARACTER(LEN=*)                   :: sweStdevFile
    INTEGER :: ncols_ease,nrows_ease,nmonths,nhem,err
    REAL, DIMENSION(nrows_ease,ncols_ease,nmonths,nhem) :: sweStdev

    INTEGER :: iu,ncols,nrows,nm,nhm
    INTEGER :: icol,irow,imonth,ihem
    CHARACTER(LEN=250)                   :: dummy
    

    print *,'Start subroutine ReadSweStdev'
    !---Open file SWE climatology
    err = 0
    iu=get_lun()
    OPEN(iu,file=sweStdevFile,form='formatted',status='old',err=10)
    print *,'finish open'
    !---Read header and dimension information
    READ(iu,'(a)',err=10)dummy
    print *,'dummy=',dummy
    READ(iu,'(a)',err=10)dummy
    print *,'dummy=',dummy
    READ(iu,'(4(1x,i3))',err=10)ncols,nrows,nm,nhm
    print *,'Reading SWE Std Dev Climatology file'
    print *,'SWE Std Dev Climatology grid: ncols,nrows,nm,nhm=',ncols,nrows,nm,nhm
    IF(ncols .ne. ncols_ease)THEN
       err=1
       WRITE(*,*)'ERROR: ncols,ncols_ease inconsistent=',ncols,ncols_ease
       RETURN
    ENDIF
    IF(nrows .ne. nrows_ease)THEN
       err=1
       WRITE(*,*)'ERROR: nrows,nrows_ease inconsistent=',nrows,nrows_ease
       RETURN
    ENDIF
    IF(nm .ne. nmonths)THEN
       err=1
       WRITE(*,*)'ERROR: nm,nmonths inconsistent=',nm,nmonths
       RETURN
    ENDIF
    IF(nhm .ne. nhem)THEN
       err=1
       WRITE(*,*)'ERROR: nhm,nhem inconsistent=',nhm,nhem
       RETURN
    ENDIF
    !---Read SWE Std Dev climatology data
!    READ(iu,*,err=10)sweStdev(:,:,:,:)
!    print *,'reading sweStdev file'
    DO ihem=1,nhem
!       print *,'ihem: ',ihem
       DO imonth=1,nmonths
!          print *,'imonth: ',imonth
          DO icol=1,ncols_ease
!             print *,'irow: ',irow
             READ(iu,'(721(1x,f6.1))',err=10)sweStdev(:,icol,imonth,ihem)             
          ENDDO
          READ(iu,'(a)',err=10)dummy
!          print *,'err=',err
       ENDDO
    ENDDO

!    WHERE(sweStdev .LT. 0.)
!       sweClimo=0.
!    ENDWHERE

    RETURN
    close(iu)
    RETURN
10  err=1
    RETURN

  END SUBROUTINE ReadSweStdev

! Modified version of ezlhconv.f
! Obtained from NSIDC for handling EASE grids
! Converted to f90 module

! C. Grassotti IMSG @ NOAA/NESDIS

!==========================================================================
! ezlhconv.for - FORTRAN routines for conversion of azimuthal 
!		equal area and equal area cylindrical grid coordinates
!
!	30-Jan.-1992 H.Maybee
!	20-Mar-1992 Ken Knowles  303-492-0644  knowles@kryos.colorado.edu
!       16-Dec-1993 MJ Brodzik   303-492-8263  brodzik@jokull.colorado.edu
!                   Copied from nsmconv.f, changed resolutions from 
!                   40-20-10 km to 25-12.5 km
!       21-Dec-1993 MJ Brodzik   303-492-8263  brodzik@jokull.colorado.edu
!                   Fixed sign of Southern latitudes in ease_inverse.
!	12-Sep-1994 David Hoogstrate 303-492-4116 hoogstra@jokull.colorado.edu
!		    Changed grid cell size. Changed "c","f" to "l","h"
!	25-Oct-1994 David Hoogstrate 303-492-4116 hoogstra@jokull.colorado.edu
!		    Changed row size from 587 to 586 for Mercator projection
!		    Changed function names to "ezlh-.."
! $Log: ezlhconv.f,v $
! Revision 1.3  1994/11/01 23:40:43  brodzik
! Replaced all references to 'ease' with 'ezlh'
!


!--------------------------------------------------------------------------
  subroutine ezlh_convert (grid, lat, lon, r, s, status)

    character*(*), intent(in) :: grid
    real, intent(in) :: lat, lon
    real, intent(out) :: r, s
    integer, intent(out) :: status
!
!	convert geographic coordinates (spherical earth) to 
!	azimuthal equal area or equal area cylindrical grid coordinates
!
!	status = ezlh_convert (grid, lat, lon, r, s)
!
!	input : grid - projection name '[NSM][lh]'
!               where l = "low"  = 25km resolution
!                     h = "high" = 12.5km resolution
!		lat, lon - geo. coords. (decimal degrees)
!
!	output: r, s - column, row coordinates
!
!	result: status = 0 indicates normal successful completion
!			-1 indicates error status (point not on grid)
!
!--------------------------------------------------------------------------
    integer ::  cols, rows, scale
    real :: Rg, phi, lam, rho, r0, s0, t, rad, deg


    real, parameter :: RE_km = 6371.228    ! radius of the earth (km), authalic sphere based on International datum 
    real, parameter :: CELL_km = 25.067525 ! nominal cell size in kilometers
    real, parameter :: COS_PHI1 = .866025403 !	scale factor for standard parallels at +/-30.00 degrees
    real, parameter :: PI = 3.141592653589793

    rad(t) = t*PI/180. ! statement function for deg to rad conversion

!

    status = -1
    
    if ((grid(1:1).eq.'N').or.(grid(1:1).eq.'S')) then
       cols = 721
       rows = 721
    else if (grid(1:1).eq.'M') then
       cols = 1383
       rows = 586
    else
       print *, 'ezlh_convert: unknown projection: ', grid
       return
    endif

    if (grid(2:2).eq.'l') then
       scale = 1
    else if (grid(2:2).eq.'h') then
       scale = 2
    else
       print *, 'ezlh_convert: unknown projection: ', grid
       return
    endif

    Rg = scale * RE_km/CELL_km

!
!	r0,s0 are defined such that cells at all scales
!	have coincident center points
!
    r0 = (cols-1)/2. * scale
    s0 = (rows-1)/2. * scale

    phi = rad(lat)
    lam = rad(lon)

    if (grid(1:1).eq.'N') then
       rho = 2 * Rg * sin(PI/4. - phi/2.)
       r = r0 + rho * sin(lam)
       s = s0 + rho * cos(lam)
       
    else if (grid(1:1).eq.'S') then
       rho = 2 * Rg * cos(PI/4. - phi/2.)
       r = r0 + rho * sin(lam)
       s = s0 - rho * cos(lam)
       
    else if (grid(1:1).eq.'M') then
       r = r0 + Rg * lam * COS_PHI1
       s = s0 - Rg * sin(phi) / COS_PHI1
       
    endif
    
    status = 0

    return

  end subroutine ezlh_convert

!--------------------------------------------------------------------------

  subroutine ezlh_inverse (grid, r, s, lat, lon, status)

    character*(*), intent(in) :: grid
    real, intent(in) :: r, s
    real, intent(out) :: lat, lon
    integer, intent(out) :: status
!
!	convert azimuthal equal area or equal area cylindrical 
!	grid coordinates to geographic coordinates (spherical earth)
!
!	status = ezlh_inverse (grid, r, s, lat, lon)
!
!	input : grid - projection name '[NSM][lh]'
!               where l = "low"  = 25km resolution
!                     h = "high" = 12.5km resolution
!		r, s - grid column and row coordinates
!
!	output: lat, lon - geo. coords. (decimal degrees)
!
!	result: status = 0 indicates normal successful completion
!			-1 indicates error status (point not on grid)
!
!--------------------------------------------------------------------------
    integer :: cols, rows, scale
    real :: Rg, phi, lam, rho, r0, s0, t, rad, deg
    real :: gamma, beta, c, epsilon, x, y
    real :: sinphi1, cosphi1

    real, parameter :: RE_km = 6371.228 ! radius of the earth (km), authalic sphere based on International datum 
    real, parameter :: CELL_km = 25.067525 ! nominal cell size in kilometers
    real, parameter :: COS_PHI1 = .866025403 ! scale factor for standard parallels at +/-30.00 degrees
    real, parameter :: PI = 3.141592653589793

    deg(t) = t*180./PI ! statement function for rad to deg conversion
    
    status = -1
        
    if ((grid(1:1).eq.'N').or.(grid(1:1).eq.'S')) then
       cols = 721
       rows = 721
    else if (grid(1:1).eq.'M') then
       cols = 1383
       rows = 586
    else
       print *, 'ezlh_inverse: unknown projection: ', grid
       return
    endif

    if (grid(2:2).eq.'l') then
       scale = 1
    else if (grid(2:2).eq.'h') then
       scale = 2
    else
       print *, 'ezlh_inverse: unknown projection: ', grid
       return
    endif

    Rg = scale * RE_km/CELL_km
    
    r0 = (cols-1)/2. * scale
    s0 = (rows-1)/2. * scale
    
    x = r - r0
    y = -(s - s0)
    
    if ((grid(1:1).eq.'N').or.(grid(1:1).eq.'S')) then 
       rho = sqrt(x*x + y*y)
       if (rho.eq.0.0) then
          if (grid(1:1).eq.'N') lat = 90.0 
          if (grid(1:1).eq.'S') lat = -90.0 
          lon = 0.0
       else
          if (grid(1:1).eq.'N') then
             sinphi1 = sin(PI/2.)
             cosphi1 = cos(PI/2.)
             if (y.eq.0.) then
                if (r.le.r0) lam = -PI/2.
                if (r.gt.r0) lam = PI/2.
             else
                lam = atan2(x,-y)
             endif
          else if (grid(1:1).eq.'S') then
             sinphi1 = sin(-PI/2.)
             cosphi1 = cos(-PI/2.)
             if (y.eq.0.) then
                if (r.le.r0) lam = -PI/2.
                if (r.gt.r0) lam = PI/2.
             else
                lam = atan2(x,y)
             endif
          endif
          gamma = rho/(2 * Rg)
          if (abs(gamma).gt.1.) return
          c = 2 * asin(gamma)
          beta = cos(c) * sinphi1 + y * sin(c) * (cosphi1/rho)
          if (abs(beta).gt.1.) return
          phi = asin(beta)
          lat = deg(phi)
          lon = deg(lam)
       endif
       
    else if (grid(1:1).eq.'M') then
!
!	  allow .5 cell tolerance in arcsin function
!	  so that grid coordinates which are less than .5 cells
!	  above 90.00N or below 90.00S are given a lat of 90.00
!
       epsilon = 1 + 0.5/Rg
       beta = y*COS_PHI1/Rg
       if (abs(beta).gt.epsilon) return
       if (beta.le.-1.) then
          phi = -PI/2.
       else if (beta.ge.1.) then
          phi = PI/2.
       else
          phi = asin(beta)
       endif
       lam = x/COS_PHI1/Rg
       lat = deg(phi)
       lon = deg(lam)
    endif
    
    status = 0

    return
  end subroutine ezlh_inverse


end Program PrepSweClimo
