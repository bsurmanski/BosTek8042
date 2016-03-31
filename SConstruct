project_name = 'Bostek8042'
env = Environment()
env.VariantDir('build', 'src', duplicate=0)
env.VariantDir('build/test', 'test', duplicate=0)

srcs = ['cpu.cpp',
        'bcpu.cpp',
        'northBridge.cpp',
        'memory.cpp',
        'io/file.cpp',
        'io/char.cpp',
        'io/exception.cpp',
        'io/string.cpp',
        'io/stringInput.cpp']
asm_srcs = [
            'asm.cpp',
        ]
srcs = ['build/' + s for s in srcs]
asm_srcs = ['build/' + s for s in asm_srcs]

exe_cflags = ['-Isrc', '-g']
lib_bostek = env.Library('bostek', srcs, CCFLAGS=exe_cflags )
exe_asm = env.Program('basm', asm_srcs+lib_bostek, CCFLAGS=exe_cflags )

tests = ['bcpu_test.cpp']
tests = ['build/test/' + t for t in tests]
test_cflags = ['-Isrc']
test_lflags = ['-lgtest', '-lgtest_main']
env.Program('gtest', tests+lib_bostek, CCFLAGS=test_cflags, LINKFLAGS=test_lflags)
