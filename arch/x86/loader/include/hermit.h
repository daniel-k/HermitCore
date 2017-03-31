#ifndef LOADER_INCLUDE_HERMIT_H
#define LOADER_INCLUDE_HERMIT_H

#define BUILTIN_EXPECT(exp, b)  __builtin_expect((exp), (b))

#if __SIZEOF_POINTER__ == 4
	#define CONFIG_X86_32
#elif __SIZEOF_POINTER__ == 8
	#define CONFIG_X86_64
#endif


#endif // LOADER_INCLUDE_HERMIT_H
