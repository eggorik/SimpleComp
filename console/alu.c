#include <stdbool.h>

#include <mySimpleComputer.h>

#define SC_MAGNITUDE_MASK 0b11111111111111

/* Memory word (sign in bit 14 + 14-bit magnitude) -> int */
static int
word_to_int (int word)
{
  int magnitude = word & SC_MAGNITUDE_MASK;
  return word >> 14 ? -magnitude : magnitude;
}

/* int -> memory word, sets the overflow flag if it does not fit */
static int
int_to_word (int x)
{
  bool sign = x < 0;
  int magnitude = sign ? -x : x;

  if (magnitude > SC_MAGNITUDE_MASK)
    {
      sc_regSet (SC_OVERFLOW, 1);
      magnitude &= SC_MAGNITUDE_MASK;
    }

  if (magnitude == 0)
    return 0;
  return sign ? magnitude | (0b1 << 14) : magnitude;
}

int
ALU (int command, int operand)
{
  int value, accum;
  sc_memoryGet (operand, &value);
  sc_accumulatorGet (&accum);

  int x = word_to_int (accum);
  int y = word_to_int (value);

  switch (command)
    {
    case 0x1E: // ADD
      sc_accumulatorSet (int_to_word (x + y));
      break;
    case 0x1F: // SUB
      sc_accumulatorSet (int_to_word (x - y));
      break;
    case 0x20: // DIVIDE
      if (y == 0)
        {
          sc_regSet (SC_DIV_BY_ZERO, 1);
          return -1;
        }
      sc_regSet (SC_DIV_BY_ZERO, 0);
      sc_accumulatorSet (int_to_word (x / y));
      break;
    case 0x21: // MUL
      sc_accumulatorSet (int_to_word (x * y));
      break;
    case 0x33: // NOT
      sc_memorySet (value, ~accum & 0b111111111111111);
      break;
    case 0x34: // AND
      sc_accumulatorSet (accum & value);
      break;
    case 0x35: // OR
      sc_accumulatorSet (accum | value);
      break;
    case 0x36: // XOR
      sc_accumulatorSet (accum ^ value);
      break;
    default: // INVALID COMMAND
      sc_regSet (SC_INVALID_COMMAND, 1);
      return -1;
    }

  sc_regSet (SC_INVALID_COMMAND, 0);
  return 0;
}
