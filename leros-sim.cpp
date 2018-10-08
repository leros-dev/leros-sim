#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <stdint.h>

#ifdef LEROS64
#define MVT uint64_t
#define MVT_S int64_t
#define XLen 64
#else
#define MVT uint32_t
#define MVT_S int32_t
#define XLen 32
#endif

#define ILEN 2 // instruction length in bytes

enum class LerosInstr {
  nop,
  add,
  sub,
  shr,
  unused,
  load,
  And,
  Or,
  Xor,
  loadh,
  loadh2,
  loadh3,
#ifdef LEROS64
  loadh4,
  loadh5,
  loadh6,
  loadh7,
#endif
  store,
  out,
  in,
  jal,
  br,
  brz,
  brnz,
  brp,
  brn,
  ldaddr,
  loadind,
  storeind,
  unknown
};

template <typename T, unsigned B> inline T signextend(const T x) {
  struct {
    T x : B;
  } s;
  return s.x = x;
}

/**
pattern match the instruction going from MSB->LSB. This is done to be
able to easily add more instructions later on, while not defining what a
'-' (unspecified) bit has to be, without breaking the patterns.
*/
const std::map<std::string, LerosInstr> InstMap{
    /*{"MSB-----LSB", LerosInstr}*/
    {"00000", LerosInstr::nop},      {"000010", LerosInstr::add},
    {"000011", LerosInstr::sub},     {"00010", LerosInstr::shr},
    {"00011", LerosInstr::unused},   {"0010000", LerosInstr::load},
    {"0010001", LerosInstr::And},    {"0010010", LerosInstr::Or},
    {"0010011", LerosInstr::Xor},    {"0010100", LerosInstr::loadh},
    {"0010101", LerosInstr::loadh2}, {"0010110", LerosInstr::loadh3},
#ifdef LEROS64
    {"NA", LerosInstr::loadh4},      {"NA", LerosInstr::loadh5},
    {"NA", LerosInstr::loadh6},      {"NA", LerosInstr::loadh7},
#endif
    {"00110", LerosInstr::store},    {"001110", LerosInstr::out},
    {"000001", LerosInstr::in},      {"01000", LerosInstr::jal},
    {"01001000", LerosInstr::br},    {"01001001", LerosInstr::brz},
    {"01001010", LerosInstr::brnz},  {"01001011", LerosInstr::brp},
    {"01001100", LerosInstr::brn},   {"01010", LerosInstr::ldaddr},
    {"01100", LerosInstr::loadind},  {"01110", LerosInstr::storeind}};

inline void itoa(unsigned v, char *buf) {
  switch (v) {
  case 0: {
    *buf = '0';
    return;
  }
  case 1: {
    *buf = '1';
    return;
  }
  default: { assert("unknown value"); }
  }
  return;
}

class LerosSim {
public:
  LerosSim(char *filename) {
    // Load file into main m_memory - has some overhead, but required if m_mem
    // datastructure is a std::map
    std::ifstream is(filename, std::ifstream::binary);
    is.seekg(0, is.end);
    m_textSize = is.tellg();
    assert(m_textSize % 2 == 0 && "File must be 16-bit aligned");
    is.seekg(0, is.beg);
    char *buffer = new char[m_textSize];
    is.read(buffer, m_textSize);
    for (int i = 0; i < m_textSize; i++) {
      m_mem[i] = buffer[i];
    }
    delete[] buffer;

    reset();
  }

  void printState() {
    // Print registers
    for (int i = 0; i < 256; i++) {
      std::cout << m_reg[i] << " ";
    }
  }

  void reset() {
    for (auto &r : m_reg) {
      r = 0;
    }
    m_acc = 0;
    m_addr = 0;
    m_pc = 0;
  }

  int clock() {
    uint16_t instr = m_mem[m_pc] << 8 | m_mem[m_pc + 1];
    if (m_pc > m_textSize) {
      return 1;
    } else {
      return execInstr(instr);
    }
  }

private:
  MVT_S getImmediate(uint16_t instr, bool isImmediate) {
    if (!isImmediate) {
      return m_reg.at(instr & 0xFF);
    } else {
      return signextend<MVT_S, 8>(instr & 0xFF);
    }
  }

  LerosInstr decodeInstr(uint8_t opcode) {
    char buffer[8];
    for (int i = 7; i >= 0; i--) {
      auto bufPtr = buffer + (7 - i);
      itoa((opcode >> i) & 0b1, bufPtr);
    }
    auto iMapCpy = InstMap;

    int i = 0;

    while (i < 8) {
      // Reverse iterate to allow deletion
      for (auto iter = iMapCpy.begin(); iter != iMapCpy.end();) {
        if (iMapCpy.size() == 1) {
          // Found end-of-string of a potential match instruction - ie.
          // instruction matched, or only one instruction match candidate
          // remains in map
          return iter->second;
        }

        if (iter->first[i] != buffer[i]) {
          // Could not match instruction, remove from map
          iMapCpy.erase(iter++);
        } else {
          iter++;
        }
      }
      i++;
    }
    return LerosInstr::unknown;
  }

  int execInstr(uint16_t instr) {
    const bool isImmediate = instr & 0b100000000;
    const MVT_S imm = getImmediate(instr, isImmediate);
    const uint8_t uImmRaw = instr & 0xFF;
    const int8_t sImmRaw = instr & 0xFF;
    const uint8_t upperInstr = (instr >> 8) & 0xFF;
    const LerosInstr inst = decodeInstr(upperInstr);

    switch (inst) {
    default:
    case LerosInstr::unknown:
      assert("Unknown instruction");
    case LerosInstr::nop:
      break;
    case LerosInstr::add: {
      m_acc += imm;
      break;
    }
    case LerosInstr::sub: {
      m_acc -= imm;
      break;
    }
    case LerosInstr::shr: {
      assert(imm < XLen);
#ifdef LEROS64
      MVT mask = 0x7FFFFFFFFFFFFFFF;
#else
      MVT mask = 0x7FFFFFFF;
#endif
      m_acc >>= 1;
      m_acc &= mask;
      break;
    }
    case LerosInstr::unused: {
      break;
    }
    case LerosInstr::load: {
      m_acc = imm;
      break;
    }
    case LerosInstr::And: {
      if (!isImmediate) {
        m_acc &= imm;
      } else {
        // Only & the lower 8-bits
        MVT rest = m_acc & 0xFFFFFF00;
        m_acc &= 0xFF;
        m_acc &= (imm & 0xFF);
        m_acc |= rest;
      }
      break;
    }
    case LerosInstr::Or: {
      m_acc |= !isImmediate ? imm : imm & 0xFF;
      break;
    }
    case LerosInstr::Xor: {
      if (!isImmediate) {
        m_acc ^= imm;
      } else {
        // Only and the lower 8-bits
        MVT rest = m_acc & 0xFFFFFF00;
        m_acc &= 0xFF;
        m_acc ^= (imm & 0xFF);
        m_acc = rest | m_acc;
      }
      break;
    }
    case LerosInstr::loadh: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFF;
      m_acc = 0;
      m_acc |= imm << 8;
      m_acc |= bottom;
      break;
    }
    case LerosInstr::loadh2: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFFFF;
      m_acc = 0;
      m_acc |= imm << 16;
      m_acc |= bottom;
      break;
    }
    case LerosInstr::loadh3: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFFFFFF;
      m_acc = 0;
      m_acc |= imm << 24;
      m_acc |= bottom;
      break;
    }
#ifdef LEROS64
    case LerosInstr::loadh4: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFFFFFFFF;
      m_acc = 0;
      m_acc |= imm << 32;
      m_acc |= bottom;
      break;
    }
    case LerosInstr::loadh5: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFFFFFFFFFF;
      m_acc = 0;
      m_acc |= imm << 40;
      m_acc |= bottom;
      break;
    }
    case LerosInstr::loadh6: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFFFFFFFFFFFF;
      m_acc = 0;
      m_acc |= imm << 48;
      m_acc |= bottom;
      break;
    }
    case LerosInstr::loadh7: {
      // the immediate has been sign extended through getImmediate()
      MVT bottom = m_acc & 0xFFFFFFFFFFFFFF;
      m_acc = 0;
      m_acc |= imm << 56;
      m_acc |= bottom;
      break;
    }
#endif
    case LerosInstr::store: {
      if (!isImmediate) {
        m_reg[uImmRaw] = m_acc;
      } else {
        assert("Store directly from int to m_reg?");
      }
      break;
    }
    case LerosInstr::out: {
      assert("Unimplemented");
      break;
    }
    case LerosInstr::in: {
      assert("Unimplemented");
      break;
    }
    case LerosInstr::jal: {
      m_reg[uImmRaw] = m_pc + ILEN;
      m_pc = m_acc;
      return 0;
    }
    case LerosInstr::br: {
      m_pc += sImmRaw * ILEN;
      return 0;
    }
    case LerosInstr::brz: {
      if (m_acc == 0)
        m_pc += sImmRaw * ILEN;
      return 0;
    }
    case LerosInstr::brnz: {
      if (m_acc != 0)
        m_pc += sImmRaw * ILEN;
      return 0;
    }
    case LerosInstr::brp: {
      if (m_acc >= 0)
        m_pc += sImmRaw * ILEN;
      return 0;
    }
    case LerosInstr::brn: {
      if (m_pc < 0)
        m_pc += sImmRaw * ILEN;
      return 0;
    }
    case LerosInstr::ldaddr: {
      m_addr = m_reg[uImmRaw];
      break;
    }
    case LerosInstr::loadind: {
      m_acc = m_mem[m_addr + sImmRaw];
      break;
    }
    case LerosInstr::storeind: {
      m_mem[m_addr + sImmRaw] = m_acc;
      break;
    }
    }
    m_pc += ILEN;
    return 0;
  }

  std::map<MVT, uint8_t> m_mem;
  std::array<MVT_S, 256> m_reg;
  MVT_S m_acc = 0;
  MVT m_addr = 0;
  MVT m_pc = 0;
  int m_textSize;
};

int main(int argc, char *argv[]) {
  std::cout << "Leros " << XLen << " bit ISA simulator" << std::endl;
  if (argc < 2) {
    std::cout << "Usage: leros-sim [filename.bin]";
    return 1;
  }

  LerosSim sim(argv[1]);

  while (sim.clock() == 0) {
    // Clock until return != 0
  }

  // Show the state of the processor
  sim.printState();
}
