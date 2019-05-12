using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;

namespace GPSTimerClient
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            dgTable
                .GetType()
                .GetProperty("DoubleBuffered", BindingFlags.Instance | BindingFlags.NonPublic)
                .SetValue(dgTable, true, null);
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);

            ReadPortAsync(SerialPortSelector.Query(this));
        }

        private Thread portReader;

        private void ReadPortAsync(SerialPort port)
        {
            if (port == null)
            {
                return;
            }

            if (portReader != null && portReader.IsAlive)
            {
                portReader.Abort();
                portReader.Join();
                portReader = null;
            }

            portReader = new Thread(() =>
            {
                Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
                var data = ReadFromPort(port).ToArray();
                this.Invoke(new MethodInvoker(() => AddDataItems(data)));
            });
            portReader.IsBackground = true;
            portReader.Start();
        }

        private void mnuShowMap_Click(object sender, EventArgs e)
        {
            ShowMap();
        }

        private void mnuReadAgain_Click(object sender, EventArgs e)
        {
            ReadPortAsync(SerialPortSelector.Query(this));
        }

        private IEnumerable<DataItem> ReadFromPort(SerialPort port)
        {
            try
            {
                port.Open();
                Queue<string> lines = new Queue<string>();
                tsDebugOut.Text = "[" + port.PortName + "] Waiting for data...";
                for (string line = port.ReadLine().Trim(); line != "###"; line = port.ReadLine().Trim())
                {
                    tsDebugOut.Text = line.Replace('\t', ' ');
                    if (line.StartsWith("#"))
                    {
                        lines.Enqueue(line);
                    }
                }
                tsDebugOut.Text = "(End of data)";
                if (lines.Count > 0)
                {
                    string[] keys = lines.Dequeue().Split('\t');
                    DateTime? prev = null;
                    while (lines.Count > 0)
                    {
                        var newItem = new DataItem(keys, lines.Dequeue());
                        if (prev.HasValue)
                        {
                            newItem.Delta = newItem.DateTimeTotal.Subtract(prev.Value);
                        }
                        prev = newItem.DateTimeTotal;
                        yield return newItem;
                    }
                }
            }
            finally
            {
                port.Close();
            }
        }

        private DataItem[] data;

        private void AddDataItems(DataItem[] data)
        {
            this.data = data;
            dgTable.DataSource = data;
            dgTable.AutoResizeColumns(DataGridViewAutoSizeColumnsMode.AllCells);
        }

        private void ShowMap()
        {
            if (data == null)
            {
                return;
            }

            JObject root = new JObject();
            switch (data.Length)
            {
                case 0:
                    break;
                case 1:
                    {
                        var center = new JArray(data[0].Latitude, data[0].Longitude);
                        root.Add("center", center);
                    }
                    break;
                default:
                    {
                        var bounds = new JObject();
                        bounds.Add("a", new JArray(data.Min(x => x.Latitude), data.Min(x => x.Longitude)));
                        bounds.Add("b", new JArray(data.Max(x => x.Latitude), data.Max(x => x.Longitude)));

                        root.Add("bounds", bounds);
                    }
                    break;
            }
            JArray points = new JArray();
            root.Add("points", points);
            foreach (var item in data)
            {
                JObject pos = new JObject();
                points.Add(pos);

                pos.Add("lon", item.Longitude);
                pos.Add("lat", item.Latitude);
                pos.Add("alt", item.Altitude);

                pos.Add("time", item.SDateTimeTotal);
                pos.Add("type", item.SType);
                pos.Add("offset", item.Offset);
            }

            var content = Properties.Resources.GMap.Replace("\"###\"", root.ToString());
            var fileName = Path.GetTempFileName() + ".html";
            File.WriteAllText(fileName, content);
            Process.Start(fileName);
        }

        private void mnuSaveToFile_Click(object sender, EventArgs e)
        {
            if (dlgSave.ShowDialog(this) == DialogResult.OK)
            {
                using (StreamWriter sw = new StreamWriter(dlgSave.OpenFile()))
                {
                    sw.WriteLine(string.Join("\t", dgTable.Columns.Cast<DataGridViewColumn>().Select(x => x.HeaderText).ToArray()));
                    dgTable.Rows.Cast<DataGridViewRow>().ToList().ForEach(row =>
                    {
                        sw.WriteLine(string.Join("\t", row.Cells.Cast<DataGridViewCell>().Select(x => x.FormattedValue?.ToString()).ToArray()));
                    });
                }
            }
        }
    }
}
