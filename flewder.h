
#ifdef __cplusplus
extern "C" {
#endif
  void ban(const char *nick, const char *mask, const char *note, int seconds);
  void msg(const char *nick, const char *text);
  void notice(const char *nick, const char *text);
#ifdef __cplusplus
};
#endif
