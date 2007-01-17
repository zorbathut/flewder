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
//static Function *server_funcs = NULL;
static Function *irc_funcs = NULL;
//static Function *channels_funcs = NULL;

/* Calculate the memory we keep allocated.
 */
static int flewder_expmem()
{
  int size = 0;

  Context;
  return size;
}

static int pub_flewder(char *nick, char *host, char *hand, char *channel, char *text)
{
  /* Define a context.
   *
   * If the bot crashes after the context, it will be  the last mentioned
   * in the resulting DEBUG file. This helps you debugging.
   */
  Context;
  
  printf("in pub\n");
  
  printf("%s, %s, %s, %s, %s\n", nick, host, hand, channel, text);
  
  struct chanset_t *chan = findchan_by_dname(channel);
    
  printf("%p\n", chan);

  if ((chan != NULL) && channel_seen(chan)) {
    char prefix[1024];
    egg_snprintf(prefix, sizeof prefix, "PRIVMSG %s :", chan->name);
    dprintf(DP_HELP, "%sthis is a test", prefix);
  }
  return 0;
/*
  putlog(LOG_CMDS, "*", "#%s# flewder", dcc[idx].nick);

  dprintf(idx, "flewder!\n");
  return 0;*/
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

static char *flewder_close()
{
  Context;
  
  p_tcl_bind_list H_temp;

  rem_builtins(H_pubm, mypub);
  
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
  
  return NULL;
}
