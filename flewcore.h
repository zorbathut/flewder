
#ifdef __cplusplus
extern "C" {
#endif
  enum { INC_NOTICE, INC_PUBMSG, INC_CTCP };
  
  void incoming(int type, char *username, char *userhost, char *text);
#ifdef __cplusplus
};
#endif
