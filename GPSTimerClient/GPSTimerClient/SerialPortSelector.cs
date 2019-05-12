using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Windows.Forms;
using System.Management;

namespace GPSTimerClient
{
    public partial class SerialPortSelector : Form
    {
        private SerialPortSelector()
        {
            InitializeComponent();
        }

        private string[] GetPorts()
        {
            return SerialPort.GetPortNames();
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            UpdateItems();
            tmrUpdatePorts.Enabled = true;
        }

        protected override void OnClosed(EventArgs e)
        {
            tmrUpdatePorts.Enabled = false;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            UpdateItems();
        }

        private void UpdateItems()
        {
            var item = lbPorts.SelectedItem;
            lbPorts.Items.Clear();
            var ports = GetPorts().ToArray();
            lbPorts.Items.AddRange(ports);
            if (item == null && lbPorts.Items.Count > 0)
            {
                lbPorts.SelectedIndex = 0;
            }
            if (item != null && lbPorts.Items.Count > 0)
            {
                lbPorts.SelectedItem =
                    lbPorts.Items.OfType<string>().FirstOrDefault(x => x.Equals(item))
                    ?? lbPorts.Items.OfType<string>().FirstOrDefault();
            }
            UpdateButtons();

            if (cbAuto.Checked && availablePorts != null)
            {
                foreach (var p in ports)
                {
                    if (!availablePorts.Contains(p))
                    {
                        lbPorts.SelectedItem = p;
                        DialogResult = DialogResult.OK;
                        break;
                    }
                }
            }
        }

        public static SerialPort Query(IWin32Window owner)
        {
            using (var dlg = new SerialPortSelector())
            {
                if (dlg.ShowDialog(owner) == DialogResult.OK)
                {
                    var name = (string)dlg.lbPorts.SelectedItem;
                    if (name == null)
                    {
                        return null;
                    }
                    return new SerialPort(name, 115200);
                }
                return null;
            }
        }

        private void lbPorts_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateButtons();
        }

        private void UpdateButtons()
        {
            btnOK.Enabled = (lbPorts.SelectedIndex != -1);
            if (btnOK.Enabled)
            {
                Control ctrl = null;
                for (Control c = this; c as ContainerControl != null; c = (c as ContainerControl)?.ActiveControl)
                {
                    ctrl = c;
                }
                btnOK.Focus();
                if (ctrl != null)
                {
                    ctrl.Focus();
                }
            }
        }

        private string[] availablePorts;

        private void cbAuto_CheckedChanged(object sender, EventArgs e)
        {
            if (cbAuto.Checked)
            {
                availablePorts = lbPorts.Items.OfType<string>().ToArray();
            }
        }
    }
}
