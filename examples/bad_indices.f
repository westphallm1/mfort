      DIMENSION X(20,25,3), Y(8),Z(3+2,18.5,COSF(7))

      X(1,1,1) = 1
      X(I,J,K) = 2
      X(I + 1, J - 2, K) = 3
      X(4*I + 1, 3*J + 2, K - 1) = 3

      TESTF(X,Y,Z) = X + Y + Z

  180 SUMSQ = SUMSQ + X(I,1,2)**2 + Y(1)
      END

