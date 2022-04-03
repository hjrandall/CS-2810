Multiplication extensions
-------------------------

The base instruction set for RISC-V does not include multiplication
and division instructions, but these are a common and useful
extension that we will implement. These instructions are added to
the existing opcode for ALU instructions and should be implemented
in `execute_alu` (in addition to the instructions you have already
implemented there). See the documentation for details of the
instruction encodings.

*   `mul rd, rs1, rs2`: “multiply”

    Multiply the values in rs1 and rs2 and store the result in rd.
    Note that the result may be bigger than 32 bits, but only the
    least-significant 32 bits are stored. C already works this way
    so you can just implement this as a simple multiply.

*   `mulh rd, rs1, rs2`: “multiply high”

    Multiply the two values but store only the most-significant 32
    bits. This can be implemented by converting the inputs to 64-bit
    values and capturing the 64-bit result, then shifting it right
    32 times to obtain the final value.

*   `mulhsu rd, rs1, rs2`: “multiply high signed unsigned”

    Multiply rs1 as a signed 64-bit value and rs2 as an unsigned
    64-bit value and capture the most-significant 32 bits. Use the
    appropriate value types in C to get the correct results.

*   `mulhu rd, rs1, rs2`: “multiply high unsigned”

    Treat both values as unsigned 64-bit values, multiply, and
    capture the most-significant 32 bits.

*   `div rd, rs1, rs2`: “divide”

    Divide rs1 by rs2 and store the result in rd.

*   `divu rd, rs1, rs2`: “divide unsigned”

    Perform division with rs1 and rs2 treated as unsigned values.

*   `rem rd, rs1, rs2`: “remainder”

    Divide rs1 by rs2 and store the remainder in rd.

*   `remu rd, rs1, rs2`: “remainder unsigned”

    Capture the remainder but treat rs1 and rs2 as unsigned values.

When implementing variants with different bit sizes and unsigned
operands, you may find it helpful to create variables of the
appropriate types, copy the values into those variables, and then
perform the operations.
