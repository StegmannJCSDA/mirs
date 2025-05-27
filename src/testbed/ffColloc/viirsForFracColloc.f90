!$Id: $
!===============================================================
! Name:     viirsForFracColloc
!
!
! Type:         Main Program
!
!
! Description:
!       Program that accesses VIIRS-derived forest fraction file
!       collocates the surface type values with Rad measurement file FOVs
!       and writes output to DEP format file for later use
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
!       03-04-2016      C. Grassotti - NOAA/NESDIS/STAR
!
!===============================================================

Program viirsForFracColloc
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
  INTEGER,            PARAMETER                     :: ncols=1440,nrows=720 ! assume 0.25 degree grid for now
  INTEGER,            PARAMETER                     :: nForestFrac=5 ! assume 5 different forest types (VIIRS classification)
  INTEGER,            PARAMETER                     :: nHalfGrdFov=1 ! half width (in grid units) of averaging window for FOV
  INTEGER                                           :: iu_list,nFiles,ifile,iu1,ii,jj
  INTEGER                                           :: iff,jff,iff0,jff0
  INTEGER                                           :: nprofiles,iprof,iu,iuDep,ierr
  INTEGER                                           :: errReadingFF,errReadingLL
  TYPE(Dep_type)                                    :: Dep
  TYPE(MeasurData_type)                             :: Ym
  CHARACTER(LEN=250), DIMENSION(:), POINTER         :: RadMeasFiles,depFiles
  INTEGER,            DIMENSION(nqc)                :: qc
  !-- Surface type-related variables
  REAL, DIMENSION(nForestFrac+1,ncols,nrows)        :: forestFrac
  REAL, DIMENSION(nHalfgrdFov+1,nHalfgrdFov+1)      :: forFracFov
  REAL                                              :: ff1s,ff2s,ff3s,ff4s,ff5s,fftots,nff
  REAL                                              :: ff1,ff2,ff3,ff4,ff5,fftot
  REAL                                              :: lat0,dlat,lon0,dlon
  INTEGER                                           :: nlat,nlon
  REAL                                              :: scanDay
  REAL                                              :: ir,ic
  INTEGER                                           :: imonth,iday
  INTEGER                                           :: i,j,kk,yyyy,ddd
  INTEGER                                           :: dddDep,dpyDep
  REAL                                              :: lat,lon
  !---Namelist data 
  INTEGER            :: nAlgors=DEFAULT_VALUE_INT
  INTEGER            :: sensor_id=DEFAULT_VALUE_INT
  INTEGER            :: nOrbits2Process=DEFAULT_VALUE_INT
  INTEGER            :: AlgSN=DEFAULT_VALUE_INT
  CHARACTER(LEN=250) :: ListRadMeasFiles=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: pathDep=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: forestFracFile=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: forestFracLatLonFile=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: LogFile=DEFAULT_VALUE_STR4

  NAMELIST /ContrlFFColloc/ListRadMeasFiles,pathDep,LogFile,&
       forestFracFile,forestFracLatLonFile,sensor_id,nOrbits2Process,AlgSN

  !---Read control-data from namelist
  READ(*,NML=ContrlFFColloc)
  !---Prepare Log file
  CALL OpenLogFile(Logfile)

  print *,'forestFracFile=',forestFracFile
!  stop

  !---Read the file names of FM- SDRs
  call ReadList(iu_list,trim(ListRadMeasFiles),RadMeasFiles,nFiles,depFiles,pathDep,'FFcol')
  IF (nfiles .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in viirsForFracColloc)') 
  nFiles=minval((/nFiles,nOrbits2Process/))

  !---Read forest fraction file

  call read_viirs_ForFrac(forestFracFile,nForestFrac,ncols,nrows,forestFrac,errReadingFF)
  if(errReadingFF .ne. 0)then
     print *,'ERROR: Reading VIIRS Forest Fraction File'
     CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in viirsForestFracColloc)')
  endif

  call read_viirs_ForFracLatLon(forestFracLAtLonFile,Lat0,dlat,nlat,Lon0,dlon,nlon,errReadingLL)
  !---Compare grid definitions from two files and make sure they are consistent  
  print *,'Lat/Lon file grid: nlon,nlat=',nlon,nlat
  print *,'Forest Fraction data file grid: ncols,nrows=',ncols,nrows
  if(nlon .ne. ncols .or. nlat .ne. nrows)then
     print *,'ERROR: inconsistent grid definitions in data and Lat/Lon files'
     stop
  endif
!  stop

  !------------------------------------------------------------------------------
  !  Read radiances (measurements), read forest fraction, collocate to FMSDR and output 
  !------------------------------------------------------------------------------
  FileLoop: DO ifile=1,nFiles 
!  FileLoop: DO ifile=1,1
    write(*,*)
    write(*,*) 'viirsForFracColloc, ifile: ',ifile
    write(*,*) 'Input    FMSDR='//TRIM(RadMeasFiles(ifile))
    write(*,*) 'Output DEP(FFcol)='//TRIM(DepFiles(ifile))

!    print *,'ReadHdrMeasurmts'
    call ReadHdrMeasurmts(RadMeasFiles(ifile),iu,nProfiles,Ym)
    !---Populate DEP header from Scene items
    DEP%AlgSN      = AlgSN
    DEP%iTyp       = 0 ! DEP from a scene, not a retrieval
    DEP%nPosScan   = Ym%nPosScan 
    DEP%nScanLines = Ym%nScanLines

    !---Output header of DEP (derived products, will contain collocated surface type)
!    print *,'WriteHdrDEP'
    CALL WriteHdrDEP(iuDep,depFiles(ifile),DEP,nProfiles)

    !---Loop over the profiles
!    print *,'start ProfLoop: nprofiles=',nprofiles
    ProfLoop: DO iprof=1,nprofiles
!    ProfLoop: DO iprof=1,100
!       print *,'iprof=',iprof
       qc(:) = 0

       !---Read the measurement(s)
       call ReadMeasurmts(iu,Ym,ierr)
       IF (ierr.eq.Warn_EndOfFile)   EXIT  ProfLoop
       IF (ierr.eq.Warn_readInvalid) CYCLE ProfLoop
       IF (ierr.ne.0) CALL ErrHandl(ErrorType,Err_ReadingFile,'. Radiance file. (viirsForFracColloc)')

       !---QC for this profile
       IF (Ym%nqc .gt. 0) THEN
          Dep%qc(1:nqc) = Ym%qc(1)
       ELSE
          Dep%qc(1:nqc) = 0
       ENDIF
       !---Fill-in DEP structure (mostly missing, since forest fraction from the data set will be stored)
!       print *,'Fill DEP structure'
!       print *,'Ym%lat,Ym%lon=',Ym%lat,Ym%lon
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
       DEP%itypsfc    = DEFAULT_VALUE_INT
       DEP%SnowCover  = DEFAULT_VALUE_REAL ! fractional snow cover, but currently only takes values 0 or 1
! Fill values that will later store Forest Fraction data
       DEP%WindSp     = DEFAULT_VALUE_REAL ! FF1
       DEP%WindDir    = DEFAULT_VALUE_REAL ! FF2
       DEP%WindU      = DEFAULT_VALUE_REAL ! FF3
       DEP%WindV      = DEFAULT_VALUE_REAL ! FF4
       DEP%SM         = DEFAULT_VALUE_REAL ! FF5
       DEP%SWE        = DEFAULT_VALUE_REAL ! Total FF

       !---For use if averaging grid values around FOV center
       forFracFov(1:nHalfgrdFov+1,1:nHalfgrdFov+1) = DEFAULT_VALUE_REAL ! Total FF
       !---Make sure lat/lons are within expected range
       !---Otherwise, fill with missing values, write out, and cycle to next
       !---Output resultsscene
       IF((abs(Ym%lat) .gt. 90.) .or. (abs(Ym%lon) .gt. 360.) )THEN
          call WriteDEP(iuDep,Dep)
          CYCLE ProfLoop
       ENDIF

       !---Convert year and julian day to month and day
       call day_month(DEP%scanYear,imonth,iday,DEP%scanDay)
!       print *,'DEP%scanYear,DEP%scanDay,imonth,iday=',DEP%scanYear,DEP%scanDay,imonth,iday
       
       !---Use lat/lon information to find the corresponding point in the
       !---VIIRS-based forest fraction dataset. Data are stored on a simple global lat/lon grid
       !---Output: ii (column), kk (row, starting from top)
       lon=DEP%lon
       lat=DEP%lat
       ! test replace lat/lon with specified values
!       lat=90.
!       lat=-90.
!       lat=38.
!       lat=50.
!       lon=-180.0
!       lon=-179.875
!       lon=-179.76
!       lon=-179.75
!       lon=180.0
!       lon=-80.0
!       lon=-120.0
!       print *,'lon-Lon0=',lon-Lon0
!       print *,'lon-Lon0+dlon/2.=',lon-Lon0+dlon/2.
!       print *,'(lon-Lon0)/dlon)=',(lon-Lon0)/dlon
!       print *,'(lon-Lon0+(dlon/2.))/dlon)=',(lon-Lon0+(dlon/2.))/dlon
!       print *,'(lon-Lon0+(dlon/2.))/dlon)+0.5=',((lon-Lon0+(dlon/2.))/dlon) +0.5
!       print *,'(lat-Lat0+(dlat/2.))/dlat)+0.5=',((lat-Lat0+(dlat/2.))/dlat) +0.5
       ic=((lon-Lon0+(dlon/2.))/dlon) +0.5
       ir=((lat-Lat0+(dlat/2.))/dlat) +0.5
       ii=nint(ic)
       jj=nint(ir)
       if(ii .lt. 1)ii=1
       if(ii .gt. ncols)ii=ncols
       if(jj .lt. 1)jj=1
       if(jj .gt. nrows)jj=nrows
       !--- Compute average in the 2*nHalfgrdFov+1 x 2*nHalfgrdFov+1
       !--- window surrounding the collocated FOV center
       ff1s=0.
       ff2s=0.
       ff3s=0.
       ff4s=0.
       ff5s=0.
       fftots=0.
       nff=0.
       do jff=jj-nHalfgrdFov,jj+nHalfgrdFov
          jff0=jff
          if(jff .lt. 1)cycle
          if(jff .gt. nrows)cycle
          do iff=ii-nHalfgrdFov,ii+nHalfgrdFov
             iff0=iff
             if(iff .lt. 1)iff0=ncols+iff
             if(iff .gt. ncols)iff0=iff-ncols
             nff=nff+1.
!             print *,'iff0,jff0=',iff0,jff0
             ff1s=ff1s+forestFrac(1,iff0,jff0)
             ff2s=ff2s+forestFrac(2,iff0,jff0)
             ff3s=ff3s+forestFrac(3,iff0,jff0)
             ff4s=ff4s+forestFrac(4,iff0,jff0)
             ff5s=ff5s+forestFrac(5,iff0,jff0)
             fftots=fftots+forestFrac(6,iff0,jff0)
          enddo
       enddo
       if(nff .gt. 0.)then
          fftot=ff1s/nff
          DEP%WindSp     = fftot ! FF1
          fftot=ff2s/nff
          DEP%WindDir    = fftot ! FF2
          fftot=ff3s/nff
          DEP%WindU      = fftot ! FF3
          fftot=ff4s/nff
          DEP%WindV      = fftot ! FF4
          fftot=ff5s/nff
          DEP%SM         = fftot ! FF5
          fftot=fftots/nff
          DEP%SWE        = fftot ! Total FF
          if(fftot .lt. 0.)then
             print *,'WARNING: fftot<0 for lat,lon,fftot=',lat,lon,fftot
          endif
       else
          print *,'WARNING: nff=0 for lat,lon=',lat,lon
       endif
!       print *,'DEP%WindSp(FF1)=',DEP%WindSp
!       print *,'DEP%WindDir(FF2)=',DEP%WindDir
!       print *,'DEP%WindU(FF3)=',DEP%WindU
!       print *,'DEP%WindV(FF4)=',DEP%WindV
!       print *,'DEP%SM(FF5)=',DEP%SM
!       print *,'DEP%SWE(FFtot)=',DEP%SWE

!       print *,'Input:lat,lon=',lat,lon
!       print *,'Output:ic,ir,',ic,ir
!       print *,'Output:ii,jj,',ii,jj
!       print *

       !--- Assign Forest Fractions to DEP structure variables
!       DEP%WindSp     = forestFrac(1,ii,jj) ! FF1
!       DEP%WindDir    = forestFrac(2,ii,jj) ! FF2
!       DEP%WindU      = forestFrac(3,ii,jj) ! FF3
!       DEP%WindV      = forestFrac(4,ii,jj) ! FF4
!       DEP%SM         = forestFrac(5,ii,jj) ! FF5
!       DEP%SWE        = forestFrac(6,ii,jj) ! Total FF

       !---Output results
       call WriteDEP(iuDep,Dep)
    END DO ProfLoop
!    print *,'end ProfLoop'
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
! Name:        read_viirs_ForFrac
!
!
! Type:        Subroutine
!
!
! Description:  Reads in the VIIRS-based forest fractions on a regular lat/lon grid
!               
!
!
! Arguments:
!
!           Name            Type           Description
!      ---------------------------------------------------
!       - forestFracFile     I             Forest fraction file (binary,direct access)
!       - ncols              I             No. columns on global grid
!       - nrows              I             No. rows on global grid
!       - forestFrac         O             Array containing forest fraction (5 classes+ total FF)
!       - err                O             Error status returned
!
!
! Modules needed:
!       - None
!
!
! History:
!       3-4-2016     C. Grassotti, NOAA/NESDIS/STAR
!
!===============================================================

subroutine read_viirs_ForFrac(forestFracFile,nForestFrac,ncols,nrows,forestFrac,err)

  implicit none

  !--input  
  character(len=250) :: forestFracFile
  integer :: nForestFrac,ncols,nrows,err
  !---output
  real, dimension(nforestFrac+1,ncols,nrows) :: forestFrac
  !---local
  integer :: iu,i


  err=0
  iu=get_lun()
  OPEN(iu,file=forestFracFile,form='unformatted',access='DIRECT',&
       RECL=4*ncols*nrows,status='old',err=10)
  do i=1,nForestFrac+1
     print *,'i=',i
     READ(iu,REC=i)forestFrac(i,1:ncols,1:nrows)
     print *,'minval(forestFrac(i,1:ncols,1:nrows))=',&
          minval(forestFrac(i,1:ncols,1:nrows))
     print *,'maxval(forestFrac(i,1:ncols,1:nrows))=',&
          maxval(forestFrac(i,1:ncols,1:nrows))
  enddo
  CLOSE(iu)
  return

10 err=1
  close(iu)
  return


end subroutine read_viirs_ForFrac

!===============================================================
! Name:        read_viirs_ForFracLatLon
!
!
! Type:        Subroutine
!
!
! Description:  Reads in the lat/lon grid parameters of VIIRS-based forest fraction data
!               
!
!
! Arguments:
!
!           Name                Type           Description
!      ------------------------------------------------------
!       - forestFracLatLonFile  I              Forest fraction lat/lon file 
!       - Lat0                  O              Starting latitude
!       - dlat                  O              Latitude increment
!       - Lon0                  O              Starting longitude
!       - dlon                  O              Longitude increment
!       - nlat                  O              No. of latitude values on the grid
!       - nlon                  O              No. of longitude values on the grid
!       - err                   O              Error status returned
!
!
! Modules needed:
!       - None
!
!
! History:
!       3-4-2016     C. Grassotti, NOAA/NESDIS/STAR
!
!===============================================================

subroutine read_viirs_ForFracLatLon(forestFracLatLonFile,Lat0,dlat,nlat,&
     Lon0,dlon,nlon,err)

  implicit none

  !--input  
  character(len=250) :: forestFracLatLonFile
  !---output
  real :: Lat0,dlat,Lon0,dlon
  integer :: nlat,nlon,err
  !---local
  integer :: iu


  err=0
  iu=get_lun()

  OPEN(iu,file=forestFracLatLonFile,form='formatted',status='old',err=10)
  READ(iu,*)nlon,Lon0,dlon
  READ(iu,*)nlat,Lat0,dlat

  CLOSE(iu)
  return

10 err=1
  close(iu)
  return


end subroutine read_viirs_ForFracLatLon

end Program viirsForFracColloc
