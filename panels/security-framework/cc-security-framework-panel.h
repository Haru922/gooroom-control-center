/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2020 gooroom <gooroom@gooroom.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * * This program is distributed in the hope that it will be useful, * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <shell/cc-panel.h> 
#include <lsf/lsf-main.h>
#include <lsf/lsf-util.h>
#include <lsf/lsf-auth.h>
#include <lsf/lsf-dbus.h>

G_BEGIN_DECLS

#define CC_TYPE_SECURITY_FRAMEWORK_PANEL (cc_security_framework_panel_get_type ())
G_DECLARE_FINAL_TYPE (CcSecurityFrameworkPanel, cc_security_framework_panel, CC, SECURITY_FRAMEWORK_PANEL, CcPanel)

#define RADIUS_SMALL                         3
#define RADIUS_MEDIUM                        4
#define RADIUS_LARGE                         6

#define XPOS                                 0
#define YPOS                                 1

#define VT                                  15
#define HT                                  20 
  
#define SCENE_POLICY_RELOAD_GPMS_BLINKING    5
#define SCENE_POLICY_RELOAD_GAGENT_BLINKING 15

#define STARTING_BLINK_CNT                   5
#define MOVING_CNT                           6
#define KEY_EXCHANGE_FIN                     3

#define SCENE_CNT                           17
#define SCENE_END                           -1

#define DEFAULT_BUF_SIZE                  4096
#define JSON_VALUE_BUF                      30

#define NORM                                 0
#define REV                                  1
#define LOG_BUF                              8
#define EVENTS_NUM                          10
#define PRESENTER_TIMEOUT                   50 
#define PRESENTER_TIMEOUT_SLOW             200
#define MINUTE                           60000
#define UPDATER_TIMEOUT               1*MINUTE

#define RESOURCE_DIR     "/org/gnome/control-center/security-framework/resources"
#define CC_IMG           RESOURCE_DIR"/control-center-image.svg"
#define CC_IMG_SMALL     RESOURCE_DIR"/control-center-small-image.png"
#define GHUB_IMG         RESOURCE_DIR"/ghub-image.svg"
#define GHUB_IMG_SMALL   RESOURCE_DIR"/ghub-small-image.png"
#define GAUTH_IMG        RESOURCE_DIR"/gauth-image.svg"
#define GAUTH_IMG_SMALL  RESOURCE_DIR"/gauth-small-image.png"
#define GCTRL_IMG        RESOURCE_DIR"/gcontroller-image.svg"
#define GCTRL_IMG_SMALL  RESOURCE_DIR"/gcontroller-small-image.png"
#define APPS_IMG         RESOURCE_DIR"/apps-image.png"
#define APPS_IMG_SMALL   RESOURCE_DIR"/apps-small-image.png"
#define GAGENT_IMG       RESOURCE_DIR"/gagent-image.svg"
#define GAGENT_IMG_SMALL RESOURCE_DIR"/gagent-small-image.png"
#define GPMS_IMG         RESOURCE_DIR"/gpms-image.svg"
#define GPMS_IMG_SMALL   RESOURCE_DIR"/gpms-small-image.png"
#define LSF_IMG          RESOURCE_DIR"/lsf.png"
#define SOUND_WAVE_GIF   RESOURCE_DIR"/sound-wave.gif"

#define CC_DBUS_NAME     "kr.gooroom.controlcenter"
#define GHUB_DBUS_NAME   "kr.gooroom.ghub"
#define GAUTH_DBUS_NAME  "kr.gooroom.gauth"
#define GCTRL_DBUS_NAME  "kr.gooroom.gcontroller"
#define GAGENT_DBUS_NAME "kr.gooroom.agent"
#define APPS_DBUS_NAME   "kr.gooroom.testapp"
#define GPMS_NAME        "gpms"
#define LOG_DIRECTORY    "/var/log/lsf/"
#define LOG_FILE_PREFIX  "message"

#define GPMS_DOMAIN      "https://dev-c4i-gpms.gooroom.kr"
#define V3_DOMAIN        "http://localhost:88"

#define PASS_PHRASE      "n6x6myibEAvfN9vIDDPQi+iCoE7yTuHP//eC195+g7w="
gchar *lsf_panel_symm_key;
gchar *lsf_panel_access_token;

enum
{
  SCENE_IDLE,
  SCENE_METHOD_CALL,
  SCENE_METHOD_CALL_REV,
  SCENE_PRESENTING,
  SCENE_POLICY_RELOAD,
  SCENE_NUM
};

enum
{
  CC_CELL,
  GHUB_CELL,
  GAUTH_CELL,
  GCTRL_CELL,
  GAGENT_CELL,
  APPS_CELL,
  GPMS_CELL,
  CELL_NUM
};

char *module_name[CELL_NUM] = { "CC", "GHUB", "GAUTH",
                                "GCTRL", "GAGENT", "V3",
                                "GPMS" };

enum
{
  DIRECTION_CC_GHUB,
  DIRECTION_GHUB_GAUTH,
  DIRECTION_GHUB_GAGENT,
  DIRECTION_GHUB_GCTRL,
  DIRECTION_GHUB_APPS,
  DIRECTION_GAGENT_GPMS,
  DIRECTION_NUM
};

enum
{
  COLOR_NONE,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_YELLOW,
  COLOR_BLACK,
  COLOR_NUM
};

enum
{
  GET_CONFIG,
  SET_CONFIG,
  UNSET_CONFIG,
  LAUNCH_GAGENT,
  KILL_GAGENT,
  LAUNCH_APPS,
  KILL_APPS,
  GET_STATUS,
  NUM_DBUS_ARGS
};

enum
{
  DMSG_SEQ,
  DMSG_DIRECTION,
  DMSG_METHOD,
  DMSG_ABS,
  DMSG_GLYPH,
  DMSG_FROM,
  DMSG_TO,
  DMSG_FUNC,
  DMSG_ERR,
  DMSG_PAYLOAD,
  DMSG_NUM
};

enum
{
  GCTRL_STATUS_RET_GHUB,
  GCTRL_STATUS_RET_GAUTH,
  GCTRL_STATUS_RET_GCTRL,
  GCTRL_STATUS_RET_TESTAPP,
  GCTRL_STATUS_RET_AGENT,
  GCTRL_STATUS_RET_AHNLAB,
  GCTRL_STATUS_RET_CC,
  GCTRL_STATUS_RET_NUM
};

enum
{
  RET_ARG_DBUS_NAME,
  RET_ARG_EXE_STAT,
  RET_ARG_AUTH_STAT,
  RET_ARG_NUM
};

enum
{
  SOURCE_FUNC_PRESENTER,
  SOURCE_FUNC_UPDATER,
  SOURCE_FUNC_NUM
};
  
GtkWidget *cc_security_framework_panel_new (void);

G_END_DECLS
