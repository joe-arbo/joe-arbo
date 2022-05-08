#!/usr/bin/env python3
import sys
import os.path as opath

libraries = []
core_modules = []
files = []

library = False

for i in range(1, len(sys.argv)):
    if sys.argv[i] == '-l':
        library = True
    elif library:
        files.append([sys.argv[i], False])
        library = False
    else:
        files.append([sys.argv[i], True])

symbols_list = []
references = []

for file in files:
    
    f = open(file[0], 'r')
    lines = f.readlines()
    for line in lines:
        line = line.split()
        if line[0] == 'U':
            references.append([line[1], file[0]])
        elif line[3].find('__') != 0:
            line.append(file[0])
            symbols_list.append(line)

for file in files:
    globals = []

    for symbol in symbols_list:
        referenced = False
        for next_symbol in symbols_list:
            if symbol == next_symbol:
                continue

            if symbol[3] == next_symbol[3]:
                # Verify that each common symbol is defined in exactly one module
                if next_symbol[2] == 'C' and symbol[2] == 'C':
                    print('Common symbol', symbol[3],'multiply defined, first in', symbol[4].replace('.nm',''),
                            'and now in', next_symbol[4].replace('.nm',''))
                    symbols_list.remove(next_symbol)
                # Verify that no common symbol is dominated by a strong global symbol
                elif next_symbol[2].isupper() and symbol[2] == 'C' and next_symbol[2] != 'C':
                    print('Weak', symbol[3],'overridden, defined in', symbol[4].replace('.nm',''),
                            'and overridden as', next_symbol[2],'in', next_symbol[4].replace('.nm',''))
                    symbols_list.remove(symbol)
                    referenced = True
                elif symbol[2].isupper() and next_symbol[2] == 'C' and symbol[2] != 'C':
                    print('Weak', next_symbol[3],'overridden, defined in', next_symbol[4].replace('.nm',''),
                            'and overridden as', symbol[2],'in', symbol[4].replace('.nm',''))
                    symbols_list.remove(next_symbol)
                    referenced = True
                # Check if there are any static functions that are defined in multiple 
                # modules with the same name and the same size
                elif (next_symbol[2] == 't' and symbol[2] == 't' and next_symbol[1] == symbol[1]):
                    print('Static function', symbol[3], 'of size', hex(int(symbol[1], 16)), 'appears in both', 
                            symbol[4].replace('.nm',''), 'and', next_symbol[4].replace('.nm','') + ', check for inlining')
                    symbols_list.remove(next_symbol)
        # Check that each global symbol that is defined in a core module is
        # referenced outside the module in which it is defined
        if file[1] and symbol[2] != 'C' and symbol[2].isupper() and symbol[4] == file[0] and symbol[3] != 'main':
            # referenced = False
            for reference in references:
                if reference[0] == symbol[3] and reference[1] != symbol[4]:
                    referenced = True
            if not referenced:
                print('Global symbol', symbol[3], 'defined in', symbol[4].replace('.nm',''),
                        'is not referenced by any other file, should be static')
        # For each library module, determine the longest prefix shared by all
        # exported symbols
        elif (not file[1]) and symbol[2].isupper() and symbol[4] == file[0]:
            globals.append(symbol[3])
    
    prefix = opath.commonprefix(globals)
    if prefix == '' and (not file[1]):
        print('Global symbols exported by library', file[0].replace('.nm',''), 'do not share common prefix')
    elif (not file[1]):
        print('Global symbols exported by library', file[0].replace('.nm',''), 'share common prefix', prefix)
        