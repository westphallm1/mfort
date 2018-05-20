      LEG = 5
      X = 7.0
      LEG1 = LEG + 1
      GO TO (61,62,63,64,65), LEG1
   61 P = 1
      GO TO 70
   62 P = X 
      GO TO 70
   63 P =  1.5*X**2 - .5
      GO TO 70
   64 P =  2.5*X**3 - 1.5*X
      GO TO 70
   65 P =  4.375*X**4 - 3.75*X**2 + .375
   70 STOP
      END
