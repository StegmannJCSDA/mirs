!===============================================================
! Name:      fm_gpm
!
!
! Type:         Main Program
!
!
! Description:
!       Transforms the SDRs  into FM-SDRs.
!       No attempt to do co-registration.
!
!
! Modules needed:
!       - IO_MeasurData
!       - misc
!       - ErrorHandling
!       - Consts
!       - misc
!
!
! History:
!       11-18-2014      Dr. Tanvir Islam                   
!                       NOAA/NESDIS/STAR & CIRA/CSU       Original Coder
!===============================================================

program fm_gpm
  USE IO_MeasurData
  USE IO_Misc
  USE ErrorHandling
  USE Consts
  USE misc  
  IMPLICIT NONE
  !---INTRINSIC functions used
  INTRINSIC :: MINVAL,MOD,TRIM,NINT,ANINT

  CHARACTER(LEN=250), DIMENSION(:),    POINTER     :: sdrFiles,FMsdrFiles
  CHARACTER(LEN=250)                               :: fileFMSDR
  INTEGER                                          :: iu_sdr,iu_fmsdr,nfiles,ifile
  INTEGER                                          :: iscanline,ifov,node,ifovr
  INTEGER                                          :: nScanL,nFovs,nqc,nchan,nFovsR,FOVInc
  REAL,               DIMENSION(:),    POINTER     :: Cfreq
  INTEGER,            DIMENSION(:),    POINTER     :: pol
  REAL,               DIMENSION(:),    ALLOCATABLE :: lat,lon,angle,RelAziAngle,SolZenAngle
  REAL,               DIMENSION(:),    ALLOCATABLE :: angleFM
  REAL,               DIMENSION(:,:),  ALLOCATABLE :: tb
  INTEGER,            DIMENSION(:),    ALLOCATABLE :: qc
  INTEGER                                          :: scanDAY,scanYear
  INTEGER                                          :: scanUTC
  INTEGER                                          :: iu_list
  INTEGER                                          :: nProcessScanLines=0,iProcessScanLine 
  INTEGER,            DIMENSION(:),    ALLOCATABLE :: ScanLineInROI
  INTEGER                                          :: WRITE_HEADER_DONE=0

  !---Namelist data 
  INTEGER            :: norbits2process=DEFAULT_VALUE_INT
  CHARACTER(LEN=250) :: sdrfileList=DEFAULT_VALUE_STR4
  CHARACTER(LEN=250) :: pathFMSDR=DEFAULT_VALUE_STR4
  INTEGER            :: FMresol=DEFAULT_VALUE_INT
  CHARACTER(LEN=250) :: LogFile=DEFAULT_VALUE_STR4
  INTEGER            :: GeogrLimits=DEFAULT_VALUE_INT
  REAL               :: minLat=DEFAULT_VALUE_REAL
  REAL               :: maxLat=DEFAULT_VALUE_REAL
  REAL               :: minLon=DEFAULT_VALUE_REAL
  REAL               :: maxLon=DEFAULT_VALUE_REAL

  NAMELIST /ContrlFM/sdrfileList,pathFMSDR,FMresol,norbits2process,&
       LogFile,GeogrLimits,minLat,maxLat,minLon,maxLon

  !---Read control-data from namelist
  READ(*,NML=ContrlFM)
  !---Prepare Log file
  CALL OpenLogFile(Logfile)
  !---Read the file names of SDR data and build FMSDR files names
  call ReadList(iu_list,trim(sdrfileList),sdrFiles,nFiles,FMsdrFiles,pathFMSDR,'FMSDR_')
  IF (nfiles .lt. 1) CALL ErrHandl(ErrorType,Err_NoFilesFound,'')
  nfiles=minval((/nfiles,norbits2process/))

  !-------------------------------------------------------------------------------
  !   Add extension (CR, HR or LR) depending on the resolution chosen
  !-------------------------------------------------------------------------------
  DO ifile=1,nFiles
!liusy-     call replace_path_string(sdrFiles(ifile), pathFMSDR, 'SDR', 'FMSDR', fileFMSDR)
     call replace_path_string_mt(sdrFiles(ifile), pathFMSDR, '_','SDR', 'FMSDR', fileFMSDR) !liusy+
     IF( FMresol .eq. -1 ) THEN
        FMsdrFiles(ifile) = trim(fileFMSDR)//'.CR'
     ELSE IF( FMresol .eq. 0 ) THEN
        FMsdrFiles(ifile) = trim(fileFMSDR)//'.LR'
     ELSE IF( FMresol .eq. 1 ) THEN
        FMsdrFiles(ifile) = trim(fileFMSDR)//'.HR'
     ELSE
        CALL ErrHandl(ErrorType,Err_DifferFromExpect,'iFMType not supported')
     ENDIF
  ENDDO

  !---------------------------------------------------------
  !  Loop over the TDR files
  !---------------------------------------------------------
  FilesLoop: DO ifile=1,nFiles
     WRITE_HEADER_DONE=0

     write(*,*) 'ifile=',ifile
     write(*,*) 'SDR='//TRIM(sdrFiles(ifile))
     write(*,*) 'FMSDR='//TRIM(FMsdrFiles(ifile))

     !---Open/Read SDR header
     CALL ReadRadHdrScanLMode(sdrFiles(ifile),iu_sdr,nScanL,nFovs,nqc,nchan,CFreq,pol)
     !---Release memory
     DEALLOCATE(CFreq,pol)

     !---Allocate memory for to-be-read vectors/arrays
     ALLOCATE(lat(nFovs),lon(nFovs),angle(nFovs),tb(nFovs,nchan),&
          qc(nqc),RelAziAngle(nFovs),SolZenAngle(nFovs),angleFM(nchan) )

     !---Loop over scanlines to flag whether the scanline need to process or not
     ALLOCATE(ScanLineInROI(nScanL))
     ScanLineInROI = 0
     nProcessScanLines=0

     ScanLinesLoopFlag: DO iscanLine=1,nScanL
        !---Read SDR scanline content
        CALL ReadRadMeasScanLMode(iu_sdr,nqc,qc,nchan,nFovs,angle,tb,lat,lon,node,&
             scanUTC,scanDAY,scanYear,RelAziAngle,SolZenAngle)

        IF (GeogrLimits .eq. 0) THEN
           ScanLineInROI(iscanLine) = 1
           nProcessScanLines = nProcessScanLines + 1
        ELSE IF (GeogrLimits .eq. 1) THEN
           GeoLoop: DO ifov=1,nFovs
              IF( lat(ifov) .ge. minLat .and. lat(ifov) .le. maxLat .and. &
                   lon(ifov) .ge. minLon .and. lon(ifov) .le. maxLon) THEN
                 ScanLineInROI(iscanLine) = 1
                 nProcessScanLines = nProcessScanLines + 1
                 EXIT GeoLoop
              ENDIF
           ENDDO GeoLoop
        ELSE
           CALL ErrHandl(ErrorType,Err_OptionNotSupported,'(in AQUA-AMSRE FM)') 
        ENDIF
     ENDDO ScanLinesLoopFlag
     CLOSE(iu_sdr)

     !---FM resolution define (thinning)
     IF( FMresol .eq. -1 ) THEN
        FOVInc = 8 ! FOV = 20
        nFovsR = NINT(160/real(FovInc))
     ELSE IF ( FMresol .eq. 0 ) THEN
        FOVInc = 2 ! FOV = 80
        nFovsR = NINT(160/real(FovInc))
     ELSE IF ( FMresol .eq. 1 ) THEN
        FOVInc = 1 ! FOV = 160
        nFovsR = NINT(160/real(FovInc))
     END IF

     !---Open/Read SDR header
     CALL ReadRadHdrScanLMode(sdrFiles(ifile),iu_sdr,nScanL,nFovs,nqc,nchan,CFreq,pol)
     iProcessScanLine = 0
     !---Loop over scanlines
     ScanLinesLoop: DO iscanLine=1,nScanL  
        !---Read SDR scanline content
        CALL ReadRadMeasScanLMode(iu_sdr,nqc,qc,nchan,nFovs,angle,tb,lat,lon,node,&
             scanUTC,scanDAY,scanYear,RelAziAngle,SolZenAngle)

        !---If scanline has any point that fit within the geographic limits, output it
        IF (ScanLineInROI(iscanLine) .EQ. 1 ) THEN
           iProcessScanLine = iProcessScanLine + 1
           IF( WRITE_HEADER_DONE .EQ. 0 ) THEN
              !---Open/Write FM-SDR header
              CALL WriteHdrMeasurmts(FMsdrFiles(ifile),iu_fmsdr,nProcessScanLines*nFovsR,&
                   nqc,nchan,nFovsR,CFreq,pol,nProcessScanLines)
              WRITE_HEADER_DONE = 1
              !---Release memory
              DEALLOCATE(CFreq,pol)
           ENDIF
           ifovr = 0
           !---Write out FM-SDR (only inner swath, 31 to 160 Fovs)
           FOVsLoop: Do ifov=31,190,FOVInc
              angleFM(1:nchan)=angle(ifov)    
              ifovr = ifovr+1
              CALL WriteMeasurmts(iu_fmsdr,nqc,qc,nchan,angleFM,tb(ifov,:),lat(ifov),lon(ifov),node,&
                   scanUTC/1000.,scanDAY,scanYear,ifovr,iProcessScanLine,RelAziAngle(ifov),SolZenAngle(ifov))
           ENDDO FOVsLoop
        ENDIF
     ENDDO ScanLinesLoop

     DEALLOCATE(lat,lon,angle,tb,qc,RelAziAngle,SolZenAngle,angleFM)
     DEALLOCATE(ScanLineInROI)
     !---Close FM-SDR file
     IF( WRITE_HEADER_DONE .EQ. 1 ) CLOSE(iu_fmsdr)
     !---Close SDR file
     CLOSE(iu_sdr)
  ENDDO FilesLoop

  DEALLOCATE(sdrFiles)
  DEALLOCATE(FMsdrFiles)
  CALL CloseLogFile()

end program fm_gpm
