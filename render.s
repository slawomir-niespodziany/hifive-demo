    .text
    .global render

render:
    # a0 - W
    # a1 - H
    # a2 - pBuffer
    # a6 - color

    mv t0, zero # t0 - column index
    mv t1, zero # t1 - row index

loop:
    # t2 - offset in pixels
    mul t2, t1, a0 # t2 = (row index * W)
    add t2, t2, t0 # t2 += column index

    # t3 - offset in bytes (offset in pixels * 3)
    slli t3, t2, 1 # t3 = (t2 << 1) = (t2 * 2)
    add t3, t3, t2 # t3 += t2

    # t3 - address + offset in bytes
    add t3, t3, a2 # t3 += pBuffer

    sb zero, 0(t3)
    sb zero, 1(t3)
    sb zero, 2(t3)

    bne t0, t1, leave_as_black
    
    # t2 - color (R)
    andi t2, a6, 0xFF
    sb t2, 0(t3)
    
    # t2 - color (G)
    srl t2, a6, 8
    andi t2, t2, 0xFF
    sb t2, 1(t3)
    
    # t2 - color (B)
    srl t2, a6, 16
    andi t2, t2, 0xFF
    sb t2, 2(t3)
    
leave_as_black:

    addi t0, t0, 1
    blt t0, a0, loop
    
    mv t0, zero
    addi t1, t1, 1
    blt t1, a1, loop

    ret
