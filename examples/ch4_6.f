      READ 18, X
   18 FORMAT (F10.0)
      IF(X) 20, 21, 22
   21 IF(X - 50.) 22, 20, 20
   20 STOP 1
   22 IF(X - 10.9) 23, 24, 24
   23 Y = 8.72
      GO TO 30
   24 IF(X - 21.6) 25, 26, 26
   25 Y = 16.9
      GO TO 30
   26 Y = 24.07
   30 PRINT 19, X, Y
   19 FORMAT (2E20.8)
      STOP 2
      END

