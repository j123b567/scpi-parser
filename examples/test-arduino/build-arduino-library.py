'''
This script creates scpi-parser library that is compatible with Arduino IDE.
As a convinience, it also copies a newly create library into arduino libraries
directory so any Arduino sketch can see it.
Execute this script if scpi-parser is changed or scpi_user_config.h (from this directory) is changed.
'''
from __future__ import (print_function)

import os
import platform
import sys
import shutil

def rm_then_cp(src, dest):
    if os.path.exists(dest):
        shutil.rmtree(dest)
    shutil.copytree(src, dest)

def build_scpi_parser_lib(libscpi_dir, scpi_parser_dir):
    '''
    Build scpi-parser as arduino compatible library
    '''

    # copy *.h files
    rm_then_cp(os.path.join(libscpi_dir, 'inc/scpi'), os.path.join(scpi_parser_dir, 'src/scpi'))

    # modify config.h
    config_h_file_path = os.path.join(scpi_parser_dir, 'src/scpi/config.h')
    config_h_file = open(config_h_file_path)
    tmp_file_path = config_h_file_path + ".tmp"
    tmp_file = open(tmp_file_path, "w")
    for line in config_h_file:
        if line == '#ifdef SCPI_USER_CONFIG\n':
            tmp_file.write('// This is automatically added by the build-arduino-library.py\n')
            tmp_file.write('#define SCPI_USER_CONFIG 1\n')
            tmp_file.write('\n')
        tmp_file.write(line)
    config_h_file.close()
    tmp_file.close()
    os.unlink(config_h_file_path)
    os.rename(tmp_file_path, config_h_file_path)

    # copy scpi_user_config.h
    shutil.copyfile(os.path.join(os.path.dirname(__file__), 'scpi_user_config.h'), os.path.join(scpi_parser_dir, 'src/scpi/scpi_user_config.h'))

    # copy *.c files
    rm_then_cp(os.path.join(libscpi_dir, 'src'), os.path.join(scpi_parser_dir, 'src/impl'))

def copy_lib(src_lib_dir, dst_name):
    #
    # find arduino libraries directory
    #
    ARDUINO_LIB_DIR_CANDIDATES = {
        'Linux': ['Arduino/libraries/', 'Documents/Arduino/libraries/'],
        'Darwin': ['Documents/Arduino/libraries/'],
        'Windows': ['Documents\\Arduino\\libraries\\', 'My Documents\\Arduino\\libraries\\']
    }

    home_dir = os.path.expanduser('~')

    arduino_libs_dir = None

    candidates = ARDUINO_LIB_DIR_CANDIDATES.get(platform.system())
    if candidates:
        for candidate_dir in ARDUINO_LIB_DIR_CANDIDATES.get(platform.system()):
            arduino_libs_dir = os.path.join(home_dir, candidate_dir)
            if os.path.exists(arduino_libs_dir):
                break

    if arduino_libs_dir:
        # copy arduino scpi-parser library to the arduino libraries folder
        rm_then_cp(src_lib_dir, os.path.join(arduino_libs_dir, dst_name))
        return True
    else:
        print("Arduino libraries directory not found!")
        return False

if __name__ == "__main__":
    libscpi_dir = os.path.join(os.path.dirname(__file__), '../../libscpi')
    scpi_parser_dir = os.path.join(os.path.dirname(__file__), 'scpi-parser')
    build_scpi_parser_lib(libscpi_dir, scpi_parser_dir)
    copy_lib(scpi_parser_dir, 'scpi-parser')