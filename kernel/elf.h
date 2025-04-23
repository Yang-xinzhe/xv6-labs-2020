// Format of an ELF executable file
#include "types.h"
// ELF magic number (0x7F followed by "ELF" in ASCII)
#define ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// ELF file header - appears at the start of every ELF file
struct elfhdr {
    uint magic;      // Must equal ELF_MAGIC to be valid ELF file
    uchar elf[12];   // ELF identification bytes
    ushort type;     // Object file type (executable, shared object, etc)
    ushort machine;  // Target architecture (RISC-V, x86, etc)
    uint version;    // ELF version
    uint64 entry;    // Memory address of entry point
    uint64 phoff;    // File offset of program header table
    uint64 shoff;    // File offset of section header table
    uint flags;      // Processor-specific flags
    ushort ehsize;   // ELF header size in bytes
    ushort phentsize;// Size of program header entry
    ushort phnum;    // Number of program header entries
    ushort shentsize;// Size of section header entry
    ushort shnum;    // Number of section header entries
    ushort shstrndx; // Index of section name string table
};

// Program header - describes a segment to be loaded into memory
struct proghdr {
    uint32 type;     // Segment type (loadable, dynamic, etc)
    uint32 flags;    // Segment flags (read, write, execute)
    uint64 off;      // Offset in file where segment data begins
    uint64 vaddr;    // Virtual address where segment should be loaded
    uint64 paddr;    // Physical address (unused in most systems)
    uint64 filesz;   // Size of segment in the file (bytes)
    uint64 memsz;    // Size of segment in memory (bytes)
    uint64 align;    // Required alignment of segment
};

// Value for Proghdr type
#define ELF_PROG_LOAD           1  // Program section type - indicates a loadable segment

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC  1   // Executable segment  01
#define ELF_PROG_FLAG_WRITE 2   // Writable segment    10 
#define ELF_PROG_FLAG_READ  4   // Readable segment    100