c    
c     Symtable entry and subscript testing
c     Make sure dimensioned variables, functions, and subprocesses
c     Are distinguished correctly, and can't be interchanged
c      
c     Good dimensions, no errors
      DIMENSION A(10,10), B(17)
c     Bad dimensions
      DIMENSION C(12.5), D(I)
c     Wrong number of dimensions in access 
      A(11) = B(12,13)
c     Valid expression access
      A(I,I+1) = A(2*I,3*I+1) + B(I-1) + B(4*I-6)
c     Invalid expression access
      A(I/2,E) = A(1+2,2*4)+B(1+2+3) + B(3+I*2)
c     Function declaration (covered by syntax checking)
      TESTF(A,I,B) = A + B + I 
c     Good function call      
      A(1,1) = TESTF(A(1,2), B(1)+7.5/2, 4)
c     Bad function call (wrong number of args)
      A(1,2) = TESTF(A,B,C,D) + TESTF(A)
c     Extern function 
      A(1,3) = EXTERN(A(1,1),B(3),6)
c     Inconsistent extern function calls
      A(1,4) = EXTERN(A(1,2)) + EXTERN(TESTF(1,2,3),EXTERN(1))
c     Extern subprocess
      CALL EXTERN2(1,2,3)
c     Inconsistent extern subprocess calls
      CALL EXTERN2(1,2)
      CALL EXTERN2(1,2,3,4)
c     calling a subscripted variable
      CALL A(1,2,3)
c     calling a function
      CALL EXTERN(1,2,3)
c     subprocess outside CALL
      A(1,5) = EXTERN2(4,5,6)
c     Make sure tags get added to the sym table
 15   A(1,2) = 4
 20   B(3) = 7
      IF(A(2,4)) 15, 15, 21
c     Common statement
      COMMON (A, B, C, D, E)
c     Equivalence statement
      EQUIVALENCE (A, A2, A3, A4)

      END
