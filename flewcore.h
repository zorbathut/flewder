
#ifdef __cplusplus
extern "C" {
#endif
  enum { INC_NOTICE, INC_PUBMSG };
  
  void incoming(int type, char *username, char *userhost, char *text);
#ifdef __cplusplus
};
#endif
