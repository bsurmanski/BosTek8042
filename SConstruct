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

exe_cflags = ['-Isrc', '-Ilib/cpplib/src', '-g']
lflags = ['-Llib/cpplib/bin', '-L.']
libs = ['-lcpp_common']

src_o = env.Object(srcs, CCFLAGS=exe_cflags)
env.Library('bin/bostek', src_o, CCFLAGS=exe_cflags, LINKFLAGS=lflags, LIBS=libs)
env.Program('bin/basm', asm_srcs, CCFLAGS=exe_cflags, LINKFLAGS=lflags, LIBS=libs)

test_src = ['bcpu_test.cpp']
test_src = ['build/test/' + t for t in test_src]
test_cflags = ['-Isrc', '-Ilib/cpplib/src']
test_libs = libs + ['-lgtest', '-lgtest_main']
test_lflags = lflags + ['-pthread']
env.Program('bin/gtest', test_src+src_o, CCFLAGS=test_cflags, LINKFLAGS=test_lflags, LIBS=test_libs)
