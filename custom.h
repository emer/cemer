/* Make sure maketa doesn't get gui stuff */
#ifdef MAKING_MAKETA
  #ifdef TA_GUI
   #undef TA_GUI
  #endif  
  #ifdef TA_USE_QT
   #undef TA_USE_QT
  #endif
  #ifndef TA_NO_GUI
   #define TA_NO_GUI
  #endif
  #ifndef NO_TA_BASE
   #define NO_TA_BASE
  #endif
#endif
