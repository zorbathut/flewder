
#include <cstdio>
#include <map>
#include <string>
#include <algorithm>

#include "flewcore.h"
#include "flewder.h"

using namespace std;

map<string, pair<int, int> > userdata;

const int seconds = 1;
const int minutes = seconds * 60;
const int hours = minutes * 60;
const int days = hours * 24;
const int months = days * 30;

int cooldown[] = {0, 10 * minutes, 60 * minutes, 24 * hours, 7 * days, 37 * days };
int bantime[] =   {0, 0,                15 * seconds, 1 * hours, 1 * days, 1 * months };

int generateCooldownTime(int level) {
  level = min<int>(level, sizeof(cooldown) / sizeof(*cooldown) - 1);
  return cooldown[level];
}

int generateBanTime(int level) {
  level = min<int>(level, sizeof(cooldown) / sizeof(*cooldown) - 1);
  return bantime[level];
}

void testForDefract() {
  for(map<string, pair<int, int> >::iterator itr = userdata.begin(); itr != userdata.end(); ) {
    bool advance = true;
    while(itr->second.second < time(NULL)) {
      itr->second.first--;
      itr->second.second = itr->second.second + generateCooldownTime(itr->second.first);
      printf("Defracting %s to %d\n", itr->first.c_str(), itr->second.first);
      if(itr->second.first == 0) {
        map<string, pair<int, int> >::iterator titr = itr;
        itr++;
        userdata.erase(titr);
        advance = false;
        break;
      }
    }
    if(advance)
      itr++;
  }
}

void saveUserdata() {
  FILE *writ = fopen("flewdata", "w");
  for(map<string, pair<int, int> >::iterator itr = userdata.begin(); itr != userdata.end(); itr++)
    fprintf(writ, "%d %d %s\n", itr->second.first, itr->second.second, itr->first.c_str());
  fclose(writ);
}

void infract(char *userhost, char *reason, int startlevel) {
  printf("Infraction - %s did %s level %d\n", userhost, reason, startlevel);
  testForDefract();
  
  userdata[userhost].first++;
  userdata[userhost].first = max(userdata[userhost].first, startlevel);
  userdata[userhost].second = time(NULL) + generateCooldownTime(userdata[userhost].first);
  
  saveUserdata();
  
  if(userdata[userhost].first == 1) {
    // TODO: send /notice
  } else {
    printf("Banning %s for %d\n", (string("*!*@") + userhost).c_str(), generateBanTime(userdata[userhost].first));
    ban((string("*!*@") + userhost).c_str(), reason, generateBanTime(userdata[userhost].first));
  }
}

extern "C" {
void incoming(int type, char *username, char *userhost, char *text) {
  printf("inc: %d %s %s %s\n", type, username, userhost, text);
  
  if(!strchr(userhost, '@'))
    return;
  
  userhost = strchr(userhost, '@') + 1;
  
  if(type == INC_NOTICE) {
    infract(userhost, "No channel-wide notices allowed.", 2);
    return;
  }
  
  if(strstr(text, "\002:")) { // I'm not sure if it's \002:\017 or \002:\002
    infract(userhost, "Disable your bold colon immediately - \"/msg notabot bold colon\" for instructions.", 1);
    return;
  }
  
  if(strchr(text, '\002') || strchr(text, '\037') || strchr(text, '\026') || strchr(text, '\017') || strchr(text, '\001')) {
    infract(userhost, "No color allowed.", 1);
    return;
  }
};
};
