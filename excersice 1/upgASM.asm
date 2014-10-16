##### Q5a #####
Loop: LL Rx, X
      ADDi Rx, Rx, a
      SC Rx, X
      BEQZ Rx, Loop

##### Q5d #####
Loop: LL  R2, X         // Load data at X into R2
      ADDi R1, Ry, #0   // Move Ry to R1
      SC  R1, X         // Try and store R1(data from Ry) into X
      BEQZ R1, Loop     // If store failed(R1 == 0), retry 
      ADDi Ry, R2, #0   // Move R2 (data from X) into Ry

##### Q5e #####
Loop: LL  R1, X         // Load data at X into R1
      BNE Rx, R1, End   // Check if Rx == R1 (data from X), if not end.
      ADD R2, Ry, #0    // Move Ry into R2
      SC  R2, X         // Try and store R2 (data from Ry) into X
      BEQZ R2, Loop     // If store fails (R2 == 0), retry
      ADD Ry, R1, #0    // Move R1 (Data from X) into Ry
End:

##### Q4b ######
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

##### Q4c ####### 
      F&A BAR, Rx, #1
Ltn: 
      LD  Rx, BAR
      BLT Rx, N, Ltn