!===============================================================
! Name:       footprintMatching
!
!
! Type:         Main Program
!
!
! Description:
!       Performs footprint matching of the two MW
!       instruments onboard NOAA-18, namely AMSUA & MHS
!
! Modules needed:
!       - IO_MeasurData
!       - misc
!       - ErrorHandling
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!       02-01-2015      Craig Kenton Smith, AER@NOAA/STAR
!                       (1) Cleaned of all TB adjustments that tried to make
!                       AMSU measurements match MHS measurements.
!                       (2) For High Resolution only, added interpolation of 
!                       AMSU measurements to MHS footprint locations, in the 
!                       interior of the MHS scan (scan positions 2 through 89), 
!                       and in the interior of a contiguous group of valid MHS 
!                       and AMSU scans.  The variable interp_AMSU must be set
!                       to .TRUE. for this to occur. It does not do extrapolation 
!                       to MHS scan positions 1 and 90, or to the first and
!                       last MHS scan in a contiguous group.
!===============================================================

program fm_metopB
  USE IO_MeasurData
  USE IO_Misc
  USE misc  
  USE ErrorHandling
  USE Consts
  IMPLICIT NONE
  !---INTRINSIC functions used in this module
  INTRINSIC :: ABS,ASSOCIATED,MAXVAL,MINVAL,NINT,RESHAPE,SUM,TRIM,ANY,COUNT

  LOGICAL,            PARAMETER  :: interp_AMSU=.TRUE. ! Interpolation of AMSU to MHS
  INTEGER,            PARAMETER  :: ExpdAMSUAfov=30,ExpdMHSfov=90
  INTEGER,            PARAMETER  :: iu_Check=40,np=11
  INTEGER,            PARAMETER  :: FMautoORmanual = 1 !0:Automatic, 1:Manual
  INTEGER,            PARAMETER  :: maxMSallowed=100 !ms
  REAL,               PARAMETER  :: mxdlatAllowed=0.5,mxdangAllowed=0.9

  CHARACTER(LEN=250), DIMENSION(:),    POINTER     :: sdrFilesAMSUA,sdrFilesMHS
  CHARACTER(LEN=250), DIMENSION(:),    POINTER     :: FMsdrFiles
  REAL,               DIMENSION(:),    POINTER     :: Cfreq_1,Cfreq_2
  INTEGER,            DIMENSION(:),    POINTER     :: pol_1,pol_2
  REAL,               DIMENSION(:),    ALLOCATABLE :: angleFM,Cfreq
  REAL,               DIMENSION(:),    ALLOCATABLE :: lat_1,lon_1,angle_1,RelAziAngle_1,SolZenAngle_1
  REAL,               DIMENSION(:,:),  ALLOCATABLE :: lat_2,lon_2,angle_2,RelAziAngle_2,SolZenAngle_2
  REAL,               DIMENSION(:,:),  ALLOCATABLE :: tb_1
  REAL,               DIMENSION(:,:,:),ALLOCATABLE :: tb,tb_2
  INTEGER,            DIMENSION(:),    ALLOCATABLE :: qc,qc_1,pol
  INTEGER,            DIMENSION(:,:),  ALLOCATABLE :: qc_2
  REAL,               DIMENSION(:,:,:),ALLOCATABLE :: FMerr
  INTEGER,            DIMENSION(:),    ALLOCATABLE :: ScanLineInROI
  
  ! For interpolation of AMSUto MHS at iFMType=1 only
  REAL, DIMENSION(:), ALLOCATABLE   :: dLat_arr
  REAL, DIMENSION(:), ALLOCATABLE   :: dLon_arr
  REAL, DIMENSION(:), ALLOCATABLE   :: dAng_arr

  CHARACTER(LEN=2)  :: ext
  INTEGER    :: iu_sdr_1,iu_sdr_2,iu_FMsdr,nScanL_mhs_2skip0,nScanL_amsua_2skip0
  INTEGER    :: nScanL_mhs_2skip_requested,nScanL_amsua_2skip_requested
  INTEGER    :: nfilesAMSUA,nfilesMHS,nFiles,iFile,nChan,nqc,nFovs,nScanL,nBatch
  INTEGER    :: nScanL_1,nFovs_1,nqc_1,nchan_1,nScanL_2,nFovs_2,nqc_2,nchan_2,nScanL_1_0
  INTEGER    :: iscanline,i,ifov,ichan,node_1,node_2(3),nScanLines,jscanline
  INTEGER    :: scanDAY_1,scanYear_1,scanDAY_2(3),scanYear_2(3),Day_mhs,Yr_mhs,dDay,dYr
  INTEGER    :: scanUTC_1,scanUTC_2(3),UTC_mhs,dUTC,dUTC0
  INTEGER    :: scanDAY,scanYear,scanUTC,iu_listAMSUA,iu_listMHS
  REAL       :: lat_mhs,lon_mhs,ang_mhs,dLat,dLon,dAng,xVec(9)
  INTEGER    :: WRITE_HEADER_DONE=0
  INTEGER    :: nProcessScanLines=0
  INTEGER    :: nCountScanLinesROI=0 ! record individual file's number of scans falling in ROI
  INTEGER    :: TotalScanLinesROI=0  ! record total number of scans of all files falling in ROI
  INTEGER    :: dUTC_min=99999,nScanL_amsua_2skip0_min=0,nScanL_mhs_2skip0_min=0 

  ! Buffer Structure Definitions for AMSU Interpolation (FMType=1)

  INTEGER :: Index_BUF

  TYPE :: Input_Buffer_Type
     LOGICAL :: Is_Allocated = .FALSE.
     ! Dimension Values
     INTEGER :: nFovs_1 = 0
     INTEGER :: nchan_1 = 0
     INTEGER :: nFovs_2 = 0
     INTEGER :: nchan_2 = 0
     INTEGER :: nqc     = 0
     ! Allocated Variables
     REAL,    DIMENSION(:,:,:), ALLOCATABLE :: tb
     REAL,    DIMENSION(:,:),   ALLOCATABLE :: tb_1
     REAL,    DIMENSION(:,:,:), ALLOCATABLE :: tb_2
     INTEGER, DIMENSION(:),     ALLOCATABLE :: qc
     REAL,    DIMENSION(:,:),   ALLOCATABLE :: angle_2
     REAL,    DIMENSION(:,:),   ALLOCATABLE :: lat_2
     REAL,    DIMENSION(:,:),   ALLOCATABLE :: lon_2
     REAL,    DIMENSION(:,:),   ALLOCATABLE :: RelAziAngle_2
     REAL,    DIMENSION(:,:),   ALLOCATABLE :: SolZenAngle_2
     ! Dimensioned Variables
     INTEGER, DIMENSION(3) :: node_2
     INTEGER, DIMENSION(3) :: scanUTC_2
     INTEGER, DIMENSION(3) :: scanDAY_2
     INTEGER, DIMENSION(3) :: scanYear_2
     ! Scalar Values used for comparison with previous obs for continuity
     INTEGER :: scanDAY_1
     INTEGER :: scanUTC_1
     ! Used for indexing obs during ROI test and FMerr computation
     INTEGER    :: iscanline
     ! Additional Data used for filling in FMERR
     REAL, DIMENSION(:), ALLOCATABLE   :: dLat_arr
     REAL, DIMENSION(:), ALLOCATABLE   :: dLon_arr
     REAL, DIMENSION(:), ALLOCATABLE   :: dAng_arr
     INTEGER    :: node_1
     INTEGER    :: Day_mhs
  END TYPE Input_Buffer_Type

  TYPE(Input_Buffer_Type) :: BUF1, BUF2, BUF3, BUFN, BUFdummy ! 3 Element Buffer for AMSU Interp, plus input and dummy

  !Logicals used in interpolation of AMSU to MHS
  LOGICAL  :: match_UTC, match_lat, match_ang, match, contig_scanline, lastLine

  !---Namelist data 
  CHARACTER(LEN=250) :: sdrfileList_amsua=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: sdrfileList_mhs=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: pathFMSDR=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: prefCheck=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: LogFile=DEFAULT_VALUE_STR4
  INTEGER            :: iOutpFM_accur=DEFAULT_VALUE_INT
  INTEGER            :: iFMtype=DEFAULT_VALUE_INT
  INTEGER            :: nScanL_mhs_2skip=DEFAULT_VALUE_INT
  INTEGER            :: nScanL_amsua_2skip=DEFAULT_VALUE_INT
  INTEGER            :: ScanLindx_mhs_TimeColloc=DEFAULT_VALUE_INT
  INTEGER            :: norbits2process=DEFAULT_VALUE_INT
  INTEGER            :: GeogrLimits=DEFAULT_VALUE_INT
  REAL               :: minLat=DEFAULT_VALUE_REAL
  REAL               :: maxLat=DEFAULT_VALUE_REAL
  REAL               :: minLon=DEFAULT_VALUE_REAL
  REAL               :: maxLon=DEFAULT_VALUE_REAL

  NAMELIST /ContrlFM/sdrfileList_amsua,sdrfileList_mhs,pathFMSDR,iOutpFM_accur,&
       prefCheck,iFMtype,nScanL_amsua_2skip,nScanL_mhs_2skip,ScanLindx_mhs_TimeColloc,&
       norbits2process,LogFile,GeogrLimits,minLat,maxLat,minLon,maxLon

  !---Read control-data from namelist
  READ(*,NML=ContrlFM)
  !---Prepare Log file
  CALL OpenLogFile(Logfile)
  !---Read the file names of AMSUA SDRs
  call ReadList(iu_listAMSUA,trim(sdrfileList_amsua),sdrFilesAMSUA,nFilesAMSUA,&
       FMsdrFiles,pathFMSDR,'FMSDR_')
  DEALLOCATE(FMsdrFiles)
  IF (nfilesAMSUA .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'AMSUA') 
  !---Read the file names of MHS SDRs
  call ReadList(iu_listMHS,trim(sdrfileList_mhs),sdrFilesMHS,nFilesMHS,&
       FMsdrFiles,pathFMSDR,'FMSDR_')
  IF (nfilesMHS .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'MHS')
  !-------------------------------------------------------------------------------
  !   Consistency checks
  !-------------------------------------------------------------------------------
  IF (nfilesAMSUA.ne.nfilesMHS) CALL ErrHandl(ErrorType,Err_InconsNumber,'of files AMSUA/MHS ')
  nFiles=minval((/nfilesAMSUA,norbits2process/))
  !-------------------------------------------------------------------------------
  !   Add extension (HR or LR) depending on the resolution chosen)
  !-------------------------------------------------------------------------------
  DO ifile=1,nFiles
     IF (iFMtype.eq.0) FMsdrFiles(ifile)=trim(FMsdrFiles(ifile))//'.LR'
     IF (iFMtype.eq.1) FMsdrFiles(ifile)=trim(FMsdrFiles(ifile))//'.HR'
  ENDDO
  !-------------------------------------------------------------------------------
  !   Begin FM process
  !-------------------------------------------------------------------------------
  nScanL_mhs_2skip_requested   = nScanL_mhs_2skip
  nScanL_amsua_2skip_requested = nScanL_amsua_2skip
  TotalScanLinesROI=0
  FilesLoop: DO ifile=1,nFiles
     WRITE_HEADER_DONE=0
     write(*,*)
     write(*,'(A)')'Input AMSUA file='//trim(sdrFilesAMSUA(ifile))
     write(*,'(A)')'Input MHS file='//trim(sdrFilesMHS(ifile))
     !---Open/Read AMSU/A SDR header
     CALL ReadRadHdrScanLMode(sdrFilesAMSUA(ifile),iu_sdr_1,nScanL_1,nFovs_1, &
          nqc_1,nchan_1,CFreq_1,pol_1)
     write(*,*)'AMSUA scan=',nScanL_1, ', AMSUA nfov=', nFovs_1, ', AMSUA nprf=', nScanL_1*nFovs_1  
     !---Open/Read MHS SDR header
     CALL ReadRadHdrScanLMode(sdrFilesMHS(ifile),iu_sdr_2,nScanL_2,nFovs_2,   &
          nqc_2,nchan_2,CFreq_2,pol_2)
     write(*,*)'MHS scan=',nScanL_2, ', MHS nfov=', nFovs_2, ', MHS nprf=', nScanL_2*nFovs_2  
     !---Consistency checks
     IF (nFovs_1.ne.ExpdAMSUAfov) CALL ErrHandl(ErrorType,Err_DifferFromExpect,'AMSUA nFOV') 
     IF (nFovs_2.ne.ExpdMHSfov)   CALL ErrHandl(ErrorType,Err_DifferFromExpect,'MHS nFOV') 
     !---Allocate memory for vectors/arrays
     nChan           = nChan_1+nChan_2
     nqc             = nqc_1+nqc_2+1
     !---FM at AMSU A resolution
     IF (iFMtype .eq. 0) THEN
        nScanL          = minval((/nScanL_1,nint(nScanL_2/3.)/))
        nFovs           = nFovs_1
        nBatch          = 1
        !----test
        nScanL=nScanL-1
        !--------
        nscanL_1        = nScanL
     ENDIF
     !---FM at MHS resolution
     IF (iFMtype .eq. 1) THEN
        nScanL          = minval((/(nScanL_1*3),(nint(nScanL_2/3.)*3)/))
        nFovs           = nFovs_2
        nBatch          = 3
        !----test
        nScanL=nScanL-3
        !--------
        nscanL_1        = nScanL/3
     ENDIF
     ALLOCATE(angleFM(nchan),tb(nFovs,nchan,nBatch),qc(nqc),&
          lat_1(nFovs_1),lon_1(nFovs_1),angle_1(nFovs_1),tb_1(nFovs_1,nchan_1),qc_1(nqc_1),    &
          lat_2(nFovs_2,3),lon_2(nFovs_2,3),angle_2(nFovs_2,3),tb_2(nFovs_2,nchan_2,3),        &
          qc_2(nqc_2,3),Cfreq(nChan),pol(nChan),FMerr(nScanL,nFovs,np),                        &
          RelAziAngle_1(nFovs_1),SolZenAngle_1(nFovs_1), &
          RelAziAngle_2(nFovs_2,3),SolZenAngle_2(nFovs_2,3))
     Cfreq(1:nChan)  = (/Cfreq_1(1:nchan_1),Cfreq_2(1:nchan_2)/)
     pol(1:nChan)    = (/pol_1(1:nchan_1),pol_2(1:nchan_2)/)

     IF (interp_AMSU .AND. (iFMtype .eq. 1)) then

        ALLOCATE (dLat_arr(nFovs_1), dLon_arr(nFovs_1), dAng_arr(nFovs_1))
               
        ALLOCATE(BUF1%tb(nFovs,nchan,nBatch),BUF1%tb_1(nFovs_1,nchan_1), BUF1%tb_2(nFovs_2,nchan_2,3), BUF1%qc(nqc), &
             BUF1%angle_2(nFovs_2,3), BUF1%lat_2(nFovs_2,3), BUF1%lon_2(nFovs_2,3), BUF1%RelAziAngle_2(nFovs_2,3), &
             BUF1%SolZenAngle_2(nFovs_2,3),BUF1%dLat_arr(nFovs_1),BUF1%dLon_arr(nFovs_1), BUF1%dAng_arr(nFovs_1))

        ALLOCATE(BUF2%tb(nFovs,nchan,nBatch),BUF2%tb_1(nFovs_1,nchan_1), BUF2%tb_2(nFovs_2,nchan_2,3), BUF2%qc(nqc), &
             BUF2%angle_2(nFovs_2,3), BUF2%lat_2(nFovs_2,3), BUF2%lon_2(nFovs_2,3), BUF2%RelAziAngle_2(nFovs_2,3), &
             BUF2%SolZenAngle_2(nFovs_2,3),BUF2%dLat_arr(nFovs_1),BUF2%dLon_arr(nFovs_1), BUF2%dAng_arr(nFovs_1))

        ALLOCATE(BUF3%tb(nFovs,nchan,nBatch),BUF3%tb_1(nFovs_1,nchan_1), BUF3%tb_2(nFovs_2,nchan_2,3), BUF3%qc(nqc), &
             BUF3%angle_2(nFovs_2,3), BUF3%lat_2(nFovs_2,3), BUF3%lon_2(nFovs_2,3), BUF3%RelAziAngle_2(nFovs_2,3), &
             BUF3%SolZenAngle_2(nFovs_2,3),BUF3%dLat_arr(nFovs_1),BUF3%dLon_arr(nFovs_1), BUF3%dAng_arr(nFovs_1))

        ALLOCATE(BUFN%tb(nFovs,nchan,nBatch),BUFN%tb_1(nFovs_1,nchan_1), BUFN%tb_2(nFovs_2,nchan_2,3), BUFN%qc(nqc), &
             BUFN%angle_2(nFovs_2,3), BUFN%lat_2(nFovs_2,3), BUFN%lon_2(nFovs_2,3), BUFN%RelAziAngle_2(nFovs_2,3), &
             BUFN%SolZenAngle_2(nFovs_2,3),BUFN%dLat_arr(nFovs_1),BUFN%dLon_arr(nFovs_1), BUFN%dAng_arr(nFovs_1))

        ALLOCATE(BUFdummy%tb(nFovs,nchan,nBatch),BUFdummy%tb_1(nFovs_1,nchan_1), BUFdummy%tb_2(nFovs_2,nchan_2,3), &
             BUFdummy%qc(nqc), BUFdummy%angle_2(nFovs_2,3), BUFdummy%lat_2(nFovs_2,3), BUFdummy%lon_2(nFovs_2,3), &
             BUFdummy%RelAziAngle_2(nFovs_2,3), BUFdummy%SolZenAngle_2(nFovs_2,3),BUFdummy%dLat_arr(nFovs_1), &
             BUFdummy%dLon_arr(nFovs_1), BUFdummy%dAng_arr(nFovs_1))

     ENDIF

     !---Determine how much scanlines to skip to time-synchronize AMSUA and MHS
     nScanL_amsua_2skip0 = 0
     dUTC                = 9999
     dUTC_min            = 99999
     !---Always skip first scanline of AMSU-A
     CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
          tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
     nScanL_amsua_2skip0 = nScanL_amsua_2skip0+1
     !---Loop over scanlines
     AMSUALoop: DO WHILE (nScanL_amsua_2skip0 .le. nScanL_1-1) 
        CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
             tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
        nScanL_amsua_2skip0 = nScanL_amsua_2skip0+1
        nScanL_mhs_2skip0   = 0
        MHSLoop: DO WHILE (nScanL_mhs_2skip0 .le. nScanL_2-1) 
           CALL ReadRadMeasScanLMode(iu_sdr_2,nqc_2,qc_2(:,1),nchan_2,nFovs_2,angle_2(:,1),&
                tb_2(:,:,1),lat_2(:,1),lon_2(:,1),node_2(1),scanUTC_2(1),scanDAY_2(1),     &
                scanYear_2(1),RelAziAngle_2(:,1),SolZenAngle_2(:,1))
           IF (scanDAY_2(1).ne.scanDAY_1) scanUTC_2(1)=scanUTC_2(1)-86400000
           nScanL_mhs_2skip0=nScanL_mhs_2skip0+1
           dUTC=abs(scanUTC_1-scanUTC_2(1))
           dLat=abs(Lat_1(1)-Lat_2(1,1))
           IF (dUTC .lt. dUTC_min) THEN
              dUTC_min = dUTC
              nScanL_amsua_2skip0_min = nScanL_amsua_2skip0
              nScanL_mhs_2skip0_min = nScanL_mhs_2skip0 
           ENDIF
           IF (dUTC .le. maxMSallowed) EXIT AMSUALoop
        ENDDO MHSLoop
        close(iu_sdr_2)
        CALL ReadRadHdrScanLMode(sdrFilesMHS(ifile),iu_sdr_2,nScanL_2,nFovs_2,   &
             nqc_2,nchan_2,CFreq_2,pol_2)
     ENDDO AMSUALoop
     close(iu_sdr_1)
     close(iu_sdr_2)

     nScanL_amsua_2skip0 = nScanL_amsua_2skip0_min
     nScanL_mhs_2skip0   = nScanL_mhs_2skip0_min

     !---Rewind files and re-open their headers and skip the computed number of scanlines 
     CALL ReadRadHdrScanLMode(sdrFilesAMSUA(ifile),iu_sdr_1,nScanL_1_0,nFovs_1,nqc_1,nchan_1,CFreq_1,pol_1)
     CALL ReadRadHdrScanLMode(sdrFilesMHS(ifile),iu_sdr_2,nScanL_2,nFovs_2,nqc_2,nchan_2,CFreq_2,pol_2)

     !-----------------------------------------------------------------
     !     Manual Shift of scanlines
     !-----------------------------------------------------------------
     IF (FMautoORmanual .eq. 1) THEN
        nScanL_amsua_2skip = nScanL_amsua_2skip0 + nScanL_amsua_2skip_requested
        nScanL_mhs_2skip   = nScanL_mhs_2skip0   + nScanL_mhs_2skip_requested
     ENDIF

     !-----------------------------------------------------------------
     !     Automatic Shift of scanlines (based on time synchronzation only)
     !-----------------------------------------------------------------
     IF (FMautoORmanual .eq. 0) THEN 
        nScanL_amsua_2skip = nScanL_amsua_2skip0 
        nScanL_mhs_2skip   = nScanL_mhs_2skip0  
     ENDIF

     nScanL=nScanL-maxval((/nint(nScanL_mhs_2skip/3.),nScanL_amsua_2skip/))-1

     !---shift MHS by X scan lines 
     DO i=1,nScanL_mhs_2skip
        CALL ReadRadMeasScanLMode(iu_sdr_2,nqc_2,qc_2(:,1),nchan_2,nFovs_2,angle_2(:,1),&
             tb_2(:,:,1),lat_2(:,1),lon_2(:,1),node_2(1),scanUTC_2(1),scanDAY_2(1),     &
             scanYear_2(1),RelAziAngle_2(:,1),SolZenAngle_2(:,1))
        IF (scanDAY_2(1).ne.scanDAY_1) scanUTC_2(1)=scanUTC_2(1)-86400000
     ENDDO
     !---shift AMSUA by X scan lines
     DO i=1,nScanL_amsua_2skip
        CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
             tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
     ENDDO

     !---Open/Write FM-SDR header
     FmErr      = -99
     !---This is AMUSA scan lines !!!
     nScanLines = nScanL_1-maxval((/nint(nScanL_mhs_2skip/3.),nScanL_amsua_2skip/))-1

     !------------------------------------------------------------------------------
     !   Loop over scanlines to determine whether scanline falls in Region of Interest
     !------------------------------------------------------------------------------
     ALLOCATE(ScanLineInROI(nScanLines))
     ScanLineInROI(:) = 0
     nProcessScanLines = 0
     ScanLLoopFlag: DO iscanLine=1,nScanLines  ! loop over AMSUA scan lines
        !---Read AMSUA SDR scanline content
        CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
             tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
        !---Read 3 MHS SDR consecutive scanlines content
        DO i=1,3
           CALL ReadRadMeasScanLMode(iu_sdr_2,nqc_2,qc_2(:,i),nchan_2,nFovs_2,angle_2(:,i),&
                tb_2(:,:,i),lat_2(:,i),lon_2(:,i),node_2(i),scanUTC_2(i),scanDAY_2(i),     &
                scanYear_2(i),RelAziAngle_2(:,i),SolZenAngle_2(:,i))
        ENDDO

        IF (GeogrLimits .eq. 0) THEN
           ScanLineInROI(iscanLine) = 1
           if( iFMtype .eq. 0 ) then
              nProcessScanLines = nProcessScanLines + 1
           else if( iFMtype .eq. 1 ) then
              nProcessScanLines = nProcessScanLines + 3
           endif
        ELSE IF (GeogrLimits .eq. 1  .and. iFMtype .eq. 0) THEN
           GeoLoop1: DO ifov=1,nFovs
              IF( lat_1(ifov) .ge. minLat .and. lat_1(ifov) .le. maxLat .and. &
                   lon_1(ifov) .ge. minLon .and. lon_1(ifov) .le. maxLon) THEN
                 ScanLineInROI(iscanLine) = 1
                 nProcessScanLines = nProcessScanLines + 1
                 exit GeoLoop1
              ENDIF
           ENDDO GeoLoop1
        ELSE IF (GeogrLimits .eq. 1  .and. iFMtype .eq. 1) THEN
           DO i=1,nBatch
              GeoLoop2: DO ifov=1,nFovs
                 IF( lat_2(ifov,i) .ge. minLat .and. lat_2(ifov,i) .le. maxLat .and. &
                      lon_2(ifov,i) .ge. minLon .and. lon_2(ifov,i) .le. maxLon) THEN
                    ScanLineInROI(iscanLine) = 1 
                    nProcessScanLines = nProcessScanLines + 1
                    exit GeoLoop2
                 ENDIF
              ENDDO GeoLoop2
           ENDDO
        ELSE
           CALL ErrHandl(ErrorType,Err_OptionNotSupported,'(in FM)') 
        ENDIF
     ENDDO ScanLLoopFlag

     close(iu_sdr_1)
     close(iu_sdr_2)

     !---- if nCountScanLinesROI < 1, no scanline falls in ROI, move on to the next file ----
     nCountScanLinesROI = COUNT( ScanLineInROI .eq. 1 )
     if( nCountScanLinesROI .lt. 1 ) then
        !print *, 'No scanline found in the ROI!'
        !print *, 'No FMSDR data is generated'
        CALL ErrHandl(WarningType,Warn_NotCoverROI,'orbit: '//trim(FMsdrFiles(ifile)) )

        DEALLOCATE(angleFM,tb,qc,lat_1,lon_1,angle_1,tb_1,qc_1,lat_2,&
             lon_2,angle_2,tb_2,qc_2,Cfreq,pol,FMerr,&
             RelAziAngle_1,SolZenAngle_1,RelAziAngle_2,SolZenAngle_2)

        DEALLOCATE(ScanLineInROI)

        IF (interp_AMSU .AND. (iFMtype .eq. 1)) then

           DEALLOCATE(dLat_arr, dLon_arr, dAng_arr)

           DEALLOCATE(BUF1%tb, BUF1%tb_1, BUF1%tb_2, BUF1%qc, BUF1%angle_2, &
                BUF1%lat_2, BUF1%lon_2, BUF1%RelAziAngle_2, &
                BUF1%SolZenAngle_2, BUF1%dLat_arr, BUF1%dLon_arr, BUF1%dAng_arr)

           DEALLOCATE(BUF2%tb, BUF2%tb_1, BUF2%tb_2, BUF2%qc, BUF2%angle_2, &
                BUF2%lat_2, BUF2%lon_2, BUF2%RelAziAngle_2, &
                BUF2%SolZenAngle_2, BUF2%dLat_arr, BUF2%dLon_arr, BUF2%dAng_arr)

           DEALLOCATE(BUF3%tb, BUF3%tb_1, BUF3%tb_2, BUF3%qc, BUF3%angle_2, &
                BUF3%lat_2, BUF3%lon_2, BUF3%RelAziAngle_2, &
                BUF3%SolZenAngle_2, BUF3%dLat_arr, BUF3%dLon_arr, BUF3%dAng_arr)

           DEALLOCATE(BUFN%tb, BUFN%tb_1, BUFN%tb_2, BUFN%qc, BUFN%angle_2, &
                BUFN%lat_2, BUFN%lon_2, BUFN%RelAziAngle_2, &
                BUFN%SolZenAngle_2, BUFN%dLat_arr, BUFN%dLon_arr, BUFN%dAng_arr)

           DEALLOCATE(BUFdummy%tb, BUFdummy%tb_1, BUFdummy%tb_2, BUFdummy%qc, BUFdummy%angle_2, &
                BUFdummy%lat_2, BUFdummy%lon_2, BUFdummy%RelAziAngle_2, &
                BUFdummy%SolZenAngle_2, BUFdummy%dLat_arr, BUFdummy%dLon_arr, BUFdummy%dAng_arr)

        ENDIF

        CYCLE FilesLoop
        !CALL ErrHandl(ErrorType,Err_NoFmsdrGenerated,'No FMSDR is generated!' )
     endif

     TotalScanLinesROI = TotalScanLinesROI + nCountScanLinesROI

     !---Rewind files and re-open their headers and skip the computed number of scanlines 
     CALL ReadRadHdrScanLMode(sdrFilesAMSUA(ifile),iu_sdr_1,nScanL_1_0,nFovs_1,nqc_1,nchan_1,CFreq_1,pol_1)
     CALL ReadRadHdrScanLMode(sdrFilesMHS(ifile),iu_sdr_2,nScanL_2,nFovs_2,nqc_2,nchan_2,CFreq_2,pol_2)

     !-----------------------------------------------------------------
     !     Manual Shift of scanlines
     !-----------------------------------------------------------------
     IF (FMautoORmanual .eq. 1) THEN
        nScanL_amsua_2skip = nScanL_amsua_2skip0 + nScanL_amsua_2skip_requested
        nScanL_mhs_2skip   = nScanL_mhs_2skip0   + nScanL_mhs_2skip_requested
     ENDIF

     !-----------------------------------------------------------------
     !     Automatic Shift of scanlines (based on time synchronzation only)
     !-----------------------------------------------------------------
     IF (FMautoORmanual .eq. 0) THEN 
        nScanL_amsua_2skip = nScanL_amsua_2skip0 
        nScanL_mhs_2skip   = nScanL_mhs_2skip0  
     ENDIF

     nScanL=nScanL-maxval((/nint(nScanL_mhs_2skip/3.),nScanL_amsua_2skip/))-1

     !---shift MHS by X scan lines 
     DO i=1,nScanL_mhs_2skip
        CALL ReadRadMeasScanLMode(iu_sdr_2,nqc_2,qc_2(:,1),nchan_2,nFovs_2,angle_2(:,1),&
             tb_2(:,:,1),lat_2(:,1),lon_2(:,1),node_2(1),scanUTC_2(1),scanDAY_2(1),     &
             scanYear_2(1),RelAziAngle_2(:,1),SolZenAngle_2(:,1))
        IF (scanDAY_2(1).ne.scanDAY_1) scanUTC_2(1)=scanUTC_2(1)-86400000
     ENDDO
     !---shift AMSUA by X scan lines
     DO i=1,nScanL_amsua_2skip
        CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
             tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
     ENDDO

     !---Open/Write FM-SDR header
     FmErr      = -99
     nScanLines = nScanL_1-maxval((/nint(nScanL_mhs_2skip/3.),nScanL_amsua_2skip/))-1

     !------------------------------------------------------------------------------
     !   Loop over scanlines
     !------------------------------------------------------------------------------

     IF (.NOT. (interp_AMSU .AND. (iFMtype .eq. 1))) THEN ! Use the heritage FM

        jscanline=0

        ScanLLoop: DO iscanLine=1,nScanLines
           !---Read AMSUA SDR scanline content
           CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
                tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
           !---Read 3 MHS SDR consecutive scanlines content
           DO i=1,3
              CALL ReadRadMeasScanLMode(iu_sdr_2,nqc_2,qc_2(:,i),nchan_2,nFovs_2,angle_2(:,i),&
                   tb_2(:,:,i),lat_2(:,i),lon_2(:,i),node_2(i),scanUTC_2(i),scanDAY_2(i),     &
                   scanYear_2(i),RelAziAngle_2(:,i),SolZenAngle_2(:,i))
              IF (scanDAY_2(i).ne.scanDAY_1) scanUTC_2(i)=scanUTC_2(i)-86400000
              IF (scanUTC_2(i) .lt. 0) scanUTC_2(i)=scanUTC_2(i)+86400000
           ENDDO

           !---Find out which is the MHS scanline that synchs with AMSUA (on the fly), by default, 1
           ScanLindx_mhs_TimeColloc=1
           dUTC0=999999
           DO i=1,3
              UTC_mhs = scanUTC_2(i)
              Day_mhs = scanDAY_2(i)
              Yr_mhs  = scanYear_2(i)
              dUTC    = ABS(UTC_mhs-scanUTC_1)
              IF (dUTC0 .gt. dUTC) THEN
                 ScanLindx_mhs_TimeColloc=i
                 dUTC0=dUTC
              ENDIF
           ENDDO

           !---Flag Output QC if any scanline affected by a non-0 QC
           qc(1:nqc)                   = 0
           qc(1+1:nqc_1+1)             = qc_1(1:nqc_1)
           DO i=1,nqc_2
              qc(nqc_1+1+i)            = SUM(qc_2(i,:))
           ENDDO

           !---- only set 1 if both AMSUA and MHS have channels bad
           IF (ANY(qc(2:nqc_1+1) .ne. 0) .and. ANY(qc(nqc_1+2:nqc_1+nqc_2+1) .ne. 0)) qc(1)=1

           !---Take the time of the user-driven scanline
           UTC_mhs= scanUTC_2(ScanLindx_mhs_TimeColloc)
           Day_mhs= scanDAY_2(ScanLindx_mhs_TimeColloc)
           Yr_mhs = scanYear_2(ScanLindx_mhs_TimeColloc)

           !---Compute Time/day/Yr difference
           dUTC   = ABS(UTC_mhs-scanUTC_1)
           dDay   = ABS(Day_mhs-scanDAY_1)
           dYr    = ABS(Yr_mhs-scanYear_1)

           !---Perform time and position quality checks then FM if scanline not already flagged
           tb =-99
           IF (qc(1) .eq. 0) THEN

              !---Check time-collocation 
              IF (dUTC .gt. maxMSallowed) THEN
                 print *, 'Scan#',iscanLine,' dUTC:',dUTC,' MxAllowed:',maxMSallowed,&
                      UTC_mhs,scanUTC_1,'===',scanUTC_2(1:3)
                 CALL ErrHandl(WarningType,Warn_TimeIncons,'Please check SDR files.')
                 CYCLE ScanLLoop
              ENDIF

              !---Check Lat/Lon collocation and Angle similarity
              FOVsLoop: DO ifov=1,nFovs_1
                 !---Take MHS lat/lon/Angle from the 2nd scanline/2nd position (footprint center)
                 Lat_mhs = lat_2((ifov-1)*3+2,2)
                 Lon_mhs = lon_2((ifov-1)*3+2,2)
                 Ang_mhs = angle_2((ifov-1)*3+2,2)
                 !---Compute Difference in Lat/Lon/Angle
                 dlat    = abs(Lat_mhs-lat_1(ifov))
                 dlon    = abs(Lon_mhs-lon_1(ifov))
                 dAng    = abs(Ang_mhs-angle_1(ifov))
                 dLat    = minval((/dLat,  90.-dLat /))
                 !---Check Lat/Lon/Angle-collocation
                 IF (dLat .ge. mxdlatAllowed) THEN
                    CALL ErrHandl(WarningType,Warn_LatIncons,'Please check SDR files.')
                    CYCLE ScanLLoop
                 ENDIF
                 IF (dAng .ge. mxdAngAllowed) THEN
                    CALL ErrHandl(WarningType,Warn_AngleIncons,'Please check SDR files.')
                    CYCLE ScanLLoop
                 ENDIF
                 !----FM @ AMSUA spatial resolution
                 IF (iFMtype .eq. 0) THEN
                    !---Perform footprint-matching (for now, simple averaging of 3x3 MHS )
                    tb(iFov,1:nchan_1,1)        = tb_1(iFov,1:nchan_1)
                    DO ichan=1,nchan_2
                       tb(iFov,nchan_1+ichan,1) = SUM(tb_2((ifov-1)*3+1:(ifov-1)*3+3,ichan,1:3))/9.
                    ENDDO
                 ENDIF
                 !----FM @ MHS spatial resolution
                 IF (iFMtype .eq. 1) THEN
                    !---Perform footprint-matching (for now, consider AMSUA valid atthe 3x3 MHS footprints)
                    DO i=1,3
                       DO ichan=1,nchan_2
                          !----store high-resolution MHS TBs
                          tb((iFov-1)*3+1:(iFov-1)*3+3,nchan_1+ichan,i) = tb_2((iFov-1)*3+1:(iFov-1)*3+3,ichan,i)
                       ENDDO
                       !---Adjust low-resolution TBs with weighting fcts (but only those that are 89-like sensitive)
                       DO ichan=1,nchan_1
                          tb((iFov-1)*3+1:(iFov-1)*3+3,ichan,i) = tb_1(iFov,ichan)
                       ENDDO
                    ENDDO
                 ENDIF
                 !---Metric for assessing FM accuracy is DeltaTB @89.
                 !----FM @ AMSUA spatial resolution
                 IF (iFMtype .eq. 0) THEN
                    Xvec(1:9)                = RESHAPE(tb_2((ifov-1)*3+1:(ifov-1)*3+3,1,1:3),(/9/))
                    FMerr(iscanLine,iFov,1)  = tb(iFov,nchan_1,1)
                    FMerr(iscanLine,iFov,2)  = tb(iFov,nchan_1+1,1)
                    FMerr(iscanLine,iFov,3)  = Stdev(Xvec)
                    FMerr(iscanLine,iFov,4)  = qc(1)
                    FMerr(iscanLine,iFov,5)  = dLat
                    FMerr(iscanLine,iFov,6)  = dLon
                    FMerr(iscanLine,iFov,7)  = dAng
                    FMerr(iscanLine,iFov,8)  = Lat_mhs
                    FMerr(iscanLine,iFov,9)  = Lon_mhs
                    FMerr(iscanLine,iFov,10) = node_1
                    FMerr(iscanLine,iFov,11) = Day_MHS
                 ENDIF
                 !----FM @ MHS spatial resolution
                 IF (iFMtype .eq. 1) THEN
                    Xvec(1:9)                  = RESHAPE(tb_2((ifov-1)*3+1:(ifov-1)*3+3,1,1:3),(/9/))
                    DO i=1,3
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,1)  = tb((iFov-1)*3+1:(iFov-1)*3+3,nchan_1,i)
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,2)  = tb((iFov-1)*3+1:(iFov-1)*3+3,nchan_1+1,i)
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,3)  = Stdev(Xvec)
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,4)  = qc(1)
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,5)  = dLat
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,6)  = dLon
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,7)  = dAng
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,8)  = lat_2((ifov-1)*3+1:(ifov-1)*3+3,i)
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,9)  = Lon_2((ifov-1)*3+1:(ifov-1)*3+3,i)
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,10) = node_1
                       FMerr((iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,11) = Day_MHS
                    ENDDO
                 ENDIF
              ENDDO FOVsLoop

           ENDIF   ! qc(1)  .eq. 0

           !---Write out FM-SDR
           scanDAY  = scanDAY_1
           scanYear = scanYear_1
           scanUTC  = scanUTC_1

           IF (ScanLineInROI(iscanLine) .eq. 1) THEN
              !---if not write header yet, write header
              IF( WRITE_HEADER_DONE .EQ. 0 ) THEN
                 write(*,'(A)')'Output FM file='//trim(FMsdrFiles(ifile))
                 write(*,*)'FM scan =',nProcessScanLines
                 write(*,*)'FM nfov=',nFovs
                 write(*,*)'FM nprf=',nProcessScanLines*nFovs
                 CALL WriteHdrMeasurmts(FMsdrFiles(ifile),iu_FMsdr,nProcessScanLines*nFovs,&
                      nqc,nchan,nFovs,CFreq,pol,nProcessScanLines)
                 WRITE_HEADER_DONE = 1
              ENDIF
              !---write contents
              IF (iFMtype .eq. 0) THEN
                 DO ifov=1,nFovs
                    angleFM(1:nchan) = angle_1(ifov)
                    CALL WriteMeasurmts(iu_FMsdr,nqc,qc,nchan,angleFM,tb(ifov,1:nchan,1),lat_1(ifov),&
                         lon_1(ifov),node_1,scanUTC_1/1000.,scanDAY,scanYear,ifov,iscanLine,&
                         RelAziAngle_1(ifov),SolZenAngle_1(ifov))
                 ENDDO
              ELSE IF (iFMtype .eq. 1) THEN
                 DO i=1,nBatch
                    jscanline=jscanline+1
                    DO ifov=1,nFovs
                       angleFM(1:nchan) = angle_2(ifov,i)
                       CALL WriteMeasurmts(iu_FMsdr,nqc,qc,nchan,angleFM,tb(ifov,1:nchan,i),lat_2(ifov,i),&
                            lon_2(ifov,i),node_2(i),scanUTC_2(i)/1000.,scanDAY_2(i),scanYear_2(i),ifov,jscanLine,&
                            RelAziAngle_2(ifov,i),SolZenAngle_2(ifov,i))
                    ENDDO
                 ENDDO
              ENDIF
           ENDIF  !--- ENDIF (ScanLineInROI)

        ENDDO ScanLLoop

     ELSE !  Both interp_AMSU is true and iFMtype .eq. 1 -- Do Interp of AMSU TBs  to MHS centroid positions

        jscanline=0
        Index_BUF=0

        ScanLLoop2: DO iscanLine=1,nScanLines
           !---Read AMSUA SDR scanline content
           CALL ReadRadMeasScanLMode(iu_sdr_1,nqc_1,qc_1,nchan_1,nFovs_1,angle_1,&
                tb_1,lat_1,lon_1,node_1,scanUTC_1,scanDAY_1,scanYear_1,RelAziAngle_1,SolZenAngle_1)
           !---Read 3 MHS SDR consecutive scanlines content
           DO i=1,3
              CALL ReadRadMeasScanLMode(iu_sdr_2,nqc_2,qc_2(:,i),nchan_2,nFovs_2,angle_2(:,i),&
                   tb_2(:,:,i),lat_2(:,i),lon_2(:,i),node_2(i),scanUTC_2(i),scanDAY_2(i),     &
                   scanYear_2(i),RelAziAngle_2(:,i),SolZenAngle_2(:,i))
              IF (scanDAY_2(i).ne.scanDAY_1) scanUTC_2(i)=scanUTC_2(i)-86400000
              IF (scanUTC_2(i) .lt. 0) scanUTC_2(i)=scanUTC_2(i)+86400000
           ENDDO

           !---Find out which is the MHS scanline that synchs with AMSUA (on the fly), by default, 1
           ScanLindx_mhs_TimeColloc=1
           dUTC0=999999
           DO i=1,3
              UTC_mhs = scanUTC_2(i)
              Day_mhs = scanDAY_2(i)
              Yr_mhs  = scanYear_2(i)
              dUTC    = ABS(UTC_mhs-scanUTC_1)
              IF (dUTC0 .gt. dUTC) THEN
                 ScanLindx_mhs_TimeColloc=i
                 dUTC0=dUTC
              ENDIF
           ENDDO

           !---Flag Output QC if any scanline affected by a non-0 QC
           qc(1:nqc)                   = 0
           qc(1+1:nqc_1+1)             = qc_1(1:nqc_1)
           DO i=1,nqc_2
              qc(nqc_1+1+i)            = SUM(qc_2(i,:))
           ENDDO

           !---- only set 1 if both AMSUA and MHS have channels bad
           IF (ANY(qc(2:nqc_1+1) .ne. 0) .and. ANY(qc(nqc_1+2:nqc_1+nqc_2+1) .ne. 0)) qc(1)=1

           !---Take the time of the user-driven scanline
           UTC_mhs= scanUTC_2(ScanLindx_mhs_TimeColloc)
           Day_mhs= scanDAY_2(ScanLindx_mhs_TimeColloc)
           Yr_mhs = scanYear_2(ScanLindx_mhs_TimeColloc)

           !---Compute Time/day/Yr difference
           dUTC   = ABS(UTC_mhs-scanUTC_1)
           dDay   = ABS(Day_mhs-scanDAY_1)
           dYr    = ABS(Yr_mhs-scanYear_1)

           ! Code begins to differ from previous ScanLoop Here!

           BUFN%tb=0.0
           BUFN%tb_1 = tb_1
           BUFN%tb_2 = tb_2
           BUFN%qc = qc
           BUFN%angle_2=angle_2
           BUFN%lat_2 = lat_2
           BUFN%lon_2 = lon_2
           BUFN%RelAziAngle_2 = RelAziAngle_2
           BUFN%SolZenAngle_2 = SolZenAngle_2
           BUFN%node_2 = node_2
           BUFN%scanUTC_2 = scanUTC_2
           BUFN%scanDAY_2 = scanDAY_2
           BUFN%scanYear_2 = scanYear_2
           BUFN%scanDAY_1 = scanDAY_1
           BUFN%scanUTC_1 = scanUTC_1
           BUFN%iscanline = iscanLine
           BUFN%node_1=node_1
           BUFN%Day_mhs=Day_mhs

           IF (qc(1) .eq. 0) THEN

              !---Perform time and position quality checks, then FM if scanline not already flagged

              match_UTC = .TRUE.  ! see if placement of these 3 lines  jives with the conditionals tests after processing current line
              match_lat = .TRUE.
              match_ang = .TRUE.

              !---Check time-collocation 
              IF (dUTC .gt. maxMSallowed) THEN
                 match_UTC = .FALSE.
                 print *, 'Scan#',iscanLine,' dUTC:',dUTC,' MxAllowed:',maxMSallowed,&
                      UTC_mhs,scanUTC_1,'===',scanUTC_2(1:3)
                 CALL ErrHandl(WarningType,Warn_TimeIncons,'Please check SDR files.')

              ELSE
                 !---Check Lat/Lon collocation and Angle similarity
                 FOVsLoop2: DO ifov=1,nFovs_1

                    !---Take MHS lat/lon/Angle from the 2nd scanline, 2+3(ifov-1) position (coresp. AMSU footprint centers)
                    Lat_mhs = lat_2((ifov-1)*3+2,2)
                    Lon_mhs = lon_2((ifov-1)*3+2,2)
                    Ang_mhs = angle_2((ifov-1)*3+2,2)

                    !---Compute Difference in Lat/Lon/Angle
                    dlat_arr(ifov)    = abs(Lat_mhs-lat_1(ifov))
                    dlon_arr(ifov)    = abs(Lon_mhs-lon_1(ifov))
                    dAng_arr(ifov)    = abs(Ang_mhs-angle_1(ifov))
                    dLat_arr(ifov)    = minval((/dLat_arr(ifov),  90.-dLat_arr(ifov) /))

                    !---Check Lat/Lon/Angle-collocation
                    IF (dLat_arr(ifov) .ge. mxdlatAllowed) THEN
                       match_lat = .FALSE.
                       CALL ErrHandl(WarningType,Warn_LatIncons,'Please check SDR files.')
                       EXIT FOVsLoop2
                    ENDIF
                    IF (dAng_arr(ifov) .ge. mxdAngAllowed) THEN
                       match_ang = .FALSE.
                       CALL ErrHandl(WarningType,Warn_AngleIncons,'Please check SDR files.')
                       EXIT FOVsLoop2
                    ENDIF
                 ENDDO FOVsLoop2
              ENDIF

              IF (Index_BUF .eq.  0) THEN !Needs  to be fixed so that this works across date boundaries
                 contig_scanline = .true.
              ELSEIF (Index_BUF .eq. 1) THEN
                 Contig_scanline = ABS(BUF1%scanUTC_1 - scanUTC_1) .lt. 8400
              ELSEIF (Index_BUF .eq. 2) THEN
                 Contig_scanline = ABS(BUF2%scanUTC_1 - scanUTC_1) .lt. 8400
              ENDIF

              Match = match_UTC .AND. match_lat .AND. match_ang

              IF (Match) THEN
                 BUFN%dlat_arr = dlat_arr
                 BUFN%dlon_arr = dlon_arr
                 BUFN%dAng_arr = dAng_arr
              ENDIF

              IF (match .AND. contig_scanline) THEN

                 Index_BUF = Index_BUF + 1

                 IF (Index_BUF .eq. 1) THEN

                    BUF1=BUFN

                    ! Interpolate AMSU tb_1 along-scan to FM'd tb, scan 2
                    ! Also Copy the MHS TBs tb_2 to FM'd tb

                    Call Interpolate_AMSU_Along_Scan(1)

                    ! Copy FM'd AMSU tb from MHS scanline 2 to scanline 1 and 3
                    DO ichan=1,nchan_1
                       BUF1%tb(:,ichan,1)=BUF1%tb(:,ichan,2)
                       BUF1%tb(:,ichan,3)=BUF1%tb(:,ichan,2)
                    ENDDO

                 ELSEIF (Index_BUF .eq. 2) THEN

                    BUF2=BUFN

                    ! Interpolate AMSU tb_1 along-scan to FM'd tb, scan 2
                    ! Also Copy the MHS TBs tb_2 to FM'd tb

                    Call Interpolate_AMSU_Along_Scan(2)

                    ! Extrapolate BUF1 tb scan line 1 from BUF1 and BUF2 tb scan lines 2
                    Call Extrapolate(1,1,1,2)
                    ! Interpolate BUF1 tb scan line 3 from BUF1 and BUF2 tb scan lines 2
                    Call Interpolate(1,3,1,2)

                    ! Fill in  FMErr(BUF1%iscanline) using BUF1
                    Call FillInFMErr(1)

                    IF (ScanLineInROI(BUF1%iscanLine) .eq. 1) THEN

                       CALL WriteToFMFile(1)

                    ENDIF

                    !Interpolate BUF2 tb scan line 1 from BUF1 and BUF2 tb scan lines 2
                    Call Interpolate(2,1,1,2)
                    !Extrapolate BUF2 tb scan line 3 from BUF1 and BUF2 tb scan lines 2
                    Call Extrapolate(2,3,1,2)

                 ELSEIF (Index_BUF .eq. 3) THEN

                    BUF3=BUFN

                    ! Interpolate AMSU tb_1 along-scan to FM'd tb, scan 2
                    ! Also Copy the MHS TBs tb_2 to FM'd tb

                    Call Interpolate_AMSU_Along_Scan(3)

                    ! Interpolate BUF2 tb scan line 3 from BUF2 and BUF3 tb scan lines 2
                    Call Interpolate(2,3,2,3)

                    ! Fill in  FMErr(BUF2%iscanline) using BUF2
                    Call FillInFMErr(2)             

                    IF (ScanLineInROI(BUF2%iscanLine) .eq. 1) THEN

                       CALL WriteToFMFile(2)

                    ENDIF

                    ! Since all has processed through BUF2, shift the buffer down 
                    BUF1=BUF2
                    BUF2=BUF3
                    
                    !Interpolate BUF2 tb scan line 1 from BUF1 and BUF2 tb scan lines 2
                    Call Interpolate(2,1,1,2)
                    !Extrapolate BUF2 tb scan line 3 from BUF1 and BUF2 tb scan lines 2
                    Call Extrapolate(2,3,1,2)

                    Index_BUF=2 ! At this point, everything looks just like it did at the end of Index_BUF=2 

                 ENDIF ! Index_BUF
              ENDIF ! match .AND. contig_scanline
           ENDIF  !  ENDIF qc(0) .eq. 0

           lastLine = iscanline .eq. nScanLines

           IF (qc(1) .eq. 0) THEN

              IF (lastLine .OR. .NOT. match .OR. .NOT. Contig_scanline) THEN ! Write the  output, clear the buffer

                 IF(Index_BUF .eq. 1) THEN
                    Call FillInFMErr(1)             
                    IF (ScanLineInROI(BUF1%iscanLine) .eq. 1) CALL WriteToFMFile(1)
                 ELSEIF(Index_BUF .eq. 2) THEN
                    Call FillInFMErr(2)             
                    IF (ScanLineInROI(BUF2%iscanLine) .eq. 1) CALL WriteToFMFile(2)
                 ELSEIF(Index_BUF .ne. 0) THEN
                    print  *,  'ERROR: Index_BUF .NE. 0, 1, or 2 --STOP'
                    STOP
                 ENDIF

              ENDIF

              IF (match .AND. .NOT. Contig_scanline) THEN ! Add Obs as BUF1

                 BUF1=BUFN

                 ! Interpolate AMSU tb_1 along-scan to FM'd tb, scan 2
                 ! Also Copy the MHS TBs tb_2 to FM'd tb
                 
                 Call Interpolate_AMSU_Along_Scan(1)

                 ! Copy FM'd AMSU tb from MHS scanline 2 to scanline 1 and 3
                 DO ichan=1,nchan_1
                    BUF1%tb(:,ichan,1)=BUF1%tb(:,ichan,2)
                    BUF1%tb(:,ichan,3)=BUF1%tb(:,ichan,2)
                 ENDDO
                 
                 Index_BUF=1

                 IF (lastline) THEN

                    Call FillInFMErr(1)
                    IF (ScanLineInROI(BUF1%iscanLine) .eq. 1) CALL WriteToFMFile(1)

                 ENDIF

              ENDIF

              IF ( .NOT. match .OR. Lastline) Index_BUF=0 ! Clear the Buffer

              IF (lastLine .OR. .NOT. match .OR. .NOT. Contig_scanline) cycle ScanLLoop2

           ELSE ! qc(1) .ne. 0

              !Write the output, clear the buffer

                 IF(Index_BUF .eq. 1) THEN
                    Call FillInFMErr(1)             
                    IF (ScanLineInROI(BUF1%iscanLine) .eq. 1) CALL WriteToFMFile(1)
                 ELSEIF(Index_BUF .eq. 2) THEN
                    Call FillInFMErr(2)             
                    IF (ScanLineInROI(BUF2%iscanLine) .eq. 1) CALL WriteToFMFile(2)
                 ELSEIF(Index_BUF .ne. 0) THEN
                    print  *,  'ERROR: Index_BUF .NE. 0, 1, or 2 --STOP'
                    STOP
                 ENDIF

             ! Treat the current QC(1) ne 0 scan line:

              IF (ScanLineInROI(iscanLine) .eq. 1) THEN
                 BUFN%tb=-99 !Missing value
                 CALL WriteToFMFile(4)
              ENDIF

              Index_BUF=0

           ENDIF ! qc(1) value

        ENDDO ScanLLoop2

     ENDIF ! That interp_AMSU .AND. (iFMtype .eq. 1)

     !---Output FM-accuracy metric

     IF (iOutpFM_accur .eq. 1) THEN
        write(ext,'(i2.2)') ifile
        open(iu_check,file=trim(prefCheck)//ext,status='unknown',form='formatted',ACCESS='SEQUENTIAL')
        write(iu_check,'(3i10)') nScanL,nFovs,np
        write(iu_check,'(11f12.5)') FMerr(1:nScanL,1:nFovs,1:np)
        close(iu_check)
     ENDIF

     !Perform Deallocation

     DEALLOCATE(angleFM,tb,qc,lat_1,lon_1,angle_1,tb_1,qc_1,lat_2,&
          lon_2,angle_2,tb_2,qc_2,Cfreq,pol,FMerr,&
          RelAziAngle_1,SolZenAngle_1,RelAziAngle_2,SolZenAngle_2)

     IF (interp_AMSU .AND. (iFMtype .eq. 1)) then

       DEALLOCATE(dLat_arr, dLon_arr, dAng_arr)

       DEALLOCATE(BUF1%tb, BUF1%tb_1, BUF1%tb_2, BUF1%qc, BUF1%angle_2, &
             BUF1%lat_2, BUF1%lon_2, BUF1%RelAziAngle_2, &
             BUF1%SolZenAngle_2, BUF1%dLat_arr, BUF1%dLon_arr, BUF1%dAng_arr)

       DEALLOCATE(BUF2%tb, BUF2%tb_1, BUF2%tb_2, BUF2%qc, BUF2%angle_2, &
             BUF2%lat_2, BUF2%lon_2, BUF2%RelAziAngle_2, &
             BUF2%SolZenAngle_2, BUF2%dLat_arr, BUF2%dLon_arr, BUF2%dAng_arr)

       DEALLOCATE(BUF3%tb, BUF3%tb_1, BUF3%tb_2, BUF3%qc, BUF3%angle_2, &
             BUF3%lat_2, BUF3%lon_2, BUF3%RelAziAngle_2, &
             BUF3%SolZenAngle_2, BUF3%dLat_arr, BUF3%dLon_arr, BUF3%dAng_arr)

       DEALLOCATE(BUFN%tb, BUFN%tb_1, BUFN%tb_2, BUFN%qc, BUFN%angle_2, &
             BUFN%lat_2, BUFN%lon_2, BUFN%RelAziAngle_2, &
             BUFN%SolZenAngle_2, BUFN%dLat_arr, BUFN%dLon_arr, BUFN%dAng_arr)

       DEALLOCATE(BUFdummy%tb, BUFdummy%tb_1, BUFdummy%tb_2, BUFdummy%qc, BUFdummy%angle_2, &
             BUFdummy%lat_2, BUFdummy%lon_2, BUFdummy%RelAziAngle_2, &
             BUFdummy%SolZenAngle_2, BUFdummy%dLat_arr, BUFdummy%dLon_arr, BUFdummy%dAng_arr)

     ENDIF


     !---Close SDR file
     CLOSE(iu_sdr_1)
     CLOSE(iu_sdr_2)

     !---Close FM-SDR file
     IF( WRITE_HEADER_DONE .EQ. 1 ) CLOSE(iu_FMsdr)
     DEALLOCATE(ScanLineInROI)

  ENDDO FilesLoop

  If(ASSOCIATED(CFreq_1)) DEALLOCATE(Cfreq_1)
  If(ASSOCIATED(pol_1))   DEALLOCATE(pol_1)
  If(ASSOCIATED(CFreq_2)) DEALLOCATE(Cfreq_2)
  If(ASSOCIATED(pol_2))   DEALLOCATE(pol_2)
  DEALLOCATE(sdrFilesAMSUA,sdrFilesMHS,FMsdrFiles)

  !---- if no FMSDR generated, return an error code ---- 
  if( TotalScanLinesROI .lt. 1 ) then
     !print *, 'No FMSDR data is generated.'
     CALL ErrHandl(ErrorType,Err_NoFmsdrGenerated,'(in POES FM)' )
  endif

  CALL CloseLogFile()

CONTAINS       !------------------------INTERNAL SUBROUTINES ---------------------------!

  SUBROUTINE Interpolate_AMSU_Along_Scan(n)
    INTEGER, INTENT(IN) :: n
    INTEGER :: ifov, i, ichan

    IF (n .eq. 1) THEN
       BUFdummy%tb_2=BUF1%tb_2
       BUFdummy%tb_1=BUF1%tb_1
    ELSEIF (n .eq. 2) THEN
       BUFdummy%tb_2=BUF2%tb_2
       BUFdummy%tb_1=BUF2%tb_1
    ELSEIF (n .eq. 3) THEN
       BUFdummy%tb_2=BUF3%tb_2
       BUFdummy%tb_1=BUF3%tb_1
    ELSE
       WRITE(*,*) 'Subroutine Interpolate_AMSU_Along_Scan: Index of input bufer OOB (NOT 1 thru 3)'
       STOP
    ENDIF
 
    BUFdummy%tb=0

    ! First, copy the MHS TBs from tb_2 to FM'd tb
    DO i=1,3
       DO ichan=1,nchan_2
          BUFdummy%tb(:,nchan_1+ichan,i)=BUFdummy%tb_2(:,ichan,i)
       ENDDO
    ENDDO

    i=2
    DO ichan=1,nchan_1
       BUFdummy%tb(1,ichan,i)=BUFdummy%tb_1(1,ichan) ! Just copy to scanpos 1, no extrap for now
       DO ifov=1,nFovs_1-1
          BUFdummy%tb((iFov-1)*3+2,ichan,i) = BUFdummy%tb_1(iFov,ichan)
          BUFdummy%tb((iFov-1)*3+3,ichan,i) = (2./3.)*BUFdummy%tb_1(iFov,ichan)+(1./3.)*BUFdummy%tb_1(iFov+1,ichan)
          BUFdummy%tb((iFov-1)*3+4,ichan,i) = (1./3.)*BUFdummy%tb_1(iFov,ichan)+(2./3.)*BUFdummy%tb_1(iFov+1,ichan)
       ENDDO
       BUFdummy%tb((nFovs_1-1)*3+2,ichan,i)=BUFdummy%tb_1(nFovs_1,ichan)
       BUFdummy%tb((nFovs_1-1)*3+3,ichan,i)=BUFdummy%tb_1(nFovs_1,ichan) ! Ditto finl scanpos--just copy, no extrap
    ENDDO

    IF (n .eq. 1) THEN
       BUF1%tb=BUFdummy%tb
    ELSEIF (n .eq. 2) THEN
       BUF2%tb=BUFdummy%tb
    ELSEIF (n .eq. 3) THEN
       BUF3%tb=BUFdummy%tb
    ENDIF

  ENDSUBROUTINE Interpolate_AMSU_Along_Scan

  SUBROUTINE Interpolate(iTarg,jscan,inp1,inp2)
    INTEGER , INTENT(IN) :: iTarg,jscan,inp1,inp2
    INTEGER :: ifov, i, ichan
    REAL, DIMENSION(nFovs,nchan,nBatch) :: tb_out, tb_in1, tb_in2

    IF (iTarg .eq. 1) THEN
       tb_out=BUF1%tb
    ELSEIF (iTarg .eq. 2) THEN
       tb_out=BUF2%tb
    ELSE
       WRITE(*,*) 'Subroutine Interpolate: Index of output buffer OOB (NOT 1 OR 2)'
       STOP
    ENDIF

    IF (inp1 .eq. 1) THEN
       tb_in1=BUF1%tb
       IF (inp2 .eq. 2) then
          tb_in2=BUF2%tb
       ELSE
          WRITE(*,*) 'Subroutine Interpolate: Index of input buffers are not contiguous'
          STOP
       ENDIF
    ELSEIF(inp1 .eq. 2) THEN
       tb_in1=BUF2%tb
       IF (inp2 .eq. 3) THEN
          tb_in2=BUF3%tb
       ELSE
          WRITE(*,*) 'Subroutine Interpolate: Index of input buffers are not contiguous'
          STOP
       ENDIF
    ELSE
       WRITE(*,*) 'Subroutine Interpolate: Index of first input buffer is OOB (NOT 1 OR 2)'
       STOP
    ENDIF
    
    IF (jscan .eq. 1) then
       DO ichan=1,nchan_1
          tb_out(:,ichan,jscan)=(1./3.)*tb_in1(:,ichan,2)+(2./3.)*tb_in2(:,ichan,2)
       ENDDO
    ELSEIF (jscan .eq. 3) THEN
       DO ichan=1,nchan_1
          tb_out(:,ichan,jscan)=(2./3.)*tb_in1(:,ichan,2)+(1./3.)*tb_in2(:,ichan,2)
       ENDDO
    ELSE
       WRITE(*,*) 'Subroutine Interpolate: Index of target scan line is OOB (NOT 1 OR 3)'
       STOP
    ENDIF

    IF (iTarg .eq. 1) THEN
       BUF1%tb=tb_out
    ELSEIF (iTarg .eq. 2) THEN
       BUF2%tb=tb_out
    ENDIF

  ENDSUBROUTINE Interpolate

  SUBROUTINE Extrapolate(iTarg,jscan,inp1,inp2)
    INTEGER, INTENT(IN) :: iTarg,jscan,inp1,inp2

    IF (iTarg .eq. 1) THEN
       BUFdummy%tb=BUF1%tb
    ELSEIF (iTarg .eq. 2) THEN
       BUFdummy%tb=BUF2%tb
    ELSE
       WRITE(*,*) 'Subroutine Interpolate: Index of input bufer OOB (NOT 1 OR 2)'
       STOP
    ENDIF

    ! For now, duplicate old method of FM (duplicate scan 2 to other scans)
    BUFdummy%tb(:,1:nchan_1,jscan)= BUFdummy%tb(:,1:nchan_1,2)

    IF (iTarg .eq. 1) THEN
       BUF1%tb=BUFdummy%tb
    ELSEIF (iTarg .eq. 2) THEN
       BUF2%tb=BUFdummy%tb
    ENDIF

  ENDSUBROUTINE Extrapolate

  SUBROUTINE FillInFMErr(n)
    INTEGER, INTENT(IN) :: n
    INTEGER :: i,ifov

    IF (n .eq. 1) THEN
       BUFdummy=BUF1
    ELSEIF (n .eq. 2) THEN
       BUFdummy=BUF2
    ELSE
       WRITE(*,*) 'Subroutine FillInFMErr: Index of input buffer OOB (NOT 1 OR 2)'
       STOP
    ENDIF

    DO ifov=1,nFovs_1
       Xvec(1:9)                  = RESHAPE(BUFdummy%tb_2((ifov-1)*3+1:(ifov-1)*3+3,1,1:3),(/9/))
       DO i=1,3
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,1)  = BUFdummy%tb((iFov-1)*3+1:(iFov-1)*3+3,nchan_1,i)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,2)  = BUFdummy%tb((iFov-1)*3+1:(iFov-1)*3+3,nchan_1+1,i)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,3)  = Stdev(Xvec)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,4)  = BUFdummy%qc(1)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,5)  = BUFdummy%dLat_arr(ifov)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,6)  = BUFdummy%dLon_arr(ifov)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,7)  = BUFdummy%dAng_arr(ifov)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,8)  = BUFdummy%lat_2((ifov-1)*3+1:(ifov-1)*3+3,i)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,9)  = BUFdummy%Lon_2((ifov-1)*3+1:(ifov-1)*3+3,i)
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,10) = BUFdummy%node_1
          FMerr((BUFdummy%iscanLine-1)*3+i,(iFov-1)*3+1:(iFov-1)*3+3,11) = BUFdummy%Day_MHS
       ENDDO
    ENDDO

  ENDSUBROUTINE FillInFMErr

  SUBROUTINE WriteToFMFile(n)

    INTEGER, INTENT(IN) :: n

    IF (n .eq. 1) THEN
       BUFdummy = BUF1
    ELSEIF (n .eq. 2) THEN
       BUFdummy = BUF2
    ELSEIF (n .eq. 3) THEN
       BUFdummy = BUF3
    ELSEIF (n .eq. 4) THEN
       BUFdummy = BUFN
    ELSE
       WRITE(*,*) 'Subroutine WriteToFMFile: Index of input buffer OOB (NOT 1 thru 4)'
       STOP
    ENDIF

    !Write the FMSDR file header if not done already
    IF( WRITE_HEADER_DONE .EQ. 0 ) THEN
       write(*,'(A)')'Output FM file='//trim(FMsdrFiles(ifile))
       write(*,*)'FM scan =',nProcessScanLines
       write(*,*)'FM nfov=',nFovs
       write(*,*)'FM nprf=',nProcessScanLines*nFovs
       CALL WriteHdrMeasurmts(FMsdrFiles(ifile),iu_FMsdr,nProcessScanLines*nFovs,&
            nqc,nchan,nFovs,CFreq,pol,nProcessScanLines)
       WRITE_HEADER_DONE = 1
    ENDIF

    !Write the contents of BUF1 to the FMSDR file (incrementing jscanline for each scan)
    DO i=1,nBatch
       jscanline=jscanline+1
       DO ifov=1,nFovs
          angleFM(1:nchan) = BUFdummy%angle_2(ifov,i)
          CALL WriteMeasurmts(iu_FMsdr,nqc,BUFdummy%qc,nchan,angleFM,BUFdummy%tb(ifov,1:nchan,i),BUFdummy%lat_2(ifov,i),&
               BUFdummy%lon_2(ifov,i),BUFdummy%node_2(i),BUFdummy%scanUTC_2(i)/1000.,BUFdummy%scanDAY_2(i),BUFdummy%scanYear_2(i), &
               ifov,jscanLine, BUFdummy%RelAziAngle_2(ifov,i),BUFdummy%SolZenAngle_2(ifov,i))
       ENDDO
    ENDDO

  ENDSUBROUTINE WriteToFMFile

ENDPROGRAM fm_metopB


