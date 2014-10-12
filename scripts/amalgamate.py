import re
import os

INCLUDE_DIR = '../include/'
VELOX_INCLUDE = re.compile(r'\s*#include\s+"([^"]*)"')
OTHER_INCLUDE = re.compile(r'\s*#include\s+<([^>]*)>')
INCLUDE_GUARD = re.compile(r'\s*#.*VELOX_.*_H_INCLUDED')

processed_files = set([])
other_headers = set([])
whitespace_lines = 0

def amalgamate(fout, header_file):
    global whitespace_lines
    
    with open(header_file) as f:
        for line in f:
            m = VELOX_INCLUDE.match(line)
            if m:
                h = m.group(1)
                if h in processed_files:
                    continue
                
                processed_files.add(h)
                amalgamate(fout, os.path.join(INCLUDE_DIR, h))
            else:
                if INCLUDE_GUARD.match(line):
                    continue
                
                m = OTHER_INCLUDE.match(line)
                if m:
                    h = m.group(1)
                    if h in other_headers:
                        continue
                    
                    other_headers.add(h)
                
                if not line or line.isspace():
                    whitespace_lines += 1
                else:
                    whitespace_lines = 0

                if whitespace_lines < 2:
                    fout.write(line.rstrip() + '\n')

def main():
    with open('velox_amalgamation.h', 'w') as fout:
        fout.write('''// The MIT License (MIT)
//
// Copyright (c) 2014 Clayton Trychta
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.\n
''')
        fout.write('#ifndef VELOX_AMALGAMATION_H_INCLUDED\n')
        fout.write('#define VELOX_AMALGAMATION_H_INCLUDED\n')
        amalgamate(fout, os.path.join(INCLUDE_DIR, 'velox.h'))
        fout.write('#endif // VELOX_AMALGAMATION_H_INCLUDED\n\n')

if __name__ == "__main__":
    main()

