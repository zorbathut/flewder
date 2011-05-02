
#include <cstdio>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>

#include "flewcore.h"
#include "flewder.h"

using namespace std;

map<string, int> invincibility;
map<string, pair<int, int> > userdata;

class Load {
public:
  Load() {
    ifstream ifs("/home/zorba/flewdata");
    string steer;
    while(getline(ifs, steer)) {
      pair<int, int> dat;
      char host[1024]; // this is long enough because irc messages can't exceed 512 anyway
      if(sscanf(steer.c_str(), "%d %d %[^\n]", &dat.first, &dat.second, host) == 3) {
        printf("Parsed %s with level %d and timeout %d\n", host, dat.first, dat.second);
        userdata[host] = dat;
      }
    }
  }
} Loader;

const int seconds = 1;
const int minutes = seconds * 60;
const int hours = minutes * 60;
const int days = hours * 24;
const int months = days * 30;

int cooldown[] = {0, 60 * minutes, 10 * minutes, 60 * minutes, 24 * hours, 7 * days, 37 * days };
int bantime[] =   {0, 0,                 0,                15 * seconds, 1 * hours, 1 * days, 1 * months };

char chatterchannel[] = "#c++::ops::ticker";
bool active = true;

int generateCooldownTime(int level) {
  level = min<int>(level, sizeof(cooldown) / sizeof(*cooldown) - 1);
  return cooldown[level];
}

int generateBanTime(int level) {
  level = min<int>(level, sizeof(cooldown) / sizeof(*cooldown) - 1);
  return bantime[level];
}

string generateBanMessage(string startstring, int time, string username) {
  startstring += " [";
  startstring += username;
  startstring += ", ";
  char tbf[50];
  if(time % minutes) {
    sprintf(tbf, "%ds", time);
  } else if(time % hours) {
    sprintf(tbf, "%dm", time / minutes);
  } else if(time % days) {
    sprintf(tbf, "%dh", time / hours);
  } else if(time % months) {
    sprintf(tbf, "%dd", time / days);
  } else {
    sprintf(tbf, "%dmo", time / months);
  }
  startstring += tbf;
  startstring += "]";
  return startstring;
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
  FILE *writ = fopen("/home/zorba/flewdata", "w");
  for(map<string, pair<int, int> >::iterator itr = userdata.begin(); itr != userdata.end(); itr++)
    fprintf(writ, "%d %d %s\n", itr->second.first, itr->second.second, itr->first.c_str());
  fclose(writ);
}

void infract(char *usernick, char *userhost, char *reason, int startlevel) {
  printf("Infraction - %s did %s level %d\n", userhost, reason, startlevel);
  testForDefract();
  if(invincibility[userhost] > time(NULL))
    return;
  
  userdata[userhost].first++;
  userdata[userhost].first = max(userdata[userhost].first, startlevel);
  userdata[userhost].second = time(NULL) + generateCooldownTime(userdata[userhost].first);
  
  if(userdata[userhost].first >= 2)
    invincibility[userhost] = time(NULL) + 15;  // 15-sec invincibility after an infraction, just so they don't stack up more thanks to our own lag
  
  saveUserdata();
  
  if(userdata[userhost].first == 1) {
    //    msg(chatterchannel, (string(usernick) + " has been mildly evil (\"" + string(reason) + "\"), letting it slide for now").c_str());
  } else if(userdata[userhost].first == 2) {
    string banmessage = string(reason) + " Doing that again will result in increasingly longer bans.";
    if(active) {
      notice(usernick, banmessage.c_str());
      //msg(chatterchannel, ("Msging " + string(usernick) + " with error \"" + banmessage + "\"").c_str());
    } else {
      //msg(chatterchannel, ("Would have msged " + string(usernick) + " with error \"" + banmessage + "\"").c_str());
    }
  } else {
    string targetmask = string("*!*@") + userhost;
    string banmessage = generateBanMessage(reason, generateBanTime(userdata[userhost].first), usernick);
    if(active) {
      printf("Banning %s for %d\n", targetmask.c_str(), generateBanTime(userdata[userhost].first));
      ban(usernick, targetmask.c_str(), banmessage.c_str(), generateBanTime(userdata[userhost].first));
      //msg(chatterchannel, ("Banning " + targetmask + " with message \"" + banmessage + "\"").c_str());
    } else {
      //msg(chatterchannel, ("Would have banned " + targetmask + " with message \"" + banmessage + "\"").c_str());
    }
  }
}

extern "C" {
void incoming(int type, char *username, char *userhost, char *text) {
  printf("inc: %d %s %s %s\n", type, username, userhost, text);
  
  if(!strchr(userhost, '@'))
    return;
  
  userhost = strchr(userhost, '@') + 1;
  
  if(type == INC_NOTICE) {
    infract(username, userhost, "No channel-wide notices allowed.", 3);
    return;
  }
  
  if(type == INC_CTCP) {
    infract(username, userhost, "No channel-wide CTCPs allowed.", 1);
    return;
  }
  
  if(strstr(text, "\002:")) { // I'm not sure if it's \002:\017 or \002:\002
    infract(username, userhost, "Disable your bold colon immediately - \"/msg notabot calc bold colon\" for instructions.", 2);
    return;
  }
  
  if(strchr(text, '\002') || strchr(text, '\037') || strchr(text, '\026') || strchr(text, '\017') || strchr(text, '\001') || strchr(text, '\003')) {
    infract(username, userhost, "No color allowed.", 2);
    return;
  }

  if(strstr(text, "12 year old TALIBAN BOY BEHEADS")) {
    infract(username, userhost, "stop spamming your sex fantasies in here", 6);
    return;
  }
};
};
