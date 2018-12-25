#include <array>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdint.h>

#include "cxxopts/cxxopts.hpp"
#include "elfio/elfio.hpp"
#include "ripes/mainmemory.h"

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

// Position in memory where we place input arguments, used for running main()
// programs with integer arguments
#define ARGV_START 0x8ffffff0

enum class LerosInstr {
  nop,
  add,
  addi,
  sub,
  subi,
  sra,
  load,
  loadi,
  And,
  Andi,
  Or,
  Ori,
  Xor,
  Xori,
  loadhi,
  loadh2i,
  loadh3i,
#ifdef LEROS64
  loadh4i,
  loadh5i,
  loadh6i,
  loadh7i,
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
  ldind,
  ldindbu,
  stind,
  stindb,
  scall,
  unknown
};

enum SimRetval { ALL_OK, JAL_RA_EXIT, SCALL, ERROR };

template <typename T, unsigned B> inline T signextend(const T x) {
  struct {
    T x : B;
  } s;
  return s.x = x;
}

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
  std::string argv;
  std::string filename;
  bool onlyShowModifiedRegs;
  bool printState;
};

class LerosSim {
public:
  LerosSim(const LerosOptions &opt) : m_options(opt) {
    // PC entry position. Will be 0 for flat binary files, and set accordingly
    // for ELF files, where relocations have been specified relative to the
    // entry point
    unsigned long entryPoint;

    if (m_reader.load(opt.filename)) {
      // Load ELF file
      m_isELF = true;

      entryPoint = m_reader.get_entry();

      for (const ELFIO::section *section : m_reader.sections) {
        const auto sectionStart = section->get_address();
        const auto sectionEnd = sectionStart + section->get_size();
        if (sectionStart != 0) {
          if (section->get_name() == ".text") {
            m_textSize = section->get_size();
          }

          const char *p = section->get_data();
          if (p) {
            for (int i = sectionStart; i < sectionEnd; i++) {
              m_mem.write(i, *p, 1);
              p++;
            }
          }
        }
      }
    } else {
      // Loading binary file
      entryPoint = 0; // We always start at PC=0x0 for flat binary files
      std::ifstream is(opt.filename, std::ifstream::binary);
      assert(is.is_open() && "Could not open input file");

      is.seekg(0, is.end);
      m_textSize = is.tellg();
      assert(m_textSize % 2 == 0 && "File must be 16-bit aligned");
      is.seekg(0, is.beg);
      char *buffer = new char[m_textSize];
      is.read(buffer, m_textSize);
      for (int i = 0; i < m_textSize; i++) {
        m_mem.write(i, buffer[i], 1);
      }
      delete[] buffer;
    }

    m_entryPoint = entryPoint;

    reset();

    // Load register state
    for (const auto &p : opt.initRegState) {
      m_reg[p.first] = p.second;
    }
  }

  bool isModified(unsigned reg) {
    return m_modifiedRegs.find(reg) != m_modifiedRegs.end();
  }

  void setModified(unsigned reg) { m_modifiedRegs.insert(reg); }

  // Print registers
  void printState() {
    // Always display R4 state
    setModified(4);
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
    std::cout << "INSTRUCTIONS EXECUTED: " << m_instructionsExecuted
              << std::endl;
  }

  void reset() {
    for (auto &r : m_reg) {
      r = 0;
    }
    m_acc = 0;
    m_addr = 0;
    m_pc = m_entryPoint;

    if (m_isELF) {
      // Insert the input arguments into memory
      std::istringstream f(m_options.argv);
      std::string buf;
      int i = 0;
      while (getline(f, buf, ' ')) {
        auto argValue = static_cast<uint32_t>(atoi(buf.c_str()));
        m_mem.write(ARGV_START + i * sizeof(int), argValue, 4);
        i++;
      }

      // Set argc/argv
      m_reg[4] = i;
      m_reg[5] = ARGV_START;
    }

    // Set the stack pointer to a default value
    m_reg[1] = 0x7FFFFFF0;
  }

  int clock() {
    m_instructionsExecuted++;
    uint16_t instr = m_mem.read(m_pc) & 0xFFFF;

    // Constrain simulator to only run instructions in the .text segment
    if (m_pc >= m_entryPoint && m_pc <= m_entryPoint + m_textSize) {
      m_trace.insert(m_trace.begin(), m_pc);
      if (m_trace.size() > m_traceSize)
        m_trace.pop_back();
      return execInstr(instr);
    } else {
      return 1;
    }
  }

private:
  LerosInstr decodeInstr(uint8_t opcode) {
    const uint8_t bOpcode = opcode >> 4;

    // clang-format off
    switch (bOpcode) {
    default: break;
    case 0b1000: return LerosInstr::br;
    case 0b1001: return LerosInstr::brz;
    case 0b1010: return LerosInstr::brnz;
    case 0b1011: return LerosInstr::brp;
    case 0b1100: return LerosInstr::brn;
    }

    switch(opcode){
    default: assert(false && "Could not match opcode");
    case 0x0: return LerosInstr::nop;
    case 0x08: return LerosInstr::add;
    case 0x09: return LerosInstr::addi;
    case 0x0c: return LerosInstr::sub;
    case 0x0d: return LerosInstr::subi;
    case 0x10: return LerosInstr::sra;
    case 0x20: return LerosInstr::load;
    case 0x21: return LerosInstr::loadi;
    case 0x22: return LerosInstr::And;
    case 0x23: return LerosInstr::Andi;
    case 0x24: return LerosInstr::Or;
    case 0x25: return LerosInstr::Ori;
    case 0x26: return LerosInstr::Xor;
    case 0x27: return LerosInstr::Xori;
    case 0x29: return LerosInstr::loadhi;
    case 0x2a: return LerosInstr::loadh2i;
    case 0x2b: return LerosInstr::loadh3i;
    case 0x30: return LerosInstr::store;
    case 0x39: return LerosInstr::out;
    case 0x05: return LerosInstr::in;
    case 0x40: return LerosInstr::jal;
    case 0x50: return LerosInstr::ldaddr;
    case 0x60: return LerosInstr::ldind;
    case 0x61: return LerosInstr::ldindbu;
    case 0x70: return LerosInstr::stind;
    case 0x71: return LerosInstr::stindb;
    case 0xff: return LerosInstr::scall;
    }
    // clang-format on

    return LerosInstr::unknown;
  }

  int execInstr(uint16_t instr) {
    const uint8_t uimm8 = instr & 0xFF;
    const int simm8 = signextend<int, 8>(instr);
    const int simm13lsb0 = signextend<int, 13>(instr << 1);
    const LerosInstr inst = decodeInstr((instr >> 8) & 0xFF);

    // clang-format off
    switch (inst) {
    default:
    case LerosInstr::unknown: assert("Unknown instruction"); break;
    case LerosInstr::nop: break;
    case LerosInstr::addi: m_acc += simm8; break;
    case LerosInstr::add:  m_acc += m_reg[uimm8]; break;
    case LerosInstr::subi: m_acc -= simm8; break;
    case LerosInstr::sub:  m_acc -= m_reg[uimm8]; break;
    case LerosInstr::sra: {
      m_acc >>= 1;
      break;
    }
    case LerosInstr::loadi:  m_acc = simm8; break;
    case LerosInstr::load:   m_acc = m_reg[uimm8]; break;
    case LerosInstr::Andi:   m_acc &= uimm8; break;
    case LerosInstr::And:    m_acc &= m_reg[uimm8]; break;
    case LerosInstr::Ori:    m_acc |= uimm8; break;
    case LerosInstr::Or:     m_acc |= m_reg[uimm8]; break;
    case LerosInstr::Xori:   m_acc ^= uimm8; break;
    case LerosInstr::Xor:    m_acc ^= m_reg[uimm8]; break;
    case LerosInstr::loadhi:  m_acc = (m_acc & 0xff) | simm8 << 8;  break;
    case LerosInstr::loadh2i: m_acc = (m_acc & 0xffff) | simm8 << 16; break;
    case LerosInstr::loadh3i: m_acc = (m_acc & 0xffffff) | simm8 << 24; break;
#ifdef LEROS64
    case LerosInstr::loadh4i: m_acc = (m_acc & 0xffffffff) | imm << 32; break;
    case LerosInstr::loadh5i: m_acc = (m_acc & 0xffffffffff) | imm << 40; break;
    case LerosInstr::loadh6i: m_acc = (m_acc & 0xffffffffffff) | imm << 48; break;
    case LerosInstr::loadh7i: m_acc = (m_acc & 0xffffffffffffff) | imm << 56; break;
#endif
    case LerosInstr::store: {
        m_reg[uimm8] = m_acc;
        setModified(uimm8);
      break;
    }
    case LerosInstr::out: assert("Unimplemented"); break;
    case LerosInstr::in: assert("Unimplemented"); break;
    case LerosInstr::jal: {
      if (m_acc > m_entryPoint + m_textSize) {
        assert("Executing code outside of .text segment");
      }
      m_reg[uimm8] = m_pc + ILEN; // Store PC + 2 bytes
      setModified(uimm8);
      m_pc = static_cast<uint32_t>(m_acc);
      return ALL_OK;
    }
    case LerosInstr::br: m_pc += simm13lsb0; return ALL_OK;
    case LerosInstr::brz: {
      if (m_acc == 0) {
        m_pc += simm13lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::brnz: {
      if (m_acc != 0) {
        m_pc += simm13lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::brp: {
      if (m_acc >= 0) {
        m_pc += simm13lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::brn: {
      if (m_acc < 0) {
        m_pc += simm13lsb0;
        return ALL_OK;
      }
      break;
    }
    case LerosInstr::ldaddr: m_addr = m_reg[uimm8]; break;
    case LerosInstr::ldind: {
      const auto addr = (m_addr + (simm8 << 2));
      const auto value = static_cast<MVT_S>(m_mem.read(addr));
      m_acc = value;
      break;
    }
    case LerosInstr::ldindbu: m_acc = static_cast<MVT_S>(m_mem.read(m_addr + simm8)) & 0xFF; break;

    case LerosInstr::stind:{
        const auto addr = (m_addr + (simm8 << 2));
        m_mem.write(addr, m_acc, 4);
        break;
    }
    case LerosInstr::stindb: m_mem.write((m_addr + simm8), m_acc & 0xFF, 1); break;
    case LerosInstr::scall: {
      switch (uimm8) {
      default:
      case 0:
        return SCALL;
      case 1:
        m_reg[4] = m_instructionsExecuted;
        break;
      case 2:
        std::cout << static_cast<char>(m_acc);
        std::cout.flush();
        break;
      }
    }
    }
    // clang-format on

    m_pc += ILEN;
    return ALL_OK;
  }

  std::set<unsigned> m_modifiedRegs;
  MainMemoryTemplate<uint32_t, uint32_t> m_mem;
  std::array<MVT_S, 256> m_reg;
  std::vector<uint32_t> m_trace;
  const int m_traceSize =
      100; // record the 100 most recent PC's during execution
  MVT_S m_acc = 0;
  MVT m_addr = 0;
  MVT m_pc = 0;
  MVT m_entryPoint;
  int m_textSize;
  int m_instructionsExecuted = 0;
  bool m_isELF = false;
  ELFIO::elfio m_reader;

  LerosOptions m_options;
};

void setupOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options()
          ("f,file", "File name - Required", cxxopts::value<std::string>())
          ("ps", "Print simulator state after simulation", cxxopts::value<bool>()->default_value("false"))
          ("osmr", "Only show modified registers in printout (implicitely enables --ps)", cxxopts::value<bool>()->default_value("false"))
          ("rs", "Initial register staet, commaseparated list of format '0:2,4:10,...", cxxopts::value<std::string>()->default_value(""))
          ("argv", "Input argument(s) for C programs with a main(argc, argv) function, specified as a string \"1 2 foo bar\"", cxxopts::value<std::string>()->default_value(""))
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
  try {
    auto result = options.parse(argc, argv);
    opt.filename = result["f"].as<std::string>();
    opt.printState = result["ps"].as<bool>();
    opt.onlyShowModifiedRegs = result["osmr"].as<bool>();
    if (opt.onlyShowModifiedRegs) {
      opt.printState = true;
    }
    opt.initRegState = parseInitRegState(result["rs"].as<std::string>());
    opt.argv = result["argv"].as<std::string>();
  } catch (cxxopts::OptionException e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  LerosSim sim(opt);

  while (sim.clock() == SimRetval::ALL_OK) {
    // Clock until return != ALL_OK
  }

  // Show the state of the processor
  if (opt.printState)
    sim.printState();

  return 0;
}
