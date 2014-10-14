      LL  R1, X
Loop: ADDi Rx, R1, a
      SC  Rx, X
      BEQZ Rx, Loop

      LL  R2, X
Loop: ADDi R1, Ry, #0
      SC  R1, X 
      BEQZ R1, Loop
      ADDi Ry, R2, #0

Loop: ADD R2, Ry, #0
      LL  R1, X
      BNE Rx, R1, Skip
      SC  R2, X
      BEQZ R2, Loop
      ADD Ry, R1, #0
Skip:

Entr:
      T&S R1, X
      BNE R1, R0, Entr


      LW  R2, BAR
      ADDi R3, R2, #1
      SW  R3, BAR
      SW  R1, X

Ltn: 
      LD  R3, BAR
      BLT R3, N, Ltn




      ADDi R1, R0, #1
      F&A R1, X

Ltn: 
      LD  R2, BAR
      BLT R2, N, Ltn
