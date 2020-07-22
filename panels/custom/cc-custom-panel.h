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
#include <webkit2/webkit2.h>
#include <lsf/lsf-main.h>
#include <lsf/lsf-util.h>
#include <lsf/lsf-auth.h>
#include <lsf/lsf-dbus.h>

G_BEGIN_DECLS

#define CC_TYPE_CUSTOM_PANEL (cc_custom_panel_get_type ())
G_DECLARE_FINAL_TYPE (CcCustomPanel, cc_custom_panel, CC, CUSTOM_PANEL, CcPanel)

#define CUSTOM_SCHEMA    "org.gnome.desktop.custom"
#define CUSTOM_UI        "/org/gnome/control-center/custom/custom.ui"

#define CC_DBUS_NAME     "kr.gooroom.controlcenter"
#define APP_DBUS_NAME    "kr.gooroom.testapp"

#define PASS_PHRASE      "n6x6myibEAvfN9vIDDPQi+iCoE7yTuHP//eC195+g7w="
#define SET_SETTINGS_FMT "{ \"to\": \"%s\", \"from\": \"%s\", \"access_token\": \"%s\", \"function\": \"%s\", \"app_conf\": %s }"
#define GET_SETTINGS_FMT "{ \"to\": \"%s\", \"from\": \"%s\", \"access_token\": \"%s\", \"function\": \"%s\" }"

#define FREE(v) \
    if (v) { \
      free(v); \
      v=NULL; \
    }

gchar *lsf_panel_symm_key;
gchar *lsf_panel_access_token;

GtkWidget *cc_custom_panel_new (void);

G_END_DECLS
