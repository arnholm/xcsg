#ifndef CSPLINES_CONFIG_H_INCLUDED
#define CSPLINES_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler

   #ifdef CSPLINES_IMPLEMENTATION
      #define CSPLINES_PUBLIC  
      #define CSPLINES_EXTERN
   #else
      #define CSPLINES_PUBLIC 
      #define CSPLINES_EXTERN 
   #endif
   #define CSPLINES_PRIVATE

#elif __GNUC__

   /// gnu gcc
   
   #define CSPLINES_PUBLIC   
   #define CSPLINES_PRIVATE 
   #define CSPLINES_EXTERN
	  
#else

  #error "Unknown compiler"

#endif


#endif // CSPLINES_CONFIG_H_INCLUDED

