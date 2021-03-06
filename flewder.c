/*
 * flewder.c -- part of flewder.mod
 *   nonsensical command to exemplify module programming
 *
 * Originally written by ButchBub         15 July     1997
 * Comments by Fabian Knittel             29 December 1999
 *
 * $Id: flewder.c,v 1.25 2006-03-28 02:35:52 wcc Exp $
 */
/*
 * Copyright (C) 1999 - 2006 Eggheads Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define MODULE_NAME "flewder"
#define MAKING_flewder

#include "flewcore.h"

#include "../module.h"
#include "../channels.mod/channels.h"
#include "../irc.mod/irc.h"
#include "../server.mod/server.h"

#include <stdlib.h>

#undef global
/* Pointer to the eggdrop core function table. Gets initialized in
 * flewder_start().
 */
static Function *global = NULL;
static Function *server_funcs = NULL;
static Function *irc_funcs = NULL;
static Function *channels_funcs = NULL;

char channel[] = "#c++";

/* Calculate the memory we keep allocated.
 */
static int flewder_expmem()
{
  int size = 0;

  Context;
  return size;
}

void opcheck(int type, char *nick, char *host, char *text) {
  struct chanset_t *chan = findchan_by_dname(channel);
  if (chan) {
    memberlist *m = ismember(chan, nick);
    if (m && !chan_hasop(m) && !chan_hasvoice(m)) {
      incoming(type, nick, host, text);
    }
  }
}

static int notc_flewder(char *nick, char *host, char *hand, char *text, char *in_channel) {
  Context;
  
  if(in_channel && in_channel[0] == '#' && !egg_strcasecmp(in_channel, channel))
    opcheck(INC_NOTICE, nick, host, text);
  
  return 0;
}

static int ctcp_flewder(char *nick, char *from, char *handle,
                         char *object, char *keyword, char *text) {
  Context;
  
  if(!strcmp(keyword, "ACTION"))
    return 0; // argh
  
  if(object && object[0] == '#' && !egg_strcasecmp(object, channel))
    opcheck(INC_CTCP, nick, from, text);
  
  return 0;
}

static int pub_flewder(char *nick, char *host, char *hand, char *in_channel, char *text) {
  Context;
  
  if(in_channel[0] == '#' && !egg_strcasecmp(in_channel, channel))
    opcheck(INC_PUBMSG, nick, host, text);
  
  return 0;
}

void ban(const char *nick, const char *mask, const char *note, int seconds) {
  Context;
  
  struct chanset_t *chan = findchan_by_dname(channel);
  printf("%p\n", chan);
  
  u_addban(chan, (char*)mask, "floodscript", (char*)note, time(NULL) + seconds, 0);
  dprintf(DP_MODE, "MODE %s +b %s", channel, mask);
  dprintf(DP_MODE, "KICK %s %s :%s", channel, nick, note);
}

void msg(const char *nick, const char *text) {
  dprintf(DP_SERVER, "PRIVMSG %s :%s", nick, text);
}

void notice(const char *nick, const char *text) {
  dprintf(DP_SERVER, "NOTICE %s :%s", nick, text);
}

/* A report on the module status.
 *
 * details is either 0 or 1:
 *    0 - `.status'
 *    1 - `.status all'  or  `.module flewder'
 */
static void flewder_report(int idx, int details)
{
  if (details) {
    int size = flewder_expmem();

    dprintf(idx, "    Using %d byte%s of memory\n", size,
            (size != 1) ? "s" : "");
  }
}

/* Note: The tcl-name is automatically created if you set it to NULL. In
 *       the example below it would be just "*dcc:flewder". If you specify
 *       "flewder:flewder" it would be "*dcc:flewder:flewder" instead.
 *               ^----- command name   ^--- table name
 *        ^------------ module name
 *
 *       This is only useful for stackable binding tables (and H_dcc isn't
 *       stackable).
 */
static cmd_t mypub[] = {
  /* command  flags  function     tcl-name */
  {"*",  "",    pub_flewder,  NULL},
  {NULL,      NULL,  NULL,        NULL}  /* Mark end. */
};
static cmd_t mynotc[] = {
  /* command  flags  function     tcl-name */
  {"*",  "",    notc_flewder,  NULL},
  {NULL,      NULL,  NULL,        NULL}  /* Mark end. */
};
static cmd_t myctcp[] = {
  /* command  flags  function     tcl-name */
  {"*",  "",    ctcp_flewder,  NULL},
  {NULL,      NULL,  NULL,        NULL}  /* Mark end. */
};

static char *flewder_close()
{
  Context;

  rem_builtins(H_pubm, mypub);
  rem_builtins(H_notc, mynotc);
  rem_builtins(H_ctcp, myctcp);
  
  module_undepend(MODULE_NAME);
  return NULL;
}

/* Define the prototype here, to avoid warning messages in the
 * flewder_table.
 */
EXPORT_SCOPE char *flewder_start();

/* This function table is exported and may be used by other modules and
 * the core.
 *
 * The first four have to be defined (you may define them as NULL), as
 * they are checked by eggdrop core.
 */
static Function flewder_table[] = {
  (Function) flewder_start,
  (Function) flewder_close,
  (Function) flewder_expmem,
  (Function) flewder_report,
};

char *flewder_start(Function *global_funcs)
{
  /* Assign the core function table. After this point you use all normal
   * functions defined in src/mod/modules.h
   */
  global = global_funcs;

  Context;
  /* Register the module. */
  module_register(MODULE_NAME, flewder_table, 2, 0);
  /*                                            ^--- minor module version
   *                                         ^------ major module version
   *                           ^-------------------- module function table
   *              ^--------------------------------- module name
   */

  if (!module_depend(MODULE_NAME, "eggdrop", 106, 0)) {
    module_undepend(MODULE_NAME);
    return "This module requires Eggdrop 1.6.0 or later.";
  }
  
  module_entry *me;

	me = module_find("irc", 1, 0);
  module_depend(MODULE_NAME, "irc", 1, 0);
  irc_funcs = me->funcs;
  add_builtins(H_pubm, mypub);

	me = module_find("server", 1, 0);
  module_depend(MODULE_NAME, "server", 1, 0);
  server_funcs = me->funcs;
  add_builtins(H_notc, mynotc);
  add_builtins(H_ctcp, myctcp);
  
  me = module_find("channels", 1, 0);
  module_depend(MODULE_NAME, "channels", 1, 0);
  channels_funcs = me->funcs;
  
  return NULL;
}
