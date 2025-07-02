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
        "libscpi/src/*",
        "libscpi/test/*"
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

