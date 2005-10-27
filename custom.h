/* Make sure maketa doesn't get gui stuff */
#ifdef MAKING_MAKETA
  #undef TA_GUI
  #undef TA_USE_QT
  #define TA_NO_GUI
  #define NO_TA_BASE
#endif
