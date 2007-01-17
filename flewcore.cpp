
#include <cstdio>

#include "flewcore.h"

using namespace std;

extern "C" {
void incoming(int type, char *username, char *userhost, char *text) {
  printf("%d, %s, %s, %s\n", type, username, userhost, text);
};
};
