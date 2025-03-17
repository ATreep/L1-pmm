#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int print(const char *fmt)
{
  putstr(fmt);
  return 0;
}

int printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  while (*fmt)
  {
    if (*fmt == '%' && *(fmt + 1) == 'c')
    {
      // Handle character printing
      char c = va_arg(args, int); // %c expects a char, but va_arg returns int
      putch(c);
      fmt += 2;
    }
    else if (*fmt == '%' && *(fmt + 1) == 's')
    {
      // Handle string printing
      const char *str = va_arg(args, const char *);
      putstr(str);
      fmt += 2;
    }
    else if (*fmt == '%' && *(fmt + 1) == 'd')
    {
      // Handle string printing
      const int num = va_arg(args, int);
      print_int(num);
      fmt += 2;
    }
    else if (*fmt == '%' && *(fmt + 1) == 'p')
    {
      // Handle string printing
      void* ptr = va_arg(args, void *);
      print_int((intptr_t) ptr);
      fmt += 2;
    }
    else
    {
      // Handle normal character printing
      putch(*fmt);
      fmt++;
    }
  }

  va_end(args);

  return 0;
}


void print_int(int num) {
    // Buffer to store the number as a string (for negative numbers, max length of int is 11 characters)
    char buffer[12];  
    int index = 0;
    
    // Handle zero case
    if (num == 0) {
        putch('0');
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        putch('-');
        num = -num;  // Make the number positive for processing
    }

    // Convert integer to string (in reverse order)
    while (num > 0) {
        buffer[index++] = (num % 10) + '0';  // Get the last digit and convert to char
        num /= 10;  // Divide by 10 to move to the next digit
    }

    // Print the digits in reverse order
    for (int i = index - 1; i >= 0; i--) {
        putch(buffer[i]);  // Print each digit
    }
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...)
{
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

#endif
