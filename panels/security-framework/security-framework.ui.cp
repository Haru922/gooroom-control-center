<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <!-- interface-requires gtk+ 3.0 -->
  <template class="CcSecurityFrameworkPanel" parent="CcPanel">
    <property name="visible">True</property>
    <property name="can-focus">False</property>
    <property name="expand">True</property>
    <child>
      <object class="GtkImage" id="gauth_image">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
        <property name="pixbuf">images/safe-fingerprint.svg</property>
      </object>
      <object class="GtkImage" id="gcontroller_image">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
        <property name="pixbuf">images/control-eidt.svg</property>
      </object>
      <object class="GtkImage" id="ghub_image">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
      <property name="pixbuf">images/network-management.svg</property>
      </object>
      <object class="GtkImage" id="testapp_image">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
        <property name="pixbuf">images/etc-application.svg</property>
      </object>
      <object class="GtkGrid" id="main_grid">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
        <property name="row_homogeneous">True</property>
        <property name="column_homogeneous">True</property>
        <property name="halign">center</property>
        <property name="valign">center</property>
        <child>
          <object class="GtkButton" id="ghub_button">
            <property name="label" translatable="yes">GHub</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">True</property>
            <property name="image">ghub_image</property>
            <property name="relief">none</property>
            <property name="image_position">top</property>
            <property name="always_show_image">True</property>
          </object>
          <packing>
            <property name="left_attach">2</property>
            <property name="top_attach">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkButton" id="gauth_button">
            <property name="label" translatable="yes">GAuth</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">True</property>
            <property name="image">gauth_image</property>
            <property name="relief">none</property>
            <property name="image_position">top</property>
            <property name="always_show_image">True</property>
          </object>
          <packing>
            <property name="left_attach">2</property>
            <property name="top_attach">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkButton" id="gcontroller_button">
            <property name="label" translatable="yes">GController</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">True</property>
            <property name="image">gcontroller_image</property>
            <property name="relief">none</property>
            <property name="image_position">top</property>
            <property name="always_show_image">True</property>
          </object>
          <packing>
            <property name="left_attach">4</property>
            <property name="top_attach">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkButton" id="testapp_button">
            <property name="label" translatable="yes">TestApp</property>
            <property name="visible">True</property>
            <property name="can_focus">True</property>
            <property name="receives_default">True</property>
            <property name="image">testapp_image</property>
            <property name="relief">none</property>
            <property name="image_position">top</property>
            <property name="always_show_image">True</property>
          </object>
          <packing>
            <property name="left_attach">4</property>
            <property name="top_attach">4</property>
          </packing>
        </child>
        <child>
          <object class="GtkImage" id="control_center_ghub">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="icon_name">go-next</property>
          </object>
          <packing>
            <property name="left_attach">1</property>
            <property name="top_attach">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkImage" id="ghub_gauth">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="icon_name">mail-send-receive</property>
          </object>
          <packing>
            <property name="left_attach">2</property>
            <property name="top_attach">1</property>
          </packing>
        </child>
        <child>
          <object class="GtkBox" id="control_center_section">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="orientation">vertical</property>
            <child>
              <object class="GtkImage" id="control_center_image">
                <property name="visible">True</property>
                <property name="can_focus">False</property>
                <property name="pixbuf">images/control-pannel.svg</property>
              </object>
              <packing>
                <property name="expand">True</property>
                <property name="fill">False</property>
                <property name="position">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkLabel" id="control_center_label">
                <property name="visible">True</property>
                <property name="can_focus">False</property>
                <property name="label" translatable="yes">Control Center</property>
              </object>
              <packing>
                <property name="expand">False</property>
                <property name="fill">False</property>
                <property name="position">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="left_attach">0</property>
            <property name="top_attach">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkImage" id="ghub_gcontroller">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="icon_name">go-next</property>
          </object>
          <packing>
            <property name="left_attach">3</property>
            <property name="top_attach">2</property>
          </packing>
        </child>
        <child>
          <object class="GtkImage" id="gcontroller_testapp">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="icon_name">go-down</property>
          </object>
          <packing>
            <property name="left_attach">4</property>
            <property name="top_attach">3</property>
          </packing>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
        <child>
          <placeholder/>
        </child>
      </object>
    </child>
  </template>
</interface>
