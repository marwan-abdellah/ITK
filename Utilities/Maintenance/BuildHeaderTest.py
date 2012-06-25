#!/usr/bin/env python

#==========================================================================
#
#   Copyright Insight Software Consortium
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#==========================================================================*/

usage = """usage: BuildHeaderTest.py <module_name> <module_source_path> <module_binary_path> <maximum_number_of_headers>

This script generates a a source file designed to check the headers in each
module.  The generated HeaderTest can be found in the module binary 'test'
directory in a file itk<module_name>HeaderTest#.cxx.  This contains a null
main(), but includes all the classes in the module.  The primary purpose of this
test is to make sure there are not missing module dependencies.  It also tests
for syntax and missing #include's.
"""

# Headers to not test because of dependecy issues, etc.
BANNED_HEADERS = set(('itkExceptionObject.h', # There is a pre-processor check so people use itkMacro.h instead.
    'itkMINC2ImageIO.h', # In case minc2.h is not available.
    'itkFFTWForwardFFTImageFilter.h',
    'itkFFTWInverseFFTImageFilter.h',
    'itkFFTWRealToHalfHermitianForwardFFTImageFilter.h',
    'itkFFTWHalfHermitianToRealInverseFFTImageFilter.h',
    'itkFFTWComplexToComplexImageFilter.h',
    'itkFFTWCommon.h',
    'itkPyBuffer.h', # needs Python.h, etc
    'itkVanHerkGilWermanErodeDilateImageFilter.h', # circular include's
    'itkBSplineDeformableTransform.h',   # deprecated
    'vtkCaptureScreen.h',
    'itkBSplineDeformableTransformInitializer.h'))

HEADER = """/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// This file has been generated by BuildHeaderTest.py
// To regenerate, build the ITKHeaderTests target.
// This is a test to include each header file for Insight.

"""

TRAILER = """
#include <cstdlib> // needed for EXIT_SUCCESS macro

int main ( int , char* [] )
{

  return EXIT_SUCCESS;
}
"""

import glob
import os
import sys

if len(sys.argv) < 6:
    print(usage)
    sys.exit(1)

def main():
    module_name        = sys.argv[1]
    module_source_path = sys.argv[2]
    module_binary_path = sys.argv[3]
    maximum_number_of_headers = int(sys.argv[4])
    test_num           = int(sys.argv[5])

    # Get all the header files.
    include_dir = os.path.join(module_source_path, 'include')
    h_files = glob.glob(os.path.join(include_dir, '*.h'))
    h_files = [os.path.basename(h) for h in h_files]

    added_header_idx = maximum_number_of_headers * (test_num - 1)
    test_source_path = os.path.join(module_binary_path, 'test')
    if not os.path.exists(test_source_path):
        os.makedirs(test_source_path)
    test_source_file = os.path.join(test_source_path,
        str(module_name) + 'HeaderTest' + str(test_num) + '.cxx')

    test_src = open(test_source_file, 'w')
    try:
        test_src.write(HEADER)

        if added_header_idx + maximum_number_of_headers > len(h_files):
            max_idx = added_header_idx + len(h_files) % maximum_number_of_headers
        else:
            max_idx = added_header_idx + maximum_number_of_headers
        for i in range(added_header_idx, max_idx):
            # Use the .hxx if possible.
            hxx_file = h_files[i][:-1] + 'hxx'
            if h_files[i] in BANNED_HEADERS:
                to_include = '// #include "' + h_files[i] + '" // Banned in BuildHeaderTest.py\n'
            elif os.path.exists(os.path.join(module_source_path, 'include',
                hxx_file)):
                to_include = '#include "' + hxx_file + '"\n'
            else:
                to_include = '#include "' + h_files[i] + '"\n'

            test_src.write(to_include)

        test_src.write(TRAILER)
    finally:
        test_src.close()

    return 0

if __name__ == "__main__":
    ret = main()
    sys.exit(ret)
