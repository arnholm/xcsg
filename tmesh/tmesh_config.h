#ifndef TMESH_CONFIG_H_INCLUDED
#define TMESH_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler

   #ifdef TMESH_IMPLEMENTATION
      #define TMESH_PUBLIC  
      #define TMESH_EXTERN
   #else
      #define TMESH_PUBLIC 
      #define TMESH_EXTERN 
   #endif
   #define TMESH_PRIVATE

#elif __GNUC__

   /// gnu gcc
   
   #define TMESH_PUBLIC   
   #define TMESH_PRIVATE 
   #define TMESH_EXTERN
	  
#else

  #error "Unknown compiler"

#endif


#endif // TMESH_CONFIG_H_INCLUDED

