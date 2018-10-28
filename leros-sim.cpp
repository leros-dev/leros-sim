#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdint.h>

#include "cxxopts/cxxopts.hpp"
#include "elfio/elfio.hpp"

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

enum SimRetval { ALL_OK, JAL_RA_EXIT, ERROR };

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
Another benefit of this is, that we can separate the logic for
immediate/register selection away from instruction matching
*/
const std::map<std::string, LerosInstr> InstMap{
    /*{"MSB-----LSB", LerosInstr}*/
    {"00000", LerosInstr::nop},       {"000010", LerosInstr::add},
    {"000011", LerosInstr::sub},      {"00010", LerosInstr::shr},
    {"00011", LerosInstr::unused},    {"0010000", LerosInstr::load},
    {"0010001", LerosInstr::And},     {"0010010", LerosInstr::Or},
    {"0010011", LerosInstr::Xor},     {"0010100", LerosInstr::loadh},
    {"0010101", LerosInstr::loadh2},  {"0010110", LerosInstr::loadh3},
#ifdef LEROS64
    {"NANANANA", LerosInstr::loadh4}, {"NANANANA", LerosInstr::loadh5},
    {"NANANANA", LerosInstr::loadh6}, {"NANANANA", LerosInstr::loadh7},
#endif
    {"00110", LerosInstr::store},     {"001110", LerosInstr::out},
    {"000001", LerosInstr::in},       {"01000", LerosInstr::jal},
    {"10000", LerosInstr::br},        {"10001", LerosInstr::brz},
    {"10010", LerosInstr::brnz},      {"10011", LerosInstr::brp},
    {"10100", LerosInstr::brn},       {"01010", LerosInstr::ldaddr},
    {"01100", LerosInstr::loadind},   {"01110", LerosInstr::storeind}};

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

struct LerosOptions {
  std::map<unsigned, MVT_S> initRegState;
  std::string filename;
  bool onlyShowModifiedRegs;
  bool exitOnJalRA;
  bool printState;
};

class LerosSim {
public:
  LerosSim(const LerosOptions &opt) : m_options(opt) {
    // Load file into main m_memory - has some overhead, but required if m_mem
    // datastructure is a std::map

    if (!m_reader.load(opt.filename)) {
      std::ifstream is(opt.filename, std::ifstream::binary);
      assert(is.is_open() && "Could not open input file");

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
    } else {
      std::cout << "Loading .ELF file" << std::endl;
    }

    reset();

    // Load register state
    for (const auto &p : opt.initRegState) {
      m_reg[p.first] = p.second;
    }
  }

  bool isModified(unsigned reg) {
    return m_modifiedRegs.find(reg) != m_modifiedRegs.end();
  }

  void printState() {
    // Print registers
    for (unsigned i = 0; i < 256; i++) {
      if (m_options.onlyShowModifiedRegs) {
        if (!isModified(i))
          continue;
      }
      std::cout << i << ":" << m_reg[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "ACC: " << m_acc << std::endl;
    std::cout << "ADDR: " << m_addr << std::endl;
    std::cout << "PC: " << m_pc << std::endl;
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
    uint16_t instr = m_mem[m_pc] | m_mem[m_pc + 1] << 8;
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
    const int simm8 = signextend<int, 8>(instr);
    const int simm11lsb0 = signextend<int, 11>(instr) << 1;
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
        m_modifiedRegs.insert(uImmRaw);
      } else {
        assert("store does not work with and immediate operand?");
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
      if (uImmRaw == 0 & m_options.exitOnJalRA)
        return JAL_RA_EXIT;
      m_reg[uImmRaw] = m_pc;
      m_modifiedRegs.insert(uImmRaw);
      m_pc = m_acc;
      return ALL_OK;
    }
    case LerosInstr::br: {
      m_pc += simm11lsb0;
      return ALL_OK;
    }
    case LerosInstr::brz: {
      if (m_acc == 0) {
        m_pc += simm11lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::brnz: {
      if (m_acc != 0) {
        m_pc += simm11lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::brp: {
      if (m_acc >= 0) {
        m_pc += simm11lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::brn: {
      if (m_acc < 0) {
        m_pc += simm11lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::ldaddr: {
      m_addr = m_reg[uImmRaw];
      break;
    }
    case LerosInstr::loadind: {
      m_acc = m_mem[m_addr + simm8];
      break;
    }
    case LerosInstr::storeind: {
      m_mem[m_addr + simm8] = m_acc;
      break;
    }
    }
    m_pc += ILEN;
    return ALL_OK;
  }

  std::set<unsigned> m_modifiedRegs;
  std::map<MVT, uint8_t> m_mem;
  std::array<MVT_S, 256> m_reg;
  MVT_S m_acc = 0;
  MVT m_addr = 0;
  MVT m_pc = 0;
  int m_textSize;
  ELFIO::elfio m_reader;

  LerosOptions m_options;
};

void setupOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options()
          ("f,file", "File name - Required", cxxopts::value<std::string>())
          ("je", "End simulation upon encountering jal ra", cxxopts::value<bool>()->default_value("false"))
          ("ps", "Print simulator state after simulation", cxxopts::value<bool>()->default_value("false"))
          ("osmr", "Only show modified registers in printout (implicitely enables --ps)", cxxopts::value<bool>()->default_value("false"))
          ("rs", "Initial register staet, commaseparated list of format '0:2,4:10,...", cxxopts::value<std::string>()->default_value(""))
          ;
  // clang-format on
}

std::map<unsigned, MVT_S> parseInitRegState(const std::string &string) {
  if (string.empty())
    return std::map<unsigned, MVT_S>();

  std::map<unsigned, MVT_S> state;
  std::vector<std::string> pairs;
  std::string tmp;
  std::istringstream f(string);
  while (std::getline(f, tmp, ',')) {
    pairs.push_back(tmp);
  }

  // parse pairs
  for (auto &p : pairs) {
    size_t pos = p.find(':');
    unsigned reg = std::stoul(p.substr(0, pos));
    p.erase(0, pos + 1);
    int value = std::stoi(p);
    state[reg] = value;
  }
  return state;
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("leros-sim",
                           "32- and 64 bit simulator for the Leros ISA");

  setupOptions(options);
  LerosOptions opt;

  if (argc < 2) {
    std::cout << options.help();
    return 1;
  }

  std::string filename;
  bool je, ps;
  try {
    auto result = options.parse(argc, argv);
    opt.filename = result["f"].as<std::string>();
    opt.exitOnJalRA = result["je"].as<bool>();
    opt.printState = result["ps"].as<bool>();
    opt.onlyShowModifiedRegs = result["osmr"].as<bool>();
    if (opt.onlyShowModifiedRegs) {
      opt.printState = true;
    }
    opt.initRegState = parseInitRegState(result["rs"].as<std::string>());
  } catch (cxxopts::OptionException e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  LerosSim sim(opt);

  while (sim.clock() == 0) {
    // Clock until return != 0
  }

  // Show the state of the processor
  if (opt.printState)
    sim.printState();
}
