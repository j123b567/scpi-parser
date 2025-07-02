# SCPI Parser Demo Applications

This directory contains demo applications that showcase how to use the SCPI Parser library as an external Conan package dependency.

## Overview

These demos demonstrate practical usage of the SCPI Parser library in different scenarios:

- **demo-interactive**: Command-line interactive SCPI interface
- **demo-tcp**: TCP server with SCPI protocol support

Both demos simulate a measurement instrument with voltage, current, and frequency control capabilities.

## Prerequisites

- **Conan 2.x**: Package manager for C/C++
- **CMake 3.28+**: Build system
- **C Compiler**: GCC, Clang, or MSVC
- **scpi_parser/2.1.0**: The SCPI Parser library package (assumed to be available in your artifactory)

## Quick Start

### 1. Install Dependencies

```bash
# Create build directory
mkdir build && cd build

# Install dependencies from Conan
conan install .. --build=missing

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake

# Build the demos
cmake --build .
```

### 2. Run Interactive Demo

```bash
# Windows
.\demo-interactive\demo-interactive-scpi.exe

# Unix/Linux/macOS
./demo-interactive/demo-interactive-scpi
```

### 3. Run TCP Demo

```bash
# Windows
.\demo-tcp\demo-tcp-scpi.exe

# Unix/Linux/macOS  
./demo-tcp/demo-tcp-scpi
```

Then connect using telnet:
```bash
telnet localhost 5025
```

## Demo Applications

### Interactive Demo (`demo-interactive`)

**Features:**
- Command-line SCPI interface
- Built-in help system
- Interactive command entry
- Real-time command processing

**Usage:**
```bash
demo-interactive-scpi [options]

Options:
  -h, --help     Show help information
  -v, --version  Show version information
```

**Example Session:**
```
=== SCPI Parser Interactive Demo ===
Type 'help' for available commands, 'quit' to exit.

SCPI> *IDN?
DEMO_SCPI_PARSER,Interactive Demo,SN12345,Rev A

SCPI> SOUR:VOLT 5.0
Set voltage: 5.000 V

SCPI> SOUR:VOLT?
5

SCPI> MEAS:VOLT?
Measured voltage: 5.023 V
5.023

SCPI> quit
Goodbye!
```

### TCP Demo (`demo-tcp`)

**Features:**
- Cross-platform TCP server (Windows/Unix)
- Multiple client support (sequential)
- Network-based SCPI interface
- Configurable port

**Usage:**
```bash
demo-tcp-scpi [options]

Options:
  -p, --port <port>  Set TCP port (default: 5025)
  --test-mode        Enable test mode (exits after startup)
  -h, --help         Show help information
  -v, --version      Show version information
```

**Example Connection:**
```bash
# Start the server
demo-tcp-scpi --port 5025

# Connect from another terminal
telnet localhost 5025

# Send SCPI commands
*IDN?
SOUR:VOLT 10.0
MEAS:VOLT?
```

## SCPI Commands Supported

Both demos support the same set of SCPI commands:

### IEEE Mandated Commands
- `*IDN?` - Instrument identification
- `*RST` - Reset instrument
- `*TST?` - Self test

### System Commands
- `SYST:ERR?` - Query error queue

### Measurement Commands
- `MEAS:VOLT?` - Measure DC voltage
- `MEAS:CURR?` - Measure DC current

### Source Commands
- `SOUR:VOLT <value>` - Set voltage (-30V to +30V)
- `SOUR:VOLT?` - Query voltage setting
- `SOUR:CURR <value>` - Set current (-1A to +1A)
- `SOUR:CURR?` - Query current setting
- `SOUR:FREQ <value>` - Set frequency (1Hz to 1MHz)
- `SOUR:FREQ?` - Query frequency setting

### Output Commands
- `OUTP ON|OFF` - Enable/disable output
- `OUTP?` - Query output state

### Command Abbreviations

SCPI supports command abbreviations. These are equivalent:
- `SOURCE:VOLTAGE?` = `SOUR:VOLT?` = `SO:V?`
- `MEASURE:VOLTAGE?` = `MEAS:VOLT?` = `M:V?`
- `OUTPUT:STATE ON` = `OUTP:STAT ON` = `O:S ON` = `OUTP ON`

## Package Integration

### Conanfile Structure

The `conanfile.py` demonstrates how to:
- Depend on external `scpi_parser/2.1.0` package
- Configure cross-platform builds
- Handle shared/static library options
- Set up proper CMake integration

```python
def requirements(self):
    # Depend on the scpi_parser package from artifactory
    self.requires("scpi_parser/2.1.0")
```

### CMake Integration

The CMake files show how to:
- Find and link the SCPI Parser package
- Handle platform-specific libraries (Windows: ws2_32)
- Configure build options

```cmake
# Find the SCPI Parser package from Conan
find_package(scpi_parser REQUIRED)

# Link against the package
target_link_libraries(demo-tcp-scpi PRIVATE scpi_parser::scpi_parser)
```

## Development Notes

### Adding New Commands

1. **Define the callback function** in `scpi-commands.c`:
```c
static scpi_result_t DMM_MyCommand(scpi_t * context) {
    // Implementation here
    return SCPI_RES_OK;
}
```

2. **Add to command table**:
```c
const scpi_command_t scpi_commands[] = {
    // ... existing commands ...
    { .pattern = "MY:COMMAND", .callback = DMM_MyCommand,},
    SCPI_CMD_LIST_END
};
```

### Error Handling

The demos show proper SCPI error handling:
- Parameter validation
- Error queue management
- Appropriate error responses

### Cross-Platform Considerations

The TCP demo demonstrates:
- Windows socket initialization (`WSAStartup`/`WSACleanup`)
- Platform-specific socket functions
- Conditional compilation for different platforms

## Building from Source

If you need to build the scpi_parser package yourself:

```bash
# In the main scpi-parser directory
conan create . scpi_parser/2.1.0@
```

## Troubleshooting

### Common Issues

1. **Missing scpi_parser package**:
   ```
   ERROR: Package 'scpi_parser/2.1.0' not found
   ```
   - Ensure the package is available in your Conan remote
   - Check package name and version

2. **Build errors on Windows**:
   - Ensure Visual Studio 2017+ is installed
   - Check that the correct runtime library is selected

3. **TCP connection refused**:
   - Verify the port is not in use
   - Check firewall settings
   - Ensure the server is running

### Debug Build

For debugging, use:
```bash
conan install .. -s build_type=Debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
```

## License

These demos are provided under the same BSD-2-Clause license as the SCPI Parser library.

## Contributing

When adding new demos:
1. Follow the existing code structure
2. Add appropriate CMake configuration
3. Update this README
4. Test on multiple platforms 