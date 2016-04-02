project_name = 'Bostek8042'
env = Environment()
env.VariantDir('build', 'src', duplicate=0)
env.VariantDir('build/test', 'test', duplicate=0)

srcs = ['bostek/cpu.cpp',
        'bostek/bcpu.cpp',
        'bostek/northBridge.cpp',
        'bostek/memory.cpp',]

asm_srcs = ['bostek/asm.cpp',]

srcs = ['build/' + s for s in srcs]
asm_srcs = ['build/' + s for s in asm_srcs]
cpplib_a = ['lib/cpplib/bin/cpplib_common.a']

exe_cflags = ['-Isrc', '-Ilib/cpplib/src', '-g']
exe_lflags = ['-Llib/cpplib/bin', '-L.', '-lcpplib_common']
bostek_a = env.Library('bin/bostek', srcs+cpplib_a, CCFLAGS=exe_cflags)
env.Program('bin/basm', asm_srcs+bostek_a+cpplib_a, CCFLAGS=exe_cflags)

tests = ['bcpu_test.cpp']
tests = ['build/test/' + t for t in tests]
test_cflags = ['-Isrc', '-Ilib/cpplib/src']
test_lflags = ['-lgtest', '-lgtest_main']
env.Program('bin/gtest', tests+bostek_a+cpplib_a, CCFLAGS=test_cflags, LINKFLAGS=test_lflags)
