from conan import ConanFile
from os.path import join
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, collect_libs
from pathlib import Path



class ScpiParserRecipe(ConanFile):
    name = "scpi_parser"
    version = "2.1.0"
    settings = "os", "compiler", "build_type", "arch"
    
    short_paths = True

    options = {"shared": [True, False]}
    default_options = {"shared": True}

    # Add exports for source files from libscpi folder
    exports_sources = [
        "CMakeLists.txt",
        "libscpi/CMakeLists.txt",
        "libscpi/inc/*",
        "libscpi/src/*"
    ]


    def requirements(self):
        # self.requires("cunit/2.1-3")
        self.requires("conan_cunit/2.1-3")

    def layout(self):
        cmake_layout(self, src_folder=".")
        self.folders.generators = "build"
        self.cpp.build.libdirs = "lib"
        self.cpp.build.bindirs = "bin"


    def config_options(self):
        if self.settings.os == "Windows":
            if self.options.shared:
                self.options.rm_safe("fPIC")


    def generate(self):
        for dep in self.dependencies.values():
            if dep.cpp_info.libdirs:
                copy(self, "*.lib", src=dep.cpp_info.libdirs[0], dst=join(self.cpp.build.libdirs, ""), keep_path=False)  
                copy(self, "*.dll", src=dep.cpp_info.bindirs[0], dst=join(self.cpp.build.bindirs, ""), keep_path=False)
                copy(self, "*.bin", src=dep.cpp_info.bindirs[0], dst=join(self.cpp.build.bindirs, ""), keep_path=False)

        tc = CMakeToolchain(self)
        tc.variables["CMAKE_POLICY_DEFAULT_CMP0091"] = "NEW"
        tc.variables["CMAKE_FIND_ROOT_PATH_MODE_PACKAGE"] = "BOTH"


        if self.settings.compiler == "msvc":
            if self.settings.build_type == "Release" or "MD" == str(self.settings.compiler.runtime):
                print("compiler.runtime is MD")
                tc.preprocessor_definitions.debug["CMAKE_MSVC_RUNTIME_LIBRARY"] = "MultiThreadedDLL"



        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()


    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        
        # Copy headers from libscpi/inc to include directory
        copy(self, "*.h", 
             src=join(self.source_folder, "libscpi", "inc"), 
             dst=join(self.package_folder, "include"), 
             keep_path=True)
        
        # Determine build configuration directory
        build_config = str(self.settings.build_type)
        
        # Copy library files - try multiple possible locations
        lib_dirs = [
            join(self.build_folder, "build", "lib"),           # New location with output directories
            join(self.build_folder, "libscpi", build_config),  # Windows Visual Studio
            join(self.build_folder, "libscpi"),                # Unix/MinGW
            join(self.build_folder, "lib"),                    # Alternative location
        ]
        
        for lib_dir in lib_dirs:
            # Windows library files (.lib for static, .dll for shared)
            copy(self, "*.lib", 
                 src=lib_dir, 
                 dst=join(self.package_folder, "lib"), 
                 keep_path=False)
            # Unix library files (.a for static, .so for shared)
            copy(self, "*.a", 
                 src=lib_dir, 
                 dst=join(self.package_folder, "lib"), 
                 keep_path=False)
            copy(self, "*.so*", 
                 src=lib_dir, 
                 dst=join(self.package_folder, "lib"), 
                 keep_path=False)
            # macOS library files
            copy(self, "*.dylib*", 
                 src=lib_dir, 
                 dst=join(self.package_folder, "lib"), 
                 keep_path=False)
        
        # Copy DLL files for Windows shared builds
        bin_dirs = [
            join(self.build_folder, "build", "bin"),           # New location with output directories
            join(self.build_folder, "libscpi", build_config),  # Windows Visual Studio
            join(self.build_folder, "libscpi"),                # MinGW
            join(self.build_folder, "bin"),                    # Alternative location
        ]
        
        for bin_dir in bin_dirs:
            copy(self, "*.dll", 
                 src=bin_dir, 
                 dst=join(self.package_folder, "bin"), 
                 keep_path=False)
        

    def package_info(self):
        # Set the main library name (matches the CMake target name)
        self.cpp_info.libs = collect_libs(self)
        
        # Platform-specific settings
        if self.settings.os == "Windows":
            # Link with Windows socket library for TCP functionality
            self.cpp_info.system_libs.append("ws2_32")
        elif self.settings.os == "Linux":
            # Link with math library on Linux
            self.cpp_info.system_libs.append("m")
        
