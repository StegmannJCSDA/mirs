!$Id$
!-----------------------------------------------------------------------------------------------
! Name:         Preclassif
! 
! Type:         F90 module
!
! Description:
!       Module containing subroutines related to the pre-classification process.
!       Note that this module could be sensor-specific, as every sensor would
!       have its own pre-classification algorithm.
!
! Modules needed:
!       - misc
!       - Consts
!
! Subroutines contained:
!       - preClassAtm
!       - preClassSfc
!
! Data type included:
!       - none
! 
! History:
!      2006   S.A. Boukabara IMSG Inc. @ NOAA/NESDIS/ORA 
!      2017   Junye Chen ESSIC/UMD @ NOAA/NESDIS/STAR, add JPSS/N20
!
!-----------------------------------------------------------------------------------------------

MODULE Preclassif
  USE misc
  USE Consts
  IMPLICIT NONE
  PRIVATE
  !----Publicly available subroutine(s)
  PUBLIC :: preClassAtm,preClassSfc
  PUBLIC :: applySeaIceClimo
  !----INTRINSIC functions used in this module
  INTRINSIC :: ABS
CONTAINS

!===============================================================
! Name:         preClassAtm
!
!
! Type:         Subroutine
!
!
! Description:  Performs prec-classification (Atmospheric part)
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - Year               I             Year
!       - Julday             I             Julian day
!       - NCHAN              I             Number of channels
!       - FREQ               I             Central frequencies
!       - xlat               I             Latitude
!       - xlon               I             Longitude
!       - stypeSfc           I             Ocean/Land flag
!       - SENSOR_ID          I             Sensor ID
!       - Y                  I             Brightness Temperature vector
!       - AtmClass           O             Atmospheric class determined
!
!
! Modules needed:
!       - SURFACE_CLASSIFICATION_ALG
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE preClassAtm(Year, Julday, NCHAN, FREQ, xlat, xlon, stypeSfc,SENSOR_ID,Y,AtmClass,TskPreclass)
    INTEGER               :: NCHAN
    INTEGER               :: sTypeSfc,AtmClass,SENSOR_ID,LAND
    INTEGER               :: Year,Julday
    REAL,    DIMENSION(:) :: Y,FREQ
    REAL                  :: xlat,xlon,TskPreclass

! TskPreclass not yet reliable, so do not use
    TskPreclass = DEFAULT_VALUE_REAL
    IF (stypeSfc .ge. 0) THEN
       IF (stypeSfc .eq. OC_TYP .or. stypeSfc .eq. SEAICE_TYP ) THEN
          LAND=0
       ELSE
          LAND=1
       ENDIF
       CALL SURFACE_CLASSIFICATION_ALG(Year,Julday,SENSOR_ID,NCHAN,FREQ,Y,xlat,xlon,LAND,AtmClass,TskPreclass)
    ELSE
       AtmClass=DEFAULT_VALUE_INT
    ENDIF
    RETURN
  END SUBROUTINE preClassAtm


!===============================================================
! Name:         preClassSfc
!
!
! Type:         Subroutine
!
!
! Description:  Performs prec-classification (Surface part)
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - Year               I             Year
!       - Julday             I             Julian day
!       - NCHAN              I             Number of channels
!       - FREQ               I             Central frequencies
!       - xlat               I             Latitude
!       - xlon               I             Longitude
!       - stypeSfc           I             Ocean/Land flag
!       - SENSOR_ID          I             Sensor ID
!       - Y                  I             Brightness Temperature vector
!       - SfcClass           O             Surface class determined
!
!
! Modules needed:
!       - SURFACE_CLASSIFICATION_ALG
!
!
! History:
!       03-22-2007      Sid Ahmed Boukabara, IMSG Inc @ NOAA/NESDIS/ORA
!
!===============================================================

  SUBROUTINE preClassSfc(Year, Julday, NCHAN, FREQ, xlat,xlon,stypeSfc,SENSOR_ID,Y,SfcClass,TskPreclass)
    INTEGER               :: NCHAN
    INTEGER               :: SfcClass,stypeSfc,SENSOR_ID,LAND
    INTEGER               :: Year,Julday
    REAL,    DIMENSION(:) :: Y,FREQ
    REAL                  :: xlat,xlon,TskPreclass

! TskPreclass not yet reliable, so do not use
    TskPreclass = DEFAULT_VALUE_REAL
    IF (stypeSfc .ge. 0) THEN
       IF (stypeSfc .eq. OC_TYP .or. stypeSfc .eq. SEAICE_TYP ) THEN
          LAND=0
       ELSE
          LAND=1
       ENDIF
       CALL SURFACE_CLASSIFICATION_ALG(Year,Julday,SENSOR_ID,NCHAN,FREQ,Y,xlat,xlon,LAND,SfcClass,TskPreclass)
    ELSE
       SfcClass=DEFAULT_VALUE_INT
    ENDIF
    RETURN
  END SUBROUTINE preClassSfc





  SUBROUTINE SURFACE_CLASSIFICATION_ALG(Year,Julday,SENSOR_ID,NCHAN,FREQ,TA,LATITUDE,LONGITUDE,LAND_INDEX,&
             NSURFACE_TYPE,TskPreclass)
    !-----------------------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       SURFACE_CLASSIFICATION_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land, snow)
    !
    ! from land index and satellite-based microwave measurements of antenna/brightness
    !
    ! temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   Year         : Year
    !   Julday       : Julian day
    !   SENSOR_ID    : sensor ID,
    !
    !   e.g.,
    !
    !       SENSOR_ID = See Consts.f90 Module for sensor IDs
    !.....
    !
    !   NCHAN        : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   FREQ(NCHAN)  : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   TA(1:NCHAN)  : ANTENNA TEMPEATURES AT ALL CHHANELS
    !
    !   LATITUDE     : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   LATITUDE     : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   LAND_INDEX   : LAND/OCEAN INDEX (0: ocean, non-0 : land)
    !
    !
    ! OUTPUT VARIABLES:
    !
    !   NSURFACE_TYPE   : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! CALLING SEQUENCE:
    !
    !
    ! CONTAINS:
    !
    !       N18_AMSUA_MHS_ALG      : Subroutine to distinguish four surface types from N18 AMSUA-MHS
    !
    !       F16_SSMIS_ALG          : Subroutine to distinguish four surface types from SSMIS
    !
    !       N15_AMSUAB_ALG         : Subroutine to distinguish four surface types from N15 AMSUAB
    !
    !       N16_AMSUAB_ALG         : Subroutine to distinguish four surface types from N16 AMSUAB
    !
    !       N17_AMSUAB_ALG         : Subroutine to distinguish four surface types from N17 AMSUAB
    !
    !       F15_SSMI_ALG           : Subroutine to distinguish four surface types from F15 SSMI
    !
    !       AMSRE_ALG              : Subroutine to distinguish four surface types from AMSRE
    !
    !       etc.
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (03-21-2006)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !   01/09/2007    :     Add descriptions of the selection of TA at window channels
    !
    !   11/23/2007    :     S.-A. Boukabara. Added first guess tskin as input, to help reduce false alarms, and
    !                       misclassifications
    !
    !  Copyright (C) 2006 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER, PARAMETER    :: NOCEAN = 0
    INTEGER               :: LAND_INDEX, SENSOR_ID,NSURFACE_TYPE,NCHAN
    INTEGER               :: Year, Julday
    REAL,    DIMENSION(:) :: TA ,FREQ
    REAL                  :: LATITUDE,LONGITUDE,TskPreclass

    !---INITIALIZATION
    IF (LAND_INDEX == NOCEAN) THEN
       NSURFACE_TYPE = OC_TYP
    ELSE
       NSURFACE_TYPE = LD_TYP
    ENDIF
    !---UPDATE SURFACE TYPE USING SATELLITE-MEASURED ANTENNA OR BRIGHTNESS TEMPERATURES AT WINDOW CHANNELS
    IF (SENSOR_ID == sensor_id_n18)     &
        CALL N18_AMSUA_MHS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_n19)     &
        CALL N18_AMSUA_MHS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_metopA)  &
        CALL N18_AMSUA_MHS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_metopB)  &
        CALL METOPB_AMSUA_MHS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_metopC)  &
        CALL METOPB_AMSUA_MHS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_metopSGA1)  &
        CALL metopSGA1_MWS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_f16)     &
        CALL F16_SSMIS_ALG3(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_f17)     &
        CALL F16_SSMIS_ALG3(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_f18)     &
        CALL F16_SSMIS_ALG3(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_f19)     &
        CALL F16_SSMIS_ALG3(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_amsre)   &
        CALL AQUA_AMSRE_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_gcomw1)   &
        CALL GCOMW1_AMSR2_ALG2(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_fy3ri)   CALL FY3RI_MWRI_ALG(NCHAN,FREQ,LATITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_npp)     CALL NPP_ATMS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,&
                                             NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_n20)     CALL N20_ATMS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,&
                                             NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_n21)     CALL N20_ATMS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,&
                                             NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_trmm)     &
        CALL TRMM_TMI_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_gpm)     &
!        CALL GPM_GMI_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
        CALL GPM_GMI_ALG2(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_mtma)     &
        CALL MTMA_MADRAS_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
    IF (SENSOR_ID == sensor_id_mtsa)     &
        CALL MTSA_SAPHIR_ALG(Year,Julday,NCHAN,FREQ,LATITUDE,LONGITUDE,LAND_INDEX,TA,NSURFACE_TYPE,TskPreclass)
  END SUBROUTINE SURFACE_CLASSIFICATION_ALG


!===============================================================
! Name:         N18_AMSUA_MHS_ALG
!
!
! Type:         Subroutine
!
!
! Description:
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - year
!       - julday
!       - nchan
!       - freq
!       - lat
!       - lon
!       - landindex
!       - ta
!       - surface_type
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

  SUBROUTINE N18_AMSUA_MHS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       N18_AMSUA_MHS_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land, snow)
    !
    ! from land index and N18 AMSUA & MHS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : N18 AMSUA & MHS antenna temperatures at five window channels
    !
    !   taw(1)       : antenna temperature at 23.8 GHz
    !   taw(2)       : antenna temperature at 31.4 GHz
    !   taw(3)       : antenna temperature at 50.3 GHz
    !   taw(4)       : antenna temperature at 89 GHz
    !   taw(5)       : antenna temperature at 157 GHz
    !
    !   freqw        : N18 AMSUA & MHS frequencies at five window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (03-20-2006)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2006 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER,  PARAMETER      :: NOCEAN = 0,ncoe = 7,nchanw = 5
    REAL,     PARAMETER      :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH2 = 40.0,LATTH3 = 50.0
    INTEGER                  :: nchan,wchan_index,landindex, surface_type,k,ich,jch,nd
    INTEGER                  :: Year,Julday
    REAL                     :: lat,lon,TA92_SICE,TA157_SICE,TA92_SNOW,TA157_SNOW,TA_ICE(2),TA_SNOW(2),SI,TskPreclass
    REAL,     DIMENSION(:)      :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(2,ncoe) :: coe_oc,coe_land

    !---Fitting coefficients to predict ta92 over open ocean
    data (coe_oc(1,k),k=1,ncoe)/6.76185e+002,  2.55301e+000,  2.44504e-001, -6.88612e+000,   &
         -5.01409e-003, -1.41372e-003,  1.59245e-002/
    !---Fitting coefficients to predict ta157 over open ocean
    data (coe_oc(2,k),k=1,ncoe)/ 5.14546e+002,  6.06543e+000, -6.09327e+000, -2.81614e+000,   &
         -1.35415e-002,  1.29683e-002 , 7.69443e-003/
    !---Fitting coefficients to predict ta92 over snow-free land
    data (coe_land(1,k),k=1,ncoe)/-3.09009e+002,  1.74746e+000, -2.01890e+000,  3.43417e+000, &
         -2.85680e-003,  3.53140e-003, -4.39255e-003/
    !---Fitting coefficients to predict ta157 over snow-free land
    data (coe_land(2,k),k=1,ncoe)/-1.01014e+001,  3.97994e+000, -4.11268e+000,  1.26658e+000, &
         -9.52526e-003,  8.75558e-003,  4.77981e-004/
    !---Central Frequencies ct five window channel
    data freqw/23.8, 31.4, 50.3, 89.0, 157.0/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       wchan_index = 1
       DO jch = 1, NCHAN
          IF(abs(freqw(ich)-freq(jch)) <= 0.5) THEN
             wchan_index = jch
             EXIT
          ENDIF
       ENDDO
       taw(ich) = ta(wchan_index)
       IF (ich == 1 .and. wchan_index /= 1)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 2 .and. wchan_index /= 2)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 3 .and. wchan_index /= 3)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 4 .and. wchan_index /= 15) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 5 .and. wchan_index /= 17) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_ICE(nd)= coe_oc(nd,1)
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SICE  = TA_ICE(1)
    TA157_SICE = TA_ICE(2)
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_SNOW(nd)= coe_land(nd,1)
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SNOW  = TA_SNOW(1)
    TA157_SNOW = TA_SNOW(2)
    !---COMPUTE SI = TA23 - TA92
    SI = taw(1)-taw(4)
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (TA92_SICE - taw(4) >= 10.0 .and. abs(lat) >= LATTH2)  surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH2 .and. SI >= SIHIGH)                surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. SI >= SILOW)                 surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. taw(1) >= 235.0)             surface_type = SEAICE_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                  surface_type = OC_TYP 
       if (TskPreclass <= 265. .and. TskPreclass >=0.)           surface_type = SEAICE_TYP 
       !---Get help from the latitude
!       if (abs(lat) <= 50. )                                     surface_type = OC_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
       surface_type = LD_TYP
       if(TA92_SNOW-taw(4)>=10.0.and.taw(1).le.260.0 .and. abs(lat) >= LATTH1) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH2 .and. SI >= SIHIGH .and. taw(1).le.260.0)         surface_type = SNOW_TYP
       if(abs(lat) >= LATTH3 .and. SI >= SILOW .and. taw(1).le.260.0)          surface_type = SNOW_TYP
       if(taw(1) <= 210.0 .and. taw(3) <= 225.0 .and. abs(lat) >= LATTH3)      surface_type = SNOW_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                                surface_type = LD_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.)                         surface_type = SNOW_TYP 
    endif
  END SUBROUTINE N18_AMSUA_MHS_ALG


!===============================================================
! Name:         METOPB_AMSUA_MHS_ALG
!
!
! Type:         Subroutine
!
!
! Description:
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - year
!       - julday
!       - nchan
!       - freq
!       - lat
!       - lon
!       - landindex
!       - ta
!       - surface_type
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

  SUBROUTINE METOPB_AMSUA_MHS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       N18_AMSUA_MHS_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land, snow)
    !
    ! from land index and METOPB AMSUA & MHS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : N18 AMSUA & MHS antenna temperatures at five window channels
    !
    !   taw(1)       : antenna temperature at 23.8 GHz
    !   taw(2)       : antenna temperature at 31.4 GHz
    !   taw(3)       : antenna temperature at 50.3 GHz
    !   taw(4)       : antenna temperature at 89 GHz
    !   taw(5)       : antenna temperature at 157 GHz
    !
    !   freqw        : N18 AMSUA & MHS frequencies at five window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (03-20-2006)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2006 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER,  PARAMETER      :: NOCEAN = 0,ncoe = 7,nchanw = 5
    REAL,     PARAMETER      :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH2 = 40.0,LATTH3 = 50.0
    INTEGER                  :: nchan,wchan_index,landindex, surface_type,k,ich,jch,nd
    INTEGER                  :: Year,Julday
    REAL                     :: lat,lon,TA92_SICE,TA157_SICE,TA92_SNOW,TA157_SNOW,TA_ICE(2),TA_SNOW(2),SI,TskPreclass
    REAL,     DIMENSION(:)      :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(2,ncoe) :: coe_oc,coe_land

    !---Fitting coefficients to predict ta92 over open ocean
    data (coe_oc(1,k),k=1,ncoe)/6.76185e+002,  2.55301e+000,  2.44504e-001, -6.88612e+000,   &
         -5.01409e-003, -1.41372e-003,  1.59245e-002/
    !---Fitting coefficients to predict ta157 over open ocean
    data (coe_oc(2,k),k=1,ncoe)/ 5.14546e+002,  6.06543e+000, -6.09327e+000, -2.81614e+000,   &
         -1.35415e-002,  1.29683e-002 , 7.69443e-003/
    !---Fitting coefficients to predict ta92 over snow-free land
    data (coe_land(1,k),k=1,ncoe)/-3.09009e+002,  1.74746e+000, -2.01890e+000,  3.43417e+000, &
         -2.85680e-003,  3.53140e-003, -4.39255e-003/
    !---Fitting coefficients to predict ta157 over snow-free land
    data (coe_land(2,k),k=1,ncoe)/-1.01014e+001,  3.97994e+000, -4.11268e+000,  1.26658e+000, &
         -9.52526e-003,  8.75558e-003,  4.77981e-004/
    !---Central Frequencies ct five window channel
    data freqw/23.8, 31.4, 50.3, 89.0, 157.0/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       wchan_index = 1
       DO jch = 1, NCHAN
          if(jch .eq. 15)CYCLE
          IF(abs(freqw(ich)-freq(jch)) <= 0.5) THEN
             wchan_index = jch
             EXIT
          ENDIF
       ENDDO
       taw(ich) = ta(wchan_index)
       IF (ich == 1 .and. wchan_index /= 1)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 2 .and. wchan_index /= 2)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 3 .and. wchan_index /= 3)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
!       IF (ich == 4 .and. wchan_index /= 15) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 4 .and. wchan_index /= 16) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 5 .and. wchan_index /= 17) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_ICE(nd)= coe_oc(nd,1)
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SICE  = TA_ICE(1)
    TA157_SICE = TA_ICE(2)
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_SNOW(nd)= coe_land(nd,1)
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SNOW  = TA_SNOW(1)
    TA157_SNOW = TA_SNOW(2)
    !---COMPUTE SI = TA23 - TA92
    SI = taw(1)-taw(4)
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (TA92_SICE - taw(4) >= 10.0 .and. abs(lat) >= LATTH2)  surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH2 .and. SI >= SIHIGH)                surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. SI >= SILOW)                 surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. taw(1) >= 235.0)             surface_type = SEAICE_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                  surface_type = OC_TYP 
       if (TskPreclass <= 265. .and. TskPreclass >=0.)           surface_type = SEAICE_TYP 
       !---Get help from the latitude
!       if (abs(lat) <= 50. )                                     surface_type = OC_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
       surface_type = LD_TYP
       if(TA92_SNOW-taw(4)>=10.0.and.taw(1).le.260.0 .and. abs(lat) >= LATTH1) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH2 .and. SI >= SIHIGH .and. taw(1).le.260.0)         surface_type = SNOW_TYP
       if(abs(lat) >= LATTH3 .and. SI >= SILOW .and. taw(1).le.260.0)          surface_type = SNOW_TYP
       if(taw(1) <= 210.0 .and. taw(3) <= 225.0 .and. abs(lat) >= LATTH3)      surface_type = SNOW_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                                surface_type = LD_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.)                         surface_type = SNOW_TYP 
    endif
  END SUBROUTINE METOPB_AMSUA_MHS_ALG


 SUBROUTINE F16_SSMIS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       F16_SSMIS_ALG
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !
    ! from land index and N18 AMSUA & MHS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : F16 SSMIS antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 19.35 VPOL GHz
    !   taw(2)       : antenna temperature at 19.35 HPOL GHz
    !   taw(3)       : antenna temperature at 22.235 VPOL GHz
    !   taw(4)       : antenna temperature at 37 VPOL GHz
    !   taw(5)       : antenna temperature at 37 HPOL GHz
    !   taw(6)       : antenna temperature at 91.65 VPOL GHz
    !   taw(7)       : antenna temperature at 91.65 HPOL GHz
    !
    !   freqw        : F16 SSMIS frequencies at seven window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 7
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/13,12,14,16,15,17,18/
    !---Central Frequencies ct five window channel
    data freqw/19.35, 19.35, 22.235, 37.0, 37.0, 91.655, 91.655/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,22V,37V,37H,92V,92H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in F16 SSMIS surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(1)-taw(2)   !TV19-TH19
    
    PR19 = (taw(1)-taw(2)) / (taw(1)+taw(2))
    PR37 = (taw(4)-taw(5)) / (taw(4)+taw(5))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(1) - taw(4)
    SCH37 = taw(2) - taw(5)
    SCV92 = taw(1) - taw(6)
    SCVX92 = taw(3) - taw(6)
    SCH92 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(2) - taw(5)) / (taw(2) + taw(5))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coe_ts(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE
       if (abs(lat) >= 60.0) then
          if( TH92_OBS - TH92_ALG >= SILOW) surface_type = SNOW_TYP
          if( SCH37 >= SILOW) surface_type = SNOW_TYP
       else
          !--- Additional snow screens
           if ( (abs(lat) >= LATTH1) .and. (SCH37 >= SIHIGH)  .and. TS_ALG < 276.0) &
                surface_type = SNOW_TYP
           !--- Use 2 levels of detection with TS_ALG threshold adjusted depending on TB22v92v gradient
           if ( (abs(lat) >= LATTH1) .and. (SCVX92 >= 20.) .and. TS_ALG < 278.0) &
                surface_type = SNOW_TYP
           if ( (abs(lat) >= LATTH1) .and. (SCVX92 >= 5.) .and. TS_ALG < 276.0) &
                surface_type = SNOW_TYP
       endif

!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
  END SUBROUTINE F16_SSMIS_ALG

 SUBROUTINE F16_SSMIS_ALG2(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       F16_SSMIS_ALG
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !
    ! from land index and N18 AMSUA & MHS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : F16 SSMIS antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 19.35 VPOL GHz
    !   taw(2)       : antenna temperature at 19.35 HPOL GHz
    !   taw(3)       : antenna temperature at 22.235 VPOL GHz
    !   taw(4)       : antenna temperature at 37 VPOL GHz
    !   taw(5)       : antenna temperature at 37 HPOL GHz
    !   taw(6)       : antenna temperature at 91.65 VPOL GHz
    !   taw(7)       : antenna temperature at 91.65 HPOL GHz
    !
    !   freqw        : F16 SSMIS frequencies at seven window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 7
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/13,12,14,16,15,17,18/
    !---Central Frequencies ct five window channel
    data freqw/19.35, 19.35, 22.235, 37.0, 37.0, 91.655, 91.655/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,22V,37V,37H,92V,92H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in F16 SSMIS surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(1)-taw(2)   !TV19-TH19
    
    PR19 = (taw(1)-taw(2)) / (taw(1)+taw(2))
    PR37 = (taw(4)-taw(5)) / (taw(4)+taw(5))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(1) - taw(4)
    SCH37 = taw(2) - taw(5)
    SCV92 = taw(1) - taw(6)
    SCVX92 = taw(3) - taw(6)
    SCH92 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(2) - taw(5)) / (taw(2) + taw(5))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coe_ts(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE: test modified version official AMSR-E algorithm

       if(TskPreclass .gt. 0.)then
          if(taw(5) .lt. 245. .and. taw(4) .lt. 255.)then
             if(SCV37 .gt. 0. .or. SCH37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(7) .le. 255. .and. &
                     taw(6) .le. 265. .and. &
                     SCVX92 .gt. 0. .and. &
                     TskPreclass .lt. 267.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       else
          if(taw(5) .lt. 245. .and. taw(4) .lt. 255.)then
             if(SCV37 .gt. 0. .or. SCH37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(7) .le. 255. .and. &
                     taw(6) .le. 265. .and. &
                     SCVX92 .gt. 0.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       endif

!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
 END SUBROUTINE F16_SSMIS_ALG2

 SUBROUTINE F16_SSMIS_ALG3(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       F16_SSMIS_ALG
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !
    ! from land index and N18 AMSUA & MHS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : F16 SSMIS antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 19.35 VPOL GHz
    !   taw(2)       : antenna temperature at 19.35 HPOL GHz
    !   taw(3)       : antenna temperature at 22.235 VPOL GHz
    !   taw(4)       : antenna temperature at 37 VPOL GHz
    !   taw(5)       : antenna temperature at 37 HPOL GHz
    !   taw(6)       : antenna temperature at 91.65 VPOL GHz
    !   taw(7)       : antenna temperature at 91.65 HPOL GHz
    !
    !   freqw        : F16 SSMIS frequencies at seven window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 7
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/13,12,14,16,15,17,18/
    !---Central Frequencies ct five window channel
    data freqw/19.35, 19.35, 22.235, 37.0, 37.0, 91.655, 91.655/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,22V,37V,37H,92V,92H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in F16 SSMIS surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(1)-taw(2)   !TV19-TH19
    
    PR19 = (taw(1)-taw(2)) / (taw(1)+taw(2))
    PR37 = (taw(4)-taw(5)) / (taw(4)+taw(5))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(1) - taw(4)
    SCH37 = taw(2) - taw(5)
    SCV92 = taw(1) - taw(6)
    SCVX92 = taw(3) - taw(6)
    SCH92 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(2) - taw(5)) / (taw(2) + taw(5))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coe_ts(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE: test modified version official AMSR-E algorithm

       if(TskPreclass .gt. 0.)then
          if(taw(5) .lt. 245. .and. taw(4) .lt. 255.)then
             if(SCV37 .gt. 0. .or. SCH37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(7) .le. 255. .and. &
                     taw(6) .le. 265. .and. &
                     SCVX92 .gt. 0. .and. &
                     TskPreclass .lt. 267.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       else
          if(taw(5) .lt. 245. .and. taw(4) .lt. 255.)then
             if(SCV37 .gt. 0. .or. SCH37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(7) .le. 255. .and. &
                     taw(6) .le. 265. .and. &
                     SCVX92 .gt. 0.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       endif

!      Add original algorithm to detect snow
       if (abs(lat) >= 60.0) then
          if( TH92_OBS - TH92_ALG >= SILOW) surface_type = SNOW_TYP
          if( SCH37 >= SILOW) surface_type = SNOW_TYP
       else
          !--- Additional snow screens
           if ( (abs(lat) >= LATTH1) .and. (SCH37 >= SIHIGH)  .and. TS_ALG < 276.0) &
                surface_type = SNOW_TYP
           !--- Use 2 levels of detection with TS_ALG threshold adjusted depending on TB22v92v gradient
           if ( (abs(lat) >= LATTH1) .and. (SCVX92 >= 20.) .and. TS_ALG < 278.0) &
                surface_type = SNOW_TYP
           if ( (abs(lat) >= LATTH1) .and. (SCVX92 >= 5.) .and. TS_ALG < 276.0) &
                surface_type = SNOW_TYP
       endif

!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
 END SUBROUTINE F16_SSMIS_ALG3

SUBROUTINE AQUA_AMSRE_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       AQUA_AMSRE_ALG
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !       (Adapted from F16_SSMIS_ALG)    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : AQUA AMSRE antenna temperatures at 12 window channels
    !
    !   taw(1)       : antenna temperature at 6.925 VPOL GHz
    !   taw(2)       : antenna temperature at 6.925 HPOL GHz
    !   taw(3)       : antenna temperature at 10.65 VPOL GHz
    !   taw(4)       : antenna temperature at 10.65 HPOL GHz
    !   taw(5)       : antenna temperature at 18.70 VPOL GHz
    !   taw(6)       : antenna temperature at 18.70 HPOL GHz
    !   taw(7)       : antenna temperature at 23.80 VPOL GHz
    !   taw(8)       : antenna temperature at 23.80 HPOL GHz
    !   taw(9)       : antenna temperature at 36.50 VPOL GHz
    !   taw(10)      : antenna temperature at 36.50 VPOL GHz
    !   taw(11)      : antenna temperature at 89.00 VPOL GHz
    !   taw(12)      : antenna temperature at 89.00 HPOL GHz
    !
    !   freqw        : AQUA AMSRE frequencies at 12 window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 7
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/5,6,7,9,10,11,12/
    !---Central Frequencies ct five window channel
    data freqw/18.7, 18.7, 23.8, 36.5, 36.5, 89.0, 89.0/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,22V,37V,37H,92V,92H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in AQUA AMSRE surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF

    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(1)-taw(2)   !TV19-TH19
    
    PR19 = (taw(1)-taw(2)) / (taw(1)+taw(2))
    PR37 = (taw(4)-taw(5)) / (taw(4)+taw(5))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(1) - taw(4)
    SCH37 = taw(2) - taw(5)
    SCV92 = taw(1) - taw(6)
    SCVX92 = taw(3) - taw(6)
    SCH92 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(2) - taw(5)) / (taw(2) + taw(5))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP

       ! From legacy placeholder code
       IF (ta(1) > 190.) surface_type = SEAICE_TYP

       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coe_ts(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE
       if (abs(lat) >= 60.0) then
          if( TH92_OBS - TH92_ALG >= SILOW) surface_type = SNOW_TYP
          if( SCH37 >= SILOW) surface_type = SNOW_TYP
       else
          !--- Additional snow screens
           if ( (abs(lat) >= LATTH1) .and. (SCH37 >= SIHIGH)  .and. TS_ALG < 276.0) &
                surface_type = SNOW_TYP
           !--- Use 2 levels of detection with TS_ALG threshold adjusted depending on TB22v92v gradient
           if ( (abs(lat) >= LATTH1) .and. (SCVX92 >= 20.) .and. TS_ALG < 278.0) &
                surface_type = SNOW_TYP
           if ( (abs(lat) >= LATTH1) .and. (SCVX92 >= 5.) .and. TS_ALG < 276.0) &
                surface_type = SNOW_TYP
       endif

!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
 END SUBROUTINE AQUA_AMSRE_ALG

SUBROUTINE GCOMW1_AMSR2_ALG2(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       GCOMW1_AMSR2_ALG
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !       (Adapted from F16_SSMIS_ALG)    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : AQUA AMSRE antenna temperatures at 9 window channels
    !
    !   taw(1)       : antenna temperature at 10.65 VPOL GHz
    !   taw(2)       : antenna temperature at 10.65 HPOL GHz
    !   taw(3)       : antenna temperature at 18.70 VPOL GHz
    !   taw(4)       : antenna temperature at 18.70 HPOL GHz
    !   taw(5)       : antenna temperature at 23.80 VPOL GHz
    !   taw(6)       : antenna temperature at 36.50 VPOL GHz
    !   taw(7)       : antenna temperature at 36.50 VPOL GHz
    !   taw(8)       : antenna temperature at 89.00 VPOL GHz
    !   taw(9)       : antenna temperature at 89.00 HPOL GHz
    !
    !   freqw        : GCOMW1 AMSR2 frequencies at 14 window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !M-
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 9
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92,SCVY92,&
                                   SCVX37,SCHX37
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/5,6,7,8,9,11,12,13,14/
    !---Central Frequencies ct five window channel
    data freqw/10.65,10.65,18.7, 18.7, 23.8, 36.5, 36.5, 89.0, 89.0/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,22V,37V,37H,92V,92H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in GCOMW1 AMSR2 surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF

    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(3)-taw(4)   !TV19-TH19
    
    PR19 = (taw(3)-taw(4)) / (taw(3)+taw(4))
    PR37 = (taw(6)-taw(7)) / (taw(6)+taw(7))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(3) - taw(6)
    SCH37 = taw(4) - taw(7)
    SCV92 = taw(3) - taw(8)
    SCVX92 = taw(5) - taw(8)
    SCH92 = taw(4) - taw(9)
    SCVX37 = taw(1) - taw(6)
    SCHX37 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(4) - taw(7)) / (taw(4) + taw(7))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP

       ! From legacy placeholder code
       IF (ta(1+2) > 190.) surface_type = SEAICE_TYP

       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich+2)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5+2)*taw(ich-5+2)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coe_ts(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich+2)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5+2)*taw(ich-5+2)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE: Modified version of AMSR-E official algorithm
       if(TskPreclass .gt. 0.)then
          if(taw(7) .lt. 245. .and. taw(6) .lt. 255.)then
             if(SCVX37 .gt. 0. .or. SCHX37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(9) .le. 255. .and. &
                     taw(8) .le. 265. .and. &
                     SCVX92 .gt. 0. .and. &
                     TskPreclass .lt. 267.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       else
          if(taw(7) .lt. 245. .and. taw(6) .lt. 255.)then
             if(SCVX37 .gt. 0. .or. SCHX37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(8) .le. 255. .and. &
                     taw(9) .le. 265. .and. &
                     SCVX92 .gt. 0.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       endif
!      Glacial snow
     if(abs(lat) >= 60.0 .and. taw(4) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
  endif    ! non ocean
END SUBROUTINE GCOMW1_AMSR2_ALG2


  SUBROUTINE AQUA_AMSRE_ALG_OLD(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       AQUA_AMSRE_ALG
    !
    ! PURPOSE:
    !       placeholder for AMSRE preclassifier. For now simply returns the topography.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! Sid Boukabara, NOAA/NESDIS
    !
    !
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0
    INTEGER                     :: surface_type
    INTEGER, INTENT(IN)         :: nchan,landindex
    REAL                        :: lat,lon,TskPreclass
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    INTEGER, INTENT(IN)         :: Year,Julday
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF

    IF (surface_type == OC_TYP) THEN
       IF (ta(1) > 190.) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
       
    ENDIF
    
  END SUBROUTINE AQUA_AMSRE_ALG_OLD

  SUBROUTINE TRMM_TMI_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       TRMM_TMI_ALG (Adapted from F16_SSMIS_ALG)
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : TRMM TMI antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 11 VPOL GHz
    !   taw(2)       : antenna temperature at 11 HPOL GHz
    !   taw(3)       : antenna temperature at 19.35 VPOL GHz
    !   taw(4)       : antenna temperature at 19.35 HPOL GHz
    !   taw(5)       : antenna temperature at 22.235 VPOL GHz
    !   taw(6)       : antenna temperature at 37 VPOL GHz
    !   taw(7)       : antenna temperature at 37 HPOL GHz
    !   taw(8)       : antenna temperature at 91.65 VPOL GHz
    !   taw(9)       : antenna temperature at 91.65 HPOL GHz
    !
    !   freqw        : TRMM TMI frequencies at nine window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 7
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/3,4,5,6,7,8,9/
    !---Central Frequencies ct five window channel
    data freqw/19.35, 19.35, 21.3, 37.0, 37.0, 85.5, 85.5/


!    print *,'Testing: TRMM_TMI_ALG'
    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,22V,37V,37H,92V,92H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOW CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in TRMM TMI surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(1)-taw(2)   !TV19-TH19
    
    PR19 = (taw(1)-taw(2)) / (taw(1)+taw(2))
    PR37 = (taw(4)-taw(5)) / (taw(4)+taw(5))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(1) - taw(4)
    SCH37 = taw(2) - taw(5)
    SCV92 = taw(1) - taw(6)
    SCVX92 = taw(3) - taw(6)
    SCH92 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(2) - taw(5)) / (taw(2) + taw(5))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coeh_land(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE
       if (abs(lat) >= 60.0) then
          if( TH92_OBS - TH92_ALG >= SILOW) surface_type = SNOW_TYP
          if( SCH37 >= SILOW) surface_type = SNOW_TYP
       else
           if ( (abs(lat) >= LATTH1) .and. (TH92_OBS - TH92_ALG >= SIHIGH)) &
                surface_type = SNOW_TYP
           if ( (abs(lat) >= LATTH1) .and. (SCH37 >= SIHIGH)) &
                surface_type = SNOW_TYP
       endif
       if(  (SCVX92 <= 6.0 .or. SCV37 <=6) .and. DV19 >= 8.0) surface_type = LD_TYP
       if (TS_ALG >= 273.0 .or. taw(3) >=  258.0) surface_type = LD_TYP
       
!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
 END SUBROUTINE TRMM_TMI_ALG

 SUBROUTINE GPM_GMI_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       GPM_GMI_ALG (Adapted from F16_SSMIS_ALG)
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : GPM GMI antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 10.65 VPOL GHz
    !   taw(2)       : antenna temperature at 10.65 HPOL GHz
    !   taw(3)       : antenna temperature at 18.7 VPOL GHz
    !   taw(4)       : antenna temperature at 18.7 HPOL GHz
    !   taw(5)       : antenna temperature at 23.8 VPOL GHz
    !   taw(6)       : antenna temperature at 36.5 VPOL GHz
    !   taw(7)       : antenna temperature at 36.5 HPOL GHz
    !   taw(8)       : antenna temperature at 89 VPOL GHz
    !   taw(9)       : antenna temperature at 89 HPOL GHz
    !
    !   freqw        : GPM GMI frequencies at nine window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 7
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/3,4,5,6,7,8,9/
    !---Central Frequencies ct five window channel
    data freqw/18.7, 18.7, 23.8, 36.5, 36.5, 89.0, 89.0/


!    print *,'Testing: GPM_GMI_ALG'
    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,24V,37V,37H,89V,89H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOW CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in GPM GMI surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(1)-taw(2)   !TV19-TH19
    
    PR19 = (taw(1)-taw(2)) / (taw(1)+taw(2))
    PR37 = (taw(4)-taw(5)) / (taw(4)+taw(5))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(1) - taw(4)
    SCH37 = taw(2) - taw(5)
    SCV92 = taw(1) - taw(6)
    SCVX92 = taw(3) - taw(6)
    SCH92 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(2) - taw(5)) / (taw(2) + taw(5))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coeh_land(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5)*taw(ich-5)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE
       if (abs(lat) >= 60.0) then
          if( TH92_OBS - TH92_ALG >= SILOW) surface_type = SNOW_TYP
          if( SCH37 >= SILOW) surface_type = SNOW_TYP
       else
           if ( (abs(lat) >= LATTH1) .and. (TH92_OBS - TH92_ALG >= SIHIGH)) &
                surface_type = SNOW_TYP
           if ( (abs(lat) >= LATTH1) .and. (SCH37 >= SIHIGH)) &
                surface_type = SNOW_TYP
       endif
       if(  (SCVX92 <= 6.0 .or. SCV37 <=6) .and. DV19 >= 8.0) surface_type = LD_TYP
       if (TS_ALG >= 273.0 .or. taw(3) >=  258.0) surface_type = LD_TYP
       
!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
 END SUBROUTINE GPM_GMI_ALG

 SUBROUTINE GPM_GMI_ALG2(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       GPM_GMI_ALG (Adapted from F16_SSMIS_ALG)
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : GPM GMI antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 10.65 VPOL GHz
    !   taw(2)       : antenna temperature at 10.65 HPOL GHz
    !   taw(3)       : antenna temperature at 18.7 VPOL GHz
    !   taw(4)       : antenna temperature at 18.7 HPOL GHz
    !   taw(5)       : antenna temperature at 23.8 VPOL GHz
    !   taw(6)       : antenna temperature at 36.5 VPOL GHz
    !   taw(7)       : antenna temperature at 36.5 HPOL GHz
    !   taw(8)       : antenna temperature at 89 VPOL GHz
    !   taw(9)       : antenna temperature at 89 HPOL GHz
    !
    !   freqw        : GPM GMI frequencies at nine window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Banghua Yan, QSS Group Inc., Banghua.Yan@noaa.gov (05-29-2007)
    !
    !
    !       and             Fuzhong Weng, NOAA/NESDIS/ORA, Fuzhong.Weng@noaa.gov
    !
    !
    !  Copyright (C) 2007 Fuzhong Weng and Banghua Yan
    !
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0, ncoe = 11,nchanw = 9
    REAL,     PARAMETER         :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH3 = 50.0
    INTEGER                     :: surface_type,ich
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    INTEGER,DIMENSION(nchanw)   :: wchan_index
    REAL                        :: SCH92,SCH37,SCV92,SCV37,lat,lon,TS_ALG,TH92_ALG,TH92_OBS,&
                                   DV19,TskPreclass,PR19,PR37,GRH37,SCVX92,SCVY92,&
                                   SCVX37,SCHX37
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(ncoe)   :: coeh_land,coe_ts
    ! Fitting coefficients to predict ta92 over snow
    data coeh_land/9.60083e+001,-6.05660e-002, 3.51944e-001,-6.52072e-001,-3.79843e-001,   &
                   9.74554e-001,-9.93822e-004, 1.82148e-003,-1.19589e-003,-4.43338e-004,   &
                   2.32471e-003/
    ! Fitting coefficients to predict ts over snow and non-desert land surfaces
    data coe_ts/2.63055e+002,  2.24818e+000, -1.75953e+000, -2.46555e+000, -9.57587e-001,  &
                2.22300e+000, -6.05612e-003,  3.70943e-003,  8.40393e-003,  1.88572e-003,  &
               -5.06110e-003/
    ! Seven window channel indices used in the algorithm
    data wchan_index/1,2,3,4,5,6,7,8,9/
    !---Central Frequencies ct five window channel
    data freqw/10.65, 10.65, 18.7, 18.7, 23.8, 36.5, 36.5, 89.0, 89.0/


!    print *,'Testing: GPM_GMI_ALG'
    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw  !19V,19H,24V,37V,37H,89V,89H
       taw(ich) = ta(wchan_index(ich))
    ENDDO

    ! SELECT TA AT WINDOW CHANNELS
    DO ich = 1, nchanw
       IF(abs(freqw(ich)-freq(wchan_index(ich))) >= epsilonLoose) THEN
          print *, 'Frequencies do not match in GPM GMI surface pre-classifier:',&
               ich,wchan_index(ich),freqw(ich),freq(wchan_index(ich))
       ENDIF
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    ! Calculate polarization difference and indices at 19 and 37 GHz
    DV19 = taw(3)-taw(4)   !TV19-TH19
    
    PR19 = (taw(3)-taw(4)) / (taw(3)+taw(4))
    PR37 = (taw(6)-taw(7)) / (taw(6)+taw(7))

     ! Calculate scattering differences at 37 and 92 GHz with respect to 19 GHz
    SCV37 = taw(3) - taw(6)
    SCH37 = taw(4) - taw(7)
    SCV92 = taw(3) - taw(8)
    SCVX92 = taw(5) - taw(8)
    SCH92 = taw(4) - taw(9)
    SCVX37 = taw(1) - taw(6)
    SCHX37 = taw(2) - taw(7)


     ! Calculate scattering index at 37 GHz horizontal polarization
    GRH37 = (taw(4) - taw(7)) / (taw(4) + taw(7))
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (PR19 <= 0.13 .and. GRH37 >= -0.07  .and. abs(lat) >= LATTH3) surface_type = SEAICE_TYP
       ! From legacy placeholder code
       IF (ta(1+2) > 190.) surface_type = SEAICE_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
!       if (DV19 >= 20.0 .and. taw(2) >= 210.0) then
!           surface_type = Desert_TYP
!       endif
       !---Predict SNOW TH92 from TA19 ~ TA37 using snow-free land fitting coeffs.
       TH92_ALG = coeh_land(1)
       DO ich=1,5
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich+2)
       ENDDO
       DO ich=6,10
          TH92_ALG = TH92_ALG + coeh_land(ich+1)*taw(ich-5+2)*taw(ich-5+2)
       ENDDO
       TH92_OBS = taw(7)
       ! PREDICT TS
       TS_ALG = coeh_land(1)
       DO ich=1,5
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich+2)
       ENDDO
       DO ich=6,10
           TS_ALG = TS_ALG + coe_ts(ich+1)*taw(ich-5+2)*taw(ich-5+2)
       ENDDO
       ! DEFAULT TYPE
       surface_type = LD_TYP
       !UPDATE LAND TYPE: Modified version of AMSR-E official algorithm
!       print *,'PreClassif: GPM applying AMSRE algorithm: lat,lon,TskPreclass=',lat,lon,TskPreclass
!       print *,'taw(1:9)=',taw(1:9)
!       print *,'SCVX37,SCHX37,SCVX92=',SCVX37,SCHX37,SCVX92
!       write(99,*)'PreClassif: GPM applying AMSRE algorithm: lat,lon,TskPreclass=',lat,lon,TskPreclass
!       write(99,*)'taw(1:9)=',taw(1:9)
!       write(99,*)'SCVX37,SCHX37,SCVX92=',SCVX37,SCHX37,SCVX92
       if(TskPreclass .gt. 0.)then
          if(taw(7) .lt. 245. .and. taw(6) .lt. 255.)then
             if(SCVX37 .gt. 0. .or. SCHX37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(9) .le. 255. .and. &
                     taw(8) .le. 265. .and. &
                     SCVX92 .gt. 0. .and. &
                     TskPreclass .lt. 267.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       else
          if(taw(7) .lt. 245. .and. taw(6) .lt. 255.)then
!          if(taw(7) .lt. 245. .or. taw(6) .lt. 255.)then
             if(SCVX37 .gt. 0. .or. SCHX37 .gt. 0.)then
                surface_type = SNOW_TYP
             else
                if(taw(9) .le. 255. .and. &
                     taw(8) .le. 265. .and. &
                     SCVX92 .gt. 0.)then
!                if(taw(9) .le. 255. .or. &
!                     taw(8) .le. 265. .or. &
!                     SCVX92 .gt. 0.)then
                   surface_type = SNOW_TYP
                endif
             endif
          endif
       endif
!       print *,'surface_type=',surface_type
!       write(99,*)'surface_type=',surface_type
       
!      Glacial snow
       if(abs(lat) >= 60.0 .and. taw(2) .le. 215.0 .and. DV19 >= 23.0) surface_type = SNOW_TYP
 
!     endif  ! non desert
   endif    ! non ocean
 END SUBROUTINE GPM_GMI_ALG2


  SUBROUTINE MTMA_MADRAS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       MTMA_MADRAS_ALG (Simplified preclassifier)
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    ! NB: Simplified preclassifier only returns ocean, land surface types
    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : Megha-Tropiques MADRASantenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 18.7 VPOL GHz
    !   taw(2)       : antenna temperature at 18.7 HPOL GHz
    !   taw(3)       : antenna temperature at 23.8 VPOL GHz
    !   taw(4)       : antenna temperature at 37.5 VPOL GHz
    !   taw(5)       : antenna temperature at 37.5 HPOL GHz
    !   taw(6)       : antenna temperature at 89   VPOL GHz
    !   taw(7)       : antenna temperature at 89   HPOL GHz
    !   taw(8)       : antenna temperature at 157  VPOL GHz
    !   taw(9)       : antenna temperature at 157  HPOL GHz
    !   freqw        : MT MADRAS frequencies at nine window channels
    !
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    REAL,    INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,    INTENT(IN)         :: lat,lon
    REAL,    INTENT(IN)         :: TskPreclass
    INTEGER, INTENT(OUT)         :: surface_type

!    print *,'Testing: MTMA_MADRAS_ALG'

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
  END SUBROUTINE MTMA_MADRAS_ALG


  SUBROUTINE MTSA_SAPHIR_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       MTSA_SAPHIR_ALG (Simplified preclassifier)
    !
    ! PURPOSE:
    !       Distinguish five surface types (open ocean, sea ice, desert, snow-free and non-desert land, snow)
    ! NB: Simplified preclassifier only returns ocean, land surface types
    !
    ! 
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : Megha-Tropiques SAPHIR antenna temperatures at seven window channels
    !
    !   taw(1)       : antenna temperature at 183 +/- 0.2 Mixed HPOL GHz
    !   taw(2)       : antenna temperature at 183 +/- 1.1 Mixed HPOL GHz
    !   taw(3)       : antenna temperature at 183 +/- 2.8 Mixed HPOL GHz
    !   taw(4)       : antenna temperature at 183 +/- 4.2 Mixed HPOL GHz
    !   taw(5)       : antenna temperature at 183 +/- 6.8 Mixed HPOL GHz
    !   taw(6)       : antenna temperature at 183 +/- 11.0 Mixed HPOL GHz
    !   freqw        : MT SAPHIR frequencies at nine window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0
    INTEGER, INTENT(IN)         :: nchan,landindex
    INTEGER, INTENT(IN)         :: Year,Julday
    REAL,    INTENT(IN), DIMENSION(nchan)  :: ta,freq
    REAL,    INTENT(IN)         :: lat,lon
    REAL,    INTENT(IN)         :: TskPreclass
    INTEGER, INTENT(OUT)         :: surface_type

!    print *,'Testing: MTSA_SAPHIR_ALG'

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
  END SUBROUTINE MTSA_SAPHIR_ALG


!===============================================================
! Name:         applySeaIceClimo
!
!
! Type:         Subroutine
!
!
! Description:  Applies sea ice climatology to constrain equatorward extent 
!               of scenes classified as sea ice

! Source: National Snow and Ice Data Center
!         Ice persistence climatology based on combined SMMR and SSMI observations from 1979-2000
! http://nsidc.org/data/nsidc-0192.html
! ftp://sidads.colorado.edu/pub/DATASETS/seaice/polar-stereo/trends-climatologies/ice-persistence
!
!
! Arguments:
!
!      Name                 Type            Description
!      ---------------------------------------------------
!       - Year               I             Year
!       - Julday             I             Julian day
!       - xlat               I             Latitude
!       - xlon               I             Longitude
!       - landindex          I             Ocean/Land flag
!       - surface_type       I/O           Classified surface type (possibly modified from input value)
!
!
! Modules needed:
!       - misc
!
!
! History:
!       02-16-2009      C. Grassotti, IMSG Inc @ NOAA/NESDIS/STAR
!
!===============================================================

  SUBROUTINE applySeaIceClimo(Year, Julday, xlat, xlon, landindex, surface_type)
    INTEGER,INTENT(IN)    :: landindex
    INTEGER,INTENT(IN)    :: Year,Julday
    REAL,INTENT(IN)       :: xlat,xlon
    INTEGER,INTENT(INOUT) :: surface_type
    integer :: surface_type_orig

    INTEGER, PARAMETER    :: NOCEAN = 0
    INTEGER, PARAMETER :: nLonBins=36,nTimeBins=12,nLonBins1=360,nTimeBins1=53
    INTEGER :: iLonBin,month,day,jday,iweek,iLonBin1,i,j
    REAL :: WidthLonBin=10. ! SH width of longitude bins for storing latitude thresholds (degrees)
    REAL :: WidthLonBin1=1. ! NH width of longitude bins for storing latitude thresholds (degrees) 

    !---Data for latitude thresholds (varying with longitude and season)    
    !---Northern Hem. values
    REAL,    DIMENSION(nlonBins1,nTimeBins1) :: threshLatNH
    REAL,    DIMENSION(nlonBins,nTimeBins)   :: threshLatSH
    REAL,    DIMENSION(nlonBins1,20)         :: threshLatNH_01
    REAL,    DIMENSION(nlonBins1,20)         :: threshLatNH_02
    REAL,    DIMENSION(nlonBins1,13)         :: threshLatNH_03

    !---Data for latitude thresholds (varying with longitude and season)    
    !---Note: data for Feb and Aug were derived directly from data (tie points);
    !---remaining months obtained by linear interpolation

    !---Northern Hem. values
    data threshLatNH_01 / &

      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,60.,59.,53.,53.,57.,59.,59.,53.,53.,53.,53.,53.,53.,53.,51.,52.,52.,51.,54., &
      50.,49.,49.,46.,45.,58.,56.,46.,46.,46.,47.,39.,39.,39.,37.,49.,44.,44.,45.,45.,44.,44.,45.,45.,43.,47.,57.,58.,47.,43., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,37.,38.,38.,44.,40.,46.,43.,43.,34.,33.,30.,31.,29.,30.,28.,30.,30.,31.,28., &
      31.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,40.,42.,40.,40.,38.,37.,40., &
      40.,39.,40.,45.,39.,38.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,49.,47.,47.,48.,49.,47.,46.,43.,47.,56.,57.,58.,58., &
      58.,58.,58.,58.,58.,54.,53.,57.,53.,54.,56.,55.,56.,57.,59.,59.,59.,59.,60.,60.,60.,60.,60.,61.,61.,61.,61.,61.,61.,61., &
      61.,61.,61.,61.,61.,61.,60.,60.,60.,59.,59.,59.,58.,58.,58.,58.,57.,58.,58.,58.,58.,58.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,50.,47.,42.,42.,40.,43.,41.,43.,47.,41.,40.,40.,40.,41.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,46.,44.,41.,41.,45.,45.,44.,42.,44.,44.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,52.,52., &
      52.,51.,48.,48.,48.,48.,66.,65.,64.,63.,62.,61.,61.,61.,60.,60.,60.,60.,61.,62.,64.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 1 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,59.,53.,53.,52.,52.,51.,51.,52.,46.,46.,53.,53.,53.,53.,51.,51.,51.,52.,49.,51.,54., &
      50.,47.,49.,46.,45.,46.,56.,46.,45.,46.,47.,39.,39.,39.,37.,49.,44.,44.,45.,45.,44.,44.,45.,45.,43.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,37.,38.,38.,44.,40.,46.,43.,43.,34.,33.,30.,31.,29.,30.,28.,30.,30.,31.,28., &
      30.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,40.,42.,40.,40.,38.,37.,37., &
      40.,39.,40.,39.,39.,38.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,46.,46.,47.,48.,48.,47.,45.,43.,47.,50.,56.,56.,58., &
      58.,58.,58.,58.,58.,53.,53.,57.,53.,54.,56.,55.,56.,56.,58.,59.,59.,59.,60.,60.,60.,60.,60.,61.,61.,61.,61.,61.,61.,61., &
      61.,61.,61.,61.,61.,60.,60.,60.,60.,59.,59.,59.,58.,58.,58.,58.,57.,58.,58.,58.,58.,58.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,42.,40.,40.,43.,40.,43.,47.,41.,40.,40.,38.,38.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,45.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,50.,50., &
      50.,51.,48.,48.,48.,48.,51.,65.,64.,63.,62.,61.,61.,61.,60.,60.,60.,60.,61.,62.,64.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 2 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,56.,53.,53.,52.,52.,51.,51.,52.,46.,46.,49.,49.,48.,48.,51.,51.,51.,52.,49.,51.,54., &
      50.,47.,46.,46.,45.,46.,46.,46.,45.,46.,47.,39.,39.,39.,37.,37.,44.,44.,45.,45.,44.,44.,45.,45.,43.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,37.,38.,38.,44.,40.,46.,33.,32.,34.,33.,30.,31.,29.,30.,28.,30.,30.,30.,28., &
      30.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,37., &
      40.,39.,39.,39.,39.,38.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,46.,46.,47.,48.,48.,47.,44.,43.,46.,49.,56.,56.,58., &
      58.,58.,58.,58.,58.,53.,52.,57.,53.,54.,56.,55.,56.,56.,58.,59.,59.,59.,60.,60.,60.,60.,60.,60.,61.,61.,61.,61.,61.,61., &
      61.,61.,61.,61.,61.,60.,60.,60.,59.,59.,59.,58.,58.,58.,58.,58.,57.,57.,58.,58.,58.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,42.,40.,39.,43.,40.,43.,47.,41.,40.,40.,38.,38.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,45.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,50.,50., &
      50.,51.,48.,48.,48.,48.,50.,65.,64.,63.,62.,61.,61.,61.,60.,60.,60.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 3 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,56.,53.,53.,52.,52.,51.,51.,52.,50.,46.,46.,46.,48.,48.,51.,45.,51.,52.,49.,51.,46., &
      50.,47.,46.,46.,45.,46.,46.,46.,45.,46.,47.,39.,39.,39.,37.,37.,44.,43.,45.,45.,44.,44.,45.,45.,43.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,37., &
      40.,39.,39.,39.,39.,38.,43.,43.,43.,43.,70.,42.,43.,48.,48.,48.,48.,46.,46.,47.,48.,48.,44.,44.,43.,44.,48.,50.,56.,57., &
      58.,58.,58.,58.,55.,53.,52.,57.,53.,54.,54.,55.,56.,56.,58.,59.,59.,59.,60.,59.,59.,60.,60.,60.,61.,61.,61.,61.,61.,61., &
      61.,61.,61.,60.,60.,60.,59.,59.,58.,58.,58.,58.,57.,57.,57.,57.,57.,57.,58.,58.,57.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,42.,40.,39.,43.,40.,43.,47.,41.,40.,40.,38.,38.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,45.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,50.,50., &
      50.,50.,48.,48.,48.,48.,50.,51.,64.,63.,62.,61.,61.,61.,60.,60.,60.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 4 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,53.,53.,53.,52.,52.,51.,51.,52.,50.,46.,46.,46.,48.,48.,51.,45.,51.,44.,44.,45.,46., &
      47.,47.,46.,46.,45.,45.,46.,45.,45.,46.,47.,39.,39.,39.,37.,37.,44.,43.,44.,44.,44.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,37., &
      40.,39.,39.,39.,39.,38.,43.,43.,43.,43.,70.,42.,42.,48.,48.,48.,45.,45.,46.,47.,48.,48.,44.,44.,43.,44.,47.,49.,56.,56., &
      56.,57.,58.,58.,55.,53.,52.,57.,53.,53.,54.,55.,55.,56.,58.,59.,59.,59.,60.,59.,59.,60.,60.,60.,61.,61.,61.,61.,61.,61., &
      61.,61.,60.,60.,60.,59.,59.,58.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,58.,58.,56.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,42.,40.,39.,43.,40.,43.,47.,41.,40.,40.,38.,38.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,45.,47., &
      50.,50.,48.,48.,48.,48.,49.,50.,64.,63.,62.,61.,61.,61.,60.,60.,59.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,73.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 5 week
      78.,78.,78.,78.,78.,78.,78.,59.,52.,52.,53.,53.,52.,52.,51.,51.,52.,50.,47.,49.,49.,48.,48.,51.,45.,51.,44.,44.,45.,46., &
      46.,38.,46.,45.,45.,45.,45.,45.,45.,46.,47.,39.,39.,39.,37.,40.,44.,43.,44.,44.,44.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,37., &
      40.,39.,39.,39.,39.,38.,43.,36.,43.,43.,70.,42.,42.,48.,48.,48.,48.,46.,46.,47.,48.,48.,44.,44.,43.,44.,44.,48.,50.,55., &
      56.,57.,58.,58.,55.,53.,52.,57.,53.,53.,54.,54.,55.,56.,57.,58.,59.,59.,59.,59.,59.,60.,60.,60.,61.,61.,61.,61.,61.,61., &
      61.,60.,60.,60.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,58.,58.,56.,57.,57.,57.,58.,58.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,42.,40.,39.,43.,40.,43.,47.,41.,40.,40.,38.,38.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,45.,45.,45.,45.,46., &
      49.,48.,48.,48.,48.,48.,49.,49.,49.,63.,62.,61.,61.,61.,60.,60.,59.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,73.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 6 week
      78.,78.,78.,78.,78.,78.,78.,59.,52.,52.,53.,53.,52.,52.,51.,51.,52.,50.,47.,49.,49.,48.,48.,51.,45.,47.,44.,44.,45.,46., &
      46.,38.,46.,45.,45.,45.,45.,45.,45.,46.,47.,39.,39.,39.,37.,40.,44.,43.,44.,44.,44.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,37., &
      39.,39.,39.,39.,39.,38.,43.,36.,43.,43.,70.,42.,42.,48.,48.,48.,48.,46.,46.,47.,48.,48.,44.,44.,43.,43.,44.,45.,49.,55., &
      56.,57.,57.,57.,55.,52.,52.,57.,52.,53.,53.,54.,55.,56.,57.,58.,59.,59.,59.,59.,59.,60.,60.,60.,61.,61.,61.,61.,61.,60., &
      60.,60.,59.,59.,59.,58.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,58.,56.,57.,57.,57.,58.,58.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,60.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,42.,42.,39.,43.,40.,43.,47.,41.,40.,40.,40.,41.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,45.,45.,45.,45.,46., &
      46.,48.,48.,48.,48.,48.,48.,48.,49.,50.,62.,61.,61.,61.,60.,60.,59.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,73.,74.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 7 week
      78.,78.,78.,78.,78.,78.,78.,59.,52.,52.,53.,53.,52.,52.,48.,51.,49.,50.,47.,49.,49.,48.,48.,43.,45.,43.,44.,44.,45.,46., &
      47.,38.,46.,46.,45.,45.,45.,45.,45.,46.,47.,39.,39.,39.,37.,40.,44.,43.,44.,44.,44.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,37., &
      40.,39.,40.,45.,39.,38.,43.,36.,43.,43.,70.,42.,43.,48.,48.,48.,48.,46.,46.,47.,48.,48.,44.,44.,43.,43.,44.,45.,49.,52., &
      55.,56.,56.,56.,55.,52.,51.,57.,52.,53.,53.,54.,54.,55.,57.,58.,59.,59.,59.,59.,59.,60.,60.,60.,61.,61.,61.,61.,60.,60., &
      60.,60.,59.,59.,59.,58.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,56.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,60.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,40.,40., &
      42.,39.,39.,44.,46.,47.,39.,42.,39.,43.,40.,43.,47.,41.,40.,40.,40.,41.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,45.,45.,45.,45.,46., &
      46.,48.,48.,48.,48.,48.,48.,48.,48.,48.,61.,61.,61.,60.,60.,59.,59.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,72.,73.,73.,72.,74.,74.,74.,75.,75.,76.,76.,76.,77., &   ! 8 week
      78.,78.,78.,78.,78.,78.,78.,59.,52.,52.,53.,53.,52.,52.,48.,51.,49.,50.,49.,49.,49.,48.,48.,43.,45.,43.,44.,44.,45.,46., &
      47.,38.,46.,46.,45.,45.,45.,45.,45.,46.,47.,39.,39.,39.,37.,40.,44.,43.,44.,45.,44.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,38.,37.,40., &
      40.,39.,40.,45.,40.,38.,43.,36.,43.,43.,70.,43.,43.,48.,48.,48.,48.,46.,46.,47.,48.,48.,44.,44.,43.,43.,44.,45.,45.,49., &
      52.,56.,56.,56.,55.,52.,51.,57.,52.,53.,53.,54.,54.,55.,57.,58.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,60.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,56.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,61.,61.,61.,62.,60.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,39.,42.,39.,43.,40.,43.,47.,41.,40.,40.,48.,41.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,45.,45.,45.,45.,46., &
      46.,48.,48.,48.,48.,48.,48.,48.,48.,48.,48.,61.,61.,60.,60.,59.,59.,60.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,72.,73.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &    ! 9 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,56.,59.,59.,58.,53.,48.,51.,49.,50.,49.,49.,49.,48.,48.,43.,45.,43.,44.,44.,45.,46., &
      47.,38.,46.,46.,45.,45.,45.,45.,45.,45.,47.,39.,39.,39.,37.,40.,44.,44.,44.,45.,44.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,40.,49.,49., &
      42.,40.,40.,45.,40.,38.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,46.,46.,47.,48.,48.,44.,44.,43.,43.,43.,44.,45.,49., &
      51.,55.,56.,56.,55.,52.,51.,57.,52.,53.,53.,54.,54.,55.,57.,58.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,60.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,56.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,61.,61.,61.,62.,60.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,39.,39.,44.,46.,47.,39.,42.,39.,43.,40.,43.,47.,41.,40.,40.,48.,41.,41.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,41.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,45.,45.,45.,45.,46., &
      46.,46.,48.,48.,48.,48.,48.,48.,48.,48.,48.,61.,61.,60.,60.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,72.,73.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &    ! 10 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,59.,60.,59.,58.,56.,48.,53.,49.,50.,49.,49.,49.,48.,48.,43.,45.,43.,44.,44.,45.,46., &
      47.,47.,49.,46.,45.,45.,56.,45.,45.,45.,47.,39.,39.,39.,37.,40.,44.,44.,45.,45.,45.,44.,45.,45.,43.,47.,57.,47.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,40.,49.,49., &
      42.,72.,44.,45.,40.,40.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,49.,47.,48.,48.,48.,44.,44.,43.,43.,43.,45.,45.,49., &
      51.,54.,55.,55.,55.,53.,52.,57.,52.,53.,53.,54.,54.,55.,57.,58.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,59.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,56.,56.,56.,56.,56.,57.,57.,56.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,61.,61.,61.,62.,60.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,40.,39.,44.,46.,47.,39.,42.,39.,43.,40.,43.,47.,41.,40.,40.,48.,42.,41.,44.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,44.,41.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,45.,46., &
      46.,46.,48.,48.,48.,48.,48.,48.,48.,48.,48.,61.,47.,60.,59.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,69.,69.,65.,65.,71.,72.,72.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &    ! 11 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,59.,60.,59.,58.,56.,57.,57.,53.,50.,49.,49.,49.,53.,53.,51.,45.,47.,46.,49.,47.,46., &
      50.,49.,49.,47.,45.,45.,56.,47.,45.,45.,47.,39.,39.,39.,37.,40.,44.,45.,45.,45.,45.,44.,45.,45.,43.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,40.,49.,49., &
      42.,72.,44.,45.,40.,40.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,49.,53.,48.,48.,48.,47.,44.,43.,43.,44.,45.,45.,49., &
      50.,55.,55.,55.,55.,53.,52.,57.,52.,53.,53.,54.,55.,55.,57.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,59.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,56.,56.,56.,56.,56.,57.,56.,56.,57.,57.,57.,58.,59.,60.,60.,60.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,50.,41.,40., &
      42.,40.,39.,44.,46.,47.,39.,42.,39.,43.,40.,43.,47.,41.,40.,42.,48.,47.,44.,44.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,44.,45.,44.,44.,41.,41.,41.,41.,42.,42.,42.,42.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,45.,46., &
      46.,46.,48.,48.,48.,48.,48.,48.,48.,48.,48.,48.,47.,60.,59.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,69.,69.,65.,65.,71.,71.,72.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &    ! 12 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,59.,60.,59.,58.,56.,57.,57.,58.,59.,59.,53.,53.,53.,53.,51.,45.,47.,46.,49.,51.,54., &
      50.,49.,49.,48.,45.,45.,56.,56.,47.,46.,57.,39.,39.,39.,39.,40.,48.,45.,45.,46.,45.,45.,45.,46.,43.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,42.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,38.,38.,40.,40.,40.,49.,49., &
      42.,72.,44.,45.,41.,40.,43.,43.,43.,43.,70.,43.,44.,48.,48.,48.,48.,49.,53.,51.,48.,48.,47.,44.,43.,43.,44.,45.,45.,47., &
      50.,55.,57.,57.,57.,54.,52.,57.,53.,53.,54.,54.,55.,55.,58.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,59.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,56.,56.,56.,56.,56.,57.,56.,56.,57.,57.,57.,58.,59.,60.,60.,60.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,41.,40., &
      42.,40.,39.,44.,50.,47.,42.,42.,39.,43.,40.,43.,47.,41.,40.,42.,48.,47.,44.,44.,43.,44.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,44.,45.,44.,44.,41.,41.,41.,41.,42.,42.,42.,43.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,45.,46., &
      46.,46.,48.,48.,48.,48.,48.,48.,48.,48.,48.,48.,60.,60.,59.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,69.,69.,65.,65.,71.,71.,71.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &    ! 13 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,59.,60.,59.,58.,56.,56.,57.,58.,59.,59.,61.,60.,54.,54.,51.,51.,51.,52.,52.,51.,54., &
      50.,54.,55.,56.,45.,58.,56.,56.,54.,58.,57.,39.,39.,39.,39.,49.,48.,51.,48.,48.,47.,46.,48.,48.,47.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,49.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,52.,39.,39.,72.,52.,40.,40.,42.,41.,40.,45.,49.,49., &
      42.,72.,44.,45.,41.,40.,43.,43.,43.,43.,70.,43.,44.,48.,48.,48.,48.,49.,53.,51.,49.,49.,47.,44.,43.,44.,44.,45.,45.,47., &
      50.,55.,57.,57.,57.,55.,53.,57.,53.,53.,54.,54.,55.,55.,58.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,59.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,56.,56.,56.,56.,56.,57.,57.,56.,57.,57.,57.,58.,59.,60.,60.,60.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,52.,40., &
      42.,40.,39.,44.,50.,47.,52.,53.,43.,43.,40.,43.,47.,41.,40.,42.,48.,47.,44.,44.,46.,44.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,45.,45.,44.,44.,42.,42.,41.,41.,42.,42.,42.,44.,43.,45.,43.,42.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,45.,46., &
      46.,49.,48.,48.,48.,48.,48.,48.,48.,48.,48.,60.,60.,60.,59.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,68.,69.,65.,65.,71.,71.,71.,72.,72.,72.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &     ! 14 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,59.,60.,59.,58.,58.,59.,58.,58.,59.,59.,62.,60.,60.,54.,57.,56.,51.,54.,55.,54.,54., &
      55.,54.,55.,56.,57.,58.,56.,56.,54.,58.,57.,39.,57.,39.,39.,49.,48.,51.,48.,48.,48.,48.,48.,48.,47.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,49.,37.,38.,38.,44.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,30.,30.,28., &
      28.,31.,35.,37.,37.,36.,37.,34.,35.,37.,36.,42.,42.,42.,38.,51.,51.,52.,52.,53.,72.,52.,40.,40.,42.,41.,43.,45.,49.,49., &
      42.,72.,44.,45.,41.,43.,43.,43.,43.,43.,70.,43.,44.,48.,48.,48.,48.,49.,53.,51.,49.,49.,50.,46.,43.,44.,44.,45.,45.,48., &
      56.,57.,57.,57.,57.,56.,56.,57.,57.,53.,54.,54.,55.,55.,58.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,59.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,56.,56.,57.,57.,57.,57.,57.,57.,57.,57.,57.,58.,59.,60.,63.,61.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,52.,40., &
      42.,40.,39.,44.,50.,52.,52.,53.,43.,43.,52.,43.,47.,42.,40.,50.,48.,47.,47.,46.,46.,44.,44.,45.,43.,44.,43.,44.,43.,43., &
      44.,43.,45.,46.,44.,44.,43.,46.,43.,41.,42.,42.,44.,44.,43.,45.,43.,44.,43.,44.,45.,45.,45.,47.,46.,46.,45.,45.,45.,46., &
      46.,50.,48.,48.,48.,48.,48.,49.,49.,49.,49.,60.,60.,60.,59.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,68.,69.,65.,65.,71.,71.,71.,72.,72.,72.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &     ! 15 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,60.,60.,60.,60.,60.,60.,63.,63.,64.,60.,60.,54.,58.,56.,55.,54.,55.,54.,54., &
      55.,54.,55.,56.,57.,58.,56.,56.,54.,58.,57.,39.,57.,39.,39.,66.,51.,51.,52.,48.,48.,48.,48.,48.,47.,47.,57.,58.,47.,47., &
      47.,47.,48.,48.,45.,52.,45.,45.,45.,49.,49.,37.,38.,38.,46.,40.,46.,33.,32.,33.,33.,30.,31.,29.,30.,28.,29.,31.,30.,28., &
      28.,31.,35.,37.,37.,36.,45.,34.,35.,43.,42.,42.,42.,42.,43.,51.,51.,52.,52.,53.,72.,52.,51.,48.,48.,49.,43.,45.,49.,49., &
      42.,72.,44.,45.,42.,43.,43.,43.,43.,43.,70.,45.,44.,48.,48.,48.,48.,49.,53.,51.,51.,51.,53.,46.,43.,44.,45.,45.,47.,56., &
      57.,57.,57.,57.,57.,57.,56.,57.,57.,53.,54.,54.,55.,55.,58.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,60.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,59.,57.,57.,57.,58.,59.,60.,63.,61.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,52.,40., &
      43.,49.,49.,44.,50.,52.,52.,53.,43.,43.,52.,43.,47.,42.,40.,50.,48.,47.,47.,46.,46.,44.,44.,45.,45.,44.,43.,44.,45.,43., &
      44.,44.,45.,46.,46.,44.,46.,46.,45.,45.,42.,42.,45.,45.,45.,47.,43.,44.,43.,44.,45.,45.,45.,47.,46.,51.,52.,45.,46.,46., &
      50.,50.,48.,48.,48.,48.,48.,49.,49.,49.,62.,61.,60.,60.,59.,59.,59.,59.,60.,62.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,68.,69.,65.,65.,71.,71.,71.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &     ! 16 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,60.,60.,60.,60.,60.,61.,63.,65.,64.,63.,60.,60.,60.,61.,61.,61.,57.,56.,56., &
      56.,55.,55.,56.,57.,58.,56.,56.,54.,58.,57.,39.,57.,40.,39.,66.,66.,52.,52.,53.,55.,48.,49.,49.,56.,57.,57.,58.,51.,51., &
      50.,48.,48.,48.,52.,52.,52.,52.,50.,49.,49.,37.,38.,38.,52.,40.,51.,33.,32.,34.,33.,30.,31.,29.,30.,28.,30.,32.,30.,28., &
      28.,31.,35.,37.,37.,36.,45.,34.,35.,43.,42.,42.,42.,42.,43.,51.,51.,52.,52.,53.,72.,52.,51.,48.,48.,49.,43.,45.,49.,49., &
      42.,72.,44.,45.,45.,43.,43.,43.,43.,43.,70.,45.,44.,48.,48.,48.,48.,49.,53.,51.,52.,52.,53.,47.,43.,45.,47.,48.,48.,56., &
      56.,57.,57.,57.,57.,57.,56.,57.,57.,53.,54.,55.,55.,55.,58.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,60.,60., &
      60.,60.,59.,59.,59.,58.,58.,57.,57.,57.,57.,57.,57.,57.,57.,57.,57.,59.,59.,61.,59.,58.,63.,57.,58.,59.,60.,63.,61.,60., &
      61.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,52.,40., &
      43.,49.,49.,44.,50.,52.,52.,53.,43.,43.,52.,43.,48.,48.,40.,50.,48.,47.,47.,48.,46.,44.,44.,45.,45.,44.,44.,44.,45.,46., &
      45.,46.,49.,46.,46.,46.,46.,46.,46.,45.,44.,45.,45.,45.,45.,47.,46.,44.,43.,44.,45.,45.,45.,47.,46.,51.,52.,52.,52.,52., &
      50.,50.,48.,48.,48.,48.,49.,49.,49.,50.,62.,61.,61.,60.,59.,59.,59.,59.,60.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,68.,69.,65.,65.,71.,71.,71.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &     ! 17 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,60.,60.,60.,60.,60.,61.,63.,65.,64.,64.,63.,63.,61.,61.,61.,61.,61.,60.,61., &
      61.,61.,57.,57.,61.,60.,59.,58.,58.,58.,64.,64.,58.,40.,65.,66.,66.,66.,66.,67.,65.,68.,68.,68.,56.,57.,57.,58.,51.,51., &
      54.,53.,54.,60.,52.,52.,52.,54.,54.,55.,55.,37.,38.,38.,55.,40.,54.,33.,32.,34.,33.,30.,31.,29.,30.,32.,32.,32.,35.,28., &
      28.,33.,35.,37.,37.,36.,45.,34.,35.,43.,42.,42.,42.,42.,43.,51.,51.,52.,52.,53.,72.,52.,51.,48.,48.,49.,43.,45.,49.,49., &
      72.,72.,72.,45.,45.,69.,43.,43.,43.,43.,70.,70.,44.,48.,48.,48.,48.,49.,53.,51.,52.,52.,53.,47.,43.,47.,48.,48.,56.,57., &
      57.,57.,58.,58.,58.,58.,58.,57.,57.,54.,56.,55.,55.,56.,58.,59.,60.,60.,60.,60.,59.,60.,60.,60.,60.,60.,60.,61.,61.,61., &
      61.,60.,59.,59.,59.,58.,58.,58.,57.,57.,57.,57.,57.,57.,57.,59.,59.,60.,59.,61.,59.,59.,63.,57.,58.,59.,60.,63.,61.,60., &
      61.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,52.,41., &
      43.,49.,49.,44.,50.,52.,52.,53.,43.,43.,52.,43.,50.,50.,40.,50.,51.,47.,47.,48.,47.,47.,45.,45.,45.,45.,45.,45.,45.,48., &
      45.,46.,49.,49.,46.,46.,46.,46.,46.,46.,45.,45.,45.,45.,45.,47.,46.,44.,44.,44.,45.,45.,45.,47.,46.,51.,52.,52.,52.,52., &
      52.,51.,48.,48.,48.,48.,48.,49.,64.,63.,62.,61.,61.,60.,59.,59.,59.,59.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,64.,68.,69.,65.,65.,71.,71.,72.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &     ! 18 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,61.,60.,61.,61.,61.,61.,63.,65.,64.,64.,65.,64.,64.,64.,63.,63.,63.,62.,62., &
      62.,62.,61.,61.,61.,60.,60.,60.,60.,60.,64.,64.,66.,65.,65.,66.,66.,66.,66.,67.,65.,68.,68.,68.,68.,58.,58.,68.,68.,55., &
      54.,60.,59.,60.,59.,59.,58.,58.,57.,60.,57.,37.,38.,38.,67.,40.,54.,33.,32.,34.,33.,30.,31.,29.,30.,33.,32.,32.,35.,28., &
      30.,33.,35.,38.,40.,46.,45.,34.,35.,48.,46.,55.,53.,47.,46.,51.,51.,52.,52.,53.,72.,52.,51.,48.,48.,49.,48.,45.,49.,49., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,48.,48.,48.,48.,49.,53.,51.,52.,52.,53.,47.,48.,48.,50.,57.,57.,57., &
      58.,58.,58.,58.,58.,58.,58.,57.,57.,58.,56.,55.,56.,56.,59.,60.,60.,60.,60.,60.,60.,60.,61.,61.,61.,61.,61.,61.,61.,61., &
      61.,60.,59.,59.,59.,59.,58.,58.,58.,57.,57.,57.,58.,60.,60.,60.,59.,60.,60.,61.,59.,59.,63.,57.,58.,59.,60.,63.,61.,70., &
      66.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,52.,52., &
      50.,51.,50.,44.,55.,52.,52.,53.,43.,43.,52.,53.,52.,53.,40.,51.,51.,51.,52.,51.,51.,50.,50.,48.,47.,47.,46.,48.,48.,48., &
      49.,49.,49.,49.,48.,46.,46.,46.,46.,46.,46.,46.,47.,47.,46.,47.,46.,47.,48.,48.,49.,49.,51.,52.,51.,51.,52.,52.,52.,52., &
      52.,51.,48.,48.,48.,48.,48.,52.,64.,63.,62.,61.,61.,60.,59.,59.,59.,59.,61.,62.,63.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,68.,68.,69.,65.,65.,71.,72.,72.,72.,72.,73.,72.,73.,74.,74.,75.,75.,76.,76.,76.,77., &     ! 19 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,61.,61.,62.,61.,62.,63.,63.,65.,65.,64.,64.,65.,65.,64.,65.,64.,64.,64.,64.,64., &
      63.,63.,64.,63.,63.,62.,68.,76.,63.,75.,75.,64.,66.,65.,65.,66.,66.,66.,66.,67.,65.,68.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,60.,60.,60.,59.,59.,58.,59.,60.,60.,66.,37.,38.,38.,67.,40.,61.,67.,64.,34.,33.,33.,33.,29.,30.,33.,33.,33.,35.,32., &
      33.,33.,35.,50.,50.,50.,48.,34.,35.,48.,46.,55.,53.,51.,51.,51.,51.,52.,52.,53.,72.,52.,51.,48.,48.,49.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,48.,48.,54.,51.,50.,53.,51.,52.,52.,53.,50.,48.,49.,58.,58.,58.,58., &
      58.,58.,58.,58.,58.,59.,59.,58.,58.,60.,56.,56.,56.,57.,59.,60.,60.,60.,67.,60.,60.,67.,61.,61.,61.,61.,61.,61.,61.,61., &
      61.,60.,60.,59.,59.,59.,59.,58.,58.,58.,57.,58.,58.,60.,60.,60.,60.,60.,60.,61.,59.,59.,63.,58.,58.,59.,60.,63.,61.,70., &
      66.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,55.,56.,52.,52., &
      52.,52.,51.,44.,55.,55.,55.,54.,43.,43.,54.,53.,54.,53.,40.,54.,53.,53.,53.,51.,51.,50.,50.,50.,52.,50.,50.,50.,50.,49., &
      49.,49.,49.,49.,49.,48.,50.,51.,51.,48.,48.,48.,48.,48.,48.,48.,50.,51.,48.,48.,49.,49.,51.,52.,51.,51.,52.,52.,52.,52., &
      52.,52.,48.,48.,49.,49.,48.,65.,65.,63.,62.,62.,61.,60.,59.,59.,59.,59.,61.,62.,64.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,65.,65.,71.,72.,72.,72.,72.,73.,72.,75.,74.,74.,75.,75.,76.,76.,76.,77./  ! 20 week

    data threshLatNH_02 / &
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,80.,61.,62.,62.,64.,64.,64.,65.,65.,64.,67.,75.,69.,76.,66.,66.,66.,65.,65.,65., &
      66.,65.,65.,67.,67.,67.,68.,76.,66.,75.,75.,75.,76.,76.,76.,66.,66.,66.,66.,67.,65.,68.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,69.,69.,66.,65.,62.,63.,60.,60.,61.,66.,37.,38.,38.,67.,40.,61.,67.,64.,34.,33.,33.,33.,29.,30.,34.,35.,34.,35.,33., &
      33.,33.,35.,66.,66.,66.,52.,34.,35.,48.,46.,55.,55.,67.,74.,72.,51.,53.,52.,53.,72.,52.,51.,48.,72.,49.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,48.,48.,54.,51.,53.,53.,52.,52.,52.,53.,50.,49.,59.,59.,59.,59.,59., &
      59.,59.,59.,58.,58.,59.,68.,68.,69.,60.,56.,56.,56.,57.,59.,60.,69.,69.,67.,68.,68.,67.,61.,61.,61.,61.,62.,62.,61.,61., &
      61.,60.,60.,59.,59.,59.,59.,59.,59.,59.,59.,59.,60.,60.,60.,60.,60.,60.,60.,61.,59.,59.,63.,58.,58.,59.,60.,63.,62.,70., &
      66.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,65.,52.,55.,55.,56.,52.,52., &
      52.,52.,51.,60.,58.,56.,55.,57.,43.,43.,56.,55.,54.,53.,40.,54.,54.,54.,54.,53.,52.,52.,51.,53.,53.,53.,53.,52.,52.,52., &
      49.,49.,50.,50.,51.,53.,50.,51.,51.,50.,50.,49.,50.,49.,49.,48.,50.,51.,49.,49.,49.,49.,51.,52.,51.,51.,52.,52.,52.,52., &
      52.,52.,48.,48.,49.,51.,53.,65.,65.,63.,62.,62.,81.,61.,59.,59.,59.,59.,61.,62.,64.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,65.,65.,72.,72.,72.,72.,73.,73.,72.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 21 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,80.,62.,62.,62.,64.,65.,65.,65.,65.,66.,67.,76.,69.,76.,67.,67.,67.,67.,68.,66., &
      66.,66.,67.,67.,67.,67.,68.,76.,76.,76.,76.,76.,76.,76.,76.,66.,76.,76.,66.,67.,65.,68.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,69.,69.,66.,65.,63.,63.,66.,66.,66.,66.,37.,61.,38.,67.,40.,61.,67.,64.,34.,34.,33.,33.,34.,30.,34.,35.,34.,35.,33., &
      34.,33.,35.,66.,66.,66.,52.,38.,49.,48.,50.,72.,73.,67.,74.,72.,72.,73.,54.,53.,72.,55.,54.,55.,72.,49.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,63.,63.,70.,70.,71.,48.,71.,54.,53.,53.,53.,54.,53.,52.,53.,50.,49.,59.,59.,66.,66.,66., &
      59.,59.,59.,59.,59.,59.,68.,68.,69.,68.,56.,56.,56.,57.,59.,62.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,62.,62.,61., &
      61.,61.,61.,60.,60.,60.,60.,59.,59.,59.,59.,60.,60.,61.,61.,60.,60.,60.,60.,61.,59.,59.,63.,65.,59.,60.,60.,63.,62.,70., &
      66.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,54.,65.,52.,56.,62.,62.,60.,62., &
      52.,52.,51.,60.,58.,60.,58.,61.,61.,58.,58.,59.,57.,57.,40.,58.,56.,56.,56.,56.,56.,52.,53.,56.,54.,53.,54.,54.,53.,52., &
      52.,53.,53.,53.,52.,53.,53.,52.,52.,51.,50.,51.,51.,50.,50.,53.,50.,51.,52.,50.,51.,51.,51.,52.,51.,52.,52.,52.,52.,52., &
      52.,52.,51.,49.,51.,53.,67.,66.,65.,63.,62.,62.,81.,61.,59.,59.,59.,60.,61.,63.,64.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,69.,65.,72.,72.,72.,72.,73.,73.,72.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 22 week
      78.,78.,78.,78.,78.,78.,78.,60.,60.,61.,80.,78.,78.,64.,65.,65.,66.,67.,67.,67.,67.,76.,76.,76.,69.,76.,76.,77.,77.,77., &
      77.,77.,77.,76.,76.,76.,76.,76.,76.,76.,76.,76.,76.,76.,76.,77.,77.,77.,77.,77.,77.,68.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,69.,69.,66.,65.,64.,65.,66.,66.,66.,66.,37.,61.,61.,67.,63.,61.,67.,64.,34.,34.,33.,35.,34.,30.,34.,35.,34.,35.,50., &
      34.,35.,35.,66.,66.,66.,72.,38.,71.,72.,50.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,55.,54.,55.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,68.,69.,69.,70.,71.,63.,63.,70.,70.,71.,71.,71.,54.,53.,53.,53.,54.,53.,53.,53.,53.,67.,67.,67.,66.,66.,66., &
      59.,59.,67.,59.,59.,59.,68.,68.,69.,68.,68.,56.,57.,57.,59.,69.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,62.,63.,63., &
      64.,64.,64.,64.,60.,60.,63.,60.,60.,60.,60.,61.,61.,62.,63.,61.,60.,63.,63.,63.,59.,59.,63.,70.,60.,66.,60.,63.,62.,70., &
      67.,67.,70.,70.,69.,69.,70.,69.,69.,67.,62.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,54.,65.,64.,64.,62.,64.,63.,64., &
      52.,52.,51.,60.,60.,60.,60.,61.,61.,62.,59.,59.,59.,59.,58.,58.,57.,57.,57.,57.,56.,56.,56.,56.,56.,56.,58.,56.,57.,57., &
      53.,56.,54.,55.,54.,54.,54.,53.,53.,51.,50.,52.,54.,53.,56.,53.,52.,52.,52.,52.,52.,52.,53.,52.,51.,52.,52.,52.,52.,52., &
      52.,52.,52.,49.,51.,54.,67.,66.,65.,64.,63.,62.,81.,81.,59.,59.,59.,60.,61.,63.,64.,64.,65.,65.,65.,65.,65.,65.,65.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,69.,70.,72.,72.,72.,72.,73.,73.,74.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 23 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,79.,66.,66.,67.,68.,76.,68.,76.,76.,76.,69.,76.,76.,77.,77.,77., &
      77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,78.,78.,78.,78.,78.,78.,78.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,69.,69.,66.,65.,65.,65.,66.,66.,68.,67.,63.,66.,63.,67.,63.,68.,67.,64.,34.,35.,35.,70.,34.,69.,34.,35.,64.,66.,50., &
      34.,35.,67.,66.,66.,66.,72.,38.,71.,72.,50.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,55.,54.,55.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,72.,72.,69.,70.,71.,71.,71.,70.,70.,71.,71.,71.,54.,54.,53.,53.,54.,53.,54.,54.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,67.,67.,59.,67.,68.,68.,69.,68.,68.,56.,57.,59.,69.,69.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,64.,63.,63., &
      64.,65.,65.,67.,67.,64.,64.,64.,66.,66.,68.,68.,65.,65.,65.,65.,65.,65.,66.,63.,60.,60.,66.,70.,66.,66.,69.,66.,66.,70., &
      67.,67.,70.,70.,69.,69.,70.,69.,69.,67.,67.,69.,60.,68.,68.,67.,67.,67.,67.,57.,66.,65.,65.,65.,65.,65.,65.,64.,63.,64., &
      63.,63.,63.,62.,61.,61.,65.,62.,62.,62.,62.,62.,61.,61.,61.,59.,58.,57.,57.,59.,59.,57.,59.,60.,60.,58.,58.,57.,57.,57., &
      56.,56.,55.,55.,55.,55.,55.,53.,53.,51.,51.,52.,55.,55.,56.,55.,54.,54.,54.,54.,53.,54.,55.,53.,53.,53.,53.,53.,54.,54., &
      54.,54.,54.,53.,53.,71.,71.,66.,65.,64.,63.,62.,81.,81.,59.,59.,59.,60.,61.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66.,66., &
      66.,66.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,69.,70.,72.,72.,72.,73.,73.,73.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 24 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,79.,77.,66.,67.,76.,76.,68.,76.,76.,76.,69.,76.,77.,77.,77.,77., &
      77.,77.,78.,78.,78.,77.,77.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,68.,68.,68.,68.,68.,68.,68., &
      68.,69.,69.,66.,65.,69.,66.,66.,66.,68.,67.,67.,66.,66.,67.,68.,68.,67.,67.,34.,35.,35.,70.,69.,69.,34.,66.,64.,66.,68., &
      67.,35.,67.,66.,66.,66.,72.,71.,71.,72.,72.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,73.,73.,72.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,72.,72.,72.,71.,71.,71.,71.,70.,70.,71.,71.,71.,54.,54.,53.,53.,54.,53.,55.,70.,62.,67.,67.,67.,66.,70.,68., &
      68.,68.,68.,68.,67.,67.,68.,68.,69.,68.,68.,56.,57.,69.,69.,69.,69.,69.,67.,68.,68.,67.,67.,69.,69.,69.,64.,64.,63.,63., &
      65.,65.,65.,67.,67.,66.,66.,64.,66.,66.,69.,69.,69.,66.,66.,66.,65.,65.,66.,66.,66.,68.,67.,70.,66.,66.,69.,66.,66.,70., &
      67.,67.,70.,70.,69.,69.,70.,69.,69.,69.,68.,69.,60.,69.,69.,68.,68.,67.,67.,57.,69.,65.,70.,66.,65.,65.,65.,64.,65.,65., &
      65.,65.,66.,65.,62.,65.,65.,64.,62.,62.,62.,62.,62.,61.,61.,60.,60.,59.,59.,59.,59.,59.,59.,60.,60.,58.,58.,57.,57.,57., &
      56.,56.,55.,55.,55.,55.,55.,53.,53.,52.,52.,54.,55.,55.,56.,55.,54.,54.,54.,54.,54.,56.,55.,53.,53.,53.,53.,53.,54.,54., &
      54.,54.,54.,53.,72.,72.,71.,66.,65.,65.,63.,62.,81.,82.,59.,59.,59.,60.,61.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66.,66., &
      67.,66.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,69.,70.,72.,72.,72.,73.,73.,73.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 25 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,79.,77.,66.,67.,78.,78.,68.,76.,76.,76.,69.,76.,77.,77.,77.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,80.,80.,79.,79.,79.,79.,79.,78.,78.,71.,71.,71.,68.,68.,68.,68., &
      68.,69.,69.,69.,69.,69.,68.,68.,68.,68.,67.,67.,67.,67.,67.,68.,68.,68.,68.,34.,35.,35.,70.,69.,69.,34.,66.,69.,66.,68., &
      67.,67.,67.,66.,66.,66.,72.,71.,71.,72.,72.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,73.,73.,73.,72.,73.,73.,73.,72.,72., &
      72.,72.,73.,72.,73.,73.,73.,73.,71.,71.,70.,70.,71.,71.,71.,54.,54.,53.,53.,54.,53.,70.,70.,62.,70.,69.,69.,70.,70.,68., &
      69.,68.,68.,68.,68.,68.,68.,68.,69.,68.,68.,69.,69.,69.,69.,69.,69.,69.,67.,68.,68.,67.,67.,69.,69.,69.,69.,69.,69.,65., &
      65.,65.,68.,67.,67.,66.,66.,66.,66.,70.,69.,70.,70.,68.,68.,67.,70.,69.,68.,70.,70.,68.,70.,70.,66.,67.,69.,68.,69.,70., &
      67.,67.,70.,70.,69.,69.,70.,69.,69.,69.,69.,69.,69.,69.,69.,69.,68.,68.,69.,57.,69.,65.,70.,67.,66.,65.,65.,64.,65.,65., &
      65.,65.,66.,67.,65.,65.,65.,64.,63.,63.,63.,62.,62.,62.,61.,62.,62.,60.,60.,60.,60.,60.,59.,60.,60.,59.,58.,58.,57.,57., &
      56.,56.,56.,55.,55.,55.,55.,54.,53.,52.,52.,55.,55.,55.,56.,55.,56.,57.,56.,54.,54.,56.,55.,54.,55.,54.,53.,54.,55.,55., &
      55.,54.,65.,73.,72.,72.,71.,71.,65.,65.,63.,62.,81.,82.,60.,60.,59.,60.,61.,63.,64.,64.,65.,65.,65.,65.,65.,65.,66.,66., &
      67.,66.,66.,66.,67.,67.,68.,68.,68.,68.,69.,69.,70.,70.,72.,72.,72.,73.,73.,73.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 26 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,79.,78.,77.,78.,79.,79.,78.,77.,77.,77.,77.,77.,77.,77.,78.,78., &     
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,80.,80.,80.,79.,79.,79.,79.,79.,79.,71.,71.,71.,71.,70.,68.,69., &
      69.,69.,69.,69.,69.,69.,68.,68.,68.,69.,69.,69.,68.,68.,67.,68.,68.,70.,70.,70.,70.,71.,72.,73.,73.,73.,72.,70.,70.,69., &
      68.,68.,68.,68.,69.,71.,72.,71.,71.,72.,72.,72.,73.,67.,74.,74.,74.,73.,73.,72.,72.,73.,73.,73.,73.,73.,73.,73.,72.,72., &
      72.,72.,73.,73.,73.,73.,73.,73.,71.,71.,70.,70.,71.,71.,71.,70.,54.,54.,53.,54.,54.,70.,70.,70.,70.,69.,69.,70.,70.,70., &
      70.,70.,69.,69.,69.,69.,69.,68.,69.,69.,69.,69.,69.,69.,69.,69.,69.,69.,67.,68.,68.,67.,67.,69.,69.,69.,69.,69.,69.,65., &
      65.,65.,68.,67.,67.,66.,66.,66.,66.,70.,70.,70.,70.,70.,68.,68.,70.,70.,70.,70.,70.,70.,70.,70.,70.,69.,69.,70.,69.,70., &
      70.,69.,70.,70.,69.,69.,70.,69.,69.,69.,69.,70.,70.,69.,69.,69.,69.,69.,69.,69.,69.,69.,70.,69.,69.,69.,69.,66.,66.,66., &
      68.,69.,67.,67.,67.,67.,65.,65.,65.,64.,64.,63.,62.,63.,63.,62.,62.,62.,62.,62.,60.,60.,60.,62.,61.,62.,59.,58.,57.,57., &
      56.,56.,56.,55.,55.,55.,55.,55.,53.,52.,53.,55.,55.,55.,60.,59.,58.,58.,58.,59.,59.,58.,58.,58.,59.,56.,55.,55.,57.,56., &
      55.,55.,66.,74.,73.,72.,72.,71.,70.,65.,63.,62.,81.,81.,81.,60.,60.,60.,61.,63.,64.,64.,65.,65.,65.,65.,65.,66.,66.,66., &
      67.,67.,66.,67.,67.,67.,68.,68.,68.,69.,69.,69.,70.,71.,72.,72.,72.,73.,73.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 27 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,79.,79.,78.,77.,77.,77.,77.,77.,78.,78.,78.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,80.,79.,79.,79.,79.,79.,79.,79.,71.,71.,70.,70.,70., &
      70.,69.,69.,69.,69.,69.,68.,68.,68.,72.,69.,70.,68.,69.,70.,68.,71.,70.,70.,70.,70.,74.,73.,73.,73.,73.,72.,72.,71.,72., &
      72.,73.,73.,75.,75.,72.,72.,72.,72.,72.,72.,72.,73.,73.,74.,74.,74.,73.,73.,72.,72.,73.,73.,73.,73.,73.,73.,73.,73.,73., &
      73.,72.,73.,73.,73.,73.,73.,73.,71.,71.,70.,70.,71.,71.,71.,71.,71.,54.,54.,71.,72.,70.,70.,70.,70.,70.,70.,70.,70.,70., &
      70.,70.,70.,70.,70.,70.,69.,70.,70.,69.,69.,70.,69.,69.,69.,69.,69.,69.,67.,68.,68.,67.,67.,69.,69.,69.,69.,69.,69.,69., &
      68.,68.,68.,67.,67.,67.,67.,66.,66.,70.,70.,70.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70.,71.,71.,71.,70.,70.,70.,70.,70., &
      70.,70.,70.,70.,69.,69.,70.,69.,69.,69.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,69.,70.,70.,69.,68., &
      68.,69.,67.,67.,67.,67.,66.,65.,65.,65.,67.,64.,66.,68.,68.,68.,64.,65.,62.,63.,62.,62.,61.,62.,62.,62.,59.,58.,58.,57., &
      56.,56.,56.,55.,55.,55.,55.,55.,54.,53.,55.,55.,55.,60.,60.,59.,59.,59.,58.,60.,59.,59.,58.,59.,60.,61.,62.,63.,63.,64., &
      64.,65.,66.,74.,73.,72.,72.,81.,71.,65.,63.,81.,81.,81.,81.,60.,60.,60.,62.,63.,64.,65.,65.,65.,65.,65.,65.,66.,66.,67., &
      67.,67.,67.,67.,67.,67.,68.,68.,69.,69.,69.,69.,70.,71.,72.,72.,72.,73.,73.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 28 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,80.,80.,80.,79.,77.,77.,77.,78.,78.,78.,78.,78., &   
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,80.,80.,80.,80.,80.,80.,80.,80.,71.,71.,70.,70.,70., &
      70.,69.,69.,69.,69.,69.,68.,68.,68.,72.,72.,71.,71.,70.,70.,71.,71.,71.,72.,72.,75.,75.,74.,73.,73.,73.,74.,72.,71.,72., &
      72.,73.,73.,75.,75.,75.,72.,73.,73.,72.,73.,72.,73.,73.,74.,74.,74.,74.,73.,73.,73.,73.,73.,73.,73.,73.,73.,73.,73.,73., &
      73.,73.,73.,73.,73.,73.,73.,73.,73.,73.,70.,70.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72., &
      72.,71.,70.,70.,70.,71.,70.,71.,70.,70.,70.,70.,70.,69.,69.,69.,69.,69.,70.,70.,70.,67.,67.,69.,69.,69.,69.,69.,69.,69., &
      68.,68.,68.,67.,67.,67.,71.,71.,66.,70.,70.,70.,71.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70.,70.,70.,70., &
      70.,70.,70.,70.,69.,70.,70.,69.,69.,70.,71.,70.,70.,70.,70.,70.,70.,70.,71.,71.,71.,71.,70.,70.,70.,70.,71.,70.,70.,69., &
      68.,69.,68.,68.,68.,69.,68.,66.,67.,68.,67.,68.,68.,68.,68.,68.,66.,66.,63.,66.,62.,63.,63.,64.,64.,64.,68.,59.,58.,58., &
      57.,56.,56.,56.,55.,55.,55.,55.,55.,55.,57.,59.,62.,65.,65.,60.,60.,64.,63.,64.,66.,63.,63.,59.,61.,62.,62.,63.,64.,64., &
      64.,65.,66.,74.,74.,72.,72.,81.,71.,65.,63.,81.,81.,81.,81.,81.,60.,60.,62.,63.,64.,65.,65.,65.,65.,65.,65.,66.,66.,67., &
      67.,67.,67.,67.,67.,67.,68.,68.,69.,69.,69.,69.,70.,71.,72.,72.,73.,73.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 29 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,80.,80.,80.,78.,78.,77.,79.,79.,79.,79.,79.,78., &    
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,80.,80.,81.,81.,80.,80.,80.,80.,71.,71.,70.,71.,70., &
      72.,72.,74.,69.,69.,69.,69.,68.,68.,75.,75.,80.,80.,80.,80.,72.,72.,79.,78.,78.,78.,75.,75.,74.,73.,74.,74.,72.,72.,75., &
      75.,75.,73.,75.,75.,75.,75.,75.,76.,76.,76.,73.,76.,77.,77.,74.,74.,74.,76.,74.,73.,73.,73.,73.,73.,73.,73.,73.,73.,73., &
      73.,73.,73.,73.,73.,73.,74.,75.,75.,73.,70.,70.,71.,71.,71.,71.,71.,71.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72., &
      72.,71.,71.,70.,71.,71.,71.,71.,71.,70.,70.,70.,70.,70.,69.,69.,69.,69.,70.,70.,70.,70.,69.,70.,69.,69.,69.,69.,69.,69., &
      69.,68.,68.,67.,68.,68.,71.,71.,71.,70.,70.,70.,71.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70.,70., &
      70.,70.,70.,70.,70.,70.,70.,70.,71.,71.,71.,70.,70.,70.,70.,70.,70.,70.,71.,71.,71.,71.,70.,70.,72.,72.,71.,71.,70.,70., &
      71.,72.,72.,72.,70.,70.,70.,70.,70.,68.,68.,68.,68.,69.,68.,68.,67.,67.,67.,67.,67.,67.,67.,67.,68.,68.,68.,67.,65.,66., &
      66.,57.,56.,56.,55.,55.,55.,55.,55.,55.,59.,59.,65.,65.,65.,65.,65.,64.,64.,64.,66.,65.,63.,62.,62.,62.,62.,63.,64.,64., &
      64.,66.,66.,74.,82.,82.,81.,81.,71.,65.,63.,81.,81.,81.,81.,81.,60.,60.,62.,63.,64.,65.,65.,65.,65.,66.,66.,66.,66.,68., &
      68.,68.,68.,67.,67.,69.,69.,69.,69.,69.,70.,69.,70.,72.,72.,72.,73.,73.,74.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 30 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,80.,80.,80.,80.,80.,79.,80.,80.,80.,80.,80.,78., &     
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,80.,80.,71.,80.,71.,71.,80., &
      80.,80.,75.,75.,75.,75.,69.,68.,80.,80.,80.,80.,80.,81.,80.,80.,79.,79.,78.,78.,78.,78.,75.,75.,75.,74.,74.,74.,75.,75., &
      75.,75.,75.,75.,75.,75.,75.,75.,76.,76.,76.,76.,76.,77.,77.,76.,76.,74.,76.,76.,76.,73.,74.,74.,73.,73.,73.,73.,73.,73., &
      73.,73.,74.,75.,75.,75.,76.,76.,76.,76.,76.,75.,73.,72.,71.,71.,71.,72.,72.,72.,73.,72.,72.,72.,72.,72.,72.,72.,72.,72., &
      72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70.,70.,70.,69.,70.,69.,69.,70.,70.,70.,70.,70.,70.,70.,70.,70.,69.,69.,69., &
      69.,68.,69.,69.,69.,72.,72.,72.,71.,71.,72.,71.,72.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70., &
      70.,70.,70.,70.,70.,70.,71.,71.,71.,71.,71.,70.,70.,71.,71.,70.,70.,70.,71.,71.,71.,71.,70.,70.,72.,72.,73.,74.,74.,74., &
      72.,72.,72.,72.,73.,71.,71.,72.,72.,70.,72.,71.,70.,69.,69.,68.,67.,67.,67.,67.,67.,67.,69.,69.,68.,68.,69.,69.,69.,69., &
      69.,67.,56.,56.,56.,55.,55.,56.,56.,59.,59.,65.,65.,65.,65.,65.,66.,65.,64.,64.,66.,68.,63.,62.,62.,62.,62.,63.,64.,64., &
      65.,66.,66.,74.,82.,82.,81.,81.,81.,65.,63.,81.,81.,81.,81.,81.,60.,61.,63.,63.,65.,65.,65.,65.,65.,66.,66.,66.,68.,68., &
      68.,68.,68.,81.,69.,80.,71.,69.,69.,70.,70.,70.,70.,72.,72.,72.,73.,74.,74.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 31 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,80.,80.,80.,80.,80.,80.,80.,80.,80.,79.,79.,78., &    
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80., &
      80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,81.,80.,80.,79.,79.,79.,79.,78.,78.,78.,78.,78.,75.,75.,75.,75.,75., &
      75.,75.,75.,75.,75.,75.,75.,75.,76.,76.,76.,76.,77.,77.,77.,76.,76.,76.,76.,76.,76.,76.,76.,74.,74.,74.,74.,73.,74.,74., &
      74.,75.,75.,75.,76.,76.,77.,76.,76.,77.,77.,77.,77.,78.,78.,72.,72.,72.,72.,73.,73.,73.,74.,74.,74.,73.,73.,73.,73.,73., &
      72.,72.,72.,72.,72.,72.,72.,72.,72.,71.,71.,71.,71.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,70.,69.,69., &
      69.,68.,69.,69.,71.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,71.,71.,71.,71.,71.,72.,71.,71.,71.,71.,71.,71., &
      71.,70.,70.,70.,70.,70.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70.,70.,72.,72.,73.,74.,74.,74., &
      72.,74.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,71.,70.,70.,70.,68.,68.,69.,69.,69.,69.,69.,69.,71.,71.,69.,69.,69., &
      69.,67.,67.,67.,68.,65.,65.,64.,64.,64.,65.,65.,67.,65.,65.,65.,66.,68.,70.,69.,68.,68.,63.,65.,65.,62.,63.,63.,64.,64., &
      67.,75.,75.,81.,82.,82.,81.,81.,81.,65.,63.,81.,81.,81.,81.,81.,60.,61.,63.,64.,65.,65.,65.,65.,66.,66.,66.,67.,68.,68., &
      68.,71.,69.,81.,81.,80.,71.,71.,71.,71.,72.,71.,71.,72.,72.,72.,73.,74.,75.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 32 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,81.,80.,80.,80.,80.,80.,80.,80.,80.,79.,80.,78., &     
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80., &
      80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,79.,79.,79.,79.,78.,78.,78.,78.,78.,79.,77.,75.,75., &
      75.,75.,75.,75.,75.,75.,75.,75.,77.,77.,77.,77.,77.,77.,77.,76.,77.,77.,77.,76.,76.,76.,76.,76.,74.,74.,74.,74.,74.,74., &
      76.,76.,76.,77.,77.,77.,77.,77.,77.,77.,77.,77.,78.,78.,78.,78.,78.,75.,78.,74.,78.,79.,79.,78.,78.,78.,74.,74.,74.,73., &
      73.,73.,73.,73.,72.,72.,72.,72.,72.,72.,72.,71.,71.,71.,70.,71.,71.,71.,71.,71.,71.,70.,71.,71.,70.,70.,70.,70.,70.,69., &
      69.,70.,70.,71.,71.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72.,72.,72.,72.,72.,72.,72.,71.,71., &
      71.,71.,71.,71.,71.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,73.,74.,74.,74., &
      72.,74.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72.,70.,70.,70.,70.,69.,69.,69.,69.,69.,70.,73.,74.,69.,69.,69., &
      69.,67.,67.,67.,68.,65.,65.,64.,64.,64.,65.,65.,67.,65.,65.,65.,66.,68.,70.,69.,78.,78.,68.,67.,68.,65.,64.,64.,64.,67., &
      67.,81.,75.,81.,82.,82.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,61.,63.,63.,64.,65.,66.,66.,66.,66.,66.,67.,67.,68.,68., &
      70.,71.,69.,81.,81.,80.,79.,71.,72.,72.,72.,72.,72.,72.,73.,73.,74.,74.,75.,75.,75.,76.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 33 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,81.,81.,80.,80.,80.,80.,80.,79.,79.,80.,80.,78., &   
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80., &
      80.,80.,80.,80.,80.,80.,80.,80.,80.,81.,81.,81.,80.,81.,81.,80.,80.,80.,80.,79.,79.,79.,79.,79.,78.,79.,79.,79.,79.,76., &
      75.,75.,75.,75.,75.,75.,75.,76.,77.,77.,77.,78.,78.,79.,79.,79.,77.,77.,77.,77.,77.,77.,77.,78.,74.,74.,74.,74.,74.,77., &
      77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,77.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,78.,78.,78.,78.,77.,76., &
      74.,74.,74.,74.,73.,73.,73.,73.,72.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,70.,71.,71.,70.,70.,70.,70.,70.,70., &
      69.,70.,71.,71.,71.,72.,72.,72.,72.,72.,72.,72.,73.,73.,72.,72.,72.,72.,72.,72.,72.,73.,73.,73.,73.,73.,72.,72.,72.,71., &
      71.,71.,71.,71.,72.,72.,72.,72.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,73.,73.,74.,74.,74., &
      72.,74.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72.,70.,70.,70.,70.,69.,69.,69.,69.,69.,70.,73.,74.,69.,69.,69., &
      69.,67.,67.,67.,68.,65.,65.,64.,64.,64.,65.,66.,67.,68.,65.,66.,67.,68.,78.,78.,78.,78.,79.,79.,69.,68.,67.,67.,67.,76., &
      81.,81.,75.,81.,82.,82.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,61.,63.,63.,64.,65.,66.,66.,66.,66.,82.,67.,68.,81.,81., &
      70.,71.,69.,81.,81.,80.,79.,71.,72.,72.,73.,73.,73.,73.,73.,74.,74.,74.,75.,75.,76.,76.,74.,74.,77.,75.,76.,76.,76.,77., &     ! 34 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80.,78., &    
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,80.,81.,81.,81.,81.,80.,80., &
      80.,80.,80.,81.,80.,80.,80.,80.,80.,80.,81.,81.,80.,81.,81.,81.,81.,81.,81.,81.,79.,79.,79.,81.,78.,79.,79.,82.,82.,82., &
      79.,76.,76.,76.,76.,76.,76.,77.,77.,77.,77.,78.,78.,79.,79.,79.,79.,79.,79.,79.,79.,78.,78.,78.,78.,75.,75.,78.,74.,77., &
      77.,77.,77.,77.,77.,77.,77.,77.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,79.,79.,78.,77.,77., &
      76.,76.,76.,76.,73.,73.,73.,73.,72.,72.,72.,72.,72.,72.,71.,72.,71.,71.,71.,72.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72., &
      71.,71.,71.,71.,71.,72.,73.,72.,72.,73.,73.,73.,73.,73.,73.,73.,74.,74.,72.,74.,74.,74.,74.,73.,73.,73.,73.,72.,72.,72., &
      72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,73.,73.,74.,74.,74., &
      72.,74.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,73.,72.,70.,70.,70.,70.,69.,69.,69.,69.,69.,71.,73.,74.,69.,69.,69., &
      69.,67.,67.,67.,68.,65.,65.,64.,64.,64.,65.,68.,68.,68.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,68.,67.,67.,76.,76., &
      81.,81.,75.,81.,82.,82.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,62.,63.,63.,64.,65.,66.,66.,83.,82.,82.,67.,68.,81.,81., &
      70.,71.,69.,81.,81.,80.,79.,71.,72.,73.,73.,74.,73.,73.,73.,74.,74.,75.,75.,76.,76.,76.,74.,74.,77.,75.,76.,76.,76.,77., &     ! 35 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,78.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80.,78., &  
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,81.,81.,80.,81.,81.,80.,80.,81.,81.,81.,81.,81.,81.,81., &
      81.,81.,81.,81.,81.,80.,80.,81.,80.,81.,81.,81.,80.,81.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82., &
      79.,76.,77.,77.,77.,77.,77.,77.,77.,77.,80.,78.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,75.,78.,78.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,78.,77.,77., &
      77.,77.,76.,76.,76.,73.,73.,73.,73.,73.,73.,72.,72.,72.,72.,71.,71.,71.,72.,72.,72.,72.,72.,72.,72.,72.,71.,71.,73.,73., &
      72.,72.,72.,72.,72.,74.,74.,72.,73.,73.,73.,73.,75.,75.,75.,75.,74.,75.,75.,75.,75.,74.,74.,74.,74.,73.,73.,73.,73.,73., &
      72.,74.,73.,73.,73.,73.,73.,73.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,73.,73.,73.,74.,74.,74., &
      72.,72.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,73.,72.,70.,70.,70.,70.,69.,69.,70.,70.,71.,73.,74.,74.,69.,69.,69., &
      69.,67.,67.,67.,68.,65.,65.,64.,64.,64.,81.,77.,68.,77.,77.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,80.,80.,76.,76., &
      81.,81.,75.,81.,82.,82.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,62.,63.,63.,64.,65.,66.,66.,83.,82.,82.,67.,68.,81.,81., &
      70.,71.,69.,81.,81.,80.,79.,71.,76.,73.,73.,74.,73.,74.,74.,74.,75.,75.,76.,76.,76.,76.,74.,74.,77.,75.,76.,76.,76.,77., &     ! 36 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,80.,80.,80.,80.,80.,79.,79.,79.,79.,79.,78., &   
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,81.,80.,81.,81.,80.,81.,81.,80.,80.,81.,81.,81.,81.,81.,81.,81., &
      81.,81.,81.,81.,80.,80.,80.,81.,80.,81.,81.,81.,80.,81.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82., &
      82.,76.,79.,77.,77.,77.,77.,77.,77.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,78.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,77., &
      77.,77.,77.,77.,76.,76.,76.,73.,73.,73.,73.,73.,72.,72.,72.,71.,71.,72.,72.,72.,72.,72.,72.,72.,73.,73.,72.,72.,73.,73., &
      72.,72.,72.,72.,72.,75.,75.,72.,75.,73.,76.,76.,76.,75.,75.,75.,75.,75.,75.,75.,75.,74.,74.,74.,76.,74.,74.,74.,74.,74., &
      74.,74.,74.,73.,73.,73.,73.,73.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,72.,73.,73.,73.,74.,74.,74., &
      72.,72.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72.,70.,70.,70.,70.,69.,69.,70.,73.,72.,73.,75.,74.,69.,69.,69., &
      69.,67.,67.,67.,68.,68.,69.,79.,77.,77.,77.,77.,76.,77.,77.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,80.,80.,81.,76., &
      81.,81.,75.,81.,82.,82.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,62.,63.,63.,64.,65.,66.,66.,83.,82.,82.,67.,68.,81.,81., &
      70.,71.,69.,81.,81.,80.,79.,71.,72.,72.,73.,73.,73.,74.,74.,74.,75.,75.,75.,76.,76.,76.,74.,74.,77.,75.,76.,76.,76.,77., &     ! 37 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80.,78., &     
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,81.,80.,81.,81.,80.,81.,81.,80.,80.,81.,81.,81.,81.,81.,81.,81., &
      81.,80.,80.,80.,80.,80.,80.,81.,80.,81.,81.,81.,80.,81.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82.,82., &
      82.,76.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,78.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,79.,79.,78.,79.,79.,79.,79.,79.,79.,79.,79.,79., &
      79.,77.,77.,77.,76.,76.,74.,73.,73.,73.,73.,73.,73.,73.,73.,72.,72.,72.,73.,73.,73.,73.,73.,73.,73.,73.,74.,72.,74.,74., &
      75.,72.,72.,72.,75.,75.,76.,76.,76.,76.,76.,76.,76.,75.,75.,75.,75.,75.,75.,75.,75.,76.,76.,76.,74.,74.,74.,74.,74.,74., &
      74.,74.,74.,74.,73.,73.,73.,72.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,73.,73.,74.,72.,73.,74.,74.,74., &
      72.,72.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72.,70.,70.,70.,70.,69.,69.,73.,72.,72.,75.,75.,74.,69.,69.,69., &
      69.,67.,67.,67.,68.,68.,69.,78.,77.,77.,76.,76.,76.,76.,76.,77.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,80.,80.,76.,76., &
      81.,75.,75.,74.,82.,82.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,62.,63.,63.,64.,65.,66.,66.,83.,82.,82.,67.,68.,81.,81., &
      70.,71.,69.,81.,81.,80.,79.,71.,72.,72.,72.,72.,73.,73.,74.,74.,75.,75.,75.,76.,76.,76.,74.,74.,77.,75.,76.,76.,76.,77., &     ! 38 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,81.,81.,81.,81.,81.,79.,81.,80.,80.,80.,78., &    
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,81.,80.,81.,81.,80.,81.,81.,80.,80.,81.,81.,81.,81.,81.,81.,81., &
      81.,80.,80.,80.,80.,80.,80.,80.,80.,81.,81.,81.,80.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81.,81., &
      77.,76.,77.,78.,78.,78.,78.,78.,78.,78.,78.,79.,79.,79.,79.,79.,79.,79.,79.,79.,79.,78.,78.,79.,79.,79.,79.,78.,78.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78., &
      77.,77.,76.,76.,76.,74.,73.,73.,73.,73.,73.,73.,73.,72.,72.,72.,73.,73.,73.,73.,73.,73.,73.,73.,73.,73.,75.,75.,75.,75., &
      75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,74.,74.,74.,74.,74.,73.,74.,73.,72.,72., &
      72.,74.,73.,73.,73.,73.,72.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,72.,72.,73.,73.,74.,72.,73.,74.,74.,74., &
      72.,72.,72.,72.,73.,72.,72.,72.,72.,72.,72.,71.,71.,72.,72.,70.,70.,70.,70.,69.,69.,69.,69.,71.,71.,73.,74.,73.,69.,68., &
      68.,67.,67.,67.,68.,68.,69.,69.,76.,76.,76.,75.,71.,76.,76.,77.,78.,78.,78.,78.,78.,78.,79.,79.,76.,76.,80.,76.,76.,76., &
      81.,75.,75.,74.,73.,72.,81.,81.,81.,65.,64.,81.,81.,81.,81.,81.,62.,63.,63.,65.,65.,66.,66.,83.,82.,82.,67.,68.,81.,81., &
      70.,70.,69.,81.,81.,80.,71.,71.,71.,72.,72.,72.,73.,73.,74.,74.,75.,75.,75.,76.,76.,76.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 39 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,81.,81.,79.,80.,80.,79.,81.,81.,81.,81.,78., &  
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,81.,80.,81.,81.,80.,81.,81.,80.,80.,81.,81.,81.,81.,81.,81.,80., &
      80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,81.,81.,81.,81.,81.,80.,80.,80.,80.,80.,80.,80.,81.,81.,81.,81.,79., &
      77.,76.,77.,77.,77.,77.,77.,77.,77.,75.,72.,72.,73.,77.,77.,72.,72.,78.,78.,78.,78.,78.,78.,72.,78.,78.,78.,78.,78.,78., &
      77.,77.,72.,77.,77.,77.,77.,78.,72.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,75.,71.,71.,71.,71.,71.,71.,77.,77.,76.,76., &
      76.,76.,76.,76.,74.,73.,73.,74.,73.,73.,72.,72.,72.,72.,72.,72.,72.,72.,72.,73.,73.,73.,73.,72.,72.,73.,73.,73.,73.,73., &
      74.,74.,73.,73.,73.,73.,74.,73.,73.,74.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,75.,74.,74.,74.,73.,70.,73.,73.,72.,72., &
      72.,73.,73.,73.,73.,73.,72.,72.,72.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,71.,72.,72.,73.,73.,73.,72.,73.,74.,74.,72., &
      72.,72.,72.,72.,73.,72.,72.,72.,72.,70.,70.,70.,71.,70.,71.,70.,70.,70.,69.,69.,69.,69.,69.,70.,70.,71.,71.,70.,69.,68., &
      68.,67.,67.,67.,68.,68.,69.,69.,75.,75.,75.,75.,71.,71.,76.,77.,78.,78.,78.,78.,78.,78.,79.,76.,76.,76.,76.,76.,76.,76., &
      81.,75.,75.,74.,73.,72.,81.,81.,71.,65.,64.,81.,81.,81.,81.,81.,62.,63.,63.,64.,65.,66.,66.,66.,82.,82.,67.,68.,68.,81., &
      70.,70.,69.,81.,81.,80.,71.,71.,71.,71.,72.,72.,73.,73.,74.,74.,75.,75.,75.,75.,76.,76.,74.,74.,76.,75.,76.,76.,76.,77./  ! 40 week


    data threshLatNH_03 / &
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,81.,81.,79.,79.,81.,79.,81.,81.,81.,81.,78., &    
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,81.,80.,81.,81.,80.,80.,81.,80.,80.,80.,81.,81.,80.,80.,80.,80., &
      80.,80.,79.,79.,79.,79.,79.,79.,79.,80.,79.,80.,80.,81.,81.,81.,81.,70.,67.,80.,80.,80.,70.,69.,69.,68.,72.,69.,70.,70., &
      72.,71.,70.,69.,69.,71.,72.,71.,71.,72.,72.,72.,73.,73.,74.,72.,72.,73.,73.,72.,72.,73.,73.,72.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,72.,69.,69.,70.,71.,71.,71.,70.,70.,71.,71.,71.,70.,70.,70.,70.,70.,71.,70.,70.,68.,67.,67.,67.,66.,66.,68., &
      69.,69.,69.,68.,67.,67.,68.,68.,69.,68.,68.,68.,69.,69.,69.,72.,72.,72.,69.,72.,72.,72.,72.,72.,72.,72.,72.,72.,73.,73., &
      73.,73.,73.,73.,73.,73.,74.,73.,73.,74.,75.,75.,75.,75.,75.,75.,74.,74.,69.,74.,74.,68.,70.,70.,70.,69.,69.,68.,69.,72., &
      72.,69.,72.,72.,69.,69.,72.,69.,71.,67.,67.,71.,68.,68.,68.,67.,67.,67.,67.,71.,72.,72.,72.,68.,68.,72.,73.,67.,74.,68., &
      68.,72.,67.,67.,71.,71.,71.,69.,69.,69.,69.,69.,69.,70.,69.,69.,69.,69.,69.,69.,69.,69.,65.,69.,69.,71.,69.,67.,68.,66., &
      68.,67.,67.,67.,68.,68.,69.,68.,69.,70.,70.,70.,71.,68.,68.,68.,78.,66.,78.,69.,78.,78.,76.,67.,76.,76.,76.,76.,76.,76., &
      75.,75.,74.,74.,73.,72.,81.,81.,71.,65.,64.,81.,81.,81.,81.,81.,62.,62.,63.,64.,65.,66.,66.,66.,82.,82.,67.,67.,68.,68., &
      70.,70.,69.,70.,70.,71.,70.,70.,70.,71.,72.,72.,72.,73.,74.,74.,74.,75.,75.,75.,75.,76.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 41 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,79.,79.,79.,79.,79.,79.,79.,81.,81.,81.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,81.,80.,81.,81.,80.,80.,80.,80.,80.,80.,80.,80.,80.,79.,79.,79., &    
      79.,79.,79.,79.,79.,79.,79.,70.,69.,66.,66.,66.,66.,66.,67.,68.,68.,67.,67.,70.,70.,71.,70.,69.,69.,68.,66.,64.,66.,68., &
      67.,67.,67.,67.,66.,71.,72.,71.,71.,72.,72.,72.,73.,73.,74.,72.,72.,73.,73.,72.,72.,73.,73.,72.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,71.,71.,70.,70.,70.,70.,70.,71.,70.,70.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,67.,67.,67.,67.,68.,68.,69.,68.,68.,68.,69.,69.,69.,69.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,65.,72.,72., &
      68.,71.,72.,67.,73.,73.,73.,73.,73.,73.,73.,73.,74.,74.,74.,74.,74.,69.,69.,73.,73.,68.,70.,70.,70.,69.,69.,68.,69.,70., &
      67.,67.,70.,72.,69.,69.,71.,69.,69.,67.,67.,69.,68.,68.,68.,67.,67.,67.,67.,67.,69.,66.,72.,68.,68.,72.,73.,67.,74.,68., &
      68.,72.,67.,67.,71.,71.,71.,69.,69.,69.,69.,66.,66.,68.,68.,68.,64.,65.,64.,64.,63.,64.,64.,61.,65.,68.,64.,64.,65.,66., &
      66.,66.,66.,67.,68.,68.,69.,68.,69.,70.,69.,68.,69.,68.,68.,68.,65.,66.,70.,69.,66.,77.,76.,67.,76.,76.,76.,76.,76.,76., &
      75.,75.,74.,74.,73.,72.,72.,81.,70.,65.,63.,81.,81.,81.,81.,81.,61.,62.,63.,64.,65.,66.,66.,66.,66.,82.,67.,67.,68.,68., &
      68.,70.,69.,70.,69.,69.,69.,69.,69.,70.,71.,71.,72.,73.,73.,74.,74.,74.,75.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 42 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,79.,79.,79.,79.,79.,79.,79.,81.,81.,81.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,80.,68.,80.,79.,68.,79.,68., &
      68.,79.,79.,66.,65.,63.,63.,66.,66.,66.,66.,66.,66.,66.,67.,68.,68.,67.,67.,70.,70.,71.,70.,69.,69.,68.,66.,64.,66.,68., &    
      67.,67.,67.,66.,66.,66.,72.,71.,71.,72.,72.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,73.,73.,72.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,71.,71.,70.,70.,70.,70.,70.,71.,70.,70.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,67.,67.,67.,67.,68.,68.,69.,68.,68.,68.,69.,69.,69.,62.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,64.,69.,69., &
      68.,68.,68.,67.,72.,66.,66.,72.,72.,72.,73.,73.,73.,73.,65.,61.,60.,61.,61.,64.,66.,66.,66.,70.,66.,66.,69.,66.,66.,70., &
      66.,64.,66.,66.,66.,64.,66.,63.,69.,67.,67.,69.,68.,68.,68.,67.,67.,67.,67.,66.,66.,65.,65.,65.,68.,67.,69.,67.,69.,68., &
      68.,69.,67.,62.,65.,65.,64.,64.,63.,63.,63.,63.,64.,62.,62.,63.,64.,61.,64.,62.,61.,59.,59.,60.,60.,62.,64.,64.,65.,65., &
      65.,65.,66.,66.,68.,67.,69.,67.,68.,68.,68.,68.,67.,67.,67.,65.,65.,64.,64.,65.,66.,72.,72.,67.,74.,74.,74.,74.,74.,75., &
      75.,75.,74.,73.,72.,72.,72.,81.,70.,65.,63.,81.,81.,81.,81.,81.,61.,62.,63.,64.,65.,65.,66.,66.,66.,82.,67.,67.,68.,68., &
      68.,68.,68.,68.,68.,68.,68.,69.,69.,69.,70.,71.,72.,72.,73.,74.,74.,74.,75.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 43 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,81.,79.,79.,78.,79.,79.,79.,79.,79.,79.,79.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,80.,79.,79.,79.,79.,79.,68.,68.,68.,68.,68.,68.,68., &
      68.,74.,74.,66.,65.,63.,63.,66.,66.,66.,66.,66.,66.,66.,67.,68.,68.,67.,67.,70.,70.,71.,70.,69.,69.,68.,66.,64.,66.,68., &    
      67.,67.,67.,66.,66.,66.,72.,71.,71.,72.,72.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,73.,73.,72.,72.,73.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,71.,71.,70.,70.,70.,70.,70.,71.,70.,70.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,67.,67.,67.,67.,68.,68.,69.,68.,61.,68.,69.,62.,60.,62.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,64.,63.,69., &
      66.,66.,68.,67.,67.,66.,66.,66.,72.,72.,72.,72.,72.,72.,65.,60.,60.,60.,60.,61.,61.,63.,63.,65.,65.,66.,69.,63.,63.,70., &
      66.,64.,66.,64.,66.,63.,63.,62.,62.,67.,62.,61.,61.,68.,68.,67.,67.,67.,67.,66.,66.,65.,65.,65.,64.,64.,63.,63.,59.,62., &
      63.,62.,63.,62.,62.,64.,63.,64.,63.,63.,63.,62.,61.,62.,62.,61.,61.,61.,61.,59.,61.,59.,59.,60.,60.,61.,62.,63.,63.,65., &
      64.,64.,64.,65.,63.,63.,64.,64.,67.,67.,67.,67.,67.,65.,65.,65.,65.,64.,64.,64.,66.,66.,66.,67.,66.,66.,72.,74.,74.,74., &
      74.,75.,74.,73.,72.,72.,72.,81.,70.,65.,63.,81.,81.,81.,81.,81.,61.,62.,63.,64.,65.,65.,65.,66.,66.,66.,67.,67.,67.,68., &
      68.,68.,68.,67.,67.,68.,68.,68.,68.,69.,70.,71.,72.,72.,73.,74.,74.,74.,75.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 44 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,81.,78.,77.,78.,79.,79.,78.,77.,77.,77.,79.,79.,79.,79.,79.,78., &
      78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,78.,64.,78.,65.,65.,66.,66.,66.,66.,79.,65.,78.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,74.,74.,66.,65.,63.,60.,66.,61.,66.,66.,61.,61.,61.,67.,63.,61.,67.,64.,61.,61.,71.,70.,69.,69.,68.,66.,64.,66.,68., &     
      67.,67.,67.,66.,66.,66.,72.,71.,71.,72.,72.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,55.,54.,55.,50.,73.,73.,73.,72.,72., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,71.,71.,70.,53.,70.,70.,70.,71.,52.,70.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,67.,67.,67.,67.,68.,68.,69.,68.,61.,68.,59.,62.,60.,62.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,64.,63.,69., &
      66.,66.,68.,67.,65.,66.,66.,65.,66.,72.,71.,71.,72.,65.,60.,60.,59.,60.,59.,61.,59.,59.,63.,65.,61.,66.,69.,63.,63.,70., &
      66.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,62.,67.,67.,67.,66.,66.,65.,65.,65.,64.,64.,59.,62.,59.,62., &
      59.,59.,60.,57.,60.,56.,58.,57.,58.,60.,61.,58.,61.,59.,60.,59.,59.,58.,58.,57.,56.,56.,56.,56.,55.,58.,55.,55.,63.,63., &
      64.,64.,64.,53.,53.,53.,53.,62.,63.,63.,64.,66.,65.,58.,57.,58.,57.,57.,57.,57.,66.,65.,65.,66.,66.,66.,66.,66.,65.,66., &
      71.,74.,74.,73.,72.,72.,72.,71.,65.,65.,63.,81.,81.,81.,81.,81.,61.,62.,63.,63.,65.,65.,65.,66.,66.,66.,66.,67.,67.,67., &
      67.,67.,67.,67.,67.,68.,68.,68.,68.,69.,70.,71.,72.,72.,73.,73.,74.,74.,74.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 45 week
      78.,78.,78.,78.,78.,78.,78.,80.,80.,80.,80.,78.,78.,78.,80.,78.,77.,67.,67.,67.,67.,77.,77.,77.,66.,66.,66.,65.,65.,65., &
      66.,68.,78.,78.,67.,78.,78.,78.,56.,78.,57.,64.,58.,65.,65.,66.,66.,66.,66.,78.,65.,78.,68.,68.,68.,68.,68.,68.,68.,68., &
      68.,60.,59.,60.,59.,54.,58.,54.,55.,55.,55.,54.,56.,57.,53.,54.,54.,53.,54.,53.,52.,52.,54.,54.,69.,68.,66.,64.,66.,54., &     
      54.,34.,67.,66.,66.,66.,52.,71.,49.,49.,55.,72.,73.,67.,74.,72.,72.,73.,73.,72.,72.,52.,51.,48.,48.,49.,48.,47.,49.,49., &
      72.,72.,72.,66.,65.,69.,70.,63.,61.,63.,70.,70.,71.,48.,48.,48.,48.,49.,53.,51.,52.,52.,53.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,67.,67.,59.,67.,68.,68.,69.,68.,56.,58.,59.,60.,60.,62.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,64.,63.,69., &
      65.,65.,65.,65.,65.,66.,66.,65.,66.,72.,71.,70.,65.,61.,60.,60.,59.,59.,59.,61.,59.,59.,63.,59.,60.,66.,69.,63.,62.,70., &
      61.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,59.,59.,60.,67.,66.,66.,65.,65.,65.,64.,64.,59.,62.,59.,62., &
      58.,59.,56.,55.,55.,56.,55.,54.,54.,55.,54.,54.,54.,55.,52.,51.,51.,51.,52.,51.,51.,50.,50.,52.,51.,52.,52.,51.,51.,49., &
      50.,51.,49.,52.,49.,53.,51.,51.,48.,48.,48.,49.,49.,49.,49.,49.,52.,52.,53.,54.,56.,56.,55.,56.,52.,52.,52.,53.,63.,64., &
      65.,66.,73.,73.,72.,71.,71.,70.,65.,64.,63.,81.,81.,81.,81.,81.,61.,62.,63.,63.,65.,65.,65.,66.,66.,66.,66.,67.,67.,67., &
      67.,67.,67.,67.,67.,67.,68.,68.,68.,69.,70.,71.,71.,65.,72.,73.,73.,74.,74.,75.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 46 week
      78.,78.,78.,78.,78.,78.,78.,80.,61.,61.,80.,78.,78.,63.,64.,63.,62.,63.,65.,64.,64.,77.,77.,77.,66.,64.,66.,64.,64.,64., &
      64.,64.,64.,64.,67.,67.,63.,63.,56.,78.,57.,64.,58.,65.,65.,66.,66.,66.,66.,78.,65.,55.,55.,68.,56.,57.,57.,58.,68.,55., &
      54.,52.,54.,60.,52.,52.,52.,52.,50.,49.,49.,49.,49.,50.,52.,49.,51.,51.,52.,35.,50.,52.,35.,34.,47.,68.,66.,35.,35.,32., &
      54.,34.,35.,48.,50.,66.,48.,48.,49.,48.,46.,55.,53.,47.,46.,72.,51.,73.,73.,53.,72.,52.,51.,48.,42.,41.,43.,45.,49.,49., &
      72.,72.,72.,45.,45.,69.,43.,43.,43.,43.,70.,45.,44.,48.,48.,48.,48.,49.,53.,51.,52.,51.,53.,62.,67.,67.,67.,66.,66.,66., &
      66.,66.,59.,67.,59.,67.,68.,68.,69.,68.,56.,56.,59.,60.,60.,62.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,64.,63.,63., &
      65.,65.,65.,65.,65.,66.,66.,65.,65.,66.,69.,68.,65.,60.,60.,59.,59.,59.,59.,59.,59.,58.,58.,59.,60.,60.,60.,63.,61.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,65.,54.,54.,54.,55.,56.,55.,62., &
      58.,59.,56.,55.,55.,52.,52.,53.,54.,53.,52.,53.,50.,50.,50.,50.,51.,51.,52.,48.,48.,47.,50.,48.,47.,46.,47.,48.,48.,48., &
      49.,50.,49.,49.,48.,49.,50.,47.,48.,48.,48.,48.,48.,48.,48.,48.,52.,48.,50.,50.,51.,50.,51.,52.,52.,52.,52.,52.,52.,52., &
      52.,52.,52.,73.,71.,71.,70.,70.,65.,64.,63.,81.,81.,81.,81.,81.,61.,62.,63.,63.,64.,65.,65.,65.,66.,66.,66.,66.,67.,67., &
      67.,66.,66.,67.,67.,67.,68.,68.,68.,69.,69.,70.,71.,65.,72.,73.,73.,74.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 47 week
      78.,78.,78.,78.,78.,78.,78.,80.,60.,61.,80.,62.,63.,63.,64.,62.,62.,63.,65.,64.,64.,65.,77.,77.,63.,64.,63.,63.,63.,64., &
      58.,58.,57.,57.,57.,58.,59.,58.,56.,58.,57.,64.,57.,56.,65.,66.,48.,66.,46.,46.,46.,45.,45.,45.,56.,57.,57.,58.,47.,47., &
      47.,48.,48.,48.,50.,52.,52.,52.,50.,49.,49.,49.,49.,45.,44.,40.,46.,46.,46.,34.,46.,34.,34.,34.,32.,34.,32.,32.,32.,32., &  
      33.,33.,35.,47.,50.,48.,48.,38.,45.,45.,46.,55.,53.,47.,46.,72.,51.,73.,73.,53.,72.,52.,51.,40.,42.,41.,43.,45.,49.,49., &
      72.,72.,44.,45.,43.,43.,43.,43.,43.,43.,70.,45.,44.,48.,48.,48.,48.,49.,53.,51.,52.,51.,51.,59.,67.,67.,67.,66.,66.,59., &
      66.,66.,59.,67.,59.,67.,68.,68.,69.,61.,56.,56.,59.,59.,60.,60.,69.,69.,67.,68.,68.,67.,67.,69.,64.,64.,64.,62.,63.,62., &
      65.,65.,65.,65.,65.,66.,66.,64.,65.,63.,63.,61.,60.,60.,59.,59.,59.,59.,59.,59.,59.,58.,57.,58.,58.,59.,60.,63.,61.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,65.,54.,52.,54.,55.,56.,55.,62., &
      52.,59.,56.,55.,55.,52.,52.,53.,54.,53.,52.,53.,50.,50.,50.,50.,48.,51.,52.,48.,48.,47.,45.,45.,45.,46.,46.,45.,45.,48., &
      49.,50.,49.,49.,48.,48.,48.,47.,47.,46.,47.,45.,47.,47.,45.,47.,46.,47.,48.,49.,49.,50.,51.,52.,51.,52.,52.,52.,52.,52., &
      52.,52.,52.,72.,71.,71.,70.,66.,65.,63.,63.,81.,81.,81.,81.,81.,61.,62.,63.,63.,64.,65.,65.,65.,65.,66.,66.,66.,66.,67., &
      67.,66.,66.,66.,66.,67.,67.,68.,68.,69.,69.,69.,70.,65.,72.,72.,73.,74.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 48 week
      78.,78.,78.,78.,78.,78.,78.,80.,60.,61.,61.,61.,61.,61.,64.,61.,62.,63.,65.,64.,64.,65.,65.,62.,62.,63.,62.,57.,57.,59., &
      58.,58.,57.,57.,57.,58.,56.,58.,56.,58.,57.,64.,57.,56.,65.,66.,48.,46.,46.,46.,46.,45.,45.,45.,56.,57.,57.,58.,47.,47., &
      47.,47.,46.,48.,50.,52.,52.,45.,45.,49.,49.,49.,49.,38.,44.,40.,46.,46.,46.,34.,34.,32.,31.,32.,32.,32.,32.,30.,32.,30., &    
      33.,33.,35.,38.,37.,36.,45.,34.,45.,43.,42.,45.,43.,44.,43.,72.,51.,73.,73.,53.,72.,52.,51.,40.,42.,41.,43.,45.,49.,49., &
      72.,72.,44.,45.,43.,43.,43.,43.,43.,43.,70.,45.,44.,48.,48.,48.,48.,49.,53.,51.,52.,51.,51.,50.,59.,67.,67.,66.,59.,59., &
      66.,59.,59.,67.,59.,67.,61.,61.,57.,61.,56.,56.,57.,59.,60.,60.,60.,69.,67.,60.,60.,67.,61.,61.,64.,62.,62.,62.,62.,62., &
      65.,65.,65.,65.,65.,66.,64.,64.,63.,63.,63.,60.,60.,59.,59.,59.,59.,59.,59.,59.,58.,58.,57.,58.,58.,59.,60.,60.,61.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,65.,54.,52.,54.,55.,56.,55.,62., &
      42.,52.,56.,55.,55.,52.,52.,53.,44.,43.,52.,47.,47.,50.,50.,50.,48.,47.,47.,46.,46.,47.,44.,45.,45.,44.,44.,45.,45.,48., &
      44.,44.,49.,49.,48.,48.,48.,47.,47.,46.,45.,45.,45.,45.,45.,45.,46.,47.,48.,49.,49.,50.,51.,52.,51.,51.,52.,52.,52.,52., &
      52.,52.,52.,70.,70.,70.,70.,66.,64.,63.,62.,62.,81.,61.,61.,81.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,66.,66.,66.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,68.,64.,69.,69.,70.,65.,72.,72.,73.,73.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &      ! 49 week
      78.,78.,78.,78.,78.,78.,78.,59.,60.,61.,60.,60.,59.,60.,60.,58.,59.,63.,65.,64.,64.,62.,61.,61.,61.,57.,57.,57.,57.,57., &
      57.,58.,56.,56.,57.,58.,56.,58.,54.,58.,57.,64.,57.,56.,65.,66.,48.,46.,46.,46.,46.,45.,45.,45.,56.,57.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,49.,49.,38.,44.,40.,46.,43.,43.,34.,33.,32.,31.,32.,32.,32.,32.,30.,32.,30., &
      33.,33.,35.,37.,37.,36.,45.,34.,45.,43.,42.,42.,42.,42.,43.,72.,51.,73.,52.,53.,72.,52.,40.,40.,42.,41.,43.,45.,49.,49., &
      42.,72.,40.,45.,42.,43.,43.,43.,43.,43.,70.,43.,44.,48.,48.,48.,48.,49.,53.,51.,50.,50.,50.,50.,57.,59.,67.,66.,59.,59., &
      66.,59.,59.,59.,59.,60.,56.,61.,57.,58.,56.,56.,56.,59.,59.,60.,60.,60.,67.,60.,60.,67.,61.,61.,61.,62.,62.,62.,62.,62., &
      64.,65.,65.,65.,64.,64.,64.,63.,63.,63.,62.,60.,60.,59.,59.,59.,59.,59.,58.,58.,58.,58.,57.,58.,58.,59.,60.,60.,60.,60., &
      60.,64.,66.,61.,62.,63.,63.,62.,62.,67.,62.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,65.,54.,52.,54.,55.,56.,41.,41., &
      42.,40.,51.,55.,55.,52.,52.,53.,44.,43.,52.,47.,47.,47.,50.,50.,48.,47.,44.,45.,46.,44.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,49.,49.,48.,48.,43.,47.,47.,46.,45.,44.,44.,45.,45.,45.,46.,44.,44.,44.,45.,45.,51.,52.,51.,51.,52.,52.,52.,52., &
      52.,52.,52.,52.,70.,70.,69.,66.,64.,63.,62.,62.,81.,61.,61.,81.,60.,61.,62.,63.,64.,65.,65.,65.,65.,65.,66.,66.,66.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,68.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &      ! 50 233k
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,59.,59.,60.,59.,57.,59.,63.,65.,64.,64.,62.,54.,53.,53.,52.,52.,52.,51.,54., &
      55.,54.,55.,56.,57.,58.,56.,47.,47.,46.,47.,46.,46.,46.,37.,49.,48.,45.,45.,45.,45.,44.,45.,45.,47.,47.,57.,58.,44.,43., &
      44.,46.,46.,48.,45.,52.,45.,45.,45.,49.,43.,49.,49.,38.,44.,40.,46.,33.,43.,34.,33.,32.,31.,30.,31.,32.,32.,30.,32.,30., &
      31.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,43.,72.,51.,73.,52.,53.,72.,52.,40.,40.,42.,40.,40.,40.,49.,49., &
      42.,40.,40.,45.,40.,39.,43.,43.,43.,43.,70.,43.,44.,48.,48.,48.,48.,49.,53.,51.,49.,49.,49.,49.,49.,57.,57.,59.,59.,59., &
      59.,59.,59.,59.,59.,59.,53.,57.,57.,58.,56.,56.,56.,58.,59.,60.,60.,60.,60.,60.,60.,60.,61.,61.,61.,61.,62.,62.,62.,62., &
      63.,63.,64.,64.,63.,63.,62.,62.,62.,62.,61.,60.,59.,59.,59.,59.,58.,58.,58.,58.,58.,58.,57.,58.,58.,59.,59.,60.,60.,60., &
      60.,60.,66.,61.,62.,63.,63.,62.,62.,67.,60.,61.,60.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,53.,55.,56.,41.,41., &
      42.,40.,51.,55.,55.,52.,52.,53.,44.,43.,52.,47.,47.,47.,50.,50.,48.,41.,42.,42.,43.,44.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,49.,49.,48.,44.,41.,41.,46.,46.,44.,44.,44.,45.,45.,45.,44.,44.,44.,44.,45.,45.,45.,47.,51.,51.,52.,52.,52.,52., &
      52.,52.,52.,52.,68.,70.,67.,66.,64.,63.,62.,62.,81.,61.,61.,60.,60.,61.,62.,63.,64.,64.,65.,65.,65.,65.,66.,66.,66.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 51 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,59.,59.,60.,59.,57.,59.,59.,53.,53.,53.,53.,53.,53.,53.,51.,52.,52.,51.,54., &
      50.,49.,49.,49.,45.,58.,56.,47.,46.,46.,47.,46.,46.,46.,37.,49.,44.,45.,45.,45.,44.,44.,45.,45.,47.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,49.,49.,38.,44.,40.,46.,33.,43.,34.,33.,32.,31.,30.,31.,31.,32.,30.,31.,30., &
      31.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,72.,51.,37.,39.,39.,72.,52.,40.,40.,42.,40.,40.,40.,49.,49., &
      42.,40.,40.,45.,39.,38.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,49.,53.,48.,48.,49.,49.,49.,47.,48.,57.,59.,59.,59., &
      59.,59.,59.,59.,59.,55.,53.,57.,53.,54.,56.,56.,56.,58.,59.,59.,60.,60.,60.,60.,60.,60.,60.,61.,61.,61.,62.,62.,62.,62., &
      62.,63.,63.,63.,62.,62.,62.,62.,62.,61.,60.,60.,59.,59.,59.,58.,58.,58.,58.,58.,58.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,66.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,60.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,41.,41., &
      42.,40.,51.,55.,55.,52.,52.,53.,43.,43.,52.,43.,47.,47.,50.,50.,48.,41.,42.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,45.,49.,48.,44.,41.,41.,46.,46.,44.,44.,44.,44.,43.,45.,43.,44.,44.,44.,45.,45.,45.,47.,46.,46.,45.,52.,52.,52., &
      52.,52.,52.,48.,48.,67.,66.,65.,64.,63.,62.,62.,61.,61.,61.,60.,60.,61.,61.,62.,64.,64.,65.,65.,65.,65.,65.,66.,66.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,75.,75.,74.,74.,76.,75.,76.,76.,76.,77., &     ! 52 week
      78.,78.,78.,78.,78.,78.,78.,59.,59.,61.,60.,59.,59.,60.,59.,57.,59.,59.,53.,53.,53.,53.,53.,53.,53.,51.,52.,49.,51.,54., &
      50.,47.,49.,46.,45.,58.,56.,46.,45.,45.,47.,46.,46.,40.,37.,49.,44.,44.,45.,45.,44.,44.,45.,45.,43.,47.,57.,58.,47.,47., &
      47.,47.,46.,48.,45.,52.,45.,45.,45.,49.,43.,37.,49.,38.,44.,40.,46.,33.,43.,34.,33.,32.,31.,30.,31.,31.,30.,30.,31.,28., &
      31.,31.,35.,37.,37.,36.,37.,34.,35.,36.,36.,42.,42.,42.,38.,51.,51.,37.,39.,39.,72.,52.,40.,40.,42.,40.,40.,38.,37.,49., &
      42.,40.,40.,45.,39.,38.,43.,43.,43.,43.,70.,43.,43.,48.,48.,48.,48.,49.,47.,48.,48.,49.,47.,47.,47.,47.,57.,57.,59.,59., &
      59.,59.,58.,59.,59.,55.,53.,57.,53.,54.,56.,56.,56.,57.,59.,59.,60.,60.,60.,60.,60.,60.,60.,61.,61.,61.,62.,62.,61.,62., &
      62.,62.,62.,62.,61.,61.,61.,61.,60.,60.,59.,59.,59.,58.,58.,57.,57.,58.,58.,58.,58.,57.,57.,57.,58.,59.,59.,60.,60.,60., &
      60.,60.,61.,61.,62.,63.,63.,62.,62.,67.,60.,61.,59.,59.,60.,59.,59.,59.,67.,57.,66.,65.,53.,53.,52.,51.,51.,56.,41.,41., &
      42.,40.,50.,46.,46.,47.,42.,42.,43.,43.,52.,43.,47.,41.,50.,42.,48.,41.,42.,42.,43.,43.,44.,45.,43.,44.,43.,44.,43.,43., &
      42.,42.,44.,49.,48.,44.,41.,41.,46.,45.,44.,44.,44.,44.,43.,45.,43.,44.,44.,44.,45.,45.,45.,47.,46.,46.,45.,52.,52.,52., &
      52.,51.,52.,48.,48.,48.,66.,65.,64.,63.,62.,62.,61.,61.,61.,60.,60.,60.,61.,62.,64.,64.,65.,65.,65.,65.,65.,66.,66.,66., &
      66.,66.,66.,66.,66.,67.,67.,68.,64.,64.,69.,69.,65.,65.,71.,72.,72.,73.,74.,74.,74.,75.,74.,74.,76.,75.,76.,76.,76.,77./  ! 53 week
      
!    72.3,59.5,59.5,65.3,66.0,66.2,65.0,65.0,65.0,65.0,65.0,65.0,65.0,45.8,45.8,49.1,52.5,56.7, &
!    52.5,52.5,56.7,56.7,56.7,50.0,50.0,50.0,40.0,40.0,40.0,45.8,48.3,58.0,65.0,67.0,67.0,69.3, & ! Jan
!
!    72.0,57.0,57.0,64.0,64.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,42.0,42.0,46.0,50.0,55.0, &
!    50.0,50.0,55.0,55.0,55.0,50.0,50.0,50.0,40.0,40.0,40.0,45.0,48.0,58.0,65.0,67.0,67.0,69.0, & ! Feb (tie point)
!
!    72.3,59.5,59.5,65.3,66.0,66.2,65.0,65.0,65.0,65.0,65.0,65.0,65.0,45.8,45.8,49.1,52.5,56.7, &
!    52.5,52.5,56.7,56.7,56.7,50.0,50.0,50.0,40.0,40.0,40.0,45.8,48.3,58.0,65.0,67.0,67.0,69.3, & ! Mar
!
!    72.7,62.0,62.0,66.6,68.0,67.3,65.0,65.0,65.0,65.0,65.0,65.0,65.0,49.6,49.6,52.3,55.0,58.3, &
!    55.0,55.0,58.3,58.3,58.3,50.0,50.0,50.0,40.0,40.0,40.0,46.7,48.7,58.0,65.0,67.0,67.0,69.7, & ! Apr
!
!    73.0,64.5,64.5,68.0,70.0,68.5,65.0,65.0,65.0,65.0,65.0,65.0,65.0,53.5,53.5,55.5,57.5,60.0, &
!    57.5,57.5,60.0,60.0,60.0,50.0,50.0,50.0,50.0,50.0,50.0,47.5,49.0,58.0,65.0,67.0,67.0,70.0, & ! May
!
!    73.3,67.0,67.0,69.3,72.0,69.7,65.0,65.0,65.0,65.0,65.0,65.0,65.0,53.5,57.3,58.6,60.0,61.7, &
!    60.0,60.0,61.7,61.7,61.7,50.0,50.0,50.0,50.0,50.0,50.0,48.3,49.3,58.0,65.0,67.0,67.0,70.3, & ! Jun
!
!    73.7,69.5,69.5,70.6,74.0,70.8,65.0,65.0,65.0,65.0,65.0,65.0,65.0,53.5,61.1,61.8,62.5,63.3, &
!    62.5,62.5,63.3,63.3,63.3,50.0,50.0,50.0,50.0,50.0,50.0,49.2,49.7,58.0,65.0,67.0,67.0,70.7, & ! Jul
!
!    74.0,72.0,72.0,72.0,76.0,72.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0,65.0, &
!    65.0,65.0,65.0,65.0,65.0,50.0,50.0,50.0,50.0,50.0,50.0,50.0,50.0,58.0,65.0,67.0,67.0,71.0, & ! Aug (tie point)
!
!    73.7,69.5,69.5,70.6,74.0,70.8,65.0,65.0,65.0,65.0,65.0,65.0,65.0,53.5,61.1,61.8,62.5,63.3, &
!    62.5,62.5,63.3,63.3,63.3,50.0,50.0,50.0,50.0,50.0,50.0,49.2,49.7,58.0,65.0,67.0,67.0,70.7, & ! Sep
!
!    73.3,67.0,67.0,69.3,72.0,69.7,65.0,65.0,65.0,65.0,65.0,65.0,65.0,53.5,57.3,58.6,60.0,61.7, &
!    60.0,60.0,61.7,61.7,61.7,50.0,50.0,50.0,50.0,50.0,50.0,48.3,49.3,58.0,65.0,67.0,67.0,70.3, & ! Oct
!
!    73.0,64.5,64.5,68.0,70.0,68.5,65.0,65.0,65.0,65.0,65.0,65.0,65.0,53.5,53.5,55.5,57.5,60.0, &
!    57.5,57.5,60.0,60.0,60.0,50.0,50.0,50.0,40.0,40.0,40.0,47.5,49.0,58.0,65.0,67.0,67.0,70.0, & ! Nov
!
!    72.7,62.0,62.0,66.6,68.0,67.3,65.0,65.0,65.0,65.0,65.0,65.0,65.0,49.6,49.6,52.3,55.0,58.3, &
!    55.0,55.0,58.3,58.3,58.3,50.0,50.0,50.0,40.0,40.0,40.0,46.7,48.7,58.0,65.0,67.0,67.0,69.7/   ! Dec
!
    !---Southern Hem. values (note: these are absolute values of latitude)
    data threshLatSH / &

    62.5,62.8,63.3,60.8,60.0,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,62.3,62.3, &
    62.3,62.3,62.3,62.3,62.3,62.3,63.3,63.3,61.7,60.0,58.8,58.8,57.7,57.5,58.6,58.6,62.5,62.5, & ! Jan

    65.0,65.0,65.0,62.0,61.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,60.0,64.0,64.0, &
    64.0,64.0,64.0,64.0,64.0,64.0,64.0,64.0,62.0,60.0,59.0,59.0,58.0,58.0,60.0,62.0,65.0,65.0, & ! Feb (tie point)

    62.5,62.8,63.3,60.8,60.0,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,59.2,62.3,62.3, &
    62.3,62.3,62.3,62.3,62.3,62.3,63.3,63.3,61.7,60.0,58.8,58.8,57.7,57.5,58.6,58.6,62.5,62.5, & ! Mar

    60.0,60.6,61.7,59.7,59.0,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,60.7,60.7, &
    60.7,60.7,60.7,60.7,60.7,60.7,62.7,62.7,61.3,60.0,58.7,58.7,57.3,57.0,57.3,57.3,60.0,60.0, & ! Apr

    57.5,58.5,60.0,58.5,58.0,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,59.0,59.0, &
    59.0,59.0,59.0,59.0,59.0,59.0,62.0,62.0,61.0,60.0,58.5,58.5,57.0,56.5,56.0,56.0,57.5,57.5, & ! May

    55.0,56.3,58.3,57.3,57.0,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,57.3,57.3, &
    57.3,57.3,57.3,57.3,57.3,57.3,61.3,61.3,56.1,60.0,58.3,58.3,56.7,56.0,54.6,54.0,55.0,55.0, & ! Jun

    52.5,54.1,56.7,56.2,56.0,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.7,55.7, &
    55.7,55.7,55.7,55.7,55.7,55.7,57.5,57.5,55.6,60.0,58.2,58.2,56.3,55.5,53.3,52.0,52.5,52.5, & ! Jul

    50.0,52.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,55.0,54.0,54.0, &
    54.0,54.0,54.0,54.0,54.0,54.0,60.0,60.0,60.0,60.0,58.0,58.0,56.0,55.0,52.0,50.0,50.0,50.0, & ! Aug (tie point)

!    52.5,54.1,56.7,56.2,56.0,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.7,55.7, &
!    55.7,55.7,55.7,55.7,55.7,55.7,57.5,57.5,55.6,60.0,58.2,58.2,56.3,55.5,53.3,52.0,52.5,52.5, & ! Sep
    50.5,52.0,55.0,56.2,56.0,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.8,55.7,55.7, &
    55.7,55.7,55.7,55.7,55.7,55.7,57.5,57.5,55.6,60.0,58.2,58.2,56.3,55.5,53.3,52.0,50.0,50.0, & ! Sep

!    55.0,56.3,58.3,57.3,57.0,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,56.7,57.3,57.3, &
!    57.3,57.3,57.3,57.3,57.3,57.3,61.3,61.3,56.1,60.0,58.3,58.3,56.7,56.0,54.6,54.0,55.0,55.0, & ! Oct
    53.0,54.0,56.0,56.0,56.0,56.0,56.0,56.0,56.0,56.7,56.7,56.7,56.7,56.7,56.7,56.7,57.3,57.3, &
    57.3,57.3,57.3,57.3,57.3,57.3,61.3,61.3,56.1,60.0,58.3,58.3,56.7,56.0,54.6,53.0,53.0,53.0, & ! Oct

!    57.5,58.5,60.0,58.5,58.0,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,57.5,59.0,59.0, &
!    59.0,59.0,59.0,59.0,59.0,59.0,62.0,62.0,61.0,60.0,58.5,58.5,57.0,56.5,56.0,56.0,57.5,57.5, & ! Nov
    55.0,54.0,56.0,56.0,56.0,56.0,56.0,56.0,56.0,57.5,57.5,57.5,57.5,57.5,57.5,57.5,59.0,59.0, &
    59.0,59.0,59.0,59.0,59.0,59.0,62.0,62.0,61.0,60.0,58.5,58.5,57.0,56.5,56.0,55.0,55.0,55.0, & ! Nov

!    60.0,60.6,61.7,59.7,59.0,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,60.7,60.7, &
!    60.7,60.7,60.7,60.7,60.7,60.7,62.7,62.7,61.3,60.0,58.7,58.7,57.3,57.0,57.3,57.3,60.0,60.0/   ! Dec
    57.0,54.0,56.0,56.0,58.0,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,58.3,60.7,60.7, &
    60.7,60.7,60.7,60.7,60.7,60.7,62.7,62.7,61.3,60.0,58.7,58.7,57.3,57.0,57.0,57.0,57.0,57.0/   ! Dec

    DO I=1,nlonBins1
    DO J=1,20
    threshLatNH(I,J)=threshLatNH_01(I,J)
    threshLatNH(I,J+20)=threshLatNH_02(I,J)
    ENDDO
    ENDDO

    DO I=1,nlonBins1
    DO J=1,13
    threshLatNH(I,J+40)=threshLatNH_03(I,J)
    ENDDO
    ENDDO

    threshLatNH = reshape(threshLatNH,(/nlonBins1,nTimeBins1/))
    threshLatSH = reshape(threshLatSH,(/nlonBins,nTimeBins/))
    surface_type_orig=surface_type


    !---Only apply if landindex=0 (ocean flag from database)
    IF (landindex == NOCEAN) THEN
    !---Convert year and julian day to calendar month and day 
    !---Month needed to select proper element from climatology
       call day_month(Year,month,day,Julday)
        IWEEK=FLOOR(FLOAT(Julday)/7.)+1
       
    !---Convert longitude to index (account for lons -180 to +180)
    !---Index used to select proper element from climatology
     IF (xlat .ge. 0) THEN
       if(xlon .lt. 0.)then
          iLonBin1=int((xlon+360.)/widthLonBin1)+1
       else
          iLonBin1=int(xlon/widthLonBin1)+1
       endif
       if(ilonBin1 .gt. nLonBins1)ilonBin1=nlonBins1
       if(ilonBin1 .lt. 1)ilonBin1=1
     ENDIF

     IF (xlat .lt. 0) THEN
       if(xlon .lt. 0.)then
          iLonBin=int((xlon+360.)/widthLonBin)+1
       else
          iLonBin=int(xlon/widthLonBin)+1
       endif
       if(ilonBin .gt. nLonBins)ilonBin=nlonBins
       if(ilonBin .lt. 1)ilonBin=1
     ENDIF

    !---Check hemisphere, and use appropriate climatology
       if(xlat .ge. 0.)then
!          if(abs(xlat) .lt. threshLatNH(ilonBin,month)) surface_type=OC_TYP
          if(abs(xlat) .lt. threshLatNH(ilonBin1,iweek)) surface_type=OC_TYP
       else
          if(abs(xlat) .lt. threshLatSH(ilonBin,month)) surface_type=OC_TYP
       endif
    ELSE
       RETURN
    ENDIF

! debug
!    if(xlat .ge. 40.)then
!    if(xlat .ge. 75. .and. xlat .le. 80. .and. xlon .ge. -80. .and. xlon .le. -70.)then
!       write(*,*)'applySeaIceClimo: ',year,julday,month,day,xlat,xlon,ilonBin,threshLatNH(ilonBin,month),surface_type_orig,surface_type
!       print *,'applySeaIceClimo: ',&
!            year,julday,month,day,xlat,xlon,ilonBin,threshLatNH(ilonBin,month),surface_type_orig,surface_type
!    endif

    RETURN

  END SUBROUTINE applySeaIceClimo


  SUBROUTINE FY3RI_MWRI_ALG(nchan,freq,lat,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       AQUA_MWRI_ALG
    !
    ! PURPOSE:
    !       placeholder for AMSRE preclassifier. For now simply returns the topography.
    !
    !
    ! INPUT VARIABLES:
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : OC_TYP (OPEN OCEAN)
    !                   : SEAICE_TYP (SEA ICE)
    !                   : Desert_TYP (DESERT)
    !                   : LD_TYP (SNOW-FREE AND NON-DESERT LAND)
    !                   : SNOW_TYP (SNOW)
    !
    !
    !
    ! Sid Boukabara, NOAA/NESDIS
    !
    !-------------------------------------------------------------------------------------------------------
    !...using the 10v channel of Fy3-mwri to justify the seaice. if tb10v>230K, the surface type is seaice
    !...by Tiger.Yang
    !...02/25/2009
    !------------------------------------------------------------------------------------------------------ 
    !  This program is free software; you can redistribute it and/or modify it under the terms of the GNU
    !  General Public License as published by the Free Software Foundation; either version 2 of the License,
    !  or (at your option) any later version.
    !
    !  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
    !  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
    !  License for more details.
    !
    !  You should have received a copy of the GNU General Public License along with this program; if not, write
    !  to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    !
    !--------------------------------------------------------------------------------
    INTEGER,  PARAMETER         :: NOCEAN = 0
    INTEGER                     :: surface_type
    INTEGER, INTENT(IN)         :: nchan,landindex
    REAL                        :: lat,TskPreclass
    REAL, INTENT(IN), DIMENSION(nchan)  :: ta,freq
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF

    IF (surface_type == OC_TYP) THEN
       IF (ta(1) > 230.) surface_type = SEAICE_TYP
       
    ENDIF
    
  END SUBROUTINE FY3RI_MWRI_ALG

  SUBROUTINE NPP_ATMS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       NPP_ATMS_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land, snow)
    !
    ! from land index and NPP ATMS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : NPP ATMS antenna temperatures at five window channels
    !
    !   taw(1)       : antenna temperature at 23.8 GHz
    !   taw(2)       : antenna temperature at 31.4 GHz
    !   taw(3)       : antenna temperature at 50.3 GHz
    !   taw(4)       : antenna temperature at 88.2 GHz
    !   taw(5)       : antenna temperature at 165.5 GHz
    !
    !   freqw        : NPP ATMS frequencies at five window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       Written by:     Kevin Garrett (slightly modified from N18_AMSU_MHS_ALG)
    !                       2009-06-16
    !
    !
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER,  PARAMETER      :: NOCEAN = 0,ncoe = 7,nchanw = 5
    REAL,     PARAMETER      :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH2 = 40.0,LATTH3 = 50.0
    INTEGER                  :: nchan,wchan_index,landindex, surface_type,k,ich,jch,nd
    INTEGER                  :: Year,Julday
    REAL                     :: lat,lon,TA92_SICE,TA157_SICE,TA92_SNOW,TA157_SNOW,TA_ICE(2),TA_SNOW(2),SI,TskPreclass
    REAL,     DIMENSION(:)      :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(2,ncoe) :: coe_oc,coe_land

    !---Fitting coefficients to predict ta92 over open ocean
    data (coe_oc(1,k),k=1,ncoe)/6.76185e+002,  2.55301e+000,  2.44504e-001, -6.88612e+000,   &
         -5.01409e-003, -1.41372e-003,  1.59245e-002/
    !---Fitting coefficients to predict ta157 over open ocean
    data (coe_oc(2,k),k=1,ncoe)/ 5.14546e+002,  6.06543e+000, -6.09327e+000, -2.81614e+000,   &
         -1.35415e-002,  1.29683e-002 , 7.69443e-003/
    !---Fitting coefficients to predict ta92 over snow-free land
    data (coe_land(1,k),k=1,ncoe)/-3.09009e+002,  1.74746e+000, -2.01890e+000,  3.43417e+000, &
         -2.85680e-003,  3.53140e-003, -4.39255e-003/
    !---Fitting coefficients to predict ta157 over snow-free land
    data (coe_land(2,k),k=1,ncoe)/-1.01014e+001,  3.97994e+000, -4.11268e+000,  1.26658e+000, &
         -9.52526e-003,  8.75558e-003,  4.77981e-004/
    !---Central Frequencies ct five window channel
    data freqw/23.8, 31.4, 50.3, 88.2, 165.5/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       wchan_index = 1
       DO jch = 1, NCHAN
          IF(abs(freqw(ich)-freq(jch)) <= 0.5) THEN
             wchan_index = jch
             EXIT
          ENDIF
       ENDDO
       taw(ich) = ta(wchan_index)
       IF (ich == 1 .and. wchan_index /= 1)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 2 .and. wchan_index /= 2)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 3 .and. wchan_index /= 3)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 4 .and. wchan_index /= 16) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 5 .and. wchan_index /= 17) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_ICE(nd)= coe_oc(nd,1)
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SICE  = TA_ICE(1)
    TA157_SICE = TA_ICE(2)
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_SNOW(nd)= coe_land(nd,1)
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SNOW  = TA_SNOW(1)
    TA157_SNOW = TA_SNOW(2)
    !---COMPUTE SI = TA23 - TA92
    SI = taw(1)-taw(4)
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (TA92_SICE - taw(4) >= 10.0 .and. abs(lat) >= LATTH2)  surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH2 .and. SI >= SIHIGH)                surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. SI >= SILOW)                 surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. taw(1) >= 235.0)             surface_type = SEAICE_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                  surface_type = OC_TYP 
       if (TskPreclass <= 265. .and. TskPreclass >=0.)           surface_type = SEAICE_TYP 
       !---Get help from the latitude
!       if (abs(lat) <= 50. )                                     surface_type = OC_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
       surface_type = LD_TYP
       if(TA92_SNOW-taw(4)>=10.0.and.taw(1).le.260.0 .and. abs(lat) >= LATTH1) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH2 .and. SI >= SIHIGH .and. taw(1).le.260.0)         surface_type = SNOW_TYP
       if(abs(lat) >= LATTH3 .and. SI >= SILOW .and. taw(1).le.260.0)          surface_type = SNOW_TYP
       if(taw(1) <= 210.0 .and. taw(3) <= 225.0 .and. abs(lat) >= LATTH3)      surface_type = SNOW_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                                surface_type = LD_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.)                         surface_type = SNOW_TYP 
    endif
  END SUBROUTINE NPP_ATMS_ALG

  SUBROUTINE N20_ATMS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       N20_ATMS_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land, snow)
    !
    ! from land index and JPSS/N20 ATMS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : N20 ATMS antenna temperatures at five window channels
    !
    !   taw(1)       : antenna temperature at 23.8 GHz
    !   taw(2)       : antenna temperature at 31.4 GHz
    !   taw(3)       : antenna temperature at 50.3 GHz
    !   taw(4)       : antenna temperature at 88.2 GHz
    !   taw(5)       : antenna temperature at 165.5 GHz
    !
    !   freqw        : N20 ATMS frequencies at five window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !          Aded by:     Junye Chen (copied from NPP_ATMS_ALG)
    !                       2017-03-28
    !
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER,  PARAMETER      :: NOCEAN = 0,ncoe = 7,nchanw = 5
    REAL,     PARAMETER      :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH2 = 40.0,LATTH3 = 50.0
    INTEGER                  :: nchan,wchan_index,landindex, surface_type,k,ich,jch,nd
    INTEGER                  :: Year,Julday
    REAL                     :: lat,lon,TA92_SICE,TA157_SICE,TA92_SNOW,TA157_SNOW,TA_ICE(2),TA_SNOW(2),SI,TskPreclass
    REAL,     DIMENSION(:)      :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(2,ncoe) :: coe_oc,coe_land

    !---Fitting coefficients to predict ta92 over open ocean
    data (coe_oc(1,k),k=1,ncoe)/6.76185e+002,  2.55301e+000,  2.44504e-001, -6.88612e+000,   &
         -5.01409e-003, -1.41372e-003,  1.59245e-002/
    !---Fitting coefficients to predict ta157 over open ocean
    data (coe_oc(2,k),k=1,ncoe)/ 5.14546e+002,  6.06543e+000, -6.09327e+000, -2.81614e+000,   &
         -1.35415e-002,  1.29683e-002 , 7.69443e-003/
    !---Fitting coefficients to predict ta92 over snow-free land
    data (coe_land(1,k),k=1,ncoe)/-3.09009e+002,  1.74746e+000, -2.01890e+000,  3.43417e+000, &
         -2.85680e-003,  3.53140e-003, -4.39255e-003/
    !---Fitting coefficients to predict ta157 over snow-free land
    data (coe_land(2,k),k=1,ncoe)/-1.01014e+001,  3.97994e+000, -4.11268e+000,  1.26658e+000, &
         -9.52526e-003,  8.75558e-003,  4.77981e-004/
    !---Central Frequencies ct five window channel
    data freqw/23.8, 31.4, 50.3, 88.2, 165.5/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       wchan_index = 1
       DO jch = 1, NCHAN
          IF(abs(freqw(ich)-freq(jch)) <= 0.5) THEN
             wchan_index = jch
             EXIT
          ENDIF
       ENDDO
       taw(ich) = ta(wchan_index)
       IF (ich == 1 .and. wchan_index /= 1)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 2 .and. wchan_index /= 2)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 3 .and. wchan_index /= 3)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 4 .and. wchan_index /= 16) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 5 .and. wchan_index /= 17) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_ICE(nd)= coe_oc(nd,1)
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SICE  = TA_ICE(1)
    TA157_SICE = TA_ICE(2)
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_SNOW(nd)= coe_land(nd,1)
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SNOW  = TA_SNOW(1)
    TA157_SNOW = TA_SNOW(2)
    !---COMPUTE SI = TA23 - TA92
    SI = taw(1)-taw(4)
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (TA92_SICE - taw(4) >= 10.0 .and. abs(lat) >= LATTH2)  surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH2 .and. SI >= SIHIGH)                surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. SI >= SILOW)                 surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. taw(1) >= 235.0)             surface_type = SEAICE_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                  surface_type = OC_TYP 
       if (TskPreclass <= 265. .and. TskPreclass >=0.)           surface_type = SEAICE_TYP 
       !---Get help from the latitude
!       if (abs(lat) <= 50. )                                     surface_type = OC_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
       surface_type = LD_TYP
       if(TA92_SNOW-taw(4)>=10.0.and.taw(1).le.260.0 .and. abs(lat) >= LATTH1) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH2 .and. SI >= SIHIGH .and. taw(1).le.260.0)         surface_type = SNOW_TYP
       if(abs(lat) >= LATTH3 .and. SI >= SILOW .and. taw(1).le.260.0)          surface_type = SNOW_TYP
       if(taw(1) <= 210.0 .and. taw(3) <= 225.0 .and. abs(lat) >= LATTH3)      surface_type = SNOW_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                                surface_type = LD_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.)                         surface_type = SNOW_TYP 
    endif
  END SUBROUTINE N20_ATMS_ALG

  SUBROUTINE N21_ATMS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       N21_ATMS_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land, snow)
    !
    ! from land index and JPSS2/N21 ATMS antenna/brightness temperatures at window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : N21 ATMS antenna temperatures at five window channels
    !
    !   taw(1)       : antenna temperature at 23.8 GHz
    !   taw(2)       : antenna temperature at 31.4 GHz
    !   taw(3)       : antenna temperature at 50.3 GHz
    !   taw(4)       : antenna temperature at 88.2 GHz
    !   taw(5)       : antenna temperature at 165.5 GHz
    !
    !   freqw        : N21 ATMS frequencies at five window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !          Aded by:     Junye Chen (copied from NPP_ATMS_ALG)
    !                       2017-03-28
    !
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER,  PARAMETER      :: NOCEAN = 0,ncoe = 7,nchanw = 5
    REAL,     PARAMETER      :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH2 = 40.0,LATTH3 = 50.0
    INTEGER                  :: nchan,wchan_index,landindex, surface_type,k,ich,jch,nd
    INTEGER                  :: Year,Julday
    REAL                     :: lat,lon,TA92_SICE,TA157_SICE,TA92_SNOW,TA157_SNOW,TA_ICE(2),TA_SNOW(2),SI,TskPreclass
    REAL,     DIMENSION(:)      :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(2,ncoe) :: coe_oc,coe_land

    !---Fitting coefficients to predict ta92 over open ocean
    data (coe_oc(1,k),k=1,ncoe)/6.76185e+002,  2.55301e+000,  2.44504e-001, -6.88612e+000,   &
         -5.01409e-003, -1.41372e-003,  1.59245e-002/
    !---Fitting coefficients to predict ta157 over open ocean
    data (coe_oc(2,k),k=1,ncoe)/ 5.14546e+002,  6.06543e+000, -6.09327e+000, -2.81614e+000,   &
         -1.35415e-002,  1.29683e-002 , 7.69443e-003/
    !---Fitting coefficients to predict ta92 over snow-free land
    data (coe_land(1,k),k=1,ncoe)/-3.09009e+002,  1.74746e+000, -2.01890e+000,  3.43417e+000, &
         -2.85680e-003,  3.53140e-003, -4.39255e-003/
    !---Fitting coefficients to predict ta157 over snow-free land
    data (coe_land(2,k),k=1,ncoe)/-1.01014e+001,  3.97994e+000, -4.11268e+000,  1.26658e+000, &
         -9.52526e-003,  8.75558e-003,  4.77981e-004/
    !---Central Frequencies ct five window channel
    data freqw/23.8, 31.4, 50.3, 88.2, 165.5/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       wchan_index = 1
       DO jch = 1, NCHAN
          IF(abs(freqw(ich)-freq(jch)) <= 0.5) THEN
             wchan_index = jch
             EXIT
          ENDIF
       ENDDO
       taw(ich) = ta(wchan_index)
       IF (ich == 1 .and. wchan_index /= 1)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 2 .and. wchan_index /= 2)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 3 .and. wchan_index /= 3)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 4 .and. wchan_index /= 16) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 5 .and. wchan_index /= 17) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_ICE(nd)= coe_oc(nd,1)
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SICE  = TA_ICE(1)
    TA157_SICE = TA_ICE(2)
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting coeffs.
    DO nd =1, 2
       TA_SNOW(nd)= coe_land(nd,1)
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SNOW  = TA_SNOW(1)
    TA157_SNOW = TA_SNOW(2)
    !---COMPUTE SI = TA23 - TA92
    SI = taw(1)-taw(4)
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (TA92_SICE - taw(4) >= 10.0 .and. abs(lat) >= LATTH2)  surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH2 .and. SI >= SIHIGH)                surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. SI >= SILOW)                 surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. taw(1) >= 235.0)             surface_type = SEAICE_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                  surface_type = OC_TYP 
       if (TskPreclass <= 265. .and. TskPreclass >=0.)           surface_type = SEAICE_TYP 
       !---Get help from the latitude
!       if (abs(lat) <= 50. )                                     surface_type = OC_TYP
       !---Get help from the latitude and longitude 
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
       surface_type = LD_TYP
       if(TA92_SNOW-taw(4)>=10.0.and.taw(1).le.260.0 .and. abs(lat) >= LATTH1) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH2 .and. SI >= SIHIGH .and. taw(1).le.260.0)         surface_type = SNOW_TYP
       if(abs(lat) >= LATTH3 .and. SI >= SILOW .and. taw(1).le.260.0)          surface_type = SNOW_TYP
       if(taw(1) <= 210.0 .and. taw(3) <= 225.0 .and. abs(lat) >= LATTH3)      surface_type = SNOW_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                                surface_type = LD_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.)                         surface_type = SNOW_TYP 
    endif
  END SUBROUTINE N21_ATMS_ALG

  SUBROUTINE metopSGA1_MWS_ALG(Year,Julday,nchan,freq,lat,lon,landindex,ta,surface_type,TskPreclass)
    !--------------------------------------------------------------------------------
    !M+
    ! NAME:
    !       N21_ATMS_ALG
    !
    ! PURPOSE:
    !       Distinguish four surface types (open ocean, sea ice, snow-free land,
    !       snow)
    !
    ! from land index and JPSS2/N21 ATMS antenna/brightness temperatures at
    ! window channels.
    !
    !
    ! INPUT VARIABLES:
    !
    !   year        : Year
    !
    !   Julday      : Julian day
    !
    !   nchan       : CHANNEL NUMBER FOR A CERTAIN SENSOR
    !
    !   freq(nchan) : CENTRAL FREQUENCY LIST CORRESPONDING TO EACH CHANNEL AT A
    !   CERTAIN SENSOR
    !
    !   lat         : LATITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   lon         : LONGITUDE OF OBSERVED PIXEL IN DEGREE
    !
    !   landindex   : LAND/OCEAN INDEX (0: ocean, 1: land)
    !
    !   ta          : antenna temperatures at all channels
    !
    ! INTERNAL VARIABLES:
    !
    !   taw          : N21 ATMS antenna temperatures at five window channels
    !
    !   taw(1)       : antenna temperature at 23.8 GHz
    !   taw(2)       : antenna temperature at 31.4 GHz
    !   taw(3)       : antenna temperature at 50.3 GHz
    !   taw(4)       : antenna temperature at 88.2 GHz
    !   taw(5)       : antenna temperature at 165.5 GHz
    !
    !   freqw        : N21 ATMS frequencies at five window channels
    !
    ! OUTPUT VARIABLES:
    !
    !   surface_type    : 0 (OPEN OCEAN)
    !                   : 1 (SEA ICE)
    !                   : 2 (SNOW-FREE LAND)
    !                   : 3 (SNOW)
    !
    !
    !
    ! LANGUAGE:
    !
    !       Fortran-95
    !
    ! INCLUDE FILES:
    !       None.
    !
    ! EXTERNALS:
    !       None.
    !
    ! COMMON BLOCKS:
    !       None.
    !
    ! FILES ACCESSED:
    !       None.
    !
    ! CREATION HISTORY:
    !       None.
    !
    ! CREATION HISTORY:
    !          Aded by:     Junye Chen (copied from NPP_ATMS_ALG)
    !                       2017-03-28
    !
    !M-
    !--------------------------------------------------------------------------------
    IMPLICIT NONE
    INTEGER,  PARAMETER      :: NOCEAN = 0,ncoe = 7,nchanw = 5
    REAL,     PARAMETER      :: SILOW = 5.0, SIHIGH = 10.0, LATTH1 = 30.0,LATTH2 = 40.0,LATTH3 = 50.0
    INTEGER                  :: nchan,wchan_index,landindex, surface_type,k,ich,jch,nd
    INTEGER                  :: Year,Julday
    REAL                     :: lat,lon,TA92_SICE,TA157_SICE,TA92_SNOW,TA157_SNOW,TA_ICE(2),TA_SNOW(2),SI,TskPreclass
    REAL,     DIMENSION(:)      :: ta,freq
    REAL,     DIMENSION(nchanw) :: taw,freqw
    REAL,     DIMENSION(2,ncoe) :: coe_oc,coe_land

    !---Fitting coefficients to predict ta92 over open ocean
    data (coe_oc(1,k),k=1,ncoe)/6.76185e+002,  2.55301e+000,  2.44504e-001, -6.88612e+000,   &
         -5.01409e-003, -1.41372e-003,  1.59245e-002/
    !---Fitting coefficients to predict ta157 over open ocean
    data (coe_oc(2,k),k=1,ncoe)/ 5.14546e+002,  6.06543e+000, -6.09327e+000, -2.81614e+000,   &
         -1.35415e-002,  1.29683e-002 , 7.69443e-003/
    !---Fitting coefficients to predict ta92 over snow-free land
    data (coe_land(1,k),k=1,ncoe)/-3.09009e+002,  1.74746e+000, -2.01890e+000, 3.43417e+000, &
         -2.85680e-003,  3.53140e-003, -4.39255e-003/
    !---Fitting coefficients to predict ta157 over snow-free land
    data (coe_land(2,k),k=1,ncoe)/-1.01014e+001,  3.97994e+000, -4.11268e+000, 1.26658e+000, &
         -9.52526e-003,  8.75558e-003,  4.77981e-004/
    !---Central Frequencies ct five window channel
    data freqw/23.8, 31.4, 50.3, 89.0, 165.5/

    ! SELECT TA AT WINDOWN CHANNELS
    DO ich = 1, nchanw
       wchan_index = 1
       DO jch = 1, NCHAN
          IF(abs(freqw(ich)-freq(jch)) <= 0.5) THEN
             wchan_index = jch
             EXIT
          ENDIF
       ENDDO
       taw(ich) = ta(wchan_index)
       IF (ich == 1 .and. wchan_index /= 1)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 2 .and. wchan_index /= 2)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 3 .and. wchan_index /= 3)  PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 4 .and. wchan_index /= 17) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
       IF (ich == 5 .and. wchan_index /= 18) PRINT *,'CHANNEL ORDER IS INCONSISTENT TO OFFICIAL WAY'
    ENDDO

    ! START TO IDENTIFY SURFACE TYPE
    IF (landindex == NOCEAN) THEN
       surface_type = OC_TYP
    ELSE
       surface_type = LD_TYP
    ENDIF
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting
    !coeffs.
    DO nd =1, 2
       TA_ICE(nd)= coe_oc(nd,1)
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_ICE(nd) = TA_ICE(nd) + coe_oc(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SICE  = TA_ICE(1)
    TA157_SICE = TA_ICE(2)
    !---Predict SEA ICE TA92 and TA157 from TA23 ~ TA50 using open ocean fitting
    !coeffs.
    DO nd =1, 2
       TA_SNOW(nd)= coe_land(nd,1)
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+1)*taw(ich)
       ENDDO
       DO ich=1,3
          TA_SNOW(nd) = TA_SNOW(nd) + coe_land(nd,ich+4)*taw(ich)*taw(ich)
       ENDDO
    ENDDO
    TA92_SNOW  = TA_SNOW(1)
    TA157_SNOW = TA_SNOW(2)
    !---COMPUTE SI = TA23 - TA92
    SI = taw(1)-taw(4)
    !---Predict surface types
    if (landindex == NOCEAN) then    ! over ocean conditions
       surface_type = OC_TYP
       if (TA92_SICE - taw(4) >= 10.0 .and. abs(lat) >= LATTH2)  surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH2 .and. SI >= SIHIGH)                surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. SI >= SILOW)                 surface_type = SEAICE_TYP
       if (abs(lat) >= LATTH3 .and. taw(1) >= 235.0)             surface_type = SEAICE_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.)                                  surface_type = OC_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.)           surface_type = SEAICE_TYP
       !---Get help from the latitude
!       if (abs(lat) <= 50. )                                     surface_type =
!       OC_TYP
       !---Get help from the latitude and longitude
       call applySeaIceClimo(Year, Julday, lat, lon, landindex, surface_type)
    else                             ! over land conditions
       surface_type = LD_TYP
       if(TA92_SNOW-taw(4)>=10.0.and.taw(1).le.260.0 .and. abs(lat) >= LATTH1) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH2 .and. SI >= SIHIGH .and. taw(1).le.260.0) surface_type = SNOW_TYP
       if(abs(lat) >= LATTH3 .and. SI >= SILOW .and. taw(1).le.260.0) surface_type = SNOW_TYP
       if(taw(1) <= 210.0 .and. taw(3) <= 225.0 .and. abs(lat) >= LATTH3) surface_type = SNOW_TYP
       !---Get help from the tskin temperature
       if (TskPreclass >= 280.) surface_type = LD_TYP
       if (TskPreclass <= 265. .and. TskPreclass >=0.) surface_type = SNOW_TYP
    endif
  END SUBROUTINE metopSGA1_MWS_ALG


END MODULE Preclassif
