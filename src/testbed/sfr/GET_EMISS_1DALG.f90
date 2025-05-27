!-----------------------------------------------------------------------------
! NAME:
!    get_emiss_1dalg.f90 
! PURPOSE:
!   Retrieve surface emissivity and iwp from observations at window channels
! CONTAINS:
! EXTERNALS:
!       None.
! INCLUDE FILES:
!       None.
! COMMON BLOCKS:
!       None.
! CREATION HISTORY:
!       Written by:     Banghua Yan and Fuzhong Weng, NOAA/NESDIS/STAR 
!-----------------------------------------------------------------------------

MODULE GET_EMISS_1DALG
!
!---Purpose: Module containing routines for retrieving emissivity over cold 
!surfaces from AMSU measurements at window channels
!
   IMPLICIT NONE
!=============================================================================
   INTEGER, PARAMETER, PUBLIC                         :: nch   = 6
   INTEGER, PARAMETER, PUBLIC                         :: nvar  = 11

   INTEGER, PARAMETER, PUBLIC                         :: mtc   = 46
   INTEGER, PARAMETER, PUBLIC                         :: mrwp  = 51
   INTEGER, PARAMETER, PUBLIC                         :: mde   = 45
   INTEGER, PARAMETER, PUBLIC                         :: npara = 3

   INTEGER, PARAMETER, PUBLIC                         :: SUCCESS = 1
   INTEGER, PARAMETER, PUBLIC                         :: FAILURE = -1

   REAL, DIMENSION(nch), PUBLIC                       :: freq
   INTEGER, DIMENSION(nch), PUBLIC                    :: frc

CONTAINS

!=============================================================================
SUBROUTINE AMSU_CLOUDEMS_ALGORITHM(RTYPE, iter_max, ish_ind, theta, tb, &
      xin, xou, ecm, kv, ko2_coe, ss_lut, rstatus)
!
!-Purpose: Retrieve cloud liquid water and total water vapor contents
!            Retrieve cloud water path, total precipitable water, cloud 
!            particle effective diameter over lands from AMSU measurements
!
!-References: 
!(1) Weng. F. and N. C. Grody, 1994: Retrieval of cloud liquid water using 
!    the special sensor microwave imager (SSM/I), J. Geophys. Res., 99, 
!    25,535 -25, 551.
!(2) Weng. F., L. Zhao, R. R. Ferraro, G. Poe, X. Li., and N. Grody, 2003: 
!    Advanced microwave sounding unit cloud and precipitation algorithms, 
!    Radio Science, 38, Mar 33, 1-12.
!(3) Yan, B. and F. Weng,2004: "Rain and cloud water paths derived from Aqua 
!    AMSR-E measurements', the 13th conference on satellite meteorolgy and 
!    oceanography, Norfolk in VA, Sept. 20-23.
!(4) Yan, B. and F. Weng: New applications of AMSR-E measurements under 
!    tropical cyclones, Part I: retrievals of sea surface temperature and 
!    wind speed; Part II: retrieval of liquid water path to be submitted to
!    J. Geophys. Res., 2005.
!
!----------------------------------------------------------------
! Input argument list (such as MHS):
!
!    tbo(1): brighness temperature at 23.8 GHz
!    tbo(2):                          31.4 GHz
!    tbo(3):                          50.3 GHz
!    tbo(4):                          89.0 GHz
!    tbo(5):                         150.0 GHz
!    tbo(6):                         183+/-7GHz
!
!    theta  : local zenith angle in degree
!
!    DTB_max: A constraint for the maximum bias between observed and 
!             simulated TB
!
! Important internal argument list
!    x(1)   : initialized sea surface temperature
!    x(2)   :                         wind
!    x(3)   : up-(down) welling brightness temperature
!    x(4)   :                                         
!    ta     : up-(down) welling brightness temperature
!    xi     : atmospheric transmittance
!
! ish_ind : classification of ice particle shape
! ish_ind = 0: sphere
! ish_ind = 1: Rosettes
! ish_ind = 2: Type-A snowflakes
! ish_ind = 3: Type-B snowflakes
! ish_ind = 4: cylindrical column; 
! ish_ind = 5: 2-cylinder aggregate; 
! ish_ind = 6: 3-cylinder aggregate; 
! ish_ind = 7: 4-cylinder aggregate
!
! Output argument lists
!
!    ts   : sea surface temperature (K)
!    wind : sea surface wind        (m/s)
!
! Code History:
! Beta version OF 1DVAR approach using TB from 23-150 GHz: Banghua Yan (2005)
! Added 183 GHz : Huan Meng (2006)
! Added non-spheric particle calculations : Huan Meng (10/28/2006)
! Removed the bugs in the subroutines related to the non-spheric particles: 
!       B. Yan (10/30/2006)
! Added a classification algorithm of ice particles  according to the shape 
!       from the cloud temperature : H. Meng (11/?/2006)
! Added a classification algorithm of ice particles  according to the shape 
!       from AMSU TB: B. Yan and H. Meng (11/?/2006)
! Simplified and re-formatted, Jun Dong, 01/01/2020

   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER                              :: ish_ind
   INTEGER                              :: rstatus
   INTEGER                              :: RTYPE
   INTEGER                              :: ncoe, i, j, ich, iter

   REAL, DIMENSION(nch)                 :: kv, tauo, tb, em, dpol
   REAL, DIMENSION(nch,3)               :: ko2_coe
   REAL                                 :: ts, tpw, ICW
   REAL                                 :: V, L, De, emissivity
   REAL                                 :: theta, angle, mu, ev, eh
   REAL                                 :: conv_test, conv_crit
   REAL                                 :: DTB_max, A_tran, f
   REAL                                 :: frequency, error_sum1
   REAL                                 :: error_sum2, gL, ssalbL
   REAL                                 :: tauL, g, ssalb, tau
   REAL                                 :: tau_nl, tsv, tsh, tc, tbs
   integer                              :: iter_max, ij_index, ierr

   REAL,DIMENSION(*)                    :: xin,xou
   REAL,DIMENSION(nvar)                 :: xb
   real, dimension(nvar)                :: ecm

   real, allocatable,dimension(:)       :: y, y_s, x, x_ap, delta_x, dy
   real, allocatable,dimension(:,:)     :: E

   real(4), dimension(mtc,mrwp,mde,npara,nch)    :: ss_lut
   double precision, allocatable, dimension(:,:) :: Sa, Sa_inv,Sy, Sy_inv, A
   
!=============================================================================
! variable for band removal
   integer                                       :: nch_vld, ii, jj
   real, allocatable,dimension(:)                :: delta_x_bdrm, dy_bdrm
   double precision, allocatable, dimension(:,:) :: A_bdrm, Sa_bdrm 
   double precision, allocatable, dimension(:,:) :: Sa_inv_bdrm, E_bdrm
!=============================================================================

! allocate variable for normal cases (no band removal)
   ALLOCATE(y(nch), y_s(nch), x(nvar), x_ap(nvar), delta_x(nvar), dy(nch))
   ALLOCATE(A(nch,nvar), E(nvar,nvar))
   ALLOCATE(Sa(nvar,nvar), Sa_inv(nvar, nvar))
   ALLOCATE(Sy(nvar,nvar), Sy_inv(nvar,nvar))
   
!=============================================================================   
! count total number of valid band to use to allocate variable for removal
   nch_vld = 0
   do i=1,nch
      if (frc(i) == 1) nch_vld = nch_vld+1
   enddo

   ALLOCATE(delta_x_bdrm(nvar-nch+nch_vld), dy_bdrm(nch_vld))
   ALLOCATE(A_bdrm(nch_vld, nvar-nch+nch_vld), E_bdrm(nvar-nch+nch_vld, &
           nvar-nch+nch_vld))
   ALLOCATE(Sa_bdrm(nvar-nch+nch_vld, nvar-nch+nch_vld),                &
           Sa_inv_bdrm(nvar-nch+nch_vld,nvar-nch+nch_vld))

!=============================================================================   
! xin: em23,em31,em50,em89,em150,em183+/-7,tpw,icw,De,te,ts   
   em(1:nch) = xin(1:nch)
   ts        = xin(nvar)
   rstatus   = SUCCESS 

!   write(*,*) "------ in alg ------"
!   write(*,*) "tb: in alg"
!   write(*,"(6F12.6)") (tb(j),j=1,6)
!   write(*,*) "emis: in alg"
!   write(*,"(6F12.6)") (xin(j),j=1,6)
!   write(*,*) "tpw, iwp, de, tc, ts: in alg"
!   write(*,"(5F12.6)") (xin(j),j=7,11)
!   write(*,*) "freq:"
!   write(*,*) (freq(i),i=1,nch)
!   write(*,*) "frc:"
!   write(*,*) (frc(i),i=1,nch)
!   write(*,*) "kv:"
!   write(*,"(6E14.6)") (kv(i),i=1,nch)
!   write(*,*) "ko2_coe:"
!   do i = 1, nch
!      write(*,*) (ko2_coe(i,j),j=1,3)
!   enddo
!   write(*,*) "ecm:"
!   write(*,*) (ecm(i),i=1,n)
!   read(*,*)

   mu = cos(theta*3.14159/180.0)
   
   !---Calculate KW and KL and tau_o2(taut) and emissivity
   do ich = 1, nch
      tauo(ich) = ko2_coe(ich,1) + ko2_coe(ich,2)*ts + ko2_coe(ich,3)*ts*ts
   enddo
   
   !---Initialization
   y        = 0.0
   f        = 0.0
   x        = 0.0
   x_ap     = 0.0
   delta_x  = 0.0
   Sa       = 0.0
   Sy_inv   = 0.0
   A        = 0.0
   A_tran   = 0.0
   E        = 0.0
   Dy       = 0.0
   DTB_max  = 1.5
   
   delta_x_bdrm  = 0.0
   dy_bdrm       = 0.0
   A_bdrm        = 0.0
   Sa_bdrm       = 0.0
   Sa_inv_bdrm   = 0.0 
   E_bdrm        = 0.0   
   ! INITIALIZE

   ! xin: em23,em31,em50,em89,em150,em183+/-7,tpw,icw,De,tc,ts
   xb(1:nvar) = xin(1:nvar)
   tpw = xin(nch+1)

   ! Define measurement error covariance matrix
   do i=1, nvar
      E(i,i) = ecm(i)  !0.25
   enddo

   ! Set up a priori conditions
   x_ap = xb

   ! Initialize the vector of retrievables
   x = x_ap   
   y=tb

   ! Remove channel, set 0
   ! do i=1, nch
      ! if (frc(i) == 0)      y(i) = 0.0
   ! enddo

   ! Start of retrieval loop
   iter = 0

RETRIEVAL_LOOP: DO
   dy = 0.0
   ! Calculate em(m)
   do i = 1, nch
      if (frc(i)==1) em(i) = x(i)
   enddo
   ! Add a classification algorithm : to be done
   ! CALL ICEPARTICLE_SHAPE(tc,tb,ish_ind)
   ! ish_ind = 0

   ! simulate for each channel
   do ich = 1, nch
     if(frc(ich)==1) then
      frequency = freq(ich)
      emissivity = em(ich)
      ! tpw = x(nch+1)
      icw = x(nch+2)
      de  = x(nch+3)
      tc  = x(nch+4)
      ! ts  = x(nch+5) 

      CALL CLOUD_OPTOUT(frequency, tc, icw, de, &
              tauL, ssalbL, gL, ss_lut)

      ! Weighted optical parameters
      ! tauo(ich) = ko2_coe(ich,1) + ko2_coe(ich,2)*ts + ko2_coe(ich,3)*ts*ts
      tau_nl = tauo(ich) + kv(ich)*tpw
      tau    = tauo(ich) + kv(ich)*tpw + tauL
      ssalb = tauL *ssalbL/ tau
      g     = tauL * gL /tau

      call two_stream_RTsolution(mu, tc, ts, emissivity, g, &
              ssalb, tau, tau_nl, tbs)
              
      !TWOSTREAM_RTM_CORRECTION
      CALL TB_2RTM_CORRECTION(abs(theta),tau,tbs,y_s(ich))
      
      ! y_s(ich) = tbs      
      dy(ich) = y(ich) - y_s(ich)
     endif
           
   enddo

   ! if diff small enough, exit
   ij_index = 1
   
   if (maxval(abs(dy(1:5))) > DTB_max)     ij_index=0
   if (abs(dy(6)) > DTB_max+1.5)           ij_index=0
   
   if (ij_index == 1)  then    
      tb = y - dy
      exit
   endif   
   
   ! Determine A matrix   (note: em(ich) <- ev-components
   call get_AAmatrix(RTYPE, ish_ind, nch, nvar, theta, em, &
       tc, ts, x, A, kv, ko2_coe, ss_lut)
       
   ! print*, 'iter=',iter, '3'
   ! print*, 'A'
   ! do i=1,nch
   !    write(*,'(1p15F12.4)')(A(i,j),j=1,nvar)
   ! enddo
   ! print*, 'E'
   ! do i=1,nvar
   !    write(*,'(1p15F12.4)')(E(i,j),j=1,nvar)
   ! enddo
   !====================================================================      
   ! reassign matrix A, E, dy, if some channels are removed
   if(nch_vld<nch) then
       ii=1
       do i = 1, nch
           jj=1
           do j = 1, nvar
               if(j<=nch) then
                   if(frc(j)==1) then
                   A_bdrm(ii,jj) = A(i,j)
                   if (i==1) then
                      E_bdrm(jj,jj) = E(j,j)
                   endif
                   jj=jj+1
                   endif
               else
                   A_bdrm(ii,jj) = A(i,j)
                   if (i==1) then
                      E_bdrm(jj,jj) = E(j,j)
                   endif
                   jj=jj+1
               endif
           enddo
           if(frc(i)==1) then
               dy_bdrm(ii) = dy(i)
               ii=ii+1                        
           endif
       enddo
   else
       A_bdrm(1:nch,1:nvar) = A(1:nch,1:nvar)
       E_bdrm(1:nvar,1:nvar) = E(1:nvar,1:nvar)
       dy_bdrm(1:nch) = dy(1:nch)
   endif
   !====================================================================  
   ! Determine Sa (= A_Tran*A + E) and its inverse matrix
   ! print*, 'iter=',iter, '4' 
   ! print*, 'A_bdrm'
   ! do i=1,nch_vld
   !    write(*,'(1p15F12.4)')(A_bdrm(i,j),j=1,nvar-nch+nch_vld)
   ! enddo
   ! print*, 'E_bdrm'
   ! do i=1,nvar-nch+nch_vld
   !    write(*,'(1p15F12.4)')(E_bdrm(i,j),j=1,nvar-nch+nch_vld)
   ! enddo
   ! pause
    
   Sa_bdrm = (matmul(transpose(A_bdrm),A_bdrm) + E_bdrm)

   call gen_inverse(Sa_bdrm, Sa_inv_bdrm, nvar-nch+nch_vld, &
           nvar-nch+nch_vld, ierr)    

   delta_x_bdrm = matmul(matmul(Sa_inv_bdrm, transpose(A_bdrm)), dy_bdrm)

   ! reassign delta_x from delta_x_bdrm
   ii=1
   do i = 1, nvar
      if (i<=nch) then
         if(frc(i)==1) then
            delta_x(i) = delta_x_bdrm(ii)
            ii=ii+1
         else
            delta_x(i) = 0.0
         endif
      else
         delta_x(i) = delta_x_bdrm(ii)
         ii=ii+1
      endif
   enddo

   ! Adjust delta_x
   do j = 1, nvar
      if (isnan(delta_x(j)))  then
         delta_x(j) = 0.
      endif
      if (abs(delta_x(j)) > abs(x(j))/2.0) then
         delta_x(j) = sign(x(j)/2.0, delta_x(j))
      endif
   enddo
  
   !--- Update retrieved vector
   x = x + delta_x
   if (frc(5)==1 .and. frc(6)==1) x(6) = x(5)

   ! quality check
   do ich = 1, nch
      if (x(ich) .ge. 1.0)      x(ich) = 1.0
      if (x(ich) .le. 0.4)      x(ich) = 0.4
   enddo

   if (x(nch+2) .le. 0.0)       x(nch+2) = 0.0
   if (x(nch+3) .le. 0.005)     x(nch+3) = 0.005
   
   if (x(nch+4) .lt. 215.0)     x(nch+4) = 215.0
   if (x(nch+4) .gt. 275.0)     x(nch+4) = 275.0
   
   iter = iter + 1
   rstatus = iter

   ! Not convergent
   if (iter == iter_max) then
      tb = y - dy
      !print*,'not convergent!'
      !rstatus = FAILURE
      !x = -999.0
      exit
   endif

END DO RETRIEVAL_LOOP


   ! retrieval output is from x
   xou(1:nvar) =  x(1:nvar)
   !print*, iter, dy(1:nch), xin(1:nvar), xou(1:nvar)

   DEALLOCATE (y,y_s, x, x_ap, delta_x, dy)
   DEALLOCATE (A, E)
   DEALLOCATE (Sa, Sa_inv, Sy, Sy_inv)      
   DEALLOCATE (delta_x_bdrm, dy_bdrm)
   DEALLOCATE (A_bdrm, E_bdrm)
   DEALLOCATE (Sa_bdrm, Sa_inv_bdrm)

!----------------------------------------------------------------
END SUBROUTINE AMSU_CLOUDEMS_ALGORITHM



!=======================================================================
SUBROUTINE two_stream_RTsolution(mu, tc, ts, em, g, ssalb, tau, tau_nl, TB)
!
!---Purpose: two stream simulation solution
!
   IMPLICIT NONE
!----------------------------------------------------------------------------
   REAL, INTENT(IN)                     :: em, ssalb, tau, g, tc, ts
   REAL, INTENT(IN)                     :: mu, tau_nl
   REAL, INTENT(OUT)                    :: TB
!
!---local variables
!
   REAL                                 :: i0, ev, r23_v, tau_v, gv
   REAL                                 :: alfa_v,kk_v,beta_v
   REAL                                 :: ssalb_v,gamma_v
   REAL                                 :: gamma1_v,gamma2_v
   REAL                                 :: gamma3_v,gamma4_v
   REAL                                 :: TV_D1,TV_D2,TV_D3,TV
!----------------------------------------------------------------------------

! Initializations for various parameters

   i0 = 0.0 !/2.7
   ev = em
   r23_v = 1.0 - ev
   gv = g
   ssalb_v = ssalb
   tau_v = tau

   alfa_v = sqrt((1.0-ssalb_v)/(1.0 - gv*ssalb_v))
   kk_v = sqrt ((1.0-ssalb_v)*(1.0 - gv*ssalb_v))/mu
   beta_v = (1.0 - alfa_v)/(1.0 + alfa_v)
   gamma_v = (beta_v -r23_v)/(1.0-beta_v*r23_v)
   gamma1_v = 1.0 - r23_v/beta_v
   gamma2_v = 1.0 - r23_v*beta_v
   gamma3_v = 1.0/beta_v - r23_v
   gamma4_v = beta_v - r23_v

! Compute TV

   if (((tau-tau_nl) .ge. 0.005) .and. (ssalb .ge. 0.001))  then
      TV_D1 = (i0-tc)*( gamma1_v*exp(-kk_v*tau_v) &
            - gamma2_v*exp(kk_v*tau_v) )
      TV_D2 = (ev*ts  - ev*tc )*(1.0/beta_v - beta_v)
      TV_D3 = gamma4_v*exp(-kk_v*tau_v) -  gamma3_v*exp(kk_v*tau_v)

      TV = (TV_D1 - TV_D2)/TV_D3 + tc
   else
      TV =i0*(1.0-ev)*exp(-2.0*tau_v/mu) + tc*(1.0-(1.0-ev) &
            *exp(-2.0*tau_v/mu)) + (ev*ts - ev*tc)*exp(-tau_v/mu)
   endif

   TB = TV
!----------------------------------------------------------------
END SUBROUTINE two_stream_RTsolution


!===========================================================================
SUBROUTINE TB_2RTM_CORRECTION(theta,tau,tb,y)
!
!---Purpose:
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL,PARAMETER                                     :: tau_c = 0.4, &
                                                         theta_c = 50.0
!   INTEGER,INTENT(IN)                                 :: m
   REAL,INTENT(IN)                                    :: theta,tau
   REAL,INTENT(IN)                                    :: tb
   REAL,INTENT(OUT)                                   :: y
   REAL                                               :: dtb
!----------------------------------------------------------------

 ! Initialization
   y = tb
   dtb = 0.0

 ! UPDATE
   if (theta <= theta_c) then
      if (tau < tau_c) then
          dtb = (4.84+0.0084*theta-0.0021*theta*theta)*tau/tau_c
      else
          dtb = 4.84+0.0084*theta-0.0021*theta*theta
      endif
   endif
   y = y + dtb
   
!----------------------------------------------------------------
END SUBROUTINE TB_2RTM_CORRECTION



!==========================================================================
SUBROUTINE get_AAmatrix(RTYPE, ish_ind, m, n, theta, &
        em, tc, ts, xb, A, kv, ko2_coe, ss_lut)
!
!---Purpose: calculate the adjust matrix
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                            :: RTYPE
   INTEGER, INTENT(IN)                           :: ish_ind
   INTEGER, INTENT(IN)                           :: m, n
   REAL, INTENT(IN)                              :: theta, tc, ts
   REAL, DIMENSION(*), INTENT(IN)                :: xb, em
   double precision, DIMENSION(m,n), INTENT(OUT) :: A

   REAL(4), DIMENSION(mtc,mrwp,mde,npara,nch)    :: ss_lut
!
!---local variables
!
   REAL, DIMENSION(m)                            :: kv,tau_nl,tauo
   REAL, DIMENSION(m,3)                          :: ko2_coe
   REAL                                          :: v, rwp, de, i0, angle
   REAL                                          :: mu, frequency
   REAL                                          :: emissivity, ssalbL
   REAL                                          :: dssalbL_dL, dssalbL_d
   REAL                                          :: tauL, gL, ssalb, tau, g
   REAL                                          :: dtauL_dL, dtauL_dD
   REAL                                          :: dgL_dL, dgL_dD, dTB_dV
   REAL                                          :: dTB_dL, dTB_dD
   REAL                                          :: dssalbl_dd
   REAL                                          :: dTB_dem, dTB_dTC, dTB_dTS
   INTEGER                                       :: ich, i, j
!----------------------------------------------------------------

! Initialization for A

   A =  0.0

   V   = xb(m+1)
   rwp = xb(m+2)
   de  = xb(m+3)
   i0  = 0.0
   angle = theta*3.14159/180.0
   mu = cos(angle)

! Calculate KW and KL and tau_o2(taut) and emissivity

   do ich = 1, m
      tauo(ich) = ko2_coe(ich,1) + ko2_coe(ich,2)*ts + ko2_coe(ich,3)*ts*ts
      tau_nl(ich) = tauo(ich) + kv(ich) * V
   enddo

! Loop all channels

   do ich = 1, m
      frequency  = freq(ich)
      emissivity = em(ich)

      ! compute optical parameters
      call optic_dLD(ish_ind, frequency, theta, tc, kv(ich), tauo(ich), V, &
            rwp, de, ssalbL, tauL, gL, ssalb, tau, g, dssalbL_dL,          &
            dssalbL_dD, dtauL_dL, dtauL_dD, dgL_dL, dgL_dD, ss_lut)
    
      ! Scattering atmosphere
      if (abs(tau_nl(ich)-tau) .ge. 0.005 .and. (ssalb .ge. 0.001)) then
         call Amatrix_Freq(theta, emissivity, i0, tc, ts, kv(ich), gL,   &
               ssalbL, tauL, dssalbL_dL, dssalbL_dD, dtauL_dL, dtauL_dD, &
               dgL_dL, dgL_dD, g, ssalb, tau, dTB_dV, dTB_dL, dTB_dD,    &
               dTB_dem, dTB_dTC, dTB_dTS)

      else
         call Amatrix_emissFreq(theta, emissivity, i0, tc, ts, kv(ich), tauL, &
               tau, dTB_dV, dTB_dL, dTB_dD, dTB_dem, dTB_dTC, dTB_dTS)
      endif

      ! xin: em23,em31,em50,em89,em150,em183+-7,tpw,icw,De,te,ts
      do j = 1, n
         A(ich,j) = 0.0
         ! emiss
         if (j .eq. ich)        A(ich,j) = dTB_dem
         ! physical vars
         if (j .eq. (m+1))      A(ich,j) = 0.0 
         if (j .eq. (m+2))      A(ich,j) = dTB_dL
         if (j .eq. (m+3))      A(ich,j) = dTB_dD
         if (j .eq. (m+4))      A(ich,j) = dTB_dTC 
         ! if (j .eq. (m+4))      A(ich,j) = 0.0 
         if (j .eq. (m+5))      A(ich,j) = 0.0 
      enddo

  enddo   ! all channels end!

!----------------------------------------------------------------
END SUBROUTINE GET_AAMATRIX



!============================================================================
SUBROUTINE OPTIC_DLD(ish_ind, frequency, theta, tc, kv, tauo, V, rwp, de, &
      ssalbL, tauL, gL, ssalb, tau, g, dssalbL_dL, dssalbL_dD,            &
      dtauL_dL, dtauL_dD, dgL_dL, dgL_dD, ss_lut)
!
!---Purpose: Compute optical parameters of rain drops using Mie theory
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL(4), DIMENSION(mtc,mrwp,mde,npara,nch) :: ss_lut

   REAL                                       :: frequency, theta, kv
   REAL                                       :: tc, tauo, V, rwp, de
   REAL                                       :: ssalbL, tauL, gL, ssalb
   REAL                                       :: tau, g, dssalbL_dL
   REAL                                       :: dssalbL_dD, dtauL_dL
   REAL                                       :: dtauL_dD, dgL_dL
   REAL                                       :: dgL_dD, xin, xout1
   REAL                                       :: xout2, xout3
   INTEGER                                    :: ish_ind
!----------------------------------------------------------------

   CALL CLOUD_OPTOUT(frequency, tc, rwp, de, tauL, ssalbL, gL, ss_lut)

   xin  = rwp + 0.001 !---over RWP

   CALL CLOUD_OPTOUT(frequency, tc, xin, de, xout3, xout1, xout2, ss_lut)

   dssalbL_dL = (xout1 - ssalbL)/0.001
   dgL_dL     = (xout2 - gL)/0.001
   dtauL_dL   = (xout3 - tauL)/0.001

   xin  = de + 0.0001 !---over De

   CALL CLOUD_OPTOUT(frequency, tc, rwp, xin, xout3, xout1, xout2, ss_lut)

   dssalbL_dD = (xout1 - ssalbL)/0.0001
   dgL_dD     = (xout2 - gL)/0.0001
   dtauL_dD   = (xout3 - tauL)/0.0001


! Weighted optical parameters
   tau   = tauo + kv*V + tauL
   ssalb = tauL *ssalbL/ tau
   g     = tauL * gL /tau

!----------------------------------------------------------------
END SUBROUTINE OPTIC_DLD



!==========================================================================
SUBROUTINE Amatrix_Freq(theta, em, i0, tc, ts, kv,gL, ssalbL, tauL,         &
      dssalbL_dL, dssalbL_dD, dtauL_dL, dtauL_dD, dgL_dL, dgL_dD, g, ssalb, &
      tau, dTB_dV, dTB_dL, dTB_dD, dTB_dem, dTB_dTC, dTB_dTS)
!
!---Purpose: calculate dA/df matrix
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL, INTENT(IN)                     :: theta, em, ssalb, g, tc
   REAL, INTENT(IN)                     :: ts, kv, gL
   REAL, INTENT(IN)                     :: ssalbL, tauL, dtauL_dD
   REAL, INTENT(IN)                     :: dssalbL_dL, dgL_dL
   REAL, INTENT(IN)                     :: dssalbL_dD, dtauL_dL
   REAL, INTENT(IN)                     :: dgL_dD, tau
   REAL, INTENT(OUT)                    :: dTB_dV, dTB_dL, dTB_dD
   REAL, INTENT(OUT)                    :: dTB_dem, dTB_dTC
   REAL, INTENT(OUT)                    :: dTB_dTS
!
!---local variables
!
   REAL                                   :: i0
   REAL(8)                              :: mu, r23, a, kk, beta
   REAL(8)                              :: gamma_value, gamma1
   REAL(8)                              :: gamma2, gamma3, gamma4
   REAL(8)                              :: TB1, TB2, F1, F2
   REAL(8)                              :: dtau_dV, dtau_dL
   REAL(8)                              :: dtau_dD, dssalb_dV
   REAL(8)                              :: dssalb_dL, dssalb_dD
   REAL(8)                              :: dg_dV, dg_dL, dg_dD
   REAL(8)                              :: dgamma1_dbeta, dgamma2_dbeta
   REAL(8)                              :: dgamma3_dbeta, dgamma4_dbeta
   REAL(8)                              :: dbeta_da, da_dssalb, da_dg
   REAL(8)                              :: dkk_dssalb, dkk_dg, dkk_dV
   REAL(8)                              :: dkk_dL, dkk_dD
   REAL(8)                              :: dbeta_dV, dbeta_dL, dbeta_dD
   REAL(8)                              :: dgamma1_dV, dgamma1_dL, dgamma1_dD
   REAL(8)                              :: dgamma2_dV, dgamma2_dL, dgamma2_dD
   REAL(8)                              :: dgamma3_dV, dgamma3_dL, dgamma3_dD
   REAL(8)                              :: dgamma4_dV, dgamma4_dL, dgamma4_dD
   REAL(8)                              :: X11, X22, X33, dF1_dV, dF2_dV
   REAL(8)                              :: dF1_dL, dF2_dL, dF1_dD, dF2_dD
   REAL(8)                              :: dbeta_dssalb, dbeta_d
!----------------------------------------------------------------
   i0          = 0.0  !2.7
   mu          = cos(theta*3.14159/180.0)
   r23         = 1.0 - em
   a           = sqrt((1.0 - ssalb)/(1.0 - g*ssalb))
   kk          = sqrt ((1.0 - ssalb)*(1.0 -  g*ssalb))/mu
   beta        = (1.0 - a)/(1.0 + a)
   gamma_value = (beta -r23)/(1.0-beta*r23)
   gamma1      = 1.0 - r23/beta
   gamma2      = 1.0 - r23*beta
   gamma3      = 1.0/beta - r23
   gamma4      = beta - r23

! Compute variables related to TB  (= F1/F2 + tc)

    TB1 = (i0-tc)*( gamma1*exp(-kk*tau) - gamma2*exp(kk*tau) )
    TB2 = (em*ts - em*tc )*(1.0/beta - beta)

    F1 = TB1 - TB2
    F2 = gamma4*exp(-kk*tau) -  gamma3*exp(kk*tau)

! *** Compute levelA derivatives

   dtau_dV = kv
   dtau_dL = dtauL_dL
   dtau_dD = dtauL_dD

   dssalb_dV = - (tauL*ssalbL*kv)/tau/tau
   dssalb_dL = (dtauL_dL*tau -  dtau_dL*tauL)*ssalbL/tau/tau &
        + tauL/tau*dssalbL_dL
   dssalb_dD = (dtauL_dD*tau -  dtau_dD*tauL)*ssalbL/tau/tau &
        + tauL/tau*dssalbL_dD

   dg_dV = - (tauL*gL*kv)/tau/tau
   dg_dL = (dtauL_dL*tau -  dtau_dL*tauL)*gL/tau/tau + tauL/tau*dgL_dL
   dg_dD = (dtauL_dD*tau -  dtau_dD*tauL)*gL/tau/tau + tauL/tau*dgL_dD

   dgamma1_dbeta = (1.0 - em)/beta/beta
   dgamma2_dbeta = -(1.0 - em)
   dgamma3_dbeta = -1.0/beta/beta
   dgamma4_dbeta = 1.0
 
   dbeta_da  = -2.0/(1.0+a)**2

   da_dssalb = -(1.0-g)/( 2.0*(1.0-ssalb)**0.5*(1.0-ssalb*g)**1.5 )
   da_dg     = (ssalb*(1.0-ssalb)**0.5)/(2.0*(1.0-ssalb*g)**1.5)

   dkk_dssalb =  (2.0*ssalb*g-(1.0+g))/( 2.0*mu* &
           ((1.0-ssalb)*(1.0-ssalb*g))**0.5 )
   dkk_dg     = -(ssalb*(1.0-ssalb)**0.5)/( 2.0*mu*(1.0-ssalb*g)**0.5 )

! these two terms are not necessary?

!   dbeta_dssalb = (1.0-g)/( (1.0+a)**2.0*(1.0-ssalb)**0.5*(1.0-ssalb)**1.5 )
!   dbeta_dg     = - (ssalb*(1.0-ssalb)**0.5)/( (1.0+a)**2*(1.0-ssalb*g)**1.5 )

! *** Compute levelB derivatives

   dkk_dV = dkk_dssalb*dssalb_dV + dkk_dg*dg_dV
   dkk_dL = dkk_dssalb*dssalb_dL + dkk_dg*dg_dL
   dkk_dD = dkk_dssalb*dssalb_dD + dkk_dg*dg_dD

   dbeta_dV = dbeta_da*(da_dssalb *dssalb_dV + da_dg*dg_dV)
   dbeta_dL = dbeta_da*(da_dssalb *dssalb_dL + da_dg*dg_dL)
   dbeta_dD = dbeta_da*(da_dssalb *dssalb_dD + da_dg*dg_dD)

   dgamma1_dV = dgamma1_dbeta*dbeta_da*(da_dssalb*dssalb_dV + da_dg*dg_dV)
   dgamma1_dL = dgamma1_dbeta*dbeta_da*(da_dssalb*dssalb_dL + da_dg*dg_dL)
   dgamma1_dD = dgamma1_dbeta*dbeta_da*(da_dssalb*dssalb_dD + da_dg*dg_dD)

   dgamma2_dV = dgamma2_dbeta*dbeta_da*(da_dssalb*dssalb_dV + da_dg*dg_dV)
   dgamma2_dL = dgamma2_dbeta*dbeta_da*(da_dssalb*dssalb_dL + da_dg*dg_dL)
   dgamma2_dD = dgamma2_dbeta*dbeta_da*(da_dssalb*dssalb_dD + da_dg*dg_dD)

   dgamma3_dV = dgamma3_dbeta*dbeta_da*(da_dssalb*dssalb_dV + da_dg*dg_dV)
   dgamma3_dL = dgamma3_dbeta*dbeta_da*(da_dssalb*dssalb_dL + da_dg*dg_dL)
   dgamma3_dD = dgamma3_dbeta*dbeta_da*(da_dssalb*dssalb_dD + da_dg*dg_dD)

   dgamma4_dV = dbeta_da*(da_dssalb*dssalb_dV + da_dg*dg_dV)
   dgamma4_dL = dbeta_da*(da_dssalb*dssalb_dL + da_dg*dg_dL)
   dgamma4_dD = dbeta_da*(da_dssalb*dssalb_dD + da_dg*dg_dD)


! *** Compute levelC derivatives

    X11 = (i0-tc)*(  (dgamma1_dV*exp(-kk*tau) - dgamma2_dV*exp(kk*tau))  &
          - (dkk_dV*tau + dtau_dV*kk)*(gamma1*exp(-kk*tau)               &
          + gamma2*exp(kk*tau)) )
    X22 = (1.0+1.0/beta/beta)*(em*ts - em*tc)*dbeta_dV
    dF1_dV = X11 + X22
    dF2_dV = (dgamma4_dV*exp(-kk*tau) - dgamma3_dV*exp(kk*tau))    &
          - (gamma4*exp(-kk*tau) + gamma3*exp(kk*tau))*(dkk_dV*tau &
          +  dtau_dV*kk)

    dTB_dV = (F2*dF1_dV - F1*dF2_dV)/F2/F2

    X11 = (i0-tc)*(  (dgamma1_dL*exp(-kk*tau) - dgamma2_dL*exp(kk*tau))  &
          - (dkk_dL*tau + dtau_dL*kk)*(gamma1*exp(-kk*tau)               &
          + gamma2*exp(kk*tau)) )
    X22 = (1.0+1.0/beta/beta)*(em*ts - em*tc )*dbeta_dL
    dF1_dL = X11 + X22
    dF2_dL = (dgamma4_dL*exp(-kk*tau) - dgamma3_dL*exp(kk*tau))    &
          - (gamma4*exp(-kk*tau) + gamma3*exp(kk*tau))*(dkk_dL*tau &
          +  dtau_dL*kk)


    X11 = (i0-tc)*(  (dgamma1_dD*exp(-kk*tau) - dgamma2_dD*exp(kk*tau)) &
          - (dkk_dD*tau + dtau_dD*kk)*(gamma1*exp(-kk*tau)              &
          + gamma2*exp(kk*tau)) )
    X22 = (1.0+1.0/beta/beta)*(em*ts  - em*tc )*dbeta_dD
    X33 = kk*tc*(1.0-em)*exp(-kk*tau)*(1.0/beta - beta)*dtau_dD
    dF1_dD = X11 + X22
    dF2_dD = (dgamma4_dD*exp(-kk*tau) - dgamma3_dD*exp(kk*tau))    &
          - (gamma4*exp(-kk*tau) + gamma3*exp(kk*tau))*(dkk_dD*tau &
          + dtau_dD*kk)


    dTB_dL = (F2*dF1_dL - F1*dF2_dL)/F2/F2
    dTB_dD = (F2*dF1_dD - F1*dF2_dD)/F2/F2
    dTB_dTC = ( em*(1.0/beta - beta)-( gamma1*exp(-kk*tau) &
          - gamma2*exp(kk*tau)) ) /F2
    dTB_dTS = -em*(1.0/beta - beta) / F2


! 03/21/07 revised
        dTB_dem = (i0-tc)*(exp(-kk*tau)/beta-exp(kk*tau)*beta) &
            - (ts - tc )*(1.0/beta - beta)
        dTB_dem = dTB_dem/F2 - ( exp(-kk*tau)-exp(kk*tau) )*F1/F2/F2
!----------------------------------------------------------------------------
END SUBROUTINE  Amatrix_Freq



!===================================================================
SUBROUTINE Amatrix_emissFreq(theta, em, i0, tc, ts, kv, tauL, tau, &
      dTB_dV, dTB_dL, dTB_dD, dTB_dem, dTB_dTC, dTB_dTS)
   IMPLICIT NONE
!----------------------------------------------------------------------------
   REAL, INTENT(IN)                     :: theta, kv, tau, tauL
   REAL, INTENT(IN)                     :: tc, ts, em
   REAL, INTENT(OUT)                    :: i0, dTB_dV, dTB_dem
   REAL, INTENT(OUT)                    :: dTB_dTC, dTB_dTS
   REAL, INTENT(OUT)                    :: dTB_dL, dTB_dD
!
!---local variables
!
   REAL                                 :: mu, kk, tauV, dtau_dV
!----------------------------------------------------------------
! Initializations for various parameters

   i0      = 0.0  !2.7
   mu      = cos(theta*3.14159/180.0)
   kk      = 1.0/mu
   dtau_dV = kv
   tauV    = tau -tauL
   dTB_dV  = exp(-kk*tauV)*( 2.0*(1.0-em)/mu*(tc-i0)*exp(-kk*tauV) &
           - em/mu*(ts-tc) )*dtau_dV
   dTB_dem =(tc - i0)*exp(-2.0*tauV*kk)  +  ( ts - tc )*exp(-tauV*kk)
   dTB_dTC = 1.0- (1.0-em)*exp(-2.0*tauV*kk) - em*exp(-tauV*kk)
   dTB_dTS = em * exp(-tauV*kk)
   dTB_dL  = 0.0
   dTB_dD  = 0.0
!----------------------------------------------------------------
END SUBROUTINE  Amatrix_emissFreq



!=============================================================================
SUBROUTINE CLOUD_OPTOUT(FREQUENCY, TC, IWP, DE, TAU, ALBEDO, GG, CLOUDOPT)
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL, PARAMETER        :: TC_MIN = 180.0, TC_MAX = 270.0
   REAL, PARAMETER        :: IWP_MIN = 0.001, IWP_MAX = 2.0
   REAL, PARAMETER        :: DE_MIN0 = 0.001,DE_MIN = 0.05, DE_MAX = 2.0
   REAL, PARAMETER        :: DTC = 2.0, DIWP = 0.04, DDE = 0.05, DDE0 = 0.01
   INTEGER                :: ic,id,iw,ich,ip
   INTEGER                :: ich_index,ic_index,ide_index,iw_index
   REAL                   :: FREQUENCY,TC,IWP,DE,TAU,ALBEDO,GG
   REAL, DIMENSION(npara) :: you

   REAL(4), DIMENSION(mtc,mrwp,mde,npara,nch) :: CLOUDOPT

! LOCATE
   ich_index = 1
   DO ich = 1, nch
      if (abs(FREQUENCY-freq(ich)) <= 1.0 ) ich_index = ich
   ENDDO

   ic_index =  INT((TC-TC_MIN)/DTC) + 1
   if (TC <= TC_MIN) ic_index = 1
   if (TC >= TC_MAX) ic_index = mtc

   iw_index =  INT(IWP/DIWP) + 1
   if (IWP <= IWP_MIN) iw_index = 1
   if (IWP >= IWP_MAX) iw_index = mrwp
  
! DE: 0.001,0.01,0.02,0.03,0.04,0.05~2.0 BY AN INCREASEMENT OF 0.05

   IF (DE < DE_MIN) THEN
      IF (DE <= DE_MIN0) THEN
         ide_index = 1
      ELSE
         ide_index = INT(DE/DDE0) + 1  
      ENDIF
   ELSE
      ide_index = INT(DE/DDE) + 5 
   ENDIF
   if (DE >= DE_MAX ) ide_index = mde


   DO ip = 1, npara
      CALL TWOD_INTERPOLATION(TC, IWP, DE, TC_MIN, IWP_MIN, DE_MIN, DTC, &
              DIWP, DDE, DDE0, DE_MIN0, ic_index, ide_index, iw_index,   &
              ip, ich_index, you(ip), CLOUDOPT)
   ENDDO
   TAU    = you(3)
   ALBEDO = you(1)
   GG     = you(2)
!----------------------------------------------------------------
END SUBROUTINE CLOUD_OPTOUT



!=============================================================================
SUBROUTINE TWOD_INTERPOLATION(TC, IWP, DE, TC_MIN, IWP_MIN, DE_MIN, DTC,   &
        DIWP, DDE, DDE0, DE_MIN0, ic_index, ide_index, iw_index, npara_in, &
        nch_in,you, CLOUDOPT)

  REAL, PARAMETER                 :: min_distance = 0.001
  INTEGER                         :: ic_index, ide_index, iw_index
  INTEGER                         :: npara_in, nch_in
  INTEGER                         :: ic, ntime
  REAL                            :: TC, IWP, DE, TC_MIN, IWP_MIN, DE_MIN
  REAL                            :: DTC, DIWP, DDE0, DDE, DE_MIN0
  REAL                            :: x1, x2, y1, y2, ylow, yhi, you
  REAL, ALLOCATABLE, DIMENSION(:) :: Copt_tc

  REAL(4), DIMENSION(mtc,mrwp,mde,npara,nch) :: CLOUDOPT

! DETERMINE VARIABLE NUMBER OF TEMPERATURE DEPENDENCY
  
   ntime  = 1
   if (ic_index+1 <= mtc) ntime = 2
   ALLOCATE(Copt_tc(ntime))

   do ic = 1, ntime

      ! x-direction (iwp)
      if (iw_index <=2) then 
          x1 = IWP_MIN
          x2 = iw_index*DIWP
      else
          x1 = (iw_index-1) * DIWP
          x2 = iw_index* DIWP
      endif
      y1 = CLOUDOPT(ic_index+(ic-1),iw_index,ide_index,npara_in,nch_in)
      y2 = y1
      if (iw_index+1 <= mrwp ) &
      y2 = CLOUDOPT(ic_index+(ic-1),iw_index+1,ide_index,npara_in,nch_in)

      if (abs(x2-x1) <=min_distance) then
          ylow = y1
      else
          ylow = y1 + (y2-y1)*(IWP-x1)/(x2-x1)
      endif
     
      yhi = ylow
      if (ide_index+1 <= mde) then

         y1 = CLOUDOPT(ic_index+(ic-1),iw_index,ide_index+1,npara_in,nch_in)
         y2 = y1
         if (iw_index+1 <= mrwp ) &
         y2 = CLOUDOPT(ic_index+(ic-1),iw_index+1,ide_index+1, &
                 npara_in,nch_in)

         if (abs(x2-x1) <=min_distance) then
             yhi = y1
         else
             yhi = y1 + (y2-y1)*(IWP-x1)/(x2-x1)
         endif
      endif

      ! y-direction
      if (ide_index <= 5 ) then
          if (ide_index == 1) then
              x1 = DE_MIN0
              x2 = 0.01
          else
              x1 = (ide_index-1)*DDE0
              x2 = ide_index*DDE0
          endif
      else
         x1 = (ide_index-5)*DDE   
         x2 = x1
         if (ide_index+1 <= mde) x2 = (ide_index+1.0-5.0)*DDE
      endif    
      y1 = ylow
      y2 = yhi
      if (abs(x2-x1) <=min_distance) then
          Copt_tc(ic) = y1
      else

          Copt_tc(ic)  =  y1 + (y2-y1)*(DE-x1)/(x2-x1)
      endif
   enddo  

! interpolation in temperature direction

   x1 = TC_MIN + (ic_index-1.)*DTC
   x2 = x1
   if (ic_index+1 <=  mtc) x2 = TC_MIN + ic_index*DTC
   y1 = Copt_tc(1)
   y2 = Copt_tc(2)
   if (abs(x2-x1) <=min_distance) then
       you = y1
   else
       you = y1 + (y2-y1)*(TC-x1)/(x2-x1)
   endif

   RETURN
!----------------------------------------------------------------
END SUBROUTINE TWOD_INTERPOLATION



!===========================================================================
!===========================================================================
! Blow are math lib. No need to modify
!===========================================================================
!===========================================================================



!===========================================================================
SUBROUTINe gen_inverse(A,A_inv,m,p,ierr)
!
!---Purpose: compute general inverse of a matrix using svdcmp
!
   IMPLICIT NONE
!---------------------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: m,p
!---general inerse of A
   double precision,DIMENSION(m,p),INTENT(IN)         :: A
!----a matrix whose general inverse is sought
   double precision,DIMENSION(m,p),INTENT(OUT)        :: A_inv
!---general inerse of A
   INTEGER,INTENT(OUT)                                :: ierr
!
!---local variables
!
   double precision,DIMENSION(m,p)                    :: U
   REAL,DIMENSION(p,p)                                :: W
   double precision,DIMENSION(p,p)                    :: V
   double precision,DIMENSION(p)                      :: ww
   INTEGER                                            :: i,j
!---------------------------------------------------------------------------
      U = A
      if ( p == 0 ) then
           print *,"p = 0 in gen_inv",m,p,shape(W)
      stop
      endif

      CALL svdcmp(U,m,p,ww,V,ierr)
      W=0.0
      do i = 1,p
         if (ww(i) /= 0.) W(i,i)=1.0/ww(i)
      enddo

      A_inv = matmul(V,matmul(W,transpose(U)))

      do i = 1,m
         do j =1, p
             if (isnan(a_inv(i,j))) a_inv(i,j) = 0.
         end do
      end do
      
!---------------------------------------------------------------------------
END SUBROUTINE gen_inverse



!=============================================================================
SUBROUTINE svdcmp(a,m,n,w,v,ierr)

   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: m,n
   INTEGER,INTENT(OUT)                                :: ierr
   Double precision,DIMENSION(m,n)                                :: a
   Double precision,DIMENSION(n)                                  :: w
   Double precision,DIMENSION(n,n)                                :: v
!
!---local variables
!
   REAL                                               :: g,sscale,anorm,s,f,  &
                                                         h,c,x,y,z
   REAL,DIMENSION(N)                                  :: rv1
   INTEGER                                            :: i,l,its,j,jj,k,nm
!----------------------------------------------------------------

      ierr     = 0
      g        = 0.0
      sscale   = 0.0
      anorm    = 0.0
      twentyfive: do i=1,n
         l=i+1
         rv1(i)=sscale*g
         f     = 0. !liusy+
         g=0.0
         s=0.0
         sscale=0.0
         if(i <= m)then
            do k=i,m
               sscale=sscale+abs(a(k,i))
            enddo
            if(sscale/=0.0) then
               do k=i,m
               a(k,i)=a(k,i)/sscale
               s=s+a(k,i)*a(k,i)
               end do
               f=a(i,i)
               g=-sign(sqrt(s),f)
               h=f*g-s
               a(i,i)=f-g
               do j=l,n
                  s=0.0
                  do k=i,m
                     s=s+a(k,i)*a(k,j)
                  end do
!liusy-                  f=s/h
                  if (h /= 0.) f=s/h !liusy+
                  do k=i,m
                     a(k,j)=a(k,j)+f*a(k,i)
                  end do
               end do
               do k=i,m
                  a(k,i)=sscale*a(k,i)
               end do
            endif
         endif
         w(i)=sscale *g
         g=0.0
         s=0.0
         sscale=0.0
         if((i<=m).and.(i/=n))then
           do k=l,n
              sscale=sscale+abs(a(i,k))
           enddo
           if(sscale/=0.0)then
              do k=l,n
                 a(i,k)=a(i,k)/sscale
                 s=s+a(i,k)*a(i,k)
              enddo
              f=a(i,l)
              g=-sign(sqrt(s),f)
              h=f*g-s
              a(i,l)=f-g
              do k=l,n
                 rv1(k)=a(i,k)/h
              enddo
              do j=l,m
                 s=0.0
                 do k=l,n
                    s=s+a(j,k)*a(i,k)
                 enddo
                 do k=l,n
                    a(j,k)=a(j,k)+s*rv1(k)
                 enddo
              enddo
              do k=l,n
                 a(i,k)=sscale*a(i,k)
              enddo
           endif
         endif
         anorm=max(anorm,(abs(w(i))+abs(rv1(i))))
      end do twentyfive
      do i=n,1,-1
         if(i < n)then
             if(g /= 0.0)then
                do j=l,n
                   v(j,i)=(a(i,j)/a(i,l))/g
                enddo
                do j=l,n
                   s=0.0
                   do k=l,n
                      s=s+a(i,k)*v(k,j)
                   enddo
                   do k=l,n
                      v(k,j)=v(k,j)+s*v(k,i)
                   enddo
                enddo
             endif
             do j=l,n
                v(i,j)=0.0
                v(j,i)=0.0
             enddo
         endif
         v(i,i)=1.0
         g=rv1(i)
         l=i
      enddo
      do i=min(m,n),1,-1
        if (i == 0) then
          print *,"error in svdcmp",m,n,i
        endif
        l=i+1
        g=w(i)
        do j=l,n
          a(i,j)=0.0
        enddo
        if(g /= 0.0)then
           g=1.0/g
           do j=l,n
              s=0.0
              do k=l,m
                 s=s+a(k,i)*a(k,j)
              enddo
              f=(s/a(i,i))*g
              do k=i,m
                 a(k,j)=a(k,j)+f*a(k,i)
              enddo
           enddo
           do j=i,m
              a(j,i)=a(j,i)*g
           enddo
        else
           do j= i,m
              a(j,i)=0.0
           end do
        endif
        a(i,i)=a(i,i)+1.0
      enddo
      fortynine: do k=n,1,-1
        fortyeight: do its=1,30
          do l=k,1,-1
             nm=l-1
             if((abs(rv1(l))+anorm) == anorm) goto 2
             if((abs(w(nm))+anorm) == anorm) goto 1
          enddo
1         c=0.0
          s=1.0
          h=0.0 !liusy+
          do i=l,k
            f=s*rv1(i)
            rv1(i)=c*rv1(i)
            if((abs(f)+anorm) == anorm) goto 2
            g=w(i)
            h=pythag(f,g)
            w(i)=h
            h=1.0/h
            c= (g*h)
            s=-(f*h)
            do j=1,m
              y=a(j,nm)
              z=a(j,i)
              a(j,nm)=(y*c)+(z*s)
              a(j,i)=-(y*s)+(z*c)
            enddo
          enddo
2         z=w(k)
          if (l == k)then
            if(z < 0.0)then
              w(k)=-z
              do j=1,n
                 v(j,k)=-v(j,k)
              enddo
            endif
            goto 3
          endif
          if(its == 30) then
             ierr = 1
             return
          endif
          x=w(l)
          nm=k-1
          y=w(nm)
          g=rv1(nm)
          h=rv1(k)
          f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y)
          g=pythag(f,1.0)
          f=((x-z)*(x+z)+h*((y/(f+sign(g,f)))-h))/x
          c=1.0
          s=1.0
          do j=l,nm
            i=j+1
            g=rv1(i)
            y=w(i)
            h=s*g
            g=c*g
            z=pythag(f,h)
            rv1(j)=z
            c=f/z
            s=h/z
            f= (x*c)+(g*s)
            g=-(x*s)+(g*c)
            h=y*s
            y=y*c
            do jj=1,n
              x=v(jj,j)
              z=v(jj,i)
              v(jj,j)= (x*c)+(z*s)
              v(jj,i)=-(x*s)+(z*c)
            enddo
            z=pythag(f,h)
            w(j)=z
            if(z/=0.0)then
              z=1.0/z
              c=f*z
              s=h*z
            endif
            f= (c*g)+(s*y)
            x=-(s*g)+(c*y)
            do jj=1,m
              y=a(jj,j)
              z=a(jj,i)
              a(jj,j)= (y*c)+(z*s)
              a(jj,i)=-(y*s)+(z*c)
            end do
          end do
          rv1(l)=0.0
          rv1(k)=f
          w(k)=x
        end do fortyeight
3       continue
        end do fortynine
!----------------------------------------------------------------
END SUBROUTINE svdcmp


 
!============================================================================
REAL FUNCTION pythag(a,b)

   IMPLICIT NONE
!----------------------------------------------------------------
   REAL,INTENT(IN)                                    :: a,b
!
!---local variables
!
   REAL                                               :: absa,absb,pyth
!----------------------------------------------------------------

       absa=abs(a)
       absb=abs(b)
       if (absa > absb) then
           pyth = absa * sqrt(1.0+(absb/absa)**2)
       else
           if (absb == 0) then
               pyth = 0.0
           else
               pyth = absb*sqrt(1.0 + (absa/absb)**2)
           endif
       endif

       pythag=pyth

!----------------------------------------------------------------
END FUNCTION PYTHAG



!============================================================================
SUBROUTINE OPTIC(ice_shape_index1,rwp0,de,tc,frequency,SSALB,GG,TAU)
!
!---Purpose:
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,PARAMETER                                  :: MAXANG= 180,         &
                                                         MXLEG = 15
   REAL,PARAMETER                                     :: EP    = .622,        &
                                                         RG    = 0.029261,    &
                                                         C     = 0.2302585,   &
                                                         SIGMA = 1.
   INTEGER,INTENT(IN)                                 :: ice_shape_index1
   !---ICE PARTICLE SHAPE
   REAL,INTENT(IN)                                    :: frequency,rwp0,de,tc
   REAL,INTENT(OUT)                                   :: ssalb,gg,tau
!
!---local variables
!
   REAL                                               :: C1,C2,EX,FACTOR,pi,  &
                                                         AL2M3,AL2P3,bin2l,sc,&
                                                         SLAM,WAVELENGTH,     &
                                                         dssalbl_dd,rwp,      &
                                                         gg_nonsphere
   REAL,DIMENSION(MAXANG)                             :: A1,A2,A3,A4,B1,B2,   &
                                                         AMU,WT,X,YP,Y,TEMP,  &
                                                         A1S,B1S,A3S,B2S
!---Arrays defined for scattering matrix of single phase particles
!   Since particles are assumed spherical, only four elements are needed
!  (A1S = A2S, A3S = A4S)

   REAL,DIMENSION(0:MXLEG)                            :: GSF_A1,GSF_A2,GSF_A3,&
                                                         GSF_A4,GSF_B1,GSF_B2
   INTEGER                                            :: itype,j,L,nang,nleg, &
                                                         ice_shape_index
!liusy-       DOUBLE PRECISION PL00,PL02,PL22
! for generlized spherical functions
!----------------------------------------------------------------

       NANG = 180
       PI   = ACOS( -1.0 )
       C1   = 0.5
       C2   = 1.0E-6 / (PI*1.0E-3)

       ice_shape_index = ice_shape_index1

!      Gaussian quadrature for computing the expansion
!      coefficients of the phase matrix elements
       CALL QGAUS(NANG,AMU,WT)

       WAVELENGTH = 30.0/frequency     ! in GHz --> cm

!      Converting the wavelength from centimeter to milimeter

       SLAM = WAVELENGTH * 10
!      Frequency in GHz
!       F = 300 / SLAM


        NLEG = 15

        rwp = rwp0*0.001

!      looping over to  computing extinction and scattering coeff., and phase
!      function for each layer.

!         Derive the absoption coefficient due to oxygen and water vapor

          CALL ZEROIT(A1,MAXANG)
          CALL ZEROIT(A2,MAXANG)
          CALL ZEROIT(A3,MAXANG)
          CALL ZEROIT(A4,MAXANG)
          CALL ZEROIT(B1,MAXANG)
          CALL ZEROIT(B2,MAXANG)

!           for rain set itype = 2
!           for snow set itype = 5
           itype = 5   !!!!!!!!!!!!!!!!!

            CALL ATHYD(ice_shape_index,SLAM,AMU,rwp,de,itype,tc,EX,SC, &
                    NANG,A1S,B1S,A3S,B2S,gg_nonsphere)

            DO J=1,NANG
               A1(J) = A1(J) + A1S(J)
               A2(J) = A2(J) + A1S(J)
               A3(J) = A3(J) + A3S(J)
               A4(J) = A4(J) + A3S(J)
               B1(J) = B1(J) + B1S(J)
               B2(J) = B2(J) + B2S(J)
            END DO

          IF(EX.GT.0.0) THEN
!           single scattering albedo
            SSALB = SC/EX
          ELSE
            SSALB = 0
          END IF
          IF (SSALB .GT. 1.0) SSALB = 1.0


          IF (SSALB .GT. 0.0001) THEN


! line 239  normalized phase function by the scattering coeff.
           DO J = 1, NANG
             A1(J) = SLAM*SLAM*C2*A1(J)/SC
             A2(J) = SLAM*SLAM*C2*A2(J)/SC
             A3(J) = SLAM*SLAM*C2*A3(J)/SC
             A4(J) = SLAM*SLAM*C2*A4(J)/SC
             B1(J) = SLAM*SLAM*C2*B1(J)/SC
             B2(J) = SLAM*SLAM*C2*B2(J)/SC
           ENDDO

!  check the phase matrix to see if it is normlaized

           FACTOR = 0

           DO J = 1,NANG
             FACTOR =  FACTOR + A1(J)*WT(J)
           END DO
           FACTOR = .5*FACTOR - 1.0

!           IF(FACTOR.GT.0.1) PRINT*, 'OPTIC -- PHASE MATRIX NOT NORMALIZED'

           DO L = 0, NLEG
!c             compute the factor of (2*L+1)/2
               bin2l = .5*(2.0*L+1)
               GSF_A1(L) = 0
               GSF_A4(L) = 0
               AL2P3 = 0
               AL2M3 = 0
               GSF_B1(L) = 0
               GSF_B2(L) = 0
               DO J = 1, NANG
                  GSF_A1(L) = GSF_A1(L) + WT(J) * A1(J) &
                        * REAL(PL00(dble(AMU(J)),L))
                  GSF_A4(L) = GSF_A4(L) + WT(J) * A4(J) &
                        * REAL(PL00(dble(AMU(J)),L))
                  IF(L.LT.2) GOTO 30
                  AL2P3 = AL2P3 + WT(J) * (A2(J) + A3(J)) &
                        * REAL(PL22(dble(AMU(J)),L,1.d0))
                  AL2M3 = AL2M3 + WT(J) * (A2(J) - A3(J)) &
                        * REAL(PL22(dble(AMU(J)),L,-1.d0))
                  GSF_B1(L) = GSF_B1(L) + WT(J) * B1(J) &
                        * REAL(PL02(dble(AMU(J)),L))
                  GSF_B2(L) = GSF_B2(L) + WT(J) * B2(J) &
                        * REAL(PL02(dble(AMU(J)),L))
30             ENDDO

               GG = GSF_A1(1)*0.5/(2*L+1)

               if (ice_shape_index .ge. 1 ) GG = gg_nonsphere

               GSF_A1(L) = bin2l*GSF_A1(L)
               GSF_A2(L) = bin2L*.5*(AL2P3 + AL2M3)
               GSF_A3(L) = bin2L*.5*(AL2P3 - AL2M3)
               GSF_A4(L) = bin2l*GSF_A4(L)
               GSF_B1(L) = bin2l*GSF_B1(L)
               GSF_B2(L) = bin2l*GSF_B2(L)

35         END DO
          ELSE
           DO L = 0, NLEG
              GSF_A1(L) = 0
              GSF_A2(L) = 0
              GSF_A3(L) = 0
              GSF_A4(L) = 0
              GSF_B1(L) = 0
              GSF_B2(L) = 0
           END DO
! CORRECTION (12/10/06)
           GG = 0.0
          END IF

          TAU = EX

print*,'?',TAU ,GG

!----------------------------------------------------------------
END SUBROUTINE OPTIC



!============================================================================
SUBROUTINE ATHYD(ice_shape_index,SLAM,AMU,W,DE,ITYPE,TV,EXQC,SCQC,NAN,  &
                 A1S,B1S,A3S,B2S,MEAN_GGNS)
!
!---Purpose:
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL,PARAMETER                                     :: REFMED=1.0 
   INTEGER,INTENT(IN)                                 :: nan,        & !---NUMBER OF SCATTERING ANGLE
                                                         itype         !---HYDROMETEOR TYPES: 1=CLOUD LIQUID
                                                                       !                      2=RAIN WATER
                                                                       !                      3=CLOUD ICE
                                                                       !                      4=SNOW
                                                                       !                      5=GRAUPEL
   REAL,INTENT(IN)                                    :: w,          & !---WATER CONTENT (kg/m^3)
                                                         tv,         & !---TEMPERATURE (K)
                                                         slam          !---WAVELENGH (mm)
   REAL,DIMENSION(*),INTENT(IN)                       :: amu           !---COSINE OF SCATTERING ANGLE

   REAL,INTENT(OUT)                                   :: exqc,       & !---EXTINCTION COEFF. (1/km)
                                                         scqc          !---SCATTERING COEFF. (1/km)
   REAL,DIMENSION(*),INTENT(OUT)                      :: A1S,        & !---INTEGRATED VALUE OF A1P (each particle) OVER THE SIZE DISTRIBUTION
                                                         A3S,        & !---INTEGRATED VALUE OF A3P (each particle) OVER THE SIZE DISTRIBUTION
                                                         B1S,        & !---INTEGRATED VALUE OF B1P (each particle) OVER THE SIZE DISTRIBUTION
                                                         B2S           !---INTEGRATED VALUE OF B2P (each particle) OVER THE SIZE DISTRIBUTION
!
!---local variables
!
   INTEGER,PARAMETER                                  :: MPC=401,    & !---NUMBER OF PARTICLE DIAMETERS BETWEEN DL AND DU AT WHICH SCATTERING 
                                                                       !   EXTINCTION EFFICIENTS AND PHASE MATRIX ARE COMPUTED
                                                         MAXANG=180
   REAL,DIMENSION(5)                                  :: DENS,       & !---MASS DENSITY (kg/M^3) FOR DIFFERENT TYPES OF HYDROMETEORS 
                                                                       !   (Cloud and rain water=1000, cloud ice=920,snow = 100  and graupel = 600)
                                                         EXPO,       & !---EXPONENT IN GAMMA SIZE DISTRIBUTION
                                                         DL,         & !---LOW LIMIT OF DIAMETER FOR INTEGRATION (mm)(0.005)
                                                         DU            !---UPPER LIMIT OF DIAMETER FOR INTEGRATION (mm)(2.005)
   REAL                                               :: DE,         & !---EFFECTIVE DIAMETER IN SIZE DISTRIBUTION (mm)
                                                         DM,         & !---MODE DIAMETER IN SIZE DISTRIBUTION (mm)
                                                         AN0C
   REAL,DIMENSION(mpc)                                :: QE,         &
                                                         QS,         &
                                                         GGNS          !---FOR NON-SPHERE
   REAL,DIMENSION(maxang,mpc)                         :: A1P,        & !---A1 FOR PARTICLES WITH DISCRETE DIAMETERS
                                                         B1P,        & !---B1 FOR PARTICLES WITH DISCRETE DIAMETERS
                                                         A3P,        & !---A3 FOR PARTICLES WITH DISCRETE DIAMETERS
                                                         B2P           !---B2 FOR PARTICLES WITH DISCRETE DIAMETERS
   REAL                                               :: MEAN_GGNS,  & !---FOR NON-SPHERE     
                                                         SIZE_PARM,AL2M3,     &
                                                         AL2P3,bin2l,         &
                                                         AREA,freq,DD,F,F3,F0,&
                                                         PI,RNU,TEMP,TL,TU,   &
                                                         gg_nonsphere
   INTEGER                                            :: ice_shape_index,i,   &
                                                         RHOH
   COMPLEX                                            :: REF,REFQC,RK
!----------------------------------------------------------------
   DENS  = [1000,1000,920,100,600]
!       DATA DE/10E-3,500E-3,10E-3,500E-3,500E-3/
   EXPO  = [2,1,2,1,1]
   DL    = [0.005,0.005,0.005,0.005,0.005]
!       DATA DU/2.005,2.005,2.005,2.005,2.005/
   DU    = [2.005,4.005,2.005,2.005,2.005]

!      FOR CLOUD LIQUID WATER  USE AN EMPIRICAL MODEL
       IF(ITYPE.EQ.1) THEN
!        References for this model
         F0=160*EXP(7.2*(1-287.0/TV))
         F = 300/SLAM
         EXQC = .0241*W*1.0E3*F**2*F0/(F**2+F0**2)
         SCQC = 0
         CALL ZEROIT(A1S,NAN)
         CALL ZEROIT(B1S,NAN)
         CALL ZEROIT(A3S,NAN)
         CALL ZEROIT(B2S,NAN)
         RETURN
       ENDIF
! FOR NON-SPHERE
      CALL ZEROIT(GGNS,MPC)

       PI = ACOS(-1.0)
       TEMP = TV - 273.15
!
       IF(ITYPE.LE.2) THEN

         CALL IWREF(1,SLAM,TEMP,REFQC)
         RHOH = 1000    ! water phase droplet
       ELSE
         CALL IWREF(0,SLAM,TEMP,REF)
         RK = (REF*REF - 1.0) / (REF*REF+2.0)
         RHOH = DENS(ITYPE)
         REFQC = CSQRT(1.0+3.0*RHOH*RK/(920-RHOH*RK))
       END IF
       REFQC = REFQC/REFMED
       TU = DU(ITYPE)
       TL = DL(ITYPE)
       DD = (TU-TL)/(MPC - 1)
       DO I = 1, MPC
         SIZE_PARM = PI*(DD*(I-1)+TL)/SLAM     ! size parameter

         CALL SCAT_MIE(SIZE_PARM,REFQC,AMU,NAN,QE(I),QS(I), &
     &                 A1P(1,I),B1P(1,I),A3P(1,I),B2P(1,I))

!!!!!!!! Test the impact of snow shape

         AREA = PI*(DD*(I-1)+TL)*(DD*(I-1)+TL)/4.

         if (ice_shape_index .ge. 1 )  &

      CALL NONSPHERE(ice_shape_index,SIZE_PARM,AREA,SLAM,freq,QE(I),QS(I),GGNS(I)) !gg_nonsphere)

       ENDDO
!
       RNU  = EXPO(ITYPE)
       DM   = RGAMMA(RNU)*DE/RGAMMA(RNU + 1.0)  ! Mode diamter
       F3   = RGAMMA(RNU+3) / RGAMMA(RNU)
       AN0C = 1.0E9*6.0*W/(RHOH*PI*F3*DM**3)  ! number of particles/m**3/mm**(1-gamma)

!
!      AVERAGE OVER THE ENTIRE SIZE DISTRIBUTION TO GET THE MEAN VALUES
       CALL CLOUD_INT(QE,QS,A1P,B1P,A3P,B2P,TL,TU,MPC,NAN,MAXANG, &
     &                AN0C,DM,RNU,EXQC,SCQC,A1S,B1S,A3S,B2S,GGNS,MEAN_GGNS)

!----------------------------------------------------------------
END SUBROUTINE ATHYD



!============================================================================
SUBROUTINE IWREF(IFLAG,WAVEL,TEMP,REIND)
!
!--Purpose: FOR WATER AT MICROWAVE FREQUENCIES ONLY
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: iflag                   !---1 FOR WATER,0 FOR ICE
   REAL,INTENT(IN)                                    :: WAVEL
   REAL,INTENT(INOUT)                                 :: TEMP                    !---TEMPERATURE IN C
   COMPLEX,INTENT(OUT)                                :: REIND                   !---REFRACTIVE INDEX
!
!---local variables
!
   REAL                                               :: ALAM,                &  !---WAVELENGTH IN CM
                                                         CPI,SIGMA,EINF,ALPHA,&
                                                         BRIN,ALS,T,EPS,SLAM, &
                                                         SINAL,COSAL,X,AIMEPS,&  
                                                         REPS                    !---REPS,AIEPS   REAL,IMAG EFFECTIVE DIELECTRIC CONST
   COMPLEX                                            :: EP                      !---EFFECTIVE DIELECTRIC CONST
!----------------------------------------------------------------

       CPI = 4.0*ATAN(1.0)
       ALAM = WAVEL / 10.
       IF(TEMP.LE.-55) TEMP = -55
!B.YAN
!       IF(TEMP.LT.-100.0.OR.TEMP.GT.60.0)  &
!     &  CALL ERRMSG('IWREF--TEMPERATURE MIGHT BE OUT OF RANGE',.FALSE.)

       IF (IFLAG.EQ.1) THEN
!        P. S. Ray, 1973, gave Eqs (5) and (6) for computing
!        water refractive index for centimeter waves.
!        The parameteres  are given by Eq. (4) and (7).
!        SET UP CONSTANTS.
         SIGMA=12.5664E08
         EINF=5.27137+.0216474*TEMP-.00131198*(TEMP**2)  ! Eq. (7a)
         ALPHA=-16.8129/(TEMP+273.)+.0609265             ! Eq. (7b)
         BRIN=2513.98D0/(TEMP+273.000)                   ! Eq. (7c)
         ALS=.00033836*EXP(BRIN)
         T=TEMP-25.0
         EPS=78.54*(1.-4.579E-03*T+1.19E-05*(T**2)-2.8E-08*(T**3)) ! Eq. (4)
       ELSE
!        Temperature dependence of ice refractive index needed to be considered
!        The method given by P. S. Ray, 1973, Eq. (1), (5), (6). Parameteres
!        are given by Eq. (12).
!        SET UP CONSTANTS.
         EINF=3.168                                               ! Eq. (12a)
         ALPHA=0.288+0.0052*TEMP+0.00023*TEMP*TEMP                ! Eq. (12b)
         SIGMA=1.26*EXP(-12500/((TEMP+273.0)*1.9869))             ! Eq. (12c)
         ALS=9.990288*1.0E-4*EXP(13200/((TEMP+273.0)*1.9869))     ! Eq. (12d))
         EPS=203.168+2.5*TEMP+0.15*TEMP*TEMP                      ! Eq. (12e)
       ENDIF
!      SET UP COMMON PARAMETERS.
       SLAM=(ALS/ALAM)**(1.0-ALPHA)
       SINAL=SIN(ALPHA*CPI/2.000)
       COSAL=COS(ALPHA*CPI/2.000)
!      CALCULATE RE(EPSLON)
       REPS=(EPS-EINF)*(1.0+SLAM*SINAL)
       X=1.0+2.0*SLAM*SINAL+SLAM**2
       REPS=REPS/X
       REPS=REPS + EINF                            ! EQ. (5)
!      CALCULATE IM(EPSLON)
       AIMEPS=(EPS-EINF)*SLAM*COSAL
       AIMEPS=(AIMEPS/X)+SIGMA*ALAM/18.8496E10   ! EQ. (6)
!      Complex dielectric function (or relative permittivity)
!      epsilon=reps+i*aimeps. Since the complex refractive index is
!      related to epsilon by m=(epsilon)**0.5
!      If (iflag.eq.0) EP=CMPLX(3.17,0.00855)    ! Approximation for ice.
       EP=CMPLX(REPS,AIMEPS)
       REIND=CSQRT(EP)

!----------------------------------------------------------------
END SUBROUTINE IWREF



!============================================================================
SUBROUTINE CLOUD_INT(QE,QS,A1P,B1P,A3P,B2P,TL,TU,N,NANG,MXANG, &
                     AN0,DM,RNU,EX,SC,A1S,B1S,A3S,B2S,GGNS,MEAN_GGNS)
!
!---Purpose:
!
   IMPLICIT NONE
!----------------------------------------------------------------
!      INPUT VARIABLES
!        SLAM     :  WAVELENGH (mm)
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: MXANG,   & !---
                                                         N,       & !---NO. OF DROPLETS RADIUS, DIMENSION OF X AND ALSO
                                                         NANG       !---NO OF SCATTERING ANGLES, 1ST DIMENSION OF S1,...
   REAL,INTENT(IN)                                    :: TL,TU,   & !---RADIUS LOW AND UPPER LIMIT (mm)
                                                         AN0,DM,RNU !---THREE PARAMETERS RELATED TO PARTICLE DISTRIBUTION
                                                                    !   (AN0:m^-3, DM: mm),
                                                                    !   N(D)=N0*D**(RNU-1)EXP(-D/DM)/GAMMA(RNU)*DM**RNU
                                                                    !   2ND DIMESION OF S1,S2,S3 AND S4
   REAL,DIMENSION(MXANG,N),INTENT(IN)                 :: A1P,     & !---ELEMENTS OF PHASE MATRIX FOR DISCRETE PARTICLES
                                                         B1P,     & !---ELEMENTS OF PHASE MATRIX
                                                         A3P,     & !---ELEMENTS OF PHASE MATRIX
                                                         B2P        !---ELEMENTS OF PHASE MATRIX
   REAL,DIMENSION(N),INTENT(IN)                       :: QS,      & !---SCATTERING EFFICIENT FACTOR
                                                         QE,      & !---EXTINCTION EFFICIENT FACTOR
                                                         GGNS       !---ASYMMETRY FACTO
   REAL,DIMENSION(MXANG),INTENT(OUT)                  :: A1S,     & !---ELEMENT OF PHASE MATRIX AVERAGED OVER SIZE DISTRIBUTION
                                                         B1S,     & !---(1/m**3)
                                                         A3S,     & !---
                                                         B2S
   REAL,INTENT(OUT)                                   :: MEAN_GGNS,&
                                                         EX,      & !---SCATTERING COEFFICIENT (1/km)
                                                         SC         !---EXTINCTION COEFFICIENT (1/km)
!
!---local variables
!
   INTEGER,PARAMETER                                  :: LN=401     !---INTEROPOLATION
   REAL                                               :: RK,      & !---NORMALIZATION FACTOR  = SLAM^2/(PI*SCATTERING COEFF.)
                                                         CC,DD,PI,&
                                                         SIGMA,SLP1,SLPN
   REAL,DIMENSION(LN)                                 :: X,       & !---DIAMETERS OF POLYDISPERSE PARTICLES (mm)
                                                         YP,Y, TEMP
   INTEGER                                            :: NR,i,nsca

!----------------------------------------------------------------
   
       PI=2.*ASIN(1.0)
!      For converting SC,EX from 1/m to 1/km and also some contants in
!      modified gamma distribution
       CC=PI*AN0*1.0E-3/(4.0*RGAMMA(RNU)*DM**RNU)
       SIGMA=1.0
       DD=(TU-TL)/(N-1)
       DO I=1,N
         X(I)=DD*(I-1)+TL
         Y(I)=X(I)*X(I)*QE(I)*X(I)**(RNU-1)*EXP(-X(I)/DM)
       ENDDO
       CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
       EX = CURVI(TL,TU,N,X,Y,YP,SIGMA)
       DO I = 1, N
         Y(I) = X(I)*X(I)*QS(I)*X(I)**(RNU-1)*EXP(-X(I)/DM)
       ENDDO
       CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
       SC=CURVI(TL,TU,N,X,Y,YP,SIGMA)
       IF(SC.EQ.0.0) THEN
         RK=0.0
       ELSE
         RK=AN0/(RGAMMA(RNU)*DM**RNU)
       ENDIF
       SC=CC*SC
       EX=CC*EX

!B.YAN ADDED (10/30/2006)
         DO I=1,N
         X(I)=DD*(I-1)+TL
         Y(I)=X(I)*X(I)*GGNS(I)*X(I)**(RNU-1)*EXP(-X(I)/DM)
       ENDDO
       CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
       MEAN_GGNS = CURVI(TL,TU,N,X,Y,YP,SIGMA)
     

       DO NSCA=1,NANG
         DO NR=1,N
            Y(NR)=A1P(NSCA,NR)*X(NR)**(RNU-1)*EXP(-X(NR)/DM)
         ENDDO
         CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
         A1S(NSCA)=RK*CURVI(TL,TU,N,X,Y,YP,SIGMA)
         DO NR=1,N
            Y(NR)=B1P(NSCA,NR)*X(NR)**(RNU-1)*EXP(-X(NR)/DM)
         ENDDO
         CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
         B1S(NSCA)=RK*CURVI(TL,TU,N,X,Y,YP,SIGMA)
         DO NR=1,N
            Y(NR)=A3P(NSCA,NR)*X(NR)**(RNU-1)*EXP(-X(NR)/DM)
         ENDDO
         CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
         A3S(NSCA)=RK*CURVI(TL,TU,N,X,Y,YP,SIGMA)
         DO NR=1,N
            Y(NR)=B2P(NSCA,NR)*X(NR)**(RNU-1)*EXP(-X(NR)/DM)
         ENDDO
         CALL CURV1(N,X,Y,SLP1,SLPN,YP,TEMP,-SIGMA)
         B2S(NSCA)=RK*CURVI(TL,TU,N,X,Y,YP,SIGMA)
       ENDDO
       RETURN
!----------------------------------------------------------------
END SUBROUTINE CLOUD_INT



!============================================================================
SUBROUTINE SCAT_MIE(X,REFREL,AMU,NANG,QEXT,QSCA,A1P,B1P,A3P,B2P)
!
!---Purpose:   CALCULATES EFFICIENCIES FOR EXTINCTION, SCATTERING COEFF.
!              BACKSCATTERING CROSS SECTION, SCATTERING FUNCTION
!              AND THE ELEMENTS OF PHASE MATRIX. FOR GIVEN SIZE PARAMETERS AND
!              RELATIVE REFRACTIVE INDICES, ALL BESSEL FUNCTIONS COMPUTED BY
!              DOWNWARD RECURRECE 
!              ALL NOTATIONS ARE CONSISTENT WITH THE ORIGINAL PROGRAMS
!---Reference: Bohren G. and Hoffmen. 1983: Light scattering y small particles. ...
!              Dave J. V. 1970: Intensity and Polarization of  the radiation emerging from a plane-parallel
!                 atmosphere containing monodispersed aerosols. Applied Optics, V. 9, 2673-2684.
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: NANG       !---NO. OF SCATTERING ANGLES
   REAL,INTENT(IN)                                    :: x          !---SIZE PARAMETER
   REAL,DIMENSION(NANG),INTENT(IN)                    :: AMU        !---OSIN VALUES OVER SCATTERING ANGLES
   COMPLEX,INTENT(IN)                                 :: REFREL     !---RELATIVE REFRACTIVE INDEX
   REAL,INTENT(OUT)                                   :: QEXT,    & !---EXTINCTION EFFICIENT FACTOR
                                                         QSCA       !---SCATTERING EFFICIENT FACTOR
   REAL,DIMENSION(*),INTENT(OUT)                      :: A1P,     & !---AN ELEMENT OF PHASE MATRIX
                                                         B1P,     & !---AN ELEMENT OF PHASE MATRIX
                                                         A3P,     & !---AN ELEMENT OF PHASE MATRIX
                                                         B2P        !---AN ELEMENT OF PHASE MATRIX
                                                                    !   See following matrix notation
                                                                    !   (A1P   B1P    0     0 )
                                                                    !   (B1P   A1P    0     0 )
                                                                    !   (0      0    A3P   B2P)
                                                                    !   (0      0   -B2P   A3P)
!
!---local varialbes
!
   INTEGER,PARAMETER                                  :: MM=2000
   COMPLEX,DIMENSION(MM)                              :: S1,      & !---ELEMENTS IN AMPLITUDE SCATTERING MATRIX
                                                         S2         !---ELEMENTS IN AMPLITUDE SCATTERING MATRIX
                                                                    !---S3: ZERO FOR SPHERICAL PARTICLE
                                                                    !---S4: ZERO FOR SPHERICAL PARTICLE
                                                                    !   (S2    S3)
                                                                    !   (S4    S1)
   DOUBLE PRECISION,DIMENSION(MM)                     :: PI0,PI1
   DOUBLE PRECISION                                   :: DX,PSI0,PSI1,PSI, &
                                                         APSI0,APSI1,DN,   &
                                                         APSI
   COMPLEX                                            :: Y,XI0,XI1,XI,AN,BN
   COMPLEX,DIMENSION(3000)                            :: D
   REAL                                               :: XSTOP,NSTOP,YMOD, &
                                                         CHI0,CHI,FN,CHI1,RK
   REAL,DIMENSION(MM)                                 :: PI,TAU     !---original type is real. While need double check if should be double precision
   INTEGER                                            :: n,NMX,nn,rn,i,j 
!----------------------------------------------------------------
       DX=DBLE(X)
       Y=X*REFREL
       XSTOP=X+4*X**0.3333+2.0
       NSTOP=XSTOP
       YMOD=CABS(Y)
       NMX=AMAX1(XSTOP,YMOD)+15
       D(NMX)=CMPLX(0.0,0.0)
       NN=NMX-1
       DO N=1,NN
         RN=NMX-N+1
         D(NMX-N)=(RN/Y)-(1./(D(NMX-N+1)+RN/Y))
       ENDDO
       DO J=1,NANG
        PI0(J)=0.0
        PI1(J)=1.0
       ENDDO
       DO J=1,NANG
        S1(J)=CMPLX(0.0,0.0)
        S2(J)=CMPLX(0.0,0.0)
       ENDDO
       PSI0=DCOS(DX)
       PSI1=DSIN(DX)
       CHI0=-SIN(X)
       CHI1=COS(X)
       APSI0=PSI0
       APSI1=PSI1
       XI0=CMPLX(APSI0,-CHI0)
       XI1=CMPLX(APSI1,-CHI1)
       N=1
       QSCA=0.
       QEXT=0.
10     DN=N
       RN=N
       FN=(2.*RN+1.)/(RN*(RN+1.))
       PSI=(2.0*DN-1.)*PSI1/DX-PSI0
       APSI=PSI
       CHI=(2.0*RN-1.)*CHI1/X-CHI0
       XI=CMPLX(APSI,-CHI)
       AN=(D(N)/REFREL+RN/X)*APSI-APSI1
       AN=AN/((D(N)/REFREL+RN/X)*XI-XI1)
       BN=(REFREL*D(N)+RN/X)*APSI-APSI1
       BN=BN/((REFREL*D(N)+RN/X)*XI-XI1)
       DO J=1,NANG
         PI(J)=PI1(J)
         TAU(J)=RN*AMU(J)*PI(J)-(RN+1.)*PI0(J)
         S1(J)=S1(J)+FN*(AN*PI(J)+BN*TAU(J))
         S2(J)=S2(J)+FN*(AN*TAU(J)+BN*PI(J))
       ENDDO
       QSCA=QSCA+(2.*RN+1.)*(CABS(AN)*CABS(AN)+CABS(BN)*CABS(BN))
       QEXT=QEXT+(2.*RN+1.)*REAL(AN+BN)
       PSI0=PSI1
       PSI1=PSI
       APSI1=PSI1
       CHI0=CHI1
       CHI1=CHI
       XI1=CMPLX(APSI1,-CHI1)
       N=N+1
       RN=N
       DO J=1,NANG
         PI1(J)=((2.*RN-1.)/(RN-1.))*AMU(J)*PI(J)-RN*PI0(J)/(RN-1.)
         PI0(J)=PI(J)
       ENDDO
       IF(N-1-NSTOP) 10,20,20
20     QSCA=(2./(X*X))*QSCA
       QEXT=(2./(X*X))*QEXT
       RK=4.0/(QSCA*X*X)
       DO I = 1, NANG
         A1P(I)=0.5*(S2(I)*CONJG(S2(I))+S1(I)*CONJG(S1(I)))   ! I, Q, U, v
         B1P(I)=0.5*(S2(I)*CONJG(S2(I))-S1(I)*CONJG(S1(I)))
         A3P(I)=REAL(S1(I)*CONJG(S2(I)))
         B2P(I)=-AIMAG(S2(I)*CONJG(S1(I)))
       ENDDO

       RETURN
!----------------------------------------------------------------
END SUBROUTINE SCAT_MIE



!============================================================================
SUBROUTINE  ZEROIT( a, length )
!
!---Purpose: zeros a real array -a- having -length- elements
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: length
   REAL,DIMENSION(length),INTENT(OUT)                 :: a
!
!---locla variables
!
   INTEGER                                            :: i
!----------------------------------------------------------------
   do i = 1,length
      a( i ) = 0.0
   end do !---i
!-----------------------------------------------------------
END SUBROUTINE  ZEROIT



!============================================================================
SUBROUTINE CURV1 (n,x,y,slp1,slpn,yp,temp,sigma)
!
!---Purpose: this utility package contains spline interpolation, differentiation and integration
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: n
   REAL,INTENT(IN)                                    :: sigma, slp1,slpn
   REAL,DIMENSION(N),INTENT(IN)                       :: x,y
   REAL,DIMENSION(N),INTENT(OUT)                      :: yp,temp
!
!---local variables
!
   INTEGER                                            :: nn,nm1,np1,ibak,i
   REAL                                               :: delx1,dx1,slpp1,slppn,  &
                                                         sigmap,dels,exps,sinhs, &
                                                         sinhin,diag1,diagin,    &
                                                         spdiag,delx2,dx2,diag2, &
                                                         delx12,c1,c2,c3,        &
                                                         deln,delnm1,delnn
!----------------------------------------------------------------
        nn = n
        nm1 = nn-1
        np1 = nn+1
        delx1 = x(2)-x(1)
        dx1 = (y(2)-y(1))/delx1

!c       determine slopes if necessary

        if (sigma .lt. 0.) go to 107
        slpp1 = slp1
        slppn = slpn

!c       denormalize tension factor

103     sigmap = abs(sigma)*float(nn-1)/(x(nn)-x(1))

!c       set up right hand side and tridiagonal system for yp and
!c       perform forward elimination

        dels = sigmap*delx1
        exps = exp(dels)
        sinhs = .5*(exps-1./exps)
        sinhin = 1./(delx1*sinhs)
        diag1 = sinhin*(dels*.5*(exps+1./exps)-sinhs)
        diagin = 1./diag1
        yp(1) = diagin*(dx1-slpp1)
        spdiag = sinhin*(sinhs-dels)
        temp(1) = diagin*spdiag
        if (nn .eq. 2) go to 105
        do 104 i=2,nm1
         delx2 = x(i+1)-x(i)
         dx2 = (y(i+1)-y(i))/delx2
         dels = sigmap*delx2
         exps = exp(dels)
         sinhs = .5*(exps-1./exps)
         sinhin = 1./(delx2*sinhs)
         diag2 = sinhin*(dels*(.5*(exps+1./exps))-sinhs)
         diagin = 1./(diag1+diag2-spdiag*temp(i-1))
         yp(i) = diagin*(dx2-dx1-spdiag*yp(i-1))
         spdiag = sinhin*(sinhs-dels)
         temp(i) = diagin*spdiag
         dx1 = dx2
         diag1 = diag2
104     continue
105     diagin = 1./(diag1-spdiag*temp(nm1))
        yp(nn) = diagin*(slppn-dx1-spdiag*yp(nm1))

!c       perform back substitution

        do 106 i=2,nn
         ibak = np1-i
         yp(ibak) = yp(ibak)-temp(ibak)*yp(ibak+1)
106     continue
        return
107     if (nn .eq. 2) go to 108

!c       if no derivatives are given use second order polynomial
!c       interpolation on input data for values at endpoints.

        delx2 = x(3)-x(2)
        delx12 = x(3)-x(1)
        c1 = -(delx12+delx1)/delx12/delx1
        c2 = delx12/delx1/delx2
        c3 = -delx1/delx12/delx2
        slpp1 = c1*y(1)+c2*y(2)+c3*y(3)
        deln = x(nn)-x(nm1)
        delnm1 = x(nm1)-x(nn-2)
        delnn = x(nn)-x(nn-2)
        c1 = (delnn+deln)/delnn/deln
        c2 = -delnn/deln/delnm1
        c3 = deln/delnn/delnm1
        slppn = c3*y(nn-2)+c2*y(nm1)+c1*y(nn)
        go to 103
!!$c
!!$c       if only two points and no derivatives are given, use
!!$c       straight line for curve
!!$c
108     yp(1) = 0.
        yp(2) = 0.
        return
!----------------------------------------------------------------
END SUBROUTINE CURV1



!============================================================================
REAL FUNCTION CURVP(t,n,x,y,yp,sigma,it)
!
!---Purpose: interpolates a curve at 'x=t'
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: n,it
   REAL,INTENT(IN)                                    :: t,sigma
   REAL,DIMENSION(N),INTENT(IN)                       :: x,y,yp
!
!---local variables
!
   INTEGER                                            :: i,i1
   REAL                                               :: s,sigmap,del1,del2,dels,   &
                                                         exps1,sinhd1,exps,sinhd2,  &
                                                         sinhs
!----------------------------------------------------------------


        data i1/2/

        s = x(n)-x(1)
        sigmap = abs(sigma)*float(n-1)/s
        if (it .eq. 1) i1 = 2

!c       search for interval

101     do 102 i=i1,n
         if (x(i)-t) 102,102,103
102     continue
        i = n
103     if (x(i-1).le.t .or. t.le.x(1)) go to 104

        i1 = 2
        go to 101

!c       set up and perform interpolation

104     del1 = t-x(i-1)
        del2 = x(i)-t
        dels = x(i)-x(i-1)
        exps1 = exp(sigmap*del1)
        sinhd1 = .5*(exps1-1./exps1)
        exps = exp(sigmap*del2)
        sinhd2 = .5*(exps-1./exps)
        exps = exps1*exps
        sinhs = .5*(exps-1./exps)
        curvp = (yp(i)*sinhd1+yp(i-1)*sinhd2)/sinhs+ &
     &        ((y(i)-yp(i))*del1+(y(i-1)-yp(i-1))*del2)/dels
        i1 = i
        return
!----------------------------------------------------------------
END FUNCTION CURVP



!============================================================================
REAL FUNCTION CURVD(t,n,x,y,yp,sigma,it)
!
!---Purpose: differentiates a curve at a given point 'x=t'
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: n,it
   REAL,INTENT(IN)                                    :: sigma,t
   REAL,DIMENSION(N),INTENT(IN)                       :: x,y,yp
!
!---local variables
!
   INTEGER                                            :: i1,i
   REAL                                               :: s,sigmap,del1,del2,  &
                                                         dels,exps1,coshd1,   &
                                                         coshd2,exps,sinhs
!----------------------------------------------------------------
   i1 = 2
   s  = x(n)-x(1)

        sigmap = abs(sigma)*float(n-1)/s !---denormalize sigma

!c       if it.ne.1 start search where previously terminated,
!c       otherwise start from beginning
!c
        if (it .eq. 1) i1 = 2
!c
!c       search for interval
!c
101     do 102 i=i1,n
         if (x(i)-t) 102,102,103
102     continue
        i = n

!c       check to insure correct interval

103     if (x(i-1).le.t .or. t.le.x(1)) go to 104

!!$c       restart search  and reset i1
!!$c       ( input ""it"" was incorrect )
!!$c
        i1 = 2
        go to 101
!!$c
!!$c       set up and perform interpolation
!!$c
104     del1 = t-x(i-1)
        del2 = x(i)-t
        dels = x(i)-x(i-1)
        exps1 = exp(sigmap*del1)
        coshd1 = .5*(exps1+1./exps1)
        exps = exp(sigmap*del2)
        coshd2 = .5*(exps+1./exps)
        exps = exps1*exps
        sinhs = .5*(exps-1./exps)/sigmap
        curvd = (yp(i)*coshd1-yp(i-1)*coshd2)/sinhs+ &
     &        ((y(i)-yp(i))-(y(i-1)-yp(i-1)))/dels
        i1 = i
        return
!----------------------------------------------------------------
END FUNCTION CURVD



!============================================================================
REAL FUNCTION CURVI(xl,xu,n,x,y,yp,sigma)
!
!---Purpose: integrates a curv
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: n
   REAL,INTENT(IN)                                    :: xl,xu,sigma
   REAL,DIMENSION(N),INTENT(IN)                       :: x,y,yp
!
!---local variables
!
   INTEGER                                            :: nn,i,il,iu,ilp1,ium1,np1,np1mi
   REAL                                               :: s,sigmap,xxl,xxu,ssign,    &
                                                         del1,del2,dels,exps1,      &
                                                         coshd1,exps,coshd2,        &
                                                         sinhs,coshs,sum_value,     &
                                                         delu1,delu2,dell1,dell2,   &
                                                         coshu1,coshu2,coshl1,      &
                                                         coshl2
!----------------------------------------------------------------
       nn = n
       s = x(nn)-x(1)
       sigmap = abs(sigma)*float(nn-1)/s

!c      determine actual upper and lower bounds

       xxl = xl
       xxu = xu
       ssign = 1.
       if (xl .lt. xu) go to 101
       xxl = xu
       xxu = xl
       ssign = -1.
       if (xl .gt. xu) go to 101
         curvi = 0. !---return zero if upper and lower limits equal
         return
!c      search for proper intervals

101    do 102 i=2,nn
         if (x(i)-xxl) 102,103,103
102    continue
       i = nn
103    il = i
       np1 = nn+1
       do 104 i=2,nn
          np1mi = np1-i
          if (x(np1mi)-xxu) 105,105,104
104    continue
       i = nn
105    iu = nn+2-i
       if (il .eq. iu) go to 110

!c      integrate from xxl to x(il)

       sum_value = 0.
       if (xxl .eq. x(il)) go to 106
       del1 = xxl-x(il-1)
       del2 = x(il)-xxl
       dels = x(il)-x(il-1)
       exps1 = exp(sigmap*del1)
       coshd1 = .5*(exps1+1./exps1)
       exps = exp(sigmap*del2)
       coshd2 = .5*(exps+1./exps)
       exps = exps1*exps
       sinhs = sigmap*.5*(exps-1./exps)
       coshs = .5*(exps+1./exps)
       sum_value = (yp(il)*(coshs-coshd1)-yp(il-1)*(1.-coshd2))/sinhs+.5* &
     &      ((y(il)-yp(il))*(dels*dels-del1*del1)+ &
     &                                (y(il-1)-yp(il-1))*del2*del2)/dels
106    if (iu-il .eq. 1) go to 108

!c      integrate over interior intervals

       ilp1 = il+1
       ium1 = iu-1
       do 107 i=ilp1,ium1
         dels = x(i)-x(i-1)
         exps = exp(sigmap*dels)
         sinhs = sigmap*.5*(exps-1./exps)
         coshs = .5*(exps+1./exps)
         sum_value = sum_value+(yp(i)+yp(i-1))*(coshs-1.)/sinhs- &
     &         .5*((yp(i)+yp(i-1))-(y(i)+y(i-1)))*dels
107    continue
108    if (xxu .eq. x(iu-1)) go to 109
!c
!c      integrate from x(iu-1) to xxu
!c
       del1 = xxu-x(iu-1)
       del2 = x(iu)-xxu
       dels = x(iu)-x(iu-1)
       exps1 = exp(sigmap*del1)
       coshd1 = .5*(exps1+1./exps1)
       exps = exp(sigmap*del2)
       coshd2 = .5*(exps+1./exps)
       exps = exps1*exps
       sinhs = sigmap*.5*(exps-1./exps)
       coshs = .5*(exps+1./exps)
       sum_value = sum_value+(yp(iu)*(coshd1-1.)-yp(iu-1)*(coshd2-coshs))/sinhs+.5* &
     &      ((y(iu)-yp(iu))*del1*del1- &
     &                    (y(iu-1)-yp(iu-1))*(del2*del2-dels*dels))/dels
109    curvi = ssign*sum_value
       return

!c      integrate from xxl to xxu

110    delu1 = xxu-x(iu-1)
       delu2 = x(iu)-xxu
       dell1 = xxl-x(iu-1)
       dell2 = x(iu)-xxl
       dels = x(iu)-x(iu-1)
       exps1 = exp(sigmap*delu1)
       coshu1 = .5*(exps1+1./exps1)
       exps = exp(sigmap*delu2)
       coshu2 = .5*(exps+1./exps)
       exps = exps1*exps
       sinhs = sigmap*.5*(exps-1./exps)
       exps1 = exp(sigmap*dell1)
       coshl1 = .5*(exps1+1./exps1)
       exps = exp(sigmap*dell2)
       coshl2 = .5*(exps+1./exps)
       sum_value = (yp(iu)*(coshu1-coshl1)-yp(iu-1)* &
     &      (coshu2-coshl2))/sinhs+.5* &
     &      ((y(iu)-yp(iu))*(delu1*delu1-dell1 &
     &      *dell1)-(y(iu-1)-yp(iu-1))*(delu2*delu2 &
     &      -dell2*dell2))/dels
       go to 109
!----------------------------------------------------------------
END FUNCTION CURVI 



!============================================================================
SUBROUTINE QGAUS(m,gmu,gwt)
!
!---Purpose: compute weights and abscissae for ordinary Gaussian quadrature
!            (no weight function inside integral) on the interval [-1,1]
!---Reference: Davis, P.J. and P. Rabinowitz, Methods of Numerical
!              Integration, Academic Press, New York, pp. 113-114,1984.         
!---Methods: compute the abscissae as roots of the Legendre
!            polynomial p-sub-n using a cubically convergent
!            refinement of Newton's method.  compute the
!            weights from eq. 2.7.3.8 of Davis/Rabinowitz.
!---Accuracy: at least 13 significant digits
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(INOUT)                              :: m          !---order of quadrature rule
   REAL,DIMENSION(*),INTENT(OUT)                      :: gmu,     & !---array of abscissae
                                                         gwt        !---array of weights
!
!---local variables
!
   REAL                                               :: cona,pi,t
   DOUBLE PRECISION                                   :: tol,                 &  !---convergence criterion for Legendre poly root iteration
                                                                                 !   convergent version of Newton's method
                                                                                 !   ( seeking roots of Legendre polynomial )
                                                         x,xi,                &  !---successive iterates in cubically-
                                                         pm2,pm1,p,           &  !---3 successive Legendre polynomials
                                                         tmp,nnp1,ppr,en,p2pri
   INTEGER                                            :: lim,np1,k,nn
!----------------------------------------------------------------
!
!     internal variables
!
!       ppr       : derivative of Legendre polynomial
!       p2pri     : 2nd derivative of Legendre polynomial
!+------------------------------------------------------------------+
!liusy-      double   precision  d1mach

   pi = 0.0
      if ( pi.eq.0.0 )  then
         pi = 2. * asin(1.0)
         tol = 10. * d1mach(3)
      end if
      if ( m.le.1 )  then
         m = 1
         gmu( 1 ) = 0.5
         gwt( 1 ) = 1.0
         return
      end if
      en   = m
      np1  = m + 1
      nnp1 = m * np1
      cona = float( m-1 ) / ( 8 * m**3 )
!                                        ** initial guess for k-th root
!                                        ** of Legendre polynomial, from
!                                        ** Davis/Rabinowitz (2.7.3.3a)
      lim  = m / 2
      do 30  k = 1, lim
         t = ( 4.0*k - 1.0 ) * pi / ( 4.0*m + 2.0)
         x = cos ( t + cona / tan( t ) )
!                                        ** recursion relation for
!                                        ** Legendre polynomials
10       pm2 = 1.d0
         pm1 = x
         do nn = 2, m
            p   = ( ( 2*nn - 1 ) * x * pm1 - ( nn-1 ) * pm2 ) / nn
            pm2 = pm1
            pm1 = p
         end do
!
         tmp   = 1.d0 / ( 1.d0 - x**2 )
         ppr   = en * ( pm2 - x * p ) * tmp
         p2pri = ( 2.d0 * x * ppr - nnp1 * p ) * tmp
         xi    = x - ( p / ppr ) * ( 1.d0 +&
                     ( p / ppr ) * p2pri / ( 2.d0 * ppr ) )
!
!                                              ** check for convergence
!         if ( dabs(xi-x) .gt. tol ) then
         if ( dabs(xi-x) .gt. 1.e-7) then
!            print*,dabs(xi-x),tol,xi,x
            x = xi
            go to 10
         end if
!                          ** iteration finished--calc. weights,
!                          ** abscissae for [-1,1]
         gmu( k ) = - x
         gwt( k ) = 2.d0 / ( tmp * ( en * pm2 )**2 )
         gmu( np1 - k ) = - gmu( k )
         gwt( np1 - k ) =   gwt( k )
30    continue
      return
!----------------------------------------------------------------
END SUBROUTINE QGAUS



!============================================================================
DOUBLE PRECISION FUNCTION PL00(X, LL)
!
!---Purpose: COMPUTES THE ORDINARY LEGENDRE POLYNOMIAL
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: LL
   DOUBLE PRECISION,INTENT(IN)                        :: x !---ARGUMENT OF OLP
!
!---local variables
!
   DOUBLE PRECISION                                   :: PL000,PL001,fac1,fac2
   INTEGER                                            :: l
!----------------------------------------------------------------
!
      PL000 = 1.d0
      PL001 = X
      IF(LL.EQ.0) THEN
        PL00 = PL000
      ELSE IF(LL.EQ.1) THEN
        PL00 = PL001
      ELSE
        DO l = 2, LL
          fac1 = (2.d0*l -1.d0)/dble(l)
          fac2 = dble(l - 1.d0)/dble(l)
          PL00 = fac1* X* PL001 - fac2 * PL000
          PL000 = PL001
          PL001 = PL00
        END DO
      ENDIF
!----------------------------------------------------------------
END FUNCTION PL00



!============================================================================
DOUBLE PRECISION FUNCTION PL02(X, LL)
!
!---Purpose: Generalized spherical function with n = +2 and m = 0 in P_m^l,n(x)
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: LL    !---ORDER
   DOUBLE PRECISION,INTENT(IN)                        :: x     !---ARGUMENT
!
!---local variables
!
   DOUBLE PRECISION                                   :: PL020,qroot6,PL021,sql41,  &
                                                         sql4,twol1,tmp1,tmp2
   INTEGER                                            :: l
!----------------------------------------------------------------

      PL020 = 0.d0
      qroot6 = -.25d0*dsqrt(6.d0)
      PL021 = qroot6*(1.0d0-X*X)
      sql41 = 0.d0
      IF(LL .LT. 2) THEN
        PL02 = 0.d0
      ELSE IF(LL .EQ. 2) THEN
        PL02 = PL021
      ELSE
        DO l = 3, LL
           sql4  = sql41
           sql41 = dsqrt(dble(l*l)-4.d0)
           twol1 = 2.d0*dble(l)-1.d0
           tmp1  = twol1/sql41
           tmp2  = sql4/sql41
           PL02  = tmp1*x*PL021- tmp2*PL020
           PL020 = PL021
           PL021 = PL02
        END DO
      END IF
!----------------------------------------------------------------
END FUNCTION PL02



!============================================================================
DOUBLE PRECISION FUNCTION PL22(X, LL,S)
!
!---Purpose: Generalized spherical function with n = +(-)2  and m = 2 in P_m^l,n(x)
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: LL    !---ORDER
   DOUBLE PRECISION,INTENT(IN)                        :: x,  & !---ARGUMENT
                                                         s     !---1   n = + 2; -1   n = - 2
!
!---local variables
!
   DOUBLE PRECISION                                   :: PL220,PL221,      &
                                                         twol1,denom,fac1, &
                                                         fac2,fac3
   INTEGER                                            :: l
!----------------------------------------------------------------
!      implicit double precision (a-h,o-z)

      PL220 = 0.d0
      PL221 = .25d0*(1.d0 + s*x)*(1.d0 + s*x)
      IF(LL .LT. 2) THEN
        PL22 = 0.d0
      ELSE IF(LL .EQ. 2) THEN
        PL22 = PL221
      ELSE
        DO l = 3, LL
           twol1 = 2.d0*dble(l)-1.d0
           denom = (dble(l) - 1.d0)*(dble(l*l)-4.d0)
           fac1  = twol1*(dble(l)-1.d0)*dble(l)/denom
           fac2  = 4.d0*twol1/denom
           fac3  = dble(l)*((dble(l)-1.d0)*(dble(l)-1.d0)-4.d0)/denom
           PL22  = (fac1*x-s*fac2)*PL221  - fac3*PL220
           PL220 = PL221
           PL221 = PL22
        END DO
      END IF
!----------------------------------------------------------------
END FUNCTION PL22



!============================================================================
DOUBLE PRECISION FUNCTION RGAMMA(X)
!
!---Purpose: Gamma function
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL,INTENT(IN)                                    :: x
!
!---local variables
!  

   REAL                                               :: T,pi,p
   DOUBLE PRECISION                                   :: F,D1,D2,D3
   INTEGER                                            :: l,k
!----------------------------------------------------------------
      REAL A(0:10)
      DATA A/1.,0.42278433,0.41184025,0.081578217,0.074237907,    &
     &         -0.0002109075,0.010973695,-0.0024667480,0.0015397681,&
     &         -0.0003442342,0.0000677106/
      PI=ASIN(1.0)*2.0
!     X <= 3.0
      IF(X.LE.3.0) THEN
          IF (X.LE.1.0) THEN
             T=X
          ELSE IF(X.LE.2.0) THEN
             T=X-1.0
          ELSE
             T=X-2.0
          ENDIF
          P=A(10)
          DO K=9,0,-1
            P=T*P+A(K)
          ENDDO
          IF (X.LE.1.0) THEN
             RGAMMA=P/(X*(X+1))
          ELSE IF(X.LE.2.0) THEN
             RGAMMA=P/(X)
          ELSE
             RGAMMA=P
          ENDIF
        ELSE
!         STIRLING APPROXIMATION
          D1=360*X**3
          D2=1260*X**5
          D3=1680*X**7
          F=(X-0.5)*ALOG(X)-X+0.5*ALOG(2*PI)+ 1.0/(12.0*X)-1.0/D1+1.0/D2-1.0/D3
          RGAMMA=EXP(F)
      ENDIF
!----------------------------------------------------------------
END FUNCTION RGAMMA



!============================================================================
SUBROUTINE  ERRMSG( messag, fatal )
!
!---Purpose: print out a warning or error message; abort if error
!
   IMPLICIT NONE
!----------------------------------------------------------------
   LOGICAL,INTENT(IN)                                 :: fatal
   CHARACTER(*),INTENT(IN)                           :: messag
!
!---lcoal variables
!
   INTEGER,SAVE                                       :: nummsg,maxmsg
   LOGICAL,SAVE                                       :: once
!----------------------------------------------------------------
!
!---initialize
!
   nummsg   = 0
   maxmsg   = 100
   once     = .false.

   if ( fatal )  then
      write ( 6, '(/,2a)' )  ' ******* Error >>>>>>  ', messag
      stop
   end if
!
      nummsg = nummsg + 1
      if ( nummsg.gt.maxmsg )  then
        if ( .not.once )  write ( 6,99 )
        once = .true.
      else
        write ( 6, '(/,2a)' )  ' ******* Warning >>>>>>  ', messag
      endif
!
      return
!
 99   format( ///,' >>>>>>  Too many warning messages --  ', &
   &   'they will no longer be printed  <<<<<<<', /// )
!----------------------------------------------------------------
END SUBROUTINE  ERRMSG



!============================================================================
DOUBLE PRECISION FUNCTION  d1mach(i)
!
!---Purpose: returns double precision machine dependent constants
!
!----------------------------------------------------------------
   INTEGER,INTENT(IN)                                 :: i
!----------------------------------------------------------------
!
!
!     d1mach( 1) = b**(emin-1),  smallest positive magnitude.
!
!     d1mach( 2) = b**emax*(1 - b**(-t)),  largest magnitude.
!
!     d1mach(3) = b**(-t),  smallest relative spacing.  i.e.,
!                 smallest number eps such that  1+eps .ne. 1
!
!     double precision  dmach(3)
!     data dmach/0.2225073858507D-307,0.1797693134862D+309, &
!  &           0.2220446049250D-015/
      if (i.eq.1) then
        d1mach = tiny(1.d0)
      else if(i.eq.2) then
        d1mach = huge(1.d0)
      else if(i.eq.3) then
        d1mach = epsilon(1.d0)
      else
        call errmsg('mac02 - trying non-existing machine constants in d1mach', .true. )
      endif
!----------------------------------------------------------------
END FUNCTION D1MACH



!============================================================================
SUBROUTINE iwp_estimate(tb,blza,biwp)
!
!---Purpose: Initialize the ice water path
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL,INTENT(IN)                                    :: blza
   REAL,DIMENSION(*),INTENT(IN)                       :: tb
   REAL,INTENT(OUT)                                   :: biwp
!
!---local variables
!
   INTEGER                                            :: angle_region
   REAL                                               :: a23,a31,b89,b150,lza0,lza1,   &
                                                         b89_0,b89_1,ICE_DEN,b150_1,   &
                                                         mu,b150_0,pred89,pred150,     &
                                                         omega89,omega150,ratio,coef_a,&
                                                         coef_b,coef_d,coef_c,bde,     &
                                                         omega,coef_iwp_a,coef_iwp_b,  &
                                                         coef_iwp_c,omegan
!----------------------------------------------------------------
   !
   !---Initializations
   !
   if (abs(blza) <10.0 )                         angle_region = 1
   if (abs(blza) >=10.0 .and. abs(blza) <20.0  ) angle_region = 2
   if (abs(blza) >=20.0 .and. abs(blza) <30.0  ) angle_region = 3
   if (abs(blza) >=30.0 .and. abs(blza) <40.0  ) angle_region = 4
   if (abs(blza) >=40.0 .and. abs(blza) <50.0  ) angle_region = 5
   if (abs(blza) >=50.0 ) angle_region = 6

   biwp     = 0.05
   ICE_DEN  = 920.0
!
!---Read inputs
!
    a23 = tb(1)
    a31 = tb(2)
    b89 = tb(4)
    b150 =tb(5)
    mu = cos(blza * acos(-1.0) / 180.)

   GET_option: SELECT CASE (angle_region)

   CASE (1)
            lza0=0.0
            lza1=10.0
            b89_0=183.073-0.649864*b89+0.00355388*b89*b89
            b150_0=89.4492+0.133525*b150+0.00193974*b150*b150
            b89_1=168.617-0.526129*b89+0.00329590*b89*b89
            b150_1=85.7358+0.169666*b150+0.00185847*b150*b150
            b89=(b89_1-b89_0)*(abs(blza)-lza0)/(lza1-lza0)+b89_0
            b150=(b150_1-b150_0)*(abs(blza)-lza0)/(lza1-lza0)+b150_0


   CASE (2)
            lza0=10.0
            lza1=20.0
            b89_0=168.617-0.526129*b89+0.00329590*b89*b89
            b150_0=85.7358+0.169666*b150+0.00185847*b150*b150
            b89_1=135.886-0.239320*b89+0.00268872*b89*b89
            b150_1=72.1034+0.300571*b150+0.00156526*b150*b150
            b89=(b89_1-b89_0)*(abs(blza)-lza0)/(lza1-lza0)+b89_0
            b150=(b150_1-b150_0)*(abs(blza)-lza0)/(lza1-lza0)+b150_0


   CASE (3)

            lza0=20.0
            lza1=30.0
            b89_0=135.886-0.239320*b89+0.00268872*b89*b89
            b150_0=72.1034+0.300571*b150+0.00156526*b150*b150
            b89_1=99.8433+0.0911668*b89+0.00196905*b89*b89
            b150_1=51.6176+0.501623*b150+0.00110930*b150*b150
            b89=(b89_1-b89_0)*(abs(blza)-lza0)/(lza1-lza0)+b89_0
            b150=(b150_1-b150_0)*(abs(blza)-lza0)/(lza1-lza0)+b150_0
   CASE (4)


            lza0=30.0
            lza1=40.0
            b89_0=99.8433+0.0911668*b89+0.00196905*b89*b89
            b150_0=51.6176+0.501623*b150+0.00110930*b150*b150
            b89_1=52.4938+0.535288*b89+0.000986296*b89*b89
            b150_1=26.8442+0.753185*b150+0.000528123*b150*b150
            b89=(b89_1-b89_0)*(abs(blza)-lza0)/(lza1-lza0)+b89_0
            b150=(b150_1-b150_0)*(abs(blza)-lza0)/(lza1-lza0)+b150_0

   CASE (5)
            lza0=40.0
            lza1=50.0
            b89_0=52.4938+0.535288*b89+0.000986296*b89*b89
            b150_0=26.8442+0.753185*b150+0.000528123*b150*b150
            b89_1=7.92203+0.981133*b89-0.0000394*b89*b89
            b150_1=-2.74337+1.06524*b150-0.000209793*b150*b150
            b89=(b89_1-b89_0)*(abs(blza)-lza0)/(lza1-lza0)+b89_0
            b150=(b150_1-b150_0)*(abs(blza)-lza0)/(lza1-lza0)+b150_0


   CASE (6)
            b89=7.92203+0.981133*b89-0.0000394*b89*b89
            b150=-2.74337+1.06524*b150-0.000209793*b150*b150

   END SELECT GET_option


    pred89 = 17.88 + 1.61* a23 - 0.67 * a31
    pred150 = 33.78 + 1.69* a23 - 0.80* a31
    omega89 = (pred89 - b89)/b89
    omega150 = (pred150 - b150)/b150
    ratio = omega89/omega150

    coef_a = -0.300323
    coef_b = 4.30881
    coef_c = -3.98255
    coef_d = 2.78323
    bde = coef_a + coef_b * ratio + coef_c * ratio**2+ coef_d * ratio**3


! /* Calculate the ice water path   */
    omega=omega89
    coef_iwp_a = -1.19301
    coef_iwp_b = 2.08831
    coef_iwp_c = -0.857469
    if(bde <= 1.0) then
        omega=omega150
        coef_iwp_a = -0.294459
        coef_iwp_b = 1.38838
        coef_iwp_c = -0.753624
    endif

    if (bde > 0.0) then
        omegan = exp( coef_iwp_a + coef_iwp_b * alog(bde)+  coef_iwp_c * alog(bde)**2 )
        if(omegan > 0.0) biwp = (omega * bde * 0.001 * mu * ICE_DEN / omegan)

    endif

!----------------------------------------------------------------
END SUBROUTINE iwp_estimate



!============================================================================
SUBROUTINE NONSPHERE(SHAPE_INDEX,SIZE_PARM,AREA,SLAM,freq,QE,QS,gg_nonsphere)
!
!---Purpose: Calculate scattering cross section QS and extinction cross section QE
!            based on Guosheng Liu's (J. Atm. Sci. 2004) polynomial fitting functions
!            with various snow particle shapes (rosettes and snow flakes).
!
   IMPLICIT NONE
!----------------------------------------------------------------
   INTEGER                                         :: shape_index,icount,i
   REAL                                            :: SIZE_PARM,AREA,SLAM,QE, &
                                                      freq,QS,Qabs,gg_nonsphere
   DOUBLE PRECISION                                :: pi,logx,                &
                                                      a1,a2,a3,c0,c1,c2,c3
   DOUBLE PRECISION,DIMENSION(4,8)                 :: coea
   DOUBLE PRECISION,DIMENSION(5,8)                 :: coef
!----------------------------------------------------------------

   coea(1:4,1) = [-0.3353,-0.3533,-0.3597,-0.3432]
   coea(1:4,2) = [ 3.3177, 3.3295, 3.3643, 3.4542]
   coea(1:4,3) = [-1.7217,-1.6769,-1.5013,-1.4338]
   coea(1:4,4) = [-1.7254,-1.9710,-2.0822,-2.6021]
   coea(1:4,5) = [-0.1953,-0.5256,-1.2714,-2.2706]
   coea(1:4,6) = [ 0.7358, 1.1379, 0.9382, 1.1111]
   coea(1:4,7) = [ 0.4084, 1.1043, 1.6981, 2.8529]
   coea(1:4,8) = [ 0.0554, 0.2963, 0.6088, 1.1258]

   coef(1:5,1) = [-0.6304,-0.5673,-0.5832,-0.6122, -0.4654]
   coef(1:5,2) = [ 1.5281, 1.5418, 1.6818, 2.3329, -3.9724]
   coef(1:5,3) = [-0.2125,-1.0410,-1.0855, 3.6036, 81.0301]
   coef(1:5,4) = [-0.9502,-1.0442,-1.4262,13.9784,-504.904]
   coef(1:5,5) = [-1.7090,-0.0600,-0.2155,26.3336,  1569.3]
   coef(1:5,6) = [ 0.1557, 0.8422, 1.0944,26.3125, -2620.1]
   coef(1:5,7) = [ 1.4016, 0.6686, 0.8690,13.4166,  2230.9]
   coef(1:5,8) = [ 0.5477, 0.1597, 0.1937, 2.7443,-757.586]

 !  SHAPE_INDEX = 1  ! 1: rosettes (Liu); 2. snowflake-A (Liu); 3. snowflake-B (Liu); 
 !                   ! 4: cylindrical column; 5: 2-cylinder aggregate; 
 !                   ! 6: 3-cylinder aggregate; 7: 4-cylinder aggregate

! Initialization

   PI = ACOS( -1.0 )
   freq = 300./SLAM
   gg_nonsphere  = 0.0

! G. Liu's empirical equations

  if(shape_index .le. 3) then

   GET_option_Liu: SELECT CASE (shape_index)      

   CASE (1)  ! rosettes 

     if (size_parm .le. 2.2) then
       a1 = -0.036379
       a2 = 0.11716
       a3 = 0.18637
     else
       a1 = -0.60643
       a2 = 1.0934
       a3 = -0.1463
     endif

     if (size_parm .le. 1.0) then
       c0 = 0.
       c1 = -0.077361
       c2 = 0.59902
       c3 = -0.0018825
     else
       c0 = 0.30617
       c1 = 0.019795
       c2 = 0.029307
       c3 = -0.00029968
     endif

   CASE (2)  ! type-A snowflake
     if (size_parm .le. 1.4) then
       a1 = -0.036379
       a2 = 0.11716
       a3 = 0.18637
     else
       a1 = -0.1622
       a2 = 0.56253
       a3 = -0.066369
     endif

     if (size_parm .le. 1.0) then
       c0 = 0.
       c1 = -0.077361
       c2 = 0.59902
       c3 = -0.0018825
     else
       c0 = 0.42725
       c1 = 0.062429
       c2 = 0.028416
       c3 = -0.0042245
     endif

   CASE (3)  ! type-B snowflake
     if (size_parm .le. 0.5) then
       a1 = -0.036379
       a2 = 0.11716
       a3 = 0.18637
     else
       a1 = -0.0096948
       a2 = 0.15898
       a3 = 0.01078
     endif

     if (size_parm .le. 1.0) then
       c0 = 0.
       c1 = -0.077361
       c2 = 0.59902
       c3 = -0.0018825
     else
       c0 = 0.42725
       c1 = 0.062429
       c2 = 0.028416
       c3 = -0.0042245
     endif

   END SELECT GET_option_Liu

   QS = AREA*(a1*size_parm+a2*size_parm**2+a3*size_parm**3)

   Qabs = AREA*(0.007446*size_parm+0.010607*size_parm**2-0.0014505*size_parm**3)


!B.YAN added a quality control

   if (QS .lt. 0.0) QS = 0.001
   if (Qabs .lt. 0.0) Qabs = 0.001

   QE = QS + Qabs

   gg_nonsphere = c0+c1*size_parm+c2*size_parm**2+c3*size_parm**3

! B.YAN

   if (gg_nonsphere .lt. 0.0) gg_nonsphere = 0.0

  else

! M. Kim's empirical equations

! Absorption cross section only depends on frequency, not on snow crystal shape.

   call CAL_QABS(size_parm,AREA,freq,Qabs)

! Scattering cross section and asymmetry factor depend on snow crystal shape.

   logx = log10(size_parm)
   QS = 0.0
   gg_nonsphere = 0.0

   GET_option_Kim: SELECT CASE (shape_index)

   CASE (4)  ! Cylindrical column 
     do icount=1, 8
       QS = QS + coea(1,icount)*logx**(icount-1)
       gg_nonsphere = gg_nonsphere + coef(1,icount)*logx**(icount-1)
     enddo
   CASE (5)  ! 2-Cylinder aggregate
     do icount=1, 8
       QS = QS + coea(2,icount)*logx**(icount-1)
       gg_nonsphere = gg_nonsphere + coef(2,icount)*logx**(icount-1)
     enddo
   CASE (6)  ! 3-Cylinder aggregate 
     do icount=1, 8
       QS = QS + coea(3,icount)*logx**(icount-1)
       gg_nonsphere = gg_nonsphere + coef(3,icount)*logx**(icount-1)
     enddo
   CASE (7)  ! 4-Cylinder column 
     do icount=1, 8
       QS = QS + coea(4,icount)*logx**(icount-1)
       if(size_parm .lt. 1.) then
         gg_nonsphere = gg_nonsphere + coef(4,icount)*logx**(icount-1)
       else
         gg_nonsphere = gg_nonsphere + coef(5,icount)*logx**(icount-1)
       endif
     enddo
   END SELECT GET_option_Kim
   
   QS = AREA*10.**QS
 
   gg_nonsphere = 10.**gg_nonsphere

  endif
       
!----------------------------------------------------------------
END SUBROUTINE NONSPHERE 



!============================================================================
SUBROUTINE CAL_QABS(size_parm,AREA,freq,Qabs)
!
!---Purpose: Compute absorption cross sections based on M. Kim's empirical equations, JGR 111
!
   IMPLICIT NONE
!----------------------------------------------------------------
   REAL,INTENT(IN)                                    :: size_parm,freq,area
   REAL,INTENT(OUT)                                   :: Qabs
!
!---local variables
!
   INTEGER                                            :: i,icount,count_fix
   REAL                                               :: qabs1,qabs2
   REAL,DIMENSION(5)                                  :: coeb_freq
   DOUBLE PRECISION,DIMENSION(5,6)                    :: coeb
!----------------------------------------------------------------

   coeb_freq      = [95., 140., 183., 220., 340.]

   coeb(1:5,1)    = [ 1.508E-04,1.122E-04,-6.598E-04,   0.0019,   0.0063]
   coeb(1:5,2)    = [    0.0021,   0.0061,    0.0153,8.275E-04,  -0.0145]
   coeb(1:5,3)    = [    0.0081,   0.0086,   -0.0032,   0.0189,   0.0502]
   coeb(1:5,4)    = [   -0.0051,  -0.0022,    0.0062,  -0.0022,  -0.0105]
   coeb(1:5,5)    = [     0.002, 5.35E-04,   -0.0014,-4.49E-05,6.998E-04]
   coeb(1:5,6)    = [-2.596E-04,-4.82E-05,  8.49E-05, 1.24E-05, 8.68E-07]

!----------------------------------------------------------------
!
!---Initialization
!
   Qabs  = 0.0
   qabs1 = 0.0
   qabs2 = 0.0

! Update Qabs

   if(freq.le.coeb_freq(1)) then  ! if frquency is less or equal to 95 GHz
     do icount=1,6
       Qabs=Qabs+coeb(1,icount)*size_parm**(icount-1)
     enddo
     Qabs=AREA*Qabs

   else if(freq.ge.coeb_freq(5)) then ! if frequency is greater or equal to 340 GHz
     do icount=1,6
       Qabs=Qabs+coeb(5,icount)*size_parm**(icount-1)
     enddo
     Qabs=AREA*Qabs

   else ! if frequency falls in between 95 and 340 GHz
     do icount=1,4
      if(freq.ge.coeb_freq(icount).and.freq.lt.coeb_freq(icount+1)) count_fix=icount
     enddo
      
     do icount=1,6
       qabs1=qabs1+coeb(count_fix,icount)*size_parm**(icount-1)
       qabs2=qabs2+coeb(count_fix+1,icount)*size_parm**(icount-1)
     enddo

      ! Linear interpolation
     Qabs=qabs2+(qabs1-qabs2)*(freq-coeb_freq(count_fix+1))/(coeb_freq(count_fix)-coeb_freq(count_fix+1))
   endif

!----------------------------------------------------------------
END SUBROUTINE CAL_QABS



!============================================================================
END MODULE GET_EMISS_1DALG
!============================================================================




