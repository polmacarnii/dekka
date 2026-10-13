#ifndef ARG_H
#define ARG_H

typedef __builtin_va_list va_list;
#define va_start(ap, arg) __builtin_va_start((ap), (arg))
#define va_end(ap) __builtin_va_end((ap))
#define va_arg(ap, type) __builtin_va_arg((ap), type)
#define va_copy(d, s) __builtin_va_copy((d), (s))

#endif
