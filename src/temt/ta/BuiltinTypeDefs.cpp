// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "BuiltinTypeDefs.h"
#include <Variant>

//                               name           type, global, size, c_nm)
TypeDef TA_void                 ("void",        TypeDef::VOID, 1, 0);
TypeDef TA_bool                 ("bool",        TypeDef::BOOL, 1,
                                 sizeof(bool));

TypeDef TA_int8_t               ("int8_t",      TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(int8_t));
TypeDef TA_uint8_t              ("uint8_t",     TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(uint8_t));
TypeDef TA_char                 ("char",        TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(char));
TypeDef TA_signed_char          ("signed_char", TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed char), "signed char");
TypeDef TA_unsigned_char        ("unsigned_char",TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned char), "unsigned char");

TypeDef TA_int16_t              ("int16_t",     TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(int16_t));
TypeDef TA_uint16_t             ("uint16_t",    TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(uint16_t));
TypeDef TA_short                ("short",       TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(short));
TypeDef TA_signed_short         ("signed_short",TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed short), "signed short");
TypeDef TA_short_int            ("short_int",   TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(short int), "short int");
TypeDef TA_signed_short_int     ("signed_short_int", TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed short int), "signed short int");
TypeDef TA_unsigned_short       ("unsigned_short", TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned short), "unsigned short");
TypeDef TA_unsigned_short_int   ("unsigned_short_int", TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned short int), "unsigned short int");

TypeDef TA_int32_t              ("int32_t",     TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(int32_t));
TypeDef TA_uint32_t             ("uint32_t",    TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(uint32_t));
TypeDef TA_int                  ("int",         TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(int));
TypeDef TA_signed_int           ("signed_int",  TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed int), "signed int");
TypeDef TA_signed               ("signed",      TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed));
TypeDef TA_unsigned_int         ("unsigned_int",TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned int), "unsigned int");
TypeDef TA_unsigned             ("unsigned",    TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned));
TypeDef TA_uint                 ("uint",        TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(uint));

TypeDef TA_long                 ("long",        TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(long));
TypeDef TA_signed_long          ("signed_long", TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed long), "signed long");
TypeDef TA_long_int             ("long_int",    TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(long int), "long int");
TypeDef TA_signed_long_int      ("signed_long_int", TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed long int), "signed long int");
TypeDef TA_unsigned_long        ("unsigned_long",   TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned long), "unsigned long");
TypeDef TA_unsigned_long_int    ("unsigned_long_int",TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned long int), "unsigned long int");
TypeDef TA_ulong                ("ulong",            TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(ulong));

TypeDef TA_int64_t              ("int64_t",     TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(int64_t));
TypeDef TA_uint64_t             ("uint64_t",    TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(uint64_t));
TypeDef TA_long_long            ("long_long",   TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(long long), "long long");
TypeDef TA_signed_long_long     ("signed_long_long", TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(signed long long), "signed long long");
TypeDef TA_unsigned_long_long   ("unsigned_long_long", TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(unsigned long long), "unsigned long long");

TypeDef TA_intptr_t             ("intptr_t",    TypeDef::INTEGER | TypeDef::SIGNED, 1,
                                 sizeof(intptr_t));
TypeDef TA_uintptr_t            ("uintptr_t",   TypeDef::INTEGER | TypeDef::UNSIGNED, 1,
                                 sizeof(uintptr_t));

TypeDef TA_float                ("float",       TypeDef::FLOAT, 1,
                                 sizeof(float));
TypeDef TA_double               ("double",      TypeDef::FLOAT, 1,
                                 sizeof(double));

#ifdef NO_TA_BASE
TypeDef TA_taString             ("taString",      TypeDef::STRING, 1,
                                 sizeof(taString));
TypeDef TA_Variant              ("Variant",       TypeDef::VARIANT, 1,
                                 sizeof(Variant));
#endif

// TypeDef TA_TypeItem             ("TypeItem",    1, 0, 0, 0, 1, 1);
// TypeDef TA_TypeDef              ("TypeDef",     1, 0, 0, 0, 1, 1);
// TypeDef TA_EnumDef              ("EnumDef",     1, 0, 0, 0, 1, 1);
// TypeDef TA_MemberDefBase        ("MemberDefBase",       1, 0, 0, 0, 1, 1);
// TypeDef TA_MemberDef            ("MemberDef",   1, 0, 0, 0, 1, 1);
// TypeDef TA_PropertyDef          ("PropertyDef", 1, 0, 0, 0, 1, 1);
// TypeDef TA_MethodDef            ("MethodDef",   1, 0, 0, 0, 1, 1);


void tac_AddBuiltinTypeDefs() {
  // note: AddNewGlobalType() automatically makes all common derivatives
  TA_void.AddNewGlobalType();
  TA_bool.AddNewGlobalType();
                      
  TA_int8_t.AddNewGlobalType();            
  TA_uint8_t.AddNewGlobalType();           
  TA_char.AddNewGlobalType();              
  TA_signed_char.AddNewGlobalType();       
  TA_unsigned_char.AddNewGlobalType();     
                      
  TA_int16_t.AddNewGlobalType();           
  TA_uint16_t.AddNewGlobalType();          
  TA_short.AddNewGlobalType();             
  TA_signed_short.AddNewGlobalType();      
  TA_short_int.AddNewGlobalType();         
  TA_signed_short_int.AddNewGlobalType();  
  TA_unsigned_short.AddNewGlobalType();    
  TA_unsigned_short_int.AddNewGlobalType();
                      
  TA_int32_t.AddNewGlobalType();           
  TA_uint32_t.AddNewGlobalType();          
  TA_int.AddNewGlobalType();               
  TA_signed_int.AddNewGlobalType();        
  TA_signed.AddNewGlobalType();            
  TA_unsigned_int.AddNewGlobalType();      
  TA_unsigned.AddNewGlobalType();          
  TA_uint.AddNewGlobalType();              
                      
  TA_long.AddNewGlobalType();              
  TA_signed_long.AddNewGlobalType();       
  TA_long_int.AddNewGlobalType();          
  TA_signed_long_int.AddNewGlobalType();   
  TA_unsigned_long.AddNewGlobalType();     
  TA_unsigned_long_int.AddNewGlobalType(); 
  TA_ulong.AddNewGlobalType();             
                      
  TA_int64_t.AddNewGlobalType();           
  TA_uint64_t.AddNewGlobalType();          
  TA_long_long.AddNewGlobalType();         
  TA_signed_long_long.AddNewGlobalType();  
  TA_unsigned_long_long.AddNewGlobalType();

  TA_intptr_t.AddNewGlobalType();
  TA_uintptr_t.AddNewGlobalType();

  TA_float.AddNewGlobalType();
  TA_double.AddNewGlobalType();

  TA_taString.AddNewGlobalType();
  TA_Variant.AddNewGlobalType();

#if (defined(TA_OS_WIN) && defined(_MSC_VER))
  TA_int8_t.AddParents(&TA_char);
#else
  TA_int8_t.AddParents(&TA_signed_char);
#endif
  TA_uint8_t.AddParents(&TA_unsigned_char); // note: doesn't exist per se on MSVC
  TA_signed_short.AddParents(&TA_short);
  TA_short_int.AddParents(&TA_short);
  TA_signed_short_int.AddParents(&TA_short);
  TA_int16_t.AddParents(&TA_short);
  TA_unsigned_short_int.AddParents(&TA_unsigned_short);
  TA_uint16_t.AddParents(&TA_unsigned_short);
  TA_signed_int.AddParents(&TA_int);
  TA_signed.AddParents(&TA_int);
  TA_int32_t.AddParents(&TA_int);
  TA_unsigned.AddParents(&TA_unsigned_int);
  TA_uint.AddParents(&TA_unsigned_int);
  TA_uint32_t.AddParents(&TA_unsigned_int);
  TA_signed_long.AddParents(&TA_long);
  TA_long_int.AddParents(&TA_long);
  TA_signed_long_int.AddParents(&TA_long);
  TA_unsigned_long_int.AddParents(&TA_unsigned_long);
  TA_ulong.AddParents(&TA_unsigned_long);
  TA_long_long.AddParents(&TA_int64_t);
  TA_signed_long_long.AddParents(&TA_int64_t);
  TA_unsigned_long_long.AddParents(&TA_uint64_t);
  //note: (u)intptr_t has special runtime code to test size and add to either (u)int or (u)int64_t
}
