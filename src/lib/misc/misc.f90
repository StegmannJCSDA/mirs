!$Id$
!-----------------------------------------------------------------------------------------------
! Name:         misc
! 
! Type:       F90 module
!
! Description:
!       Module that contains several routines of different functions
!       and several applications.
!
! Modules needed:
!       - Consts
!
! Subroutines contained:
!       - Get_lun
!       - ShrkMatrx
!       - ShrkVect
!       - printSummary
!       - deg2rad
!       - rad2deg
!       - TransfSigma2Press
!       - ReadList 
!       - plank 
!       - cnode
!       - Linear
!       - compJulDay
!       - Translate2I1
!       - Translate2I2
!       - Translate2I4
!       - Stdev
!       - Mean
!       - read_topography
!       - day_month
!       - REMOVESIGN
!       - DetermGeogrLimits
!       - CountClassTypes
!       - EstimatSfcPress
!       - SatZenAng2ViewAng
!
! Data type included:
!       - none
! 
! History:
!        2006   S.A. Boukabara IMSG Inc. @ NOAA/NESDIS/ORA 
!
!-----------------------------------------------------------------------------------------------

MODULE misc
  USE Consts
  IMPLICIT NONE
  PRIVATE
  PUBLIC :: Get_lun,ShrkMatrx,ShrkVect,printSummary,deg2rad,rad2deg
  PUBLIC :: TransfSigma2Press,plank,cnode,Linear,compJulDay
  PUBLIC :: Translate2I1,Translate2I2,Translate2I4,Stdev,Mean
!  PUBLIC :: read_topography,day_month,REMOVESIGN,DetermGeogrLimits
  PUBLIC :: day_month,REMOVESIGN,DetermGeogrLimits
  PUBLIC :: CountClassTypes,EstimatSfcPress,SatZenAng2ViewAng
  PUBLIC :: fov_a, strcmp, insert_path_string, replace_path_string, replace_path_string_mt, get_fov_size
  PUBLIC :: read_topography_onetwentieth

  INTRINSIC :: EXP,LEN_TRIM,ABS,ATAN,SIN,ASIN,SQRT,REAL,INT,LOG,IACHAR,SIZE,SUM,DBLE,ICHAR,SNGL,MOD
  
CONTAINS


!===============================================================
! Name:         EstimatSfcPress
!
!
! Type:         Subroutine
!
!
! Description:  Estimation of the surface pressure from the 
!               altitude info (only over non-ocean)
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - xalt               I             Altitude
!       - SfcClass           I             Surface class/type
!       - SfcPress           I/O           Surface pressure
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE EstimatSfcPress(xalt,SfcClass,SfcPress)
    REAL      :: xalt,SfcPress,SfcP0=1013.,Z0=0.,T0=280.
    INTEGER   :: SfcClass
    IF (SfcClass .eq. LD_TYP .or. SfcClass .eq. SNOW_TYP) THEN
       SfcPress=SfcP0*exp(-(STANDARD_GRAVITY*(xalt-Z0))/(DryAirConst*T0))
    ENDIF
    RETURN
  END SUBROUTINE EstimatSfcPress


!===============================================================
! Name:         CountClassTypes
!
!
! Type:         Subroutine
!
!
! Description:  Counts the incremented number of class types within
!               a file.
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - SfcClass            I            Surface class
!       - nProfsOverOcean     I/O          #profiles over ocean
!       - nProfsOverSeaIce    I/O          #profiles over sea ice
!       - nProfsOverLand      I/O          #profiles over land
!       - nProfsOverSnow      I/O          #profiles over snow
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE CountClassTypes(SfcClass,nProfsOverOcean,nProfsOverSeaIce,nProfsOverLand,nProfsOverSnow)
    INTEGER :: SfcClass,nProfsOverOcean,nProfsOverSeaIce,nProfsOverLand,nProfsOverSnow
    IF (SfcClass .eq. OC_TYP)     nProfsOverOcean  = nProfsOverOcean+1
    IF (SfcClass .eq. SEAICE_TYP) nProfsOverSeaIce = nProfsOverSeaIce+1
    IF (SfcClass .eq. LD_TYP)     nProfsOverLand   = nProfsOverLand+1
    IF (SfcClass .eq. SNOW_TYP)   nProfsOverSnow   = nProfsOverSnow+1
    RETURN
  END SUBROUTINE CountClassTypes



!===============================================================
! Name:         DetermGeogrLimits
!
!
! Type:         Subroutine
!
!
! Description:  Determines a logical signal that says if the 
!               particular profile (with its lat, lon, pass, type)
!               pass the test.
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - Ok                 O            Logical signal (within geogr limits or not)
!       - GeogrLimits        I            Flag of requested geogr limits (all, 
!                                         ocean-only, land-only, within lat/lon box)
!       - stypeSfc           I            Surface type of partic. profile
!       - lat                I            Latitude of partic. profile
!       - lon                I            Longitude of partic. profile
!       - minLat             I            Minimum latitude of lat/lon box
!       - maxLat             I            Maximum latitude of lat/lon box
!       - minLon             I            Minimum Longitude of lat/lon box
!       - maxLon             I            Maximum Longitude of lat/lon box
!       - pass               I            Requested orbit mode to select
!       - node               I            Actual orbit mode of partic. profile
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE  DetermGeogrLimits(Ok,GeogrLimits,stypeSfc,lat,lon,minLat,maxLat,minLon,maxLon,pass,node)
    LOGICAL :: OK
    INTEGER :: GeogrLimits,stypeSfc,pass,node
    REAL    :: lat,lon,minLat,maxLat,minLon,maxLon
    Ok =.TRUE.
    !---Process data only in [lat-lon] box
    IF (GeogrLimits .eq. 1 .and. stypeSfc .ge. 0) THEN 
       IF( (lat .lt. minLat) .or. (lat .gt. maxLat) .or. (lon .lt. minLon) .or. (lon .gt. maxLon) ) Ok=.FALSE.
    !---Process only data over ocean
    ELSE IF (GeogrLimits .eq. 2 .and. stypeSfc .ge. 0) THEN 
       IF (stypeSfc .ne. OC_TYP) Ok=.FALSE.
    !---Process only data over land
    ELSE IF (GeogrLimits .eq. 3 .and. stypeSfc .ge. 0) THEN
       IF (stypeSfc .ne. LD_TYP) Ok=.FALSE.
    ENDIF
    !---Process according to node/pass
    IF (pass.ne.2 .and. pass.ne.node) Ok=.FALSE.
    RETURN
  END SUBROUTINE DetermGeogrLimits


!===============================================================
! Name:         day_month
!
!
! Type:         Subroutine
!
!
! Description:  Converts the year and julian day information into
!               a regular calendar day and month information
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - year               I               year
!       - month              O               month
!       - day                O               calendar day within a month
!       - jul_day            I               Julian day
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!       11-04-2008      Wanchun Chen  fixed bug for leap year
!                                     removed go to statement
!
!===============================================================

    subroutine day_month(year,month,day,jul_day)
    !---Input/Output variables
    INTEGER year,month,day,jul_day,sum1,i
    !---Local variables
    INTEGER,PARAMETER                :: nmonth=12
    INTEGER, SAVE, DIMENSION(nmonth) :: day1=(/31,28,31,30,31,30,31,31,30,31,30,31/)
    INTEGER, SAVE, DIMENSION(nmonth) :: day2=(/31,29,31,30,31,30,31,31,30,31,30,31/)
    sum1=0
    month=0
    monthLoop: DO i=1,12
       if( ( MOD(year,4).eq.0 .and. MOD(year,100).ne.0 ) .or. MOD(year,400).eq.0 ) then
          sum1=sum1+day2(i)
       else
          sum1=sum1+day1(i)
       endif
       if(jul_day.le.sum1) then
         month = i
         exit monthLoop
       endif
    ENDDO monthLoop

    if(( MOD(year,4).eq.0 .and. MOD(year,100).ne.0 ) .or. MOD(year,400).eq.0) then
       day=jul_day-(sum1-day2(i))
    else
       day=jul_day-(sum1-day1(i))
    endif
    
    return
  END subroutine day_month


!===============================================================
! Name:         compJulDay
!
!
! Type:         Subroutine
!
!
! Description:  Computes the Julian day from the month, day, year
!               information
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - year               I              Year
!       - month              I              Month
!       - day                I              Calendar day
!       - jul_day            O              Julian day
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!       11-04-2008      Wanchun Chen  fixed bug for leap year
!
!===============================================================

  subroutine compJulDay(year,month,day,jul_day)
    !---Input/Output variables
    INTEGER year,month,day,jul_day
    !---Local variables
    INTEGER                          :: sum1,i
    INTEGER,PARAMETER                :: nmonth=12
    INTEGER, SAVE, DIMENSION(nmonth) :: day1=(/31,28,31,30,31,30,31,31,30,31,30,31/)
    INTEGER, SAVE, DIMENSION(nmonth) :: day2=(/31,29,31,30,31,30,31,31,30,31,30,31/)
    sum1=0
    DO i=1,month-1
       if( ( MOD(year,4).eq.0 .and. MOD(year,100).ne.0 ) .or. MOD(year,400).eq.0 ) then
          sum1=sum1+day2(i)
       else
          sum1=sum1+day1(i)
       endif
    ENDDO
    jul_day=sum1+day
    return
  END subroutine compJulDay


!===============================================================
! Name:         read_topography
!
!
! Type:         Subroutine
!
!
! Description:  Reads the topography info (once) and gives 
!               local information (altitude, type, etc) for every
!               lat/lon point. There is a mapping vector to give
!               the surface type from the CRTM-like surface type.
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - filetopo           I            Name of topography file
!       - sensor_ID          I            sensor ID integer number from Consts.f90
!       - xlat               I            Latitude
!       - xlon               I            Longitude
!       - xalt               O            Altitude
!       - stypeSfc           O            Surface type
!       - xcover             O            Surface cover
!       - stypeSfcCRTM       O            CRTM-compatible surface type
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!       08-31-2022      Yong-Keun Lee   0.05 degree interval from 1/6 degree
!
!===============================================================


  SUBROUTINE read_topography_onetwentieth(filetopo,sensor_ID,xlat,xlon,iscanPOs,xalt,stypeSfc)
    !---Input variables
    REAL             :: xlat,xlon
    CHARACTER(LEN=*) :: filetopo
    CHARACTER(LEN=95) :: filetopo1
    !---Output variables
    REAL             :: xalt,xcover
    REAL             :: xalt1,xalt2,xcover1
    INTEGER          :: stypeSfc,stypeSfcCRTM
    INTEGER          :: stypeSfc1,stypeSfcCRTM1,ntot
    INTEGER          :: stype, sensor_ID
    !---Local variables
    REAL(KIND=8)     :: alat,alon,alon1,alt,scover,alt1,scover1
    INTEGER          :: i,index_lat,index_lon,iscanPos
    INTEGER          :: j,index_lat1,index_lon1
    CHARACTER(LEN=1) :: I1,I3
    INTEGER(KIND=2)  :: I2
    INTEGER, SAVE    :: init_topo=0,iu,ju
    INTEGER :: SurfaceID
    REAL    :: sat_zen_angle, PI
    INTEGER :: dI,dJ, i11, i12, j11, j12, iwater, iland, M, L, j10
    INTEGER,PARAMETER            :: nfov_n20=96,nfov_n19=90
    REAL,    DIMENSION(nfov_n20) :: dIJ_n20, dIJ
    INTEGER, DIMENSION(nfov_n20) :: IJ_n20, IJ
    REAL,    DIMENSION(nfov_n19) :: dIJ_n19
    INTEGER, DIMENSION(nfov_n19) :: IJ_n19
    REAL                   :: dIJ_gpm, dIJ_ssmis
    INTEGER                ::  IJ_gpm,  IJ_ssmis

! dIJ = 32.0/111.17/0.05/cos(zenith*3.14159/180.0)**1.4   FOV size 32km based on channel 3 (50 GHz)
    DATA dIJ_n20 /18.15721, 16.73128, 15.54098, 14.52938, 13.65841, 12.90499, 12.24063, 11.65405, 11.13041, 10.66175, &
                  10.24113,  9.85967,  9.51341,  9.19750,  8.90767,  8.64238,  8.39824,  8.17370,  7.96637,  7.77435, &
                   7.59717,  7.43251,  7.28006,  7.13855,  7.00725,  6.88544,  6.77225,  6.66713,  6.56975,  6.47936, &
                   6.39593,  6.31867,  6.24748,  6.18203,  6.12191,  6.06706,  6.01723,  5.97216,  5.93179,  5.89579, &
                   5.86424,  5.83689,  5.81366,  5.79449,  5.77931,  5.76807,  5.76073,  5.75726,  5.75766,  5.76192, &
                   5.77006,  5.78208,  5.79808,  5.81803,  5.84207,  5.87024,  5.90262,  5.93943,  5.98067,  6.02657, &
                   6.07735,  6.13306,  6.19422,  6.26063,  6.33291,  6.41124,  6.49591,  6.58746,  6.68620,  6.79258, &
                   6.90723,  7.03050,  7.16355,  7.30671,  7.46115,  7.62792,  7.80705,  8.00184,  8.21180,  8.43951, &
                   8.68691,  8.95573,  9.24940,  9.57052,  9.92232, 10.31095, 10.73899, 11.21612, 11.74925, 12.34817, &
                  13.02598, 13.79728, 14.68818, 15.72652, 16.94987, 18.42213/
    DATA IJ_n20 /  18,   16,   15,   14,   13,   12,   12,   11,   11,   10,  &
                   10,    9,    9,    9,    8,    8,    8,    8,    7,    7,  &
                    7,    7,    7,    7,    7,    6,    6,    6,    6,    6,  &
                    6,    6,    6,    6,    6,    6,    6,    5,    5,    5,  &
                    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,  &
                    5,    5,    5,    5,    5,    5,    5,    5,    5,    6,  &
                    6,    6,    6,    6,    6,    6,    6,    6,    6,    6,  &
                    6,    7,    7,    7,    7,    7,    7,    8,    8,    8,  &
                    8,    8,    9,    9,    9,   10,   10,   11,   11,   12,  &
                   13,   13,   14,   15,   16,   18/

! dIJ = 48.0/111.17/0.05/cos(zenith*3.14159/180.0)**1.4  based on channel 3 (50GHz)
    DATA dIJ_n19 /22.02869, 20.69209, 19.53348, 18.51468, 17.61852, 16.81682, 16.10174, 15.45707, 14.87551, 14.34600, &
                  13.86442, 13.42425, 13.02063, 12.64998, 12.30808, 11.99335, 11.70186, 11.43248, 11.18303, 10.95151, &
                  10.73667, 10.53750, 10.35239, 10.18073, 10.02104,  9.87315,  9.73641,  9.60968,  9.49231,  9.38434, &
                   9.28487,  9.19366,  9.11033,  9.03448,  8.96601,  8.90451,  8.84974,  8.80154,  8.75965,  8.72405, &
                   8.69459,  8.67114,  8.65364,  8.64196,  8.63615,  8.63615,  8.64195,  8.65362,  8.67109,  8.69456, &
                   8.72400,  8.75959,  8.80148,  8.84969,  8.90442,  8.96596,  9.03439,  9.11022,  9.19353,  9.28475, &
                   9.38422,  9.49220,  9.60949,  9.73624,  9.87298, 10.02082, 10.18042, 10.35218, 10.53709, 10.73643, &
                  10.95124, 11.18272, 11.43216, 11.70160, 11.99289, 12.30764, 12.64945, 13.02023, 13.42372, 13.86383, &
                  14.34550, 14.87445, 15.45639, 16.10110, 16.81574, 17.61722, 18.51334, 19.53150, 20.69082, 22.02670/

    DATA IJ_n19 /22,  20,  19,  18,  17,  16,  16,  15,  14,  14,  &
                 13,  13,  13,  12,  12,  11,  11,  11,  11,  10,  &
                 10,  10,  10,  10,  10,   9,   9,   9,   9,   9,  &
                  9,   9,   9,   9,   8,   8,   8,   8,   8,   8,  &
                  8,   8,   8,   8,   8,   8,   8,   8,   8,   8,  &
                  8,   8,   8,   8,   8,   8,   9,   9,   9,   9,  &
                  9,   9,   9,   9,   9,  10,  10,  10,  10,  10,  &
                 10,  11,  11,  11,  11,  12,  12,  13,  13,  13,  &
                 14,  14,  15,  16,  16,  17,  18,  19,  20,  22/

       PI=3.141592653589
       dIJ_gpm=4.76142406   ! dIJ = 13.0/111.17/0.05/cos(zenith*3.14159/180.0)**1.4, zenith=53
       IJ_gpm=4
       dIJ_ssmis=17.6378460 ! dIJ = 48.0/111.17/0.05/cos(zenith*3.14159/180.0)**1.4, zenith=53.1
       IJ_ssmis=17
!             
    !---If Lat/Lon information undefined, return undefined stype
    IF ((xlat .le. -999.) .or. (xlon .le. -999.)) THEN
       stypeSfc =DEFAULT_VALUE_INT !Undefined lat/lon->undefined stype
       return
    ENDIF
    alat = DBLE(xlat)
    alon = DBLE(xlon)
    if(init_topo.eq.0) then
       iu=Get_Lun()
       open(iu,file=filetopo,form='unformatted',access='direct',RECL=4)
       init_topo=1
    endif
    if( alon .gt. DBLE(180.0) ) then
       alon1=alon-DBLE(360.0)
    else
       alon1=DBLE(alon)
    endif
! to read surface type 0.05 degree interval VIIRS NDVI 2020 which Mark Liu provided
    index_lat1=INT((DBLE(90.0)-alat)/0.05+1)
    index_lon1=INT((DBLE(180)+alon1)/0.05+1)
    if(index_lat1.gt.3600) index_lat1=3600
    if(index_lon1.gt.7200) index_lon1=7200

    IF(sensor_ID.EQ.sensor_id_npp.OR.sensor_ID.EQ.sensor_id_n20.OR.sensor_ID.EQ.sensor_id_n21.OR. &
       sensor_ID.EQ.sensor_id_metopSGA1) THEN
       dI = int( dIJ_n20(iscanPos)/cos(alat*PI/180.0) )
       dJ = IJ_n20(iscanPos)
    ENDIF
    IF(sensor_ID.EQ.sensor_id_n19.OR.sensor_ID.EQ.sensor_id_n18.OR.sensor_ID.EQ.sensor_id_metopA.OR.  &
       sensor_ID.EQ.sensor_id_metopB.OR.sensor_ID.EQ.sensor_id_metopC) THEN
       dI = int( dIJ_n19(iscanPos)/cos(alat*PI/180.0) )
       dJ = IJ_n19(iscanPos)
    ENDIF
    IF(sensor_ID.EQ.sensor_id_gpm) THEN
       dI = int( dIJ_gpm/cos(alat*PI/180.0) )
       dJ = IJ_gpm
    ENDIF
    IF(sensor_ID.EQ.sensor_id_amsre.OR.sensor_ID.EQ.sensor_id_gcomw1) THEN
       dI = int( dIJ_gpm/cos(alat*PI/180.0) )
       dJ = IJ_gpm
    ENDIF
    IF(sensor_ID.EQ.sensor_id_trmm.OR.sensor_ID.EQ.sensor_id_trmm2a12) THEN
       dI = int( dIJ_gpm/cos(alat*PI/180.0) )
       dJ = IJ_gpm
    ENDIF
    IF(sensor_ID.EQ.sensor_id_fy3ri) THEN
       dI = int( dIJ_gpm/cos(alat*PI/180.0) )
       dJ = IJ_gpm
    ENDIF
    IF(sensor_ID.EQ.sensor_id_mtma.OR.sensor_ID.EQ.sensor_id_mtsa) THEN
       dI = int( dIJ_gpm/cos(alat*PI/180.0) )
       dJ = IJ_gpm
    ENDIF
    IF(sensor_ID.EQ.sensor_id_F16.OR.sensor_ID.EQ.sensor_id_F17.OR. &
       sensor_ID.EQ.sensor_id_F18.OR.sensor_ID.EQ.sensor_id_F19) THEN
       dI = int( dIJ_ssmis/cos(alat*PI/180.0) )
       dJ = IJ_ssmis
    ENDIF
    IF(sensor_ID.GE.24) THEN    ! This should be modified once a new instrument is added.
       dI = int( dIJ_gpm/cos(alat*PI/180.0) )
       dJ = IJ_gpm
    ENDIF
       i=index_lon1
       j=index_lat1

       i11 = i - dI
       i12 = i + dI
       j11 = j - dJ
       j12 = j + dJ
       IF( i11 < 1 ) i11 = 1
       IF( i12 > 7200 ) i12 = 7200
       IF( j11 < 1 ) j11 = 1
       IF( j12 > 3600 ) j12 = 3600
       iwater = 0
       iland  = 0
       xalt2=0.
       xalt1=0.
       xalt=0.
       ntot=0
       DO M = j11, j12
         DO L = i11, i12
           j10=(M-1)*7200+L
           read(iu,rec=j10) I1,I3,I2
           alt1         = I2*1.0
           scover1      = ICHAR(I3)*0.01
           xalt1        = REAL(alt1) 
           xcover1      = REAL(scover1)
           stypeSfcCRTM1= ICHAR(I1)
           !---Go from the topography database convention to the MIRS convention
           stypeSfc=stypeSfcCRTM1
           xalt2=xalt2+xalt1
           ntot=ntot+1

           IF( stypeSfc == 17 ) THEN
             iwater = iwater + 1
           ELSE
             iland = iland + 1
           END IF
         END DO
       END DO
       xalt=xalt2/float(ntot)

       IF( iland >= iwater ) THEN
          stypeSfc  = 2
       ELSE IF( iland < iwater) THEN
          stypeSfc  = 0
       END IF
    return
  END SUBROUTINE read_topography_onetwentieth

!===============================================================
! Name:          Stdev
!
!
! Type:         Function
!
!
! Description:  Computes standard deviation of a vector X
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - x                  I              Vector
!       - Stdev              O              Standard deviation
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION Stdev(x)
    REAL, DIMENSION(:) :: x
    REAL               :: Stdev, Mean
    INTEGER            :: N,I
    N    = SIZE(x)
    Mean = SUM(x(1:N))/N
    Stdev=0.
    DO i=1,N
       Stdev=Stdev+(X(i)-Mean)**2.
    ENDDO
    Stdev=SQRT(Stdev/N)
    RETURN
  END FUNCTION Stdev

!===============================================================
! Name:          Mean
!
!
! Type:         Function
!
!
! Description:  Computes the mean value of a vector x
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - x                  I              Vector
!       - Mean               O              Mean value
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION Mean(x)
    REAL, DIMENSION(:) :: x
    REAL               :: Mean
    INTEGER            :: N
    N    = SIZE(x)
    Mean = SUM(x(1:N))/N
    RETURN
  END FUNCTION Mean

!===============================================================
! Name:         Translate2I4
!
!
! Type:         Function
!
!
! Description:  Converts four one-byte-long characters into an 
!               four-bytes-long Integer
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - numChar           I             Vector of 4 one-byte-long CHAR 
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION Translate2I4(numChar)
    !---Big-Endian to Little-Endian Translation (4 bytes -> integer*4)
    INTEGER(4),       PARAMETER    :: POW=256   
    INTEGER(4)                     :: Translate2I4
    CHARACTER(LEN=1), DIMENSION(4) :: numChar
    INTEGER(4)                     :: I1,I2,I3,I4
    !---------------
    I1=INT(REMOVESIGN(IACHAR(numChar(1))),KIND=4)
    I2=INT(REMOVESIGN(IACHAR(numChar(2))),KIND=4)
    I3=INT(REMOVESIGN(IACHAR(numChar(3))),KIND=4)
    I4=INT(REMOVESIGN(IACHAR(numChar(4))),KIND=4)
    Translate2I4=I1*POW**3+I2*POW**2+I3*POW+I4
    !----------
    !Translate2I4=IACHAR(numChar(1))*256**3+IACHAR(numChar(2))*256**2+&
    !     IACHAR(numChar(3))*256+IACHAR(numChar(4))
    !-----------
    RETURN
  END FUNCTION Translate2I4

!===============================================================
! Name:          Translate2I2
!
!
! Type:         Function
!
!
! Description:  Converts two one-byte-long characters into an 
!               two-bytes-long Integer
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - numChar            I           Vector of 2 one-byte-long CHAR
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION Translate2I2(numChar)
    !---Big-Endian to Little-Endian Translation (2 bytes -> integer)
    INTEGER(2),       PARAMETER    :: POW=256   
    INTEGER(2)                     :: Translate2I2
    CHARACTER(LEN=1), DIMENSION(2) :: numChar
    INTEGER(2)                     :: I1,I2
    !-------------
    I1=INT(REMOVESIGN(IACHAR(numChar(1))),KIND=2)
    I2=INT(REMOVESIGN(IACHAR(numChar(2))),KIND=2)
    Translate2I2=I1*POW+I2
    !--------
    !Translate2I2=IACHAR(numChar(1))*256+IACHAR(numChar(2))
    !--------
    RETURN
  END FUNCTION Translate2I2

!===============================================================
! Name:          Translate2I1
!
!
! Type:         Function
!
!
! Description:  Converts 1 one-byte-long characters into an 
!               one-byte-long Integer
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - numChar           I            Vector of 1 one-byte-long CHAR
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION Translate2I1(numChar)
    !---Big-Endian to Little-Endian Translation (1 bytes -> integer)
    INTEGER                        :: Translate2I1
    CHARACTER(LEN=1), DIMENSION(1) :: numChar
    INTEGER(1)                     :: I1
    !---------------
    I1=INT(REMOVESIGN(IACHAR(numChar(1))),KIND=1)
    Translate2I1=I1
    !----------
    !Translate2I1=IACHAR(numChar(1))
    !----------
    RETURN
  END FUNCTION Translate2I1

!===============================================================
! Name:          REMOVESIGN
!
!
! Type:         Function
!
!
! Description:  Removes the negative sign from an integer
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - I                  I              Integer whose sign to be removed
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  INTEGER FUNCTION REMOVESIGN(I)
    INTEGER :: I
    REMOVESIGN=INT(I)
    IF (I.LT.0) REMOVESIGN=256+INT(I)
  END FUNCTION REMOVESIGN



  REAL FUNCTION plank(r, wvnm)
    !============================================================================================
    !    Purpose:
    !      Returns brightness temperature
    !    
    !    Input variables
    !        wvnm : wavenumber (cm^-1)
    !        r    : radiance (w m^-2 st^-1 cm)
    !      
    !    Output variables
    !        radiance_to_tb : brightness temperature (k)
    !
    !    Internal variables
    !        c1      : w m**-2 cm ster^-1 cm**3
    !        c2      : k cm
    ! 
    !    Record of revisions:
    !      Date           Programmer                 Description of change
    !    =======       ================         ==============================
    !    1998.8         Fuzhong Weng             Original code
    !
    !============================================================================================
    IMPLICIT NONE
    REAL(4), INTENT(IN) :: wvnm, r
    REAL(4) :: c1=1.1909E-8, c2=1.4388
    IF ( wvnm <= 0.0 .OR. r <= 0.0 ) THEN 
       PRINT*, "Wavenumber or radiance is less than or equals zero"
       plank = -999
    ELSE
       plank = c2*wvnm/LOG(c1*wvnm**3./r + 1.0)
    END IF
    RETURN 
  END FUNCTION plank
  
  
  
  SUBROUTINE linear(alat, alon, ix, jy, maxcol, maxrow, resl)
    !============================================================================================
    !    Purpose:
    !      Inteperate obs points into grid points
    !
    !    Input variables
    !      alat: obs Latitude        alon: obs Longitude
    !      maxcol: max column        maxrow: max row 
    !      resl: grid point resolution
    !
    !    Output variables
    !      ix: grid point longitude   jy: grid point latitude
    !
    !    Record of revisions:
    !      Date           Programmer                 Description of change
    !    =======       ================         ==============================
    !    1998.8         Fuzhong Weng             Original code
    !
    !============================================================================================
    IMPLICIT NONE
    REAL(4), INTENT(IN) :: alat, alon, resl
    INTEGER(2), INTENT(IN) :: maxcol, maxrow
    INTEGER(2), INTENT(OUT) :: ix, jy
    REAL(4) :: blat, blon

    blat = (90.0 - alat)
    jy = INT( blat/resl + 1, kind = 2 )
    IF (jy > maxrow) jy = maxrow
    IF (jy < 1) jy = 1
    IF (alon <= 0.0) THEN
       blon = 360 + alon
    ELSE 
       blon = alon
    END IF
    ix = INT( blon/resl + 1, kind = 2 )
    IF (ix < 1) ix = 1
    IF (ix > maxcol) ix = 1       ! put to the other side of grid data
  END SUBROUTINE linear
  
  
  
  SUBROUTINE cnode(alat1, alat2, node)
    !============================================================================================
    !    Purpose:
    !
    !    Input variables
    !      alat2:  current Latitude        
    !      alat1:  previous Latitude
    !
    !    Output variables
    !      node:
    !
    !    Record of revisions:
    !      Date           Programmer                 Description of change
    !    =======       ================         ==============================
    !    1998.8         Fuzhong Weng             Original code
    !
    !============================================================================================
    IMPLICIT NONE
    REAL(4), INTENT(IN) :: alat2, alat1
    INTEGER(2), INTENT(OUT) :: node
    REAL(4) :: diff    
    diff = alat2 - alat1
    node = 99
    IF (diff < 0.0) THEN 
       node = 0  ! Descending (Southbound 7:30 am) 
    ELSE
       node = 1  ! Ascending (Northbound 7:30 pm) 
    END IF
    IF (ABS(diff) > 5.0) node = 99  
  END SUBROUTINE cnode


!===============================================================
! Name:         TransfSigma2Press
!
!
! Type:         Subroutine
!
!
! Description:  Transforms a sigma vertical grid into a pressure 
!               grid.
!
!
! Arguments:
!
!      Name                  Type          Description
!      ---------------------------------------------------
!       - nx                  I            Number of X-direction points
!       - ny                  I            Number of Y-direction points
!       - nz                  I            Number of vertical grid points
!       - ptop                I            Top-Atmosphere pressure assumed
!       - SfcPress            I            Surface pressure
!       - hfsigma             I            Sigma grid
!       - press               O            Pressure grid field (3D)
!       - deltP               I            Delta pressure 3D field
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE TransfSigma2Press(nx,ny,nz,ptop,SfcPress,hfsigma,press,deltP)
    INTEGER                  :: nx,ny,nz
    REAL                     :: ptop
    REAL,   DIMENSION(:)     :: hfsigma
    REAL,   DIMENSION(:,:)   :: SfcPress
    REAL,   DIMENSION(:,:,:) :: press,deltP
    !---Local variables
    INTEGER                  :: i,j,k
    do i=1,nx
       do j=1,ny
          do k=1,nz
             press(i,j,k)= ptop+hfsigma(k)*(Sfcpress(i,j)-ptop)+deltP(i,j,k)
          enddo
       enddo
    enddo
    RETURN
  END SUBROUTINE TransfSigma2Press

!===============================================================
! Name:         deg2rad
!
!
! Type:         Function
!
!
! Description:  Converts degrees to radians
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - angle              I              Angle in degrees
!       - deg2rad            O              Angle in radians
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  REAL FUNCTION deg2rad(angle)
    REAL :: angle
    deg2rad=angle*3.14159/180.
  END FUNCTION deg2rad


!===============================================================
! Name:         rad2deg
!
!
! Type:         Function
!
!
! Description:  Converts radians to degrees
!
!
! Arguments:
!
!      Name                Type             Description
!      ---------------------------------------------------
!       - rad               I               Angle in radians
!       - rad2deg           O               Angle in degrees
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  REAL FUNCTION rad2deg(rad)
    REAL :: rad
    rad2deg=rad*180./3.14159
  END FUNCTION rad2deg


!===============================================================
! Name:          Get_Lun
!
!
! Type:          Function
!
!
! Description:  Gets automatically a unit number by making sure 
!               the unit number is not taken already. Eliminates
!               the risk of having unit numbers confused in many
!               places and avoids having to declare the unit numbers 
!               in a hard coded manner. Inherited from CRTM.
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!      - get_lun             O              Unit number
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION Get_Lun() RESULT( Lun )
    INTEGER :: Lun
    Lun = 9
    Lun_Search: DO
       Lun = Lun + 1
       IF ( .NOT. File_Unit_Exists( Lun ) ) THEN
          Lun = -1
          EXIT Lun_Search
       END IF
       IF ( .NOT. File_Open_by_Unit( Lun ) ) EXIT Lun_Search
    END DO Lun_Search
  END FUNCTION Get_Lun


!===============================================================
! Name:         File_Unit_Exists
!
!
! Type:         Function
!
!
! Description:  Tests if file unit exists already. Inherited from CRTM
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - FileID             I              Unit number
!       - File_Unit_Exists   O              Logical 'unit number exists'
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION File_Unit_Exists( FileID ) RESULT ( Existence )
    INTEGER, INTENT( IN ) :: FileID
    LOGICAL :: Existence
    INQUIRE( UNIT = FileID, EXIST = Existence )
  END FUNCTION File_Unit_Exists
  
!===============================================================
! Name:         File_Open_by_Unit
!
!
! Type:        Function
!
!
! Description:  Tests if file is open or not (by checking unit number).
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - FileID             I              Unit number
!       - File_Open_by_Unit  O              Logical 'File is open'
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  FUNCTION File_Open_by_Unit( FileID ) RESULT ( Is_Open )
    INTEGER, INTENT( IN ) :: FileID
    LOGICAL :: Is_Open
    INQUIRE( UNIT = FileID, OPENED = Is_Open )
  END FUNCTION File_Open_by_Unit

  
!===============================================================
! Name:         ShrkMatrx
!
!
! Type:         Subroutine
!
!
! Description:  Shrinks matrix using a selection index vector
!               Worth noting that shrinking could be done in 
!               rows, columns or both.
!
! Arguments:
!
!      Name                 Type         Description
!      ---------------------------------------------------
!       - nch                I           Number of selected channels
!       - ShrkRadVec         I           Index-Vector of selected channels
!       - C                  I           Matrix to be shrinked
!       - Cstar              O           Shrinked matrix
!       - Idim               I           Flag onhow to shrink matrix 
!                                        (rows, columns, both)
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE ShrkMatrx(nch,ShrkRadVec,C,Cstar,Idim)
    REAL,    DIMENSION(:,:) :: C,Cstar
    INTEGER, DIMENSION(:)   :: ShrkRadVec
    INTEGER                 :: nch,Idim
    !-------Shrink both dimensions (1 & 2)
    IF (idim.eq.0)  Cstar(1:nch,1:nch) = C(ShrkRadVec(1:nch),ShrkRadVec(1:nch))
    !-------Shrink dimension along row axis
    IF (idim.eq.1) Cstar(1:nch,:) = C(ShrkRadVec(1:nch),:)
    !-------Shrink dimension along column axis
    IF (idim.eq.2) Cstar(:,1:nch) = C(:,ShrkRadVec(1:nch))
    RETURN
  END SUBROUTINE ShrkMatrx
  

!===============================================================
! Name:         ShrkVect
!
!
! Type:         Subroutine
!
!
! Description:  Shrinks a vector
!
!
! Arguments:
!
!      Name                 Type         Description
!      ---------------------------------------------------
!       - nch                I           Number of selected channels
!       - ShrkRadVec         I           Index-Vector of selected channels
!       - V                  I           Vector to be shrinked
!       - Vstar              O           Shrinked vector
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE ShrkVect(nch,ShrkRadVec,V,Vstar)
    REAL,    DIMENSION(:)  :: V,Vstar
    INTEGER, DIMENSION(:)  :: ShrkRadVec
    INTEGER                :: nch
    Vstar(1:nch) = V(ShrkRadVec(1:nch))
    RETURN
  END SUBROUTINE ShrkVect



!===============================================================
! Name:         printSummary
!
!
! Type:       Subroutine
!
!
! Description:  Prints a summary of the performances after the 
!               completion of the retrieval process.
!
!
! Arguments:
!
!          Name                    Type          Description
!      ---------------------------------------------------
!       - nprofiles                  I           Total # profiles submitted
!       - nIterTot                   I           Total # iterations
!       - nConvProfs                 I           # convergent profiles
!       - nprofsEff                  I           # effectively processed profiles
!       - nProfsOverOcean            I           # profiles detected over ocean
!       - nProfsOverSeaIce           I           # profiles detected over  sea ice
!       - nProfsOverLand             I           # profiles detected over land
!       - nProfsOverSnow             I           # profiles detected over snow
!       - Tim                        I           Vector containing all 
!                                                timing for different 
!                                                sections of the retrieval
!
!
! Modules needed:
!       - None
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE printSummary(nprofiles,nIterTot,Tim,nConvProfs,nprofsEff,&
       nProfsOverOcean,nProfsOverSeaIce,nProfsOverLand,nProfsOverSnow,filein,fileout)
    INTEGER               :: nprofiles,nConvProfs,iprof,nprofsEff
    INTEGER               :: nProfsOverOcean,nProfsOverSeaIce,nProfsOverLand,nProfsOverSnow
    INTEGER, DIMENSION(:) :: nIterTot
    REAL,    DIMENSION(:) :: Tim
    REAL                  :: SumIters,avgIter=0.,avgRate=0.
    CHARACTER(LEN=*)      :: filein, fileout
    Print *, ''
    Print *, ''
    Print *, ''
    Print *, '_____________________________________'
    Print *, ''
    Print *, 'Process Summary:'
    Print *, '_____________________________________'
    SumIters  =0.
    DO iprof=1,nprofiles
       IF (nIterTot(iprof) .ge. 0) THEN
          SumIters=SumIters+nIterTot(iprof)
       ENDIF
    ENDDO
    IF (nprofsEff .ne. 0) THEN
       avgIter=SumIters/nprofsEff
       avgRate=(real(nConvProfs)/real(nprofsEff))*100.
    ENDIF
    Write(*,'(a)'      )      'Input file='//TRIM(filein)
    Write(*,'(a)'      )      'Output file='//TRIM(fileout)
    Write(*,'(a45,i15)')      'Total Number of Profiles submitted:',nprofiles
    Write(*,'(a45,i15)')      'Total Number of Profiles processed:',nprofsEff
    Write(*,'(a45,i15)')      'Total Number of Profiles detected over ocean:',nProfsOverOcean
    Write(*,'(a45,i15)')      'Total Number of Profiles detected over ice:',nProfsOverSeaIce
    Write(*,'(a45,i15)')      'Total Number of Profiles detected over land:',nProfsOverLand
    Write(*,'(a45,i15)')      'Total Number of Profiles detected over snow:',nProfsOverSnow
    Write(*,'(a45,i15)')      'Number of Convergent Profiles:',nConvProfs
    Write(*,'(a45,i15)')      'Total Number of Iterations:',int(SumIters)
    Write(*,'(a45,f15.5)')    'Avg # Iterations/Profile:',avgIter
    Write(*,'(a45,f15.5,a1)') 'Convergence Rate:',avgRate,'%'
    Print *, '_____________________________________'
    Print *, ''
    Print *, 'Timing:'
    Print *, '_____________________________________'
    Write(*,'(a45,2f15.5,a1)') 'Uploading/Initialization:',Tim(2),(Tim(2)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Reading radiance/extr data:',Tim(3),(Tim(3)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Preparing matrices(Sa,Xb,Se,Fe,U,X1st):',Tim(4),(Tim(4)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Setting up scene/Merging cov, EOF proj:',Tim(5),(Tim(5)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Forward Operator:',Tim(6),(Tim(6)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Inversion & Pre-Post Processing:',Tim(7),(Tim(7)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Error Analysis & characterization:',Tim(8),(Tim(8)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Output (Scene & Monitoring):',Tim(9),(Tim(9)/Tim(1))*100.,'%'
    Write(*,'(a45,2f15.5,a1)') 'Total:',Tim(1),(Tim(1)/Tim(1))*100.,'%'
    print *

    RETURN
  END SUBROUTINE printSummary
  
  
!===============================================================
! Name:         fov_a
!
!
! Type:        Subroutine
!
!
! Description: Compute Field Of View of Amsua
!
!
! Arguments:
!
!           Name                    Type            Description
!      ---------------------------------------------------------
!       - fov_size_a                 I           fov size array
!
!
! Modules needed:
!       - None
!
!
! History:
!       02-14-2008      Wanchun Chen                    PSGS
!
!===============================================================
  
  SUBROUTINE fov_a(FOV_SIZE_A,NUMSPOT_A,satId)

    INTEGER                 :: satId
    INTEGER                 :: NUMSPOT_A
    REAL                    :: PI=3.141593
    REAL                    :: SCAN_ANG_A=3.3
    REAL                    :: REARTH=6371.0
    REAL                    :: RSAT=833.0
    REAL,   DIMENSION(0:NUMSPOT_A-1) :: fov_size_A 
    REAL,   DIMENSION(0:NUMSPOT_A) :: ang
    !REAL,   DIMENSION(0:SIZE(fov_size_A)+1) :: ang

    INTEGER :: i=0,j
    REAL    :: angle=0.0
    REAL    :: angle1=0.0

    if (satId .eq. SENSOR_ID_NPP) RSAT=817.0

    do i=0, NUMSPOT_A
      angle = PI * SCAN_ANG_A * (i - NUMSPOT_A/2.0) / 180.0
      angle1 = (REARTH + RSAT) * sin(angle) / REARTH
      angle1 = atan(angle1 / sqrt(1 - angle1 * angle1))
      ang(i) = (angle1 * 180.0 / PI) - SCAN_ANG_A * (i - NUMSPOT_A/2.0)
    enddo

    do i=0, NUMSPOT_A-1
       fov_size_A(i) = abs(ang(i+1) - ang(i))
    enddo

    return
  
  END SUBROUTINE fov_a


!===============================================================
! Name:         get_fov_size
!
!
! Type:        Subroutine
!
!
! Description: generic form of Computing Field Of View
!
!
! Arguments:
!
!           Name                    Type          Description
!      ---------------------------------------------------------
!       - fov_size                   O            fov size array
!       - NFOV                       I            NUMBER of Points
!       - SCAN_ANG                   I            scan angle
!       - RSAT                       I            height of satellite
!
!
! Modules needed:
!       - None
!
!
! History:
!       10-07-2010      Wanchun Chen
!
!===============================================================

  SUBROUTINE get_fov_size(fov_size,NFOV,SCAN_ANG,RSAT)

    INTEGER                   :: NFOV
    REAL, DIMENSION(0:NFOV-1) :: fov_size
    REAL                      :: SCAN_ANG
    REAL                      :: RSAT
    
    REAL                      :: PI=3.141593
    REAL                      :: REARTH=6371.0
    REAL, DIMENSION(0:NFOV)   :: ang
    INTEGER                   :: i=0,j
    REAL                      :: angle=0.0
    REAL                      :: angle1=0.0

    do i=0, NFOV
      angle = PI * SCAN_ANG * (i - NFOV/2.0) / 180.0
      angle1 = (REARTH + RSAT) * sin(angle) / REARTH
      angle1 = atan(angle1 / sqrt(1 - angle1 * angle1))
      ang(i) = (angle1 * 180.0 / PI) - SCAN_ANG * (i - NFOV/2.0)
    enddo

    do i=0, NFOV-1
       fov_size(i) = abs(ang(i+1) - ang(i))
    enddo

    return
  
  END SUBROUTINE get_fov_size


!===============================================================
! Name:         strcmp
!
!
! Type:        Function
!
!
! Description: Comparison 2 strings, return 0 if equal; 1 not equal
!
!
! Arguments:
!
!           Name                    Type            Description
!      ---------------------------------------------------
!       -  S1                       char string           
!       -  S2                       char string           
!
!
! Modules needed:
!       - None
!
!
! History:
!       07-28-2008      Wanchun Chen              QSS/PSGS/DELL
!
!===============================================================
function strcmp(s1,s2)
  integer          :: strcmp
  character(len=*) :: s1, s2
  integer          :: L1, L2, i
  
  L1=LEN_TRIM(s1)
  L2=LEN_TRIM(s2)
  
  if ( L1 .ne. L2 ) then
    strcmp = 1
    return
  else
    strcmp = 0
    do i=1,L1
      if ( s1(i:i) .ne. s2(i:i) ) then
        strcmp = 1
        return
      endif
    enddo
  endif
  return
end function strcmp


!-----------------------------------------------------------------------------
! Purpose:
!
!   To insert a string after the last slash character if there is a slash,
!   otherwise, insert into at beginning of strin
!   It's mainly proper at rdr2tdr step to generate TDR file names.
!
! Input Arguments:
!     - strin : INPUT String
!     - str_replace : replace part
!     - str_insert : the string to be inserted into string
!
! Ouput Arguments:
!     - : strout : the string after being inserted
!
! Histroy:
!   Wanchun Chen   07-22-2010	    Original coder
!
!-----------------------------------------------------------------------------
  
SUBROUTINE insert_path_string(strin, str_replace, str_insert, strout)

  !---- input variables ---- 
  character(len=*) :: strin
  character(len=*) :: str_replace
  character(len=*) :: str_insert

  !---- output variable ----
  character(len=*) :: strout

  !---- local variables ----
  integer :: indx = 0
  integer :: len_effective=0

  indx = INDEX (strin, '/', BACK = .TRUE.)
  len_effective = LEN_TRIM(strin)

  !---- if no slash inside input strin ---
  if( indx .lt. 1 ) then
    strout = TRIM(str_replace)//TRIM(str_insert)//TRIM(strin)
  else
    strout = TRIM(str_replace)//TRIM(str_insert)//strin(indx+1:len_effective)
  endif

END SUBROUTINE insert_path_string


!-----------------------------------------------------------------------------
! Purpose:
!
!   To insert a string after the last slash character if there is a slash,
!   otherwise, insert into at beginning of strin
!   It's mainly proper at FM step to generate FMSDR file names.
!  
! Input Arguments:
!     - strin : INPUT String
!     - str_append : append part
!     - str_old: gone part
!     - str_new: new part
!
! Ouput Arguments:
!     - : strout : the string after being inserted
!
! Histroy:
!   Wanchun Chen   07-28-2010	    Original coder
!
!-----------------------------------------------------------------------------
  
SUBROUTINE replace_path_string(strin, str_append, str_old, str_new, strout)

  !---- input variables ---- 
  character(len=*) :: strin
  character(len=*) :: str_append
  character(len=*) :: str_old
  character(len=*) :: str_new

  !---- output variable ----
  character(len=*) :: strout

  !---- local variables ----
  integer :: indx = 0, indx_dot = 0
  integer :: len_effective = 0
  character(len=128) :: str_tmp

  indx = INDEX (strin, '/', BACK = .TRUE.)
  len_effective = LEN_TRIM(strin)

  !---- if no slash inside input strin ---
  if( indx .lt. 1 ) then
    indx_dot = INDEX (strin, '.')
    strout = TRIM(str_append)//TRIM(str_new)//TRIM(strin(indx_dot:len_effective))
  else
    str_tmp = strin(indx+1:len_effective)
    indx_dot = INDEX (str_tmp, '.')
    len_effective = LEN_TRIM(str_tmp)
    strout = TRIM(str_append)//TRIM(str_new)//str_tmp(indx_dot:len_effective)
  endif

END SUBROUTINE replace_path_string

!-----------------------------------------------------------------------------
! Purpose:
!
!   To insert a string after the last slash character if there is a slash,
!   otherwise, insert into at beginning of strin
!   It's mainly proper at FM step to generate FMSDR file names.
!   Specific for Megha-Tropiques L1A filename convention
!  
! Input Arguments:
!     - strin : INPUT String
!     - str_append : append part
!     - str_old: gone part
!     - str_new: new part
!
! Ouput Arguments:
!     - : strout : the string after being inserted
!
! Histroy:
!   Wanchun Chen   07-28-2010	    Original coder
!
!-----------------------------------------------------------------------------
  
SUBROUTINE replace_path_string_mt(strin, str_append, str_mark, str_old, str_new, strout)

  !---- input variables ---- 
  character(len=*) :: strin
  character(len=1) :: str_mark
  character(len=*) :: str_append
  character(len=*) :: str_old
  character(len=*) :: str_new

  !---- output variable ----
  character(len=*) :: strout

  !---- local variables ----
  integer :: indx = 0, indx_dot = 0
  integer :: len_effective = 0
  character(len=128) :: str_tmp

  indx = INDEX (strin, '/', BACK = .TRUE.)
  len_effective = LEN_TRIM(strin)

  !---- if no slash inside input strin ---
  if( indx .lt. 1 ) then
!    indx_dot = INDEX (strin, '.')
    indx_dot = INDEX (strin, trim(str_mark))
    strout = TRIM(str_append)//TRIM(str_new)//TRIM(strin(indx_dot:len_effective))
  else
    str_tmp = strin(indx+1:len_effective)
!    indx_dot = INDEX (str_tmp, '.')
    indx_dot = INDEX (str_tmp, trim(str_mark))
    print *,'indx_dot=',indx_dot
    len_effective = LEN_TRIM(str_tmp)
    strout = TRIM(str_append)//TRIM(str_new)//str_tmp(indx_dot:len_effective)
  endif

END SUBROUTINE replace_path_string_mt

!-----------------------------------------------------------------------------
! Purpose:
!
!   Convert a satellite zenith angle to viewing angle
!  
! Input Arguments:
!     - sensor_id        : the sensor ID from Consts module
!     - sat_zenith_angle : specified sat zenith angle
!
! Ouput Arguments:
!     - : view_angle     : the calculated viewing angle
!
! Histroy:
!   Kevin Garrett   12-21-2011	    Original coder
!
!-----------------------------------------------------------------------------

SUBROUTINE SatZenAng2ViewAng(sensor_id,sat_zenith_angle,view_angle)
  !---input variables
  INTEGER      :: sensor_id
  REAL         :: sat_zenith_angle

  !---output variables
  REAL         :: view_angle

  !---local variables
  REAL            :: sat_altitude,view_angle_rad,sat_zen_rad
  REAL, PARAMETER :: earth_radius=6378.1

  view_angle=-999
  sat_zen_rad = deg2rad(sat_zenith_angle)

  IF (sensor_id .eq. SENSOR_ID_NPP) THEN
     sat_altitude = 824.0
     view_angle_rad=(sin(sat_zen_rad)*earth_radius)/(sat_altitude+earth_radius)
     view_angle_rad=asin(view_angle_rad)
     view_angle=rad2deg(view_angle_rad)
  ENDIF

END SUBROUTINE SatZenAng2ViewAng

END MODULE misc
