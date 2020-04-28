/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/*
 * Copyright (C) 2020 gooroom <gooroom@gooroom.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
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

#define VERTICAL   0b0000000001
#define HORIZONTAL 0b0000000010
#define DIAGONAL   0b0000000100
#define UP         0b0000001000
#define DOWN       0b0000010000
#define RIGHT      0b0000100000
#define LEFT       0b0001000000
#define RED        0b0010000000
#define GREEN      0b0100000000
#define BLUE       0b1000000000

#define XPOS                  0
#define YPOS                  1
  
enum
{
  GHUB_CELL,
  GAUTH_CELL,
  GCONTROLLER_CELL,
  APPS_CELL
};

GtkWidget *cc_security_framework_panel_new (void);

G_END_DECLS
