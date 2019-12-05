      SUBROUTINE ana_stflux (ng, tile, model, itrc)
!
!! svn $Id: ana_stflux.h 830 2017-01-24 21:21:11Z arango $
!!======================================================================
!! Copyright (c) 2002-2017 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!=======================================================================
!                                                                      !
!  This routine sets kinematic surface flux of tracer type variables   !
!  "stflx" (tracer units m/s) using analytical expressions.            !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_forces
      USE mod_ncparam
!
! Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model, itrc

#include "tile.h"
!
      CALL ana_stflux_tile (ng, tile, model, itrc,                      &
     &                      LBi, UBi, LBj, UBj,                         &
     &                      IminS, ImaxS, JminS, JmaxS,                 &
#ifdef SHORTWAVE
     &                      FORCES(ng) % srflx,                         &
#endif
#ifdef TL_IOMS
     &                      FORCES(ng) % tl_stflx,                      &
#endif
     &                      FORCES(ng) % stflx)
!
! Set analytical header file name used.
!
#ifdef DISTRIBUTE
      IF (Lanafile) THEN
#else
      IF (Lanafile.and.(tile.eq.0)) THEN
#endif
        ANANAME(31)=__FILE__
      END IF

      RETURN
      END SUBROUTINE ana_stflux
!
!***********************************************************************
      SUBROUTINE ana_stflux_tile (ng, tile, model, itrc,                &
     &                            LBi, UBi, LBj, UBj,                   &
     &                            IminS, ImaxS, JminS, JmaxS,           &
#ifdef SHORTWAVE
     &                            srflx,                                &
#endif
#ifdef TL_IOMS
     &                            tl_stflx,                             &
#endif
     &                            stflx)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
#if defined SEDIMENT && defined RHONE
      USE mod_sediment
#endif
!
      USE exchange_2d_mod, ONLY : exchange_r2d_tile
#ifdef DISTRIBUTE
      USE mp_exchange_mod, ONLY : mp_exchange2d
#endif
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model, itrc
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: IminS, ImaxS, JminS, JmaxS
!
#ifdef ASSUMED_SHAPE
# ifdef SHORTWAVE
      real(r8), intent(in) :: srflx(LBi:,LBj:)
# endif
      real(r8), intent(inout) :: stflx(LBi:,LBj:,:)
# ifdef TL_IOMS
      real(r8), intent(inout) :: tl_stflx(LBi:,LBj:,:)
# endif
#else
# ifdef SHORTWAVE
      real(r8), intent(in) :: srflx(LBi:UBi,LBj:UBj)
# endif
      real(r8), intent(inout) :: stflx(LBi:UBi,LBj:UBj,NT(ng))
# ifdef TL_IOMS
      real(r8), intent(inout) :: tl_stflx(LBi:UBi,LBj:UBj,NT(ng))
# endif
#endif
!
!  Local variable declarations.
!
      integer :: i, j
#if defined SEDIMENT && defined RHONE
      real(r8) :: dep_rate
#endif

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Set kinematic surface heat flux (degC m/s) at horizontal
!  RHO-points.
!-----------------------------------------------------------------------
!
      IF (itrc.eq.itemp) THEN
        DO j=JstrT,JendT
          DO i=IstrT,IendT
#ifdef BL_TEST
            stflx(i,j,itrc)=srflx(i,j)
# ifdef TL_IOMS
            tl_stflx(i,j,itrc)=srflx(i,j)
# endif
#else
            stflx(i,j,itrc)=0.0_r8
# ifdef TL_IOMS
            tl_stflx(i,j,itrc)=0.0_r8
# endif
#endif
          END DO
        END DO
!
!-----------------------------------------------------------------------
!  Set kinematic surface freshwater flux (m/s) at horizontal
!  RHO-points, scaling by surface salinity is done in STEP3D.
!-----------------------------------------------------------------------
!
      ELSE IF (itrc.eq.isalt) THEN
        DO j=JstrT,JendT
          DO i=IstrT,IendT
            stflx(i,j,itrc)=0.0_r8
#ifdef TL_IOMS
            tl_stflx(i,j,itrc)=0.0_r8
#endif
          END DO
        END DO
!
!-----------------------------------------------------------------------
!  Set surface sediment flux (m/s) at horizontal
!  RHO-points.
!-----------------------------------------------------------------------
!
#if defined SEDIMENT && defined RHONE
      ELSE IF (itrc.eq.idsed(NST)) THEN
        DO j=JstrT,JendT
          DO i=IstrT,IendT
!           dep_rate=0.0000003171_r8
            dep_rate=0.0000003822_r8
!           stflx(i,j,idsed(1))=dep_rate*0.27_r8
            stflx(i,j,idsed(1))=dep_rate*0.4_r8
!           stflx(i,j,idsed(2))=dep_rate*0.53_r8
            stflx(i,j,idsed(2))=dep_rate*0.4_r8
            stflx(i,j,idsed(3))=dep_rate*0.20_r8
           stflx(i,j,idsed(4))=dep_rate*0.028_r8
           stflx(i,j,idsed(5))=dep_rate*0.028_r8
!           stflx(i,j,idsed(1))=0.0_r8
!           stflx(i,j,idsed(2))=0.0_r8
!           stflx(i,j,idsed(3))=0.0_r8
!           stflx(i,j,idsed(4))=0.0_r8
!           stflx(i,j,idsed(5))=0.0_r8
#ifdef TL_IOMS
            tl_stflx(i,j,itrc)=0.0_r8
#endif
          END DO
        END DO
#endif
!
!-----------------------------------------------------------------------
!  Set kinematic surface flux (T m/s) of passive tracers, if any.
!-----------------------------------------------------------------------
!
      ELSE
        DO j=JstrT,JendT
          DO i=IstrT,IendT
            stflx(i,j,itrc)=0.0_r8
#ifdef TL_IOMS
            tl_stflx(i,j,itrc)=0.0_r8
#endif
          END DO
        END DO
      END IF
!
!  Exchange boundary data.
!
      IF (EWperiodic(ng).or.NSperiodic(ng)) THEN
        CALL exchange_r2d_tile (ng, tile,                               &
     &                          LBi, UBi, LBj, UBj,                     &
     &                          stflx(:,:,itrc))
#ifdef TL_IOMS
        CALL exchange_r2d_tile (ng, tile,                               &
     &                          LBi, UBi, LBj, UBj,                     &
     &                          tl_stflx(:,:,itrc))
#endif
      END IF

#ifdef DISTRIBUTE
      CALL mp_exchange2d (ng, tile, model, 1,                           &
     &                    LBi, UBi, LBj, UBj,                           &
     &                    NghostPoints,                                 &
     &                    EWperiodic(ng), NSperiodic(ng),               &
     &                    stflx(:,:,itrc))
# ifdef TL_IOMS
      CALL mp_exchange2d (ng, tile, model, 1,                           &
     &                    LBi, UBi, LBj, UBj,                           &
     &                    NghostPoints,                                 &
     &                    EWperiodic(ng), NSperiodic(ng),               &
     &                    tl_stflx(:,:,itrc))
# endif
#endif

      RETURN
      END SUBROUTINE ana_stflux_tile