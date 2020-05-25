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
  
#define SCENE_GHUB_BROADCAST_GHUB_BLINKING   5

#define SCENE_POLICY_RELOAD_GPMS_BLINKING    5
#define SCENE_POLICY_RELOAD_GAGENT_BLINKING 15

#define SCENE_GCTRL_KILL_CC_BLINKING         5
#define SCENE_GCTRL_KILL_GHUB_BLINKING      15

#define SCENE_APP_REQUEST_APP_BLINKING       5
#define SCENE_APP_REQUEST_GHUB_BLINKING     15

#define SCENE_GAUTH_RESPONSE_GAUTH_BLINKING  5
#define SCENE_GAUTH_RESPONSE_GHUB_BLINKING  15

#define SCENE_CNT                           27
#define SCENE_END                           -1

#define LOG_BUF                              8 

#define CC_IMG           "images/control-center-image.svg"
#define CC_IMG_SMALL     "images/control-center-small-image.png"
#define GHUB_IMG         "images/ghub-image.svg"
#define GHUB_IMG_SMALL   "images/ghub-small-image.png"
#define GAUTH_IMG        "images/gauth-image.svg"
#define GAUTH_IMG_SMALL  "images/gauth-small-image.png"
#define GCTRL_IMG        "images/gcontroller-image.svg"
#define GCTRL_IMG_SMALL  "images/gcontroller-small-image.png"
#define APPS_IMG         "images/apps-image.svg"
#define APPS_IMG_SMALL   "images/apps-small-image.png"
#define GAGENT_IMG       "images/gagent-image.svg"
#define GAGENT_IMG_SMALL "images/gagent-small-image.png"
#define GPMS_IMG         "images/gpms-image.svg"
#define GPMS_IMG_SMALL   "images/gpms-small-image.png"
#define LSF_IMG          "images/lsf.png"
#define SOUND_WAVE_GIF   "images/sound-wave.gif"

enum
{
  SCENE_IDLE,
  SCENE_PRESENTING,
  SCENE_POLICY_RELOAD,
  SCENE_GHUB_BROADCAST,
  SCENE_GCTRL_KILL,
  SCENE_GCTRL_LAUNCH,
  SCENE_APP_REQUEST,
  SCENE_GAUTH_RESPONSE,
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

enum
{
  DIRECTION_NONE,
  DIRECTION_DEFAULT_HORIZONTAL,
  DIRECTION_DEFAULT_VERTICAL,
  DIRECTION_DEFAULT_DIAGONAL_DOWN_RIGHT,
  DIRECTION_UP,
  DIRECTION_DOWN,
  DIRECTION_RIGHT,
  DIRECTION_LEFT,
  DIRECTION_UP_RIGHT,
  DIRECTION_UP_LEFT,
  DIRECTION_DOWN_RIGHT,
  DIRECTION_DOWN_LEFT,
  DIRECTION_CUSTOM,
  DIRECTION_NUM
};

enum
{
  LINE_CC_GHUB,
  LINE_GHUB_GAUTH,
  LINE_GHUB_GCTRL,
  LINE_GHUB_APPS,
  LINE_GHUB_GAGENT,
  LINE_GAGENT_GPMS,
  LINE_NUM
};

enum
{
  COLOR_NONE,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_YELLOW,
  COLOR_NUM
};

GtkWidget *cc_security_framework_panel_new (void);

G_END_DECLS
