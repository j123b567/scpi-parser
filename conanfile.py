from conan import ConanFile
from os.path import join
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, collect_libs
from pathlib import Path



class ScpiParserRecipe(ConanFile):
    name = "scpi_parser"
    version = "2.1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    short_paths = True
    
    # Add exports for source files from libscpi folder
    exports_sources = [
        "libscpi/CMakeLists.txt",
        "libscpi/inc/*",
        "libscpi/src/*",
        "libscpi/test/*"
    ]
    
    def requirements(self):
        self.requires("conan_cunit/2.1-3")

    def layout(self):
        cmake_layout(self, src_folder="libscpi")
        self.folders.generators = "build"
        self.cpp.build.libdirs = "lib"
        self.cpp.build.bindirs = "bin"

    def generate(self):
        for dep in self.dependencies.values():
            if dep.cpp_info.libdirs:
                copy(self, "*.lib", src=dep.cpp_info.libdirs[0], dst=join(self.cpp.build.libdirs, ""), keep_path=False)  
                copy(self, "*.dll", src=dep.cpp_info.bindirs[0], dst=join(self.cpp.build.bindirs, ""), keep_path=False)
                copy(self, "*.bin", src=dep.cpp_info.bindirs[0], dst=join(self.cpp.build.bindirs, ""), keep_path=False)

        tc = CMakeToolchain(self)
        tc.variables["CMAKE_POLICY_DEFAULT_CMP0091"] = "NEW"
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
        
        # Copy additional files if needed
        copy(self, "*.h", src=join(self.source_folder, "inc"), dst=join(self.package_folder, "include"))
        copy(self, "*.hpp", src=join(self.source_folder, "inc"), dst=join(self.package_folder, "include"))
        copy(self, "*.cmake", src="cmake", dst=join(self.package_folder, "cmake"))
        copy(self, "*.md", src="doc", dst=join(self.package_folder, "doc"))

    def package_info(self):
        self.cpp_info.components["libscpi"].libs = ["scpi"]
        self.cpp_info.components["libscpi"].includedirs = ["include"]
        self.cpp_info.components["libscpi-static"].libs = ["scpi-static"]
        self.cpp_info.components["libscpi-static"].requires = ["libscpi"]