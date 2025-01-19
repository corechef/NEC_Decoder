## Requirements for Minimal Viable Product

- Use 16-bit Timer.
  - 13.63 ms on IDLE to reading state.
  - 11.35 ms on IDLE to repeat state.
  - 1.161 ms on 0 input.
  - 2.23  ms on 1 input.
  - total of 32 inputs on a normal command
  - 40 ms between new command and repeat command.
  - Total command length: 13.63ms + 16 * (1.161 + 2.23) = 70 - 75 ms.
- Use timer with proper prescaling.
- Use a proper interrupt.