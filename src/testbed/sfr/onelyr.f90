!-----------------------------------------------------------------------------
! NAME:
!    onelyr.f90 
! PURPOSE:
!   Interface for c to call f90 one-layer model
! CONTAINS:
! EXTERNALS:
!       None.
! INCLUDE FILES:
!       None.
! COMMON BLOCKS:
!       None.
! CREATION HISTORY:
!       Written by:     Banghua Yan and Fuzhong Weng, NOAA/NESDIS/STAR
!       ----------
!       Simplified by calling AMSU_CLOUDEMS_ALGORITHM directly, 
!       Passing in all parameters, Jun Dong, 02/29/2020
!-----------------------------------------------------------------------------

!=============================================================================
SUBROUTINE onelyr(tbo, emiss, tpw, iwp, de, tc, ts, lza, ecm, kv, ko2, &
      frc1, freq1, ss_lut, rst)
!
!---Purpose: two stream simulation solution
!
   USE GET_EMISS_1DALG

   IMPLICIT NONE
!=============================================================================
   INTEGER(4), PARAMETER                        :: rtype    = 3
!   INTEGER(4), PARAMETER                        :: iter_max = 5
   INTEGER(4)                                   :: iter_max = 5
   INTEGER(4), PARAMETER                        :: ish_ind  = 0

   REAL(4)                                      :: lza, ts, tc, iwp, tpw, de
   INTEGER(4)                                   :: rst, i
 
   REAL(4), DIMENSION(nch)                      :: tbo, emiss
   REAL(4), DIMENSION(nch)                      :: kv, frc1, freq1
   REAL(4), DIMENSION(nch, 3)                   :: ko2
   REAL(4), DIMENSION(nvar)                     :: xin, xou, ecm
   REAL(4), DIMENSION(mtc,mrwp,mde,npara,nch)   :: ss_lut

!   call pr_input(tbo, emiss, tpw, iwp, de, tc, ts, lza, &
!         ecm, kv, ko2, frc1, freq1, ss_lut)

   xin(1:nch) = emiss(1:nch)
   xin(nch+1) = tpw
   xin(nch+2) = iwp
   xin(nch+3) = de
   xin(nch+4) = tc
   xin(nch+5) = ts
   iter_max = rst

   ! set global var
   freq = freq1
   do i = 1, nch
      frc(i) = nint(frc1(i))
   enddo

   CALL AMSU_CLOUDEMS_ALGORITHM(rtype, iter_max, ish_ind, &
         lza, tbo, xin, xou, ecm, kv, ko2, ss_lut, rst)

   emiss = xou(1:nch)
   tpw   = xou(nch+1)
   iwp   = xou(nch+2)
   de    = xou(nch+3)
   tc    = xou(nch+4)
   ts    = xou(nch+5)


!   call pr_output(tbo, emiss, tpw, iwp, de, tc, ts, lza)

!----------------------------------------------------------------
END SUBROUTINE onelyr



!==========================================================================
SUBROUTINE pr_input(tbo, emiss, tpw, iwp, de, tc, ts, lza, &
        ecm, kv, ko2, frc1, freq1, ss_lut)

   USE GET_EMISS_1DALG

   IMPLICIT NONE

   REAL(4)                  :: lza, ts, tc, iwp, tpw, de
   INTEGER(4)               :: i, j
 
   REAL(4), DIMENSION(nch)                      :: tbo, emiss
   real(4), dimension(nch)                      :: ecm, kv, frc1, freq1
   real(4), dimension(nch, 3)                   :: ko2
   real(4), dimension(mtc,mrwp,mde,npara,nch)   :: ss_lut

   write(*,*)           "---------------"
   write(*,*)           "in onelyr, will call alg"
   write(*,*)           "tbo"
   write(*,"(6F12.6)")  (tbo(i),i=1,6)
   write(*,*)           "emiss"
   write(*,"(6F12.6)")  (emiss(i),i=1,6)
!   write(*,*)           "lza:", lza
   write(*,*)           "tpw, iwp, de, tc, ts:"
   write(*,"(5F12.6)")  tpw, iwp, de, tc, ts
!   write(*,*)           "ecm"
!   write(*,"(6F12.6)")  (ecm(i),i=1,6)
!   write(*,*)           "kv"
!   write(*,"(6E14.6)")  (kv(i),i=1,6)
!   write(*,*)           "ko2"
!   do i=1,6
!      write(*,"(3E14.6)")  (ko2(i,j),j=1,3)
!   enddo
   write(*,*)           "frc1"
   write(*,"(6F4.1)")     (frc1(i),i=1,6)
!   write(*,*)           "freq"
!   write(*,*)           (freq1(i),i=1,6)
!   write(*,*) "ss_lut"
!   write(*,*) ss_lut(1,3,1,1,5), ss_lut(1,3,44,1,5), ss_lut(1,50,1,1,5)
!   read(*,*)

END SUBROUTINE pr_input



!==========================================================================
SUBROUTINE pr_output(tbo, emiss, tpw, iwp, de, tc, ts, lza)

   USE GET_EMISS_1DALG

   IMPLICIT NONE

   REAL(4)                  :: lza, ts, tc, iwp, tpw, de
   INTEGER(4)               :: i, j
 
   REAL(4), DIMENSION(nch)                      :: tbo, emiss

   write(*,*)           "---------------"
   write(*,*)           "in onlyr, will back to c"
   write(*,*)           "tbo"
   write(*,"(6F12.6)")  (tbo(i),i=1,6)
   write(*,*)           "emiss"
   write(*,"(6F12.6)")  (emiss(i),i=1,6)
!   write(*,*)           "lza:", lza
   write(*,*)           "tpw, iwp, de, tc, ts:"
   write(*,"(5F12.6)")  tpw, iwp, de, tc, ts
!   read(*,*)

END SUBROUTINE pr_output




