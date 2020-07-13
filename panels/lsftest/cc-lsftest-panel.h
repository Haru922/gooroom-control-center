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
#include <lsf/lsf-main.h>
#include <lsf/lsf-util.h>
#include <lsf/lsf-auth.h>
#include <lsf/lsf-dbus.h>

G_BEGIN_DECLS

#define CC_TYPE_LSFTEST_PANEL (cc_lsftest_panel_get_type ())
G_DECLARE_FINAL_TYPE (CcLsftestPanel, cc_lsftest_panel, CC, LSFTEST_PANEL, CcPanel)

#define LSF_PANEL_OBJECT_MAX 50
#define LSF_PANEL_APP_MAX 10
#define PASS_PHRASE      "n6x6myibEAvfN9vIDDPQi+iCoE7yTuHP//eC195+g7w="
gchar *lsf_panel_symm_key;
gchar *lsf_panel_access_token;

typedef struct _lsf_app_panel_object {
  GObject *object;
  char *object_id;
  char *class_name;
} lsf_app_panel_object;

GtkWidget *cc_lsftest_panel_new (void);

G_END_DECLS
