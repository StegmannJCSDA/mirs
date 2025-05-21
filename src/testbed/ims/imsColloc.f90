!$Id: $
!===============================================================
! Name:     imsColloc
!
!
! Type:         Main Program
!
!
! Description:
!       Program that accesses NIC IMS 4 km gridded snow/ice analysis
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
!       10-12-2015      C. Grassotti - NOAA/NESDIS/STAR
!
!===============================================================

Program imsColloc
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
  INTEGER,            PARAMETER                     :: ncols=6144,nrows=6144
  INTEGER,            PARAMETER                     :: nhem=1
  INTEGER,            PARAMETER                     :: ngrdFov=0
  INTEGER                                           :: iu_list,nFiles,ifile
  INTEGER                                           :: nprofiles,iprof,iu,iuDep,ierr
  INTEGER                                           :: errReadingIms
  TYPE(Dep_type)                                    :: Dep
  TYPE(MeasurData_type)                             :: Ym
  CHARACTER(LEN=250), DIMENSION(:), POINTER         :: RadMeasFiles,depFiles
  INTEGER,            DIMENSION(nqc)                :: qc
  !-- Surface type-related variables
  INTEGER, DIMENSION(ncols,nrows,nhem)              :: ims_sfctyp, ims_sfctyp_mirs
  INTEGER, DIMENSION(ngrdFov+1,ngrdFov+1)           :: sfctypFov
  INTEGER                                           :: sfctyp
  REAL                                              :: scanDay
  REAL                                              :: ir,ic
  INTEGER                                           :: imonth,iday,ir0,ic0,ihem
  INTEGER                                           :: i,j,ii,kk,yyyy,ddd
  INTEGER                                           :: dddDep,dddIms,dpyDep,dpyIms
  REAL                                              :: lat,lon,alat,alon
  LOGICAL                                           :: dateChecked
  !---Namelist data 
  INTEGER            :: nAlgors=DEFAULT_VALUE_INT
  INTEGER            :: sensor_id=DEFAULT_VALUE_INT
  INTEGER            :: nOrbits2Process=DEFAULT_VALUE_INT
  INTEGER            :: AlgSN=DEFAULT_VALUE_INT
  INTEGER            :: maxDayLag
  CHARACTER(LEN=250) :: ListRadMeasFiles=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: pathDep=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: imsFile=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: LogFile=DEFAULT_VALUE_STR4

  NAMELIST /ContrlImsColloc/ListRadMeasFiles,pathDep,LogFile,&
       imsFile,maxDayLag,sensor_id,nOrbits2Process,AlgSN

  !---Read control-data from namelist
  READ(*,NML=ContrlImsColloc)
  !---Prepare Log file
  CALL OpenLogFile(Logfile)

!  print *,'imsFile=',imsFile
  !---Read the file names of EC-FM- SDRs
!  call ReadList(iu_list,trim(ListRadMeasFiles),RadMeasFiles,nFiles,depFiles,pathDep,'DEPims')
  call ReadList(iu_list,trim(ListRadMeasFiles),RadMeasFiles,nFiles,depFiles,pathDep,'IMScol')
  IF (nfiles .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in imsColloc)') 
  nFiles=minval((/nFiles,nOrbits2Process/))

  !---Read IMS surface analysis
  call read_ims_sfctyp(imsFile,ncols,nrows,nhem,ims_sfctyp,ims_sfctyp_mirs,yyyy,ddd,errReadingIms)
  if(errReadingIms .ne. 0)then
     print *,'ERROR: Reading IMS Analysis File'
     CALL ErrHandl(ErrorType,Err_NoFilesFound,'(in imsColloc)')
  endif

  dateChecked= .FALSE.
  !------------------------------------------------------------------------------
  !  Read radiances (measurements), read IMS analysis, collocate to FMSDR and output 
  !------------------------------------------------------------------------------
  FileLoop: DO ifile=1,nFiles 
!  FileLoop: DO ifile=1,1
    write(*,*)
    write(*,*) 'imsColoc, ifile: ',ifile
    write(*,*) 'Input    FMSDR='//TRIM(RadMeasFiles(ifile))
    write(*,*) 'Output DEP(IMScol)='//TRIM(DepFiles(ifile))

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
!       print *,'iprof=',iprof
       qc(:) = 0

       !---Read the measurement(s)
       call ReadMeasurmts(iu,Ym,ierr)
       IF (ierr.eq.Warn_EndOfFile)   EXIT  ProfLoop
       IF (ierr.eq.Warn_readInvalid) CYCLE ProfLoop
       IF (ierr.ne.0) CALL ErrHandl(ErrorType,Err_ReadingFile,'. Radiance file. (imsColloc)')

       !---QC for this profile
       IF (Ym%nqc .gt. 0) THEN
          Dep%qc(1:nqc) = Ym%qc(1)
       ELSE
          Dep%qc(1:nqc) = 0
       ENDIF
       !---Fill-in DEP structure (mostly missing, since sfctyp from IMS will be stored)
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
       DEP%SWE        = DEFAULT_VALUE_REAL

       !---If processing first valid scene, make sure year and julian day match contents of IMS file header
       !---Parameter maxDayLag defines the maximum number of Julian days allowed between FMSDRs and IMS analysis
       !---Account for possibility of year change, and leap years
       if(.not. dateChecked)then
          print *,'From FMSDR:DEP%scanYear, From IMS:yyyy= ',DEP%scanYear, yyyy
          print *,'From FMSDR:DEP%scanDay, From IMS:ddd= ',DEP%scanDay, ddd
          if( ( MOD(DEP%scanYear-1,4).eq.0 .and. MOD(DEP%scanYear-1,100).ne.0 ) &
               .or. MOD(DEP%scanYear-1,400).eq.0 ) then
             dpyDep=366
          else
             dpyDep=365
          endif
          if( ( MOD(yyyy-1,4).eq.0 .and. MOD(yyyy-1,100).ne.0 ) &
               .or. MOD(yyyy-1,400).eq.0 ) then
             dpyIms=366
          else
             dpyIms=365
          endif

          dddDep=DEP%scanDay
          dddIms=ddd
          if(dddDep .ge. 300 .and. dddIms .lt. 100)then
             dddIms=dddIms+dpyIms
          elseif(dddIms .ge. 300 .and. dddDep .lt. 100)then
             dddDep=dddDep+dpyDep
          endif
          print *,'Julian days: dddDep, dddIms, maxDayLag=',dddDep, dddIms, maxDayLag
          if(abs(dddDep-dddIms) .gt. abs(maxDayLag))then
             print *,'ERROR: Difference in julian days exceeeds maximum allowed: dddDep, dddIms=',&
                  dddDep, dddIms
          endif
!          if(DEP%scanYear .ne. yyyy)then
!             print *,'ERROR: FMSDR and IMS  year do not match: DEP%scanYear, yyyy=', &
!                   DEP%scanYear, yyyy
!             stop
!          endif
!          if(DEP%scanDay .ne. ddd)then
!             print *,'ERROR: FMSDR and IMS  julian day do not match: DEP%scanDay, ddd=', &
!                   DEP%scanDay, ddd
!             stop
!          endif
          dateChecked=.TRUE.
       endif

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
       
       sfctypFov(:,:)=DEFAULT_VALUE_INT
       !---Use lat/lon information to find the corresponding point in the
       !---IMS analysis. Currently, IMS analysis only covers N. Hemisphere

       !---Input: gtype=1 (4 km grid),ihem=1 (NH),itrans=2 (lat/lon->col,row), 
       !---       lat, lon (0-360 degrees)
       !---Output: ii (column), kk (row, starting from top)
       !---Not used (reserved for reverse transform): i,j,alat,alon
       ihem=0
       IF(DEP%lat .GE. 0.)THEN
          ihem=1
          lat=DEP%lat
          lon=DEP%lon
          if(lon .lt. 0.)lon=360.+lon
          call ims_psijll(1,ihem,2,lat,lon,i,j,alat,alon,ii,kk,ierr)
!          if(DEP%lon .le. -70. )then
!             print *,'Input:lat,lon,DEP%lon=',lat,lon,DEP%lon
!             print *,'Output:ii,kk,ierr,ims_sfctyp_mirs(ii,kk,ihem)=',ii,kk,ierr,ims_sfctyp_mirs(ii,kk,ihem)
!             print *
!          endif
       ENDIF
!       print *,'OUTPUT: EASE Grid: column, row=',ic,ir
!       print *,'INPUT: DEP%lat, DEP%lon=',DEP%lat, DEP%lon,&
!            ' OUTPUT: EASE Grid: column, row, ic0,ir0=',ic,ir,ic0,ir0
!       print *,'INPUT: DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay=',&
!            DEP%lat, DEP%lon,DEP%scanYear,DEP%scanDay,&
!            ' OUTPUT: EASE Grid: column, row,ic0,ir0,',&
!            'imonth,iday,ihem,sweClimoColloc=',&
!            ic,ir,ic0,ir0,imonth,iday,ihem,sweClimoColloc

       !--- IMS Sfc type (MiRS codes)
       !---Only apply in N. Hem (S. Hem missing in IMS analysis)
       if(ihem .eq. 1 .and. ierr .eq. 0)then
          sfctyp=ims_sfctyp_mirs(ii,kk,ihem)
          DEP%itypsfc=sfctyp
          if(DEP%itypsfc .ge. 0)then
             DEP%SnowCover=0
             if(DEP%itypsfc .eq. 3)DEP%SnowCover=1.
          endif
       endif
!       write(99,*)'scanDay,imonth,imon1,imon2,julianClimo(imon1),julianClimo(imon2),w2=',&
!            scanDay,imonth,imon1,imon2,julianClimo(imon1),julianClimo(imon2),w2
!       write(99,*)'sweClimoColloc1,sweClimoColloc2,sweClimoColloc=',&
!            sweClimoColloc1,sweClimoColloc2,sweClimoColloc

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
! Name:        read_ims_sfctyp
!
!
! Type:        Subroutine
!
!
! Description:  Reads in the global IMS 4 km snow/ice analysis
!               (stored on a P.S. grid, N. Hemisphere only)
!               
!
!
! Arguments:
!
!           Name            Type           Description
!      ---------------------------------------------------
!       - imsFile            I             IMS File (ascii)
!       - ncols              I             No. columns on 4 km P.S. grid
!       - nrows              I             No. rows on 4 km P.S. grid
!       - nhem               I             No. hemispheres stored in IMS file
!       - ims_sfctyp         O             Array containing IMS surface type (original codes)
!       - ims_sfctyp_mirs    O             Array containing IMS surface type (MiRS codes)
!       - err                O             Error status returned
!
!
! Modules needed:
!       - None
!
!
! History:
!       10-15-2015     C. Grassotti, NOAA/NESDIS/STAR
!
!===============================================================

subroutine read_ims_sfctyp(ims_file,ncol,nrow,nhem,ims_sfctyp,ims_sfctyp_mirs,yyyy,ddd,err)

  implicit none

  !--input  
  character(len=250) :: ims_file
  integer :: ncol,nrow,nhem,err
  !---output
  integer, dimension(ncol,nrow,nhem) :: ims_sfctyp,ims_sfctyp_mirs
  integer :: yyyy,ddd
  !---local
  character(len=100) :: dummy
  character(len=7) :: yyyyddd
  character(len=ncol) :: buf
  integer :: ibuf6144(ncol),iu,i,ind1


  err=0
  iu=get_lun()
  open(iu,file=ims_file,form='formatted',err=10)
  
!--- read in header 
  do i=1,30
     read(iu,'(a)')dummy
     print *,'IMS ascii file header record=',dummy
!--- first header record contains year and julian day,
!--- so extract information
     if(i .eq. 1)then
        ind1=INDEX(dummy,':')
        yyyyddd=dummy(ind1+2:ind1+8)
        print *,'yyyyddd=',yyyyddd
        read(yyyyddd,'(i4)',err=10)yyyy
        print *,'year=',yyyy
        read(yyyyddd,'(4x,i3)',err=10)ddd
        print *,'julian day=',ddd
     endif
  enddo
!  stop

  do i=1,nrow
!  do i=1,1
     read(iu,"(A6144)")buf
     read(buf,'(6144i1)',err=10)ibuf6144
     if(i .eq. 3072)then
!        print *,'buf=',buf
!        print *,'ibuf6144=',ibuf6144
!        write(*,'(a,6144(i2))')'ibuf6144=',ibuf6144
     endif
! NB: flip IMS grid so that row 1 is at top of grid
! This will also be the convention used in the output ims_sfctyp_mirs array
! and used in the mapping routines that convert lat,lon->i,j (ims_psijll.f90)
     ims_sfctyp(1:ncol,nrow-(i-1),1)=ibuf6144
  enddo
  close(iu)

! convert IMS sfc type indices to MIRS indices
! IMS
! Value Type
! ----- ----
! 0     Missing/Off P.S. grid
! 1     Sea
! 2     Land (no snow)
! 3     Sea Ice
! 4     Snow (on land)

! MiRS
! Value Type
! 0     Sea
! 1     Sea Ice
! 2     Land
! 3     Snow
! -999  Missing

  ims_sfctyp_mirs=-999
  where(ims_sfctyp .eq. 1)
     ims_sfctyp_mirs=0
  else where(ims_sfctyp .eq. 2)
     ims_sfctyp_mirs=2
  else where(ims_sfctyp .eq. 3)
     ims_sfctyp_mirs=1
  else where(ims_sfctyp .eq. 4)
     ims_sfctyp_mirs=3
  end where
  return

10 err=1
  close(iu)
  return

!  write(*,'(a,6144(i2))')'ims_sfctyp(1:6144,3072)=',ims_sfctyp(1:6144,3072)
!  write(*,'(a,6144(i2))')'ims_sfctyp_mirs(1:6144,3072)=',ims_sfctyp_mirs(1:6144,3072)

end subroutine read_ims_sfctyp

!===============================================================
! Name:           ims_psijll
!
! Type:           Subroutine
!
!
! Description:    
!                 This program transforms I,J coordinates of the IMS P.S. grid cell
!                 to latitude and longitude coordinates, or vice-versa
!                 Interfaces to the revised forms of the subroutines, MAPXY and MAPLL.
!
!                 Currently supports IMS P.S. grid at 4 km or 24 km resolution (N.H. only)
!
!                 Based on original code locate.for designed for SSMI data on P.S. grids
!                 and converted to subroutine
!
! Input Arguments:
!
!     gtype   : Integer supplied by the user to describe one of the three
!               grid cell dimensions (4 km, 24 km).
!
!     ihem    : Integer supplied by the user to describe one of the two
!               polar regions (1=North , 2=South)
!
!     itrans  : Integer supplied by the user to describe the type of
!               transformation LOCATE will perform (1=I,J-to-Lat,Lon;
!               2=Lat,Lon-to-I,J)
!
!     i,j     : Integers supplied by the user when itrans = 1. These 
!               integers describe the position of a cell in an SSM/I grid.
!               i=row, j=column
!
!     lat,lon : Reals supplied by the user when itrans = 2. These
!               integers describe the latitude and longitude in an SSM/I
!               grid which LOCATE will transform to an I,J grid cell position.
!               Note: All latitudes and longitudes must be entered as 
!                     positive numbers!
!
!  Output Arguments: 
!  NB only (alat,alon) or (ii,kk) returned with non-missing values, depending on input value of itrans
!
!     alat,
!     alon    : latitude and longitude returned from MAPXY.
!
!     ii      : grid column returned by MAPLL
!     kk      : grid row returned by MAPLL (grid is 'flipped' in the Y direction for transformations)
          

!  Internal:
!
!     x,y     : Distance in kilometers from the origin of the grid 
!               (ie., pole).
!     
!     SGN     : Sign of the latitude (positive = north latitude, 
!               negative = south latitude)
!
!     delta   : Meridian offset for the SSM/I grids (0 degrees for
!               the South Polar grids; 45 degrees for the North Polar
!               grids. 
!
!     kk      : Integer variable used for reorientation of the grid. The
!               grid is 'flipped' in the Y direction for transformations.
!
!     SLAT    : Standard latitude for the SSM/I grids is 70 degrees.
!
!     numy    : Number of lines in an SSM/I grid. This attribute varies
!               for each of the six grids.
!
!     cell    : Size of the SSM/I grid ( 12.5 km, 25.0 km)
!
!     xydist  : Distance from the origin of the grid in the cartesian plane.
!               The x-y coordinates for the edge of the lower left pixel
!               is (3850.0, 5350.0) for the northern grids and 
!               (3950.0, 3950.0) for the southern grids.
!
!     RE      : Radius of the earth in kilometers.
!
!     E       : Eccentricity of the Hughes ellipsoid
!
!     E2      : Eccentricity squared
!
!     PI      : Pi
!
! Modules needed:
!       - None
!
!
! History:
!       10-12-2015      C. Grassotti, NOAA/NESDIS/STAR
!
!===============================================================

        subroutine ims_psijll(gtype,ihem,itrans,lat,lon,i,j,alat,alon,ii,kk,ierr)



        implicit none

        real :: SLAT,E,RE,PI,E2
        real :: alat,alon,x,y
        integer :: ihem, gtype, itrans, kk, ii, jj, i, j
        real :: iif, jjf, kkf
        real :: lat, lon
        real :: SGN, delta
        integer :: numy(2,2)
        real :: cell(2), xydist(2,2)
        integer :: ierr
        data numy / 6144, 6144, 1024, 1024 /
        data cell     / 4.0 , 24.0 /
        data xydist   / 12288.0 , 12288.0 , 12288.0 , 12288. /
      
        SLAT = 60.
        RE = 6378.137
!        E2 = .006693883
        E2 = .00669437999014
        PI = 3.141592654
        E =  sqrt(E2)
!
!        gtype=1: 4.0 km
!        gtype=2: 24.0 km

!        ihem=1: N.H.
!        ihem=2: S.H.

        ierr=0
!       Currently, only N. Hem. supported
        if(ihem .ne. 1)then
           print *,'Error: invalid ihem'
           ierr=-1
           return
        endif
!
! Define the sign and meridian offset (delta) for the SSM/I grids.
!
        if (ihem.eq.1) then
           SGN   = 1.0
           delta = 80.
        else
           SGN   = -1.0
           delta = 0.0
        endif
!
!        itrans=1: Convert I,J to Latitude, Longitude'

! Start translation
!
        if (itrans.eq.1) then
!
! Obtain the I,J position of the grid cell to transform to Latitude 
! and Longitude
!      
! Convert I(row),J(column) pairs to x and y distances from origin. 
! Changed j for i and i for j to be consistant, NAS (11/95).

             x=((j-1)*cell(gtype))-(xydist(1,ihem)-cell(gtype)/2.)
             ! kk=row number starting from BOTTOM
             ! i =row number starting from TOP
             kk=numy(ihem,gtype)-(i-1)           
             print *,'i,kk=',i,kk
             y=((kk-1)*cell(gtype))-(xydist(2,ihem)-cell(gtype)/2.)
!
! Transform x and y distances to latitude and longitude
!
             call mapxy (x,y,alat,alon,SLAT,SGN,E,RE)
!
! Transform radians to degrees.
!
             alon=alon*180./PI
             alat=alat*180./PI
             alon=alon-delta   
!
! Convert longitude to positive degrees
!
             if (alon.le.0.0) alon=alon+360.
             if (alon.ge.360.0) alon=alon-360.
!
! Print the latitude and longitude for the center of the I,J cell.
!
             print *,alat,alon

       else
!      itrans=2: Convert Latitude, Longitude to column, row
!
! Obtain the latitude and longitude pair and transform to cell where
! that pair is located.
!

! Transform degrees to radians
!
             alat=abs(lat)*PI/180.
             alon=(lon+delta)*PI/180.
!
! Transform latitude and longitude to x and y distances from origin
!
           call mapll (x,y,alat,alon,SLAT,SGN,E,RE)
!
! Convert x and y distances from origin to I,J pair (ii,jj)
!
             ii=nint((x+xydist(1,ihem)-cell(gtype)/2.)/cell(gtype))+1
             jj=nint((y+xydist(2,ihem)-cell(gtype)/2.)/cell(gtype))+1
             iif=((x+xydist(1,ihem)-cell(gtype)/2.)/cell(gtype))+1
             jjf=((y+xydist(2,ihem)-cell(gtype)/2.)/cell(gtype))+1
!
! Flip grid orientation in the 'Y' direction
! 
             kk=numy(ihem,gtype)-(jj-1)
             kkf=numy(ihem,gtype)-(jjf-1)
!             kkf=numy(ihem,gtype)-(jjf)
!
! Print the I,J location of the cell.
! ii=column
! kk=row
! Convention: upper left corner is (0,0)
             ! kk=row number starting from TOP
             ! jj=row number starting from BOTTOM
!             print *,ii,kk,jj
!             print *,iif,kkf

          endif
        end subroutine ims_psijll

      SUBROUTINE MAPLL (X,Y,ALAT,ALONG,SLAT,SGN,E,RE)

        implicit none
!******************************************************************************
!*                                                                            *
!*                                                                            *
!*    DESCRIPTION:                                                            *
!*                                                                            *
!*    This subroutine converts from geodetic latitude and longitude to Polar  *
!*    Stereographic (X,Y) coordinates for the polar regions.  The equations   *
!*    are from Snyder, J. P., 1982,  Map Projections Used by the U.S.         *
!*    Geological Survey, Geological Survey Bulletin 1532, U.S. Government     *
!*    Printing Office.  See JPL Technical Memorandum 3349-85-101 for further  *
!*    details.                                                                *
!*                                                                            *
!*                                                                            *
!*    ARGUMENTS:                                                              *
!*                                                                            *
!*    Variable    Type        I/O    Description                              *
!*                                                                            *
!*    ALAT       REAL*4        I     Geodetic Latitude (degrees, +90 to -90)  *
!*    ALONG      REAL*4        I     Geodetic Longitude (degrees, 0 to 360)   *
!*    X          REAL*4        O     Polar Stereographic X Coordinate (km)    *
!*    Y          REAL*4        O     Polar Stereographic Y Coordinate (km)    *
!*                                                                            *
!*                                                                            *
!*                  Written by C. S. Morris - April 29, 1985                  *
!*                  Revised by C. S. Morris - December 11, 1985               *
!*                                                                     	      *
!*                  Revised by V. J. Troisi - January 1990                    *
!*                  SGN - provides hemisphere dependency (+/- 1)              *
!*		    Revised by Xiaoming Li - October 1996                     *
!*		    Corrected equation for RHO                                *
!******************************************************************************
        REAL :: X,Y,ALAT,ALONG,E,E2,CDR,PI,SLAT,MC,RHO,SL,TC,T,SGN,RE
!******************************************************************************
!*                                                                            *
!*    DEFINITION OF CONSTANTS:                                                *
!*                                                                            *
!*    Conversion constant from degrees to radians = 57.29577951.              *
      CDR=57.29577951
      E2=E*E
!*    Pi=3.141592654.                                                         *
      PI=3.141592654
!*                                                                            *
!******************************************************************************
!     Compute X and Y in grid coordinates.
      IF (ABS(ALAT).LT.PI/2.) GOTO 250
      X=0.0
      Y=0.0
      GOTO 999
  250 CONTINUE
      T=TAN(PI/4.-ALAT/2.)/((1.-E*SIN(ALAT))/(1.+E*SIN(ALAT)))**(E/2.)
      IF (ABS(90.-SLAT).LT.1.E-5) THEN
      RHO=2.*RE*T/((1.+E)**(1.+E)*(1.-E)**(1.-E))**(1/2.)
      ELSE
      SL=SLAT*PI/180.
      TC=TAN(PI/4.-SL/2.)/((1.-E*SIN(SL))/(1.+E*SIN(SL)))**(E/2.)
      MC=COS(SL)/SQRT(1.0-E2*(SIN(SL)**2))
      RHO=RE*MC*T/TC
      END IF
      Y=-RHO*SGN*COS(SGN*ALONG)
      X= RHO*SGN*SIN(SGN*ALONG)
  999 CONTINUE
    END SUBROUTINE MAPLL

      SUBROUTINE MAPXY (X,Y,ALAT,ALONG,SLAT,SGN,E,RE)

        implicit none

!******************************************************************************
!*                                                                            *
!*                                                                            *
!*    DESCRIPTION:                                                            *
!*                                                                            *
!*    This subroutine converts from Polar Stereographic (X,Y) coordinates     *
!*    to geodetic latitude and longitude for the polar regions. The equations *
!*    are from Snyder, J. P., 1982,  Map Projections Used by the U.S.         *
!*    Geological Survey, Geological Survey Bulletin 1532, U.S. Government     *
!*    Printing Office.  See JPL Technical Memorandum 3349-85-101 for further  *
!*    details.                                                                *
!*                                                                            *
!*                                                                            *
!*    ARGUMENTS:                                                              *
!*                                                                            *
!*    Variable    Type        I/O    Description                              *
!*                                                                            *
!*    X          REAL*4        I     Polar Stereographic X Coordinate (km)    *
!*    Y          REAL*4        I     Polar Stereographic Y Coordinate (km)    *
!*    ALAT       REAL*4        O     Geodetic Latitude (degrees, +90 to -90)  *
!*    ALONG      REAL*4        O     Geodetic Longitude (degrees, 0 to 360)   *
!*                                                                            *
!*                                                                            *
!*                  Written by C. S. Morris - April 29, 1985                  *
!*                  Revised by C. S. Morris - December 11, 1985               *
!*                                                                            *
!*                  Revised by V. J. Troisi - January 1990
!*                  SGN - provide hemisphere dependency (+/- 1)
!*
!******************************************************************************
      REAL ::  X,Y,ALAT,ALONG,E,E2,CDR,PI,CM,T,RHO,CHI,SLAT,SL,SGN,RE

!******************************************************************************
!*                                                                            *
!*    DEFINITION OF CONSTANTS:                                                *
!*                                                                            *
!*    Conversion constant from degrees to radians = 57.29577951.              *
      CDR=57.29577951
      E2=E*E
!*    Pi=3.141592654.                                                         *
      PI=3.141592654
!*                                                                            *
!******************************************************************************
      SL = SLAT*PI/180.
      RHO=SQRT(X**2+Y**2)
      IF (RHO.GT.0.1) GOTO 250
      ALAT=90.*SGN
      ALONG=0.0
      GOTO 999
  250 CM=COS(SL)/SQRT(1.0-E2*(SIN(SL)**2))
      T=TAN((PI/4.0)-(SL/(2.0)))/((1.0-E*SIN(SL))/ &
        (1.0+E*SIN(SL)))**(E/2.0)
      IF (ABS(SLAT-90.).LT.1.E-5) THEN
      T=RHO*SQRT((1.+E)**(1.+E)*(1.-E)**(1.-E))/2./RE
      ELSE
      T=RHO*T/(RE*CM)
      END IF
      CHI=(PI/2.0)-2.0*ATAN(T)
      ALAT=CHI+((E2/2.0)+(5.0*E2**2.0/24.0)+(E2**3.0/12.0))*SIN(2*CHI)+&
           ((7.0*E2**2.0/48.0)+(29.0*E2**3/240.0))*SIN(4.0*CHI)+&
           (7.0*E2**3.0/120.0)*SIN(6.0*CHI)
      ALAT=SGN*ALAT
      ALONG=ATAN2(SGN*X,-SGN*Y)
      ALONG=SGN*ALONG
  999 CONTINUE
    END SUBROUTINE MAPXY
!
!
  end Program imsColloc
