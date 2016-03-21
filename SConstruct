project_name = 'Bostek8042'
env = Environment()
env.VariantDir('build', 'src', duplicate=0)
env.VariantDir('build/test', 'test', duplicate=0)

srcs = ['cpu.cpp',
        'bcpu.cpp',
        'northBridge.cpp',
        'memory.cpp']
srcs = ['build/' + s for s in srcs]
exe_cflags = ['-Isrc']
libbostek = env.Library('bostek', srcs, CCFLAGS=exe_cflags )

tests = ['bcpu_test.cpp']
tests = ['build/test/' + t for t in tests]
test_cflags = ['-Isrc']
test_lflags = ['-lgtest', '-lgtest_main']
env.Program('gtest', tests+libbostek, CCFLAGS=test_cflags, LINKFLAGS=test_lflags)
