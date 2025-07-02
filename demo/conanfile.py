from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class ScpiParserDemoConan(ConanFile):
    name = "scpi_parser_demo"
    version = "1.0.0"

    # Package metadata
    description = "Demo applications showcasing SCPI Parser functionality"
    author = "SCPI Parser Team"
    topics = ("scpi", "parser", "demo", "measurement", "instruments")
    url = "https://github.com/j123b567/scpi-parser"
    license = "BSD-2-Clause"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "demo-*/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        # Depend on the scpi_parser package from artifactory
        self.requires("scpi_parser/2.1.0")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # This is a demo package, no libraries to export
        pass 