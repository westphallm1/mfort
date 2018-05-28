      CUBRTF(X) = X ** 1/3
      QROOT1F(A,B,C) = (-B + SQRTF(B**2 - 4 * A * C))/(2*A)
      QROOT2F(A,B,C) = (-B - SQRTF(B**2 - 4 * A * C))/(2*A)
      P4 = EXTERN(2,2.5,A)
      DIMENSION A(5), B(1,8), C(2,2,2)
      PRINT 17, QR1, QR2
      COMMON (A,B)
      EQUIVALENCE (QR1,QR2,C) 
   17 FORMAT (E11.5)
      END
