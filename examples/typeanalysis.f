c    
c     Type checking
c     Make sure no float -> int casts are being performed, assign 
c     appropriate casting rules to each node, and raise warnings 
c     for inconsistent function arguments
c      
      DIMENSION M(10,10), D(5)
      XINTF(I) = I + 2
      FLOATF(A) = A * 3.5
c     int lit to float
      A = 5
c     in expression
      B = A + 2
c     int symbol to float
      A = 4.5 * I + J
c     int function to float
      C = XINTF(3)
c     subscripted int to float
      A = M(1,1) + 3.5
c     Cast when LHS is subscripted
      D(1) = I + 3 + 4.5
c     Errors on float -> int cast
      I = 4.5 + 3
      I = D(2) * D(3)
      M(3,2) =  D(4)
c     Make sure ints don't get accidentally cast
      M(1,1) =  I + J - 3

