      READ 10, X, Y, I
      R = 16.78 * COSF(6.2832*X) + (Y + 1.667)**I
      PRINT 20, X,Y,I,R
      STOP
   10 FORMAT (2F10.0, I10)
   20 FORMAT (2E20.8, I10, E20.8)
      END
