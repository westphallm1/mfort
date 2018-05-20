   27 READ 612, Q, R, C, AL
  612 FORMAT (4F10.0)
      IF(R**2 - 4.*AL/C) 10, 12, 12
  10  F0 = 0.1592*SQRTF(1./(AL*C))
      F1 = 0.1592*SQRTF(1./(AL*C) - R**2/(4.*AL**2))
      DELT = 0.1/F0
      AIM = 6.2832*F0**2*Q/F1
      C1 = R/(2.*AL)
      C2 = 6.2832*F1
      T = 0
      DO 11 J = 1, 100
      AI = AIM * EXPF(-C1*T) * SINF(C2*T)
      PRINT 706, T, AI
  706 FORMAT(2E20.8)
   11 T = T + DELT
      GO TO 27
   12 STOP
      END
