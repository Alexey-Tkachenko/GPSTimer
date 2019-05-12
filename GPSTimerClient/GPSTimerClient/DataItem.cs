using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Threading;
using System.Globalization;
using System.Diagnostics;

namespace GPSTimerClient
{
    [DebuggerDisplay("{values}")]
    class DataItem
    {
        public DataItem(string[] keys, string values)
        {
            this.values = values.Replace('\t', ' ');
            string[] parts = values.Split('\t');
            Dictionary<string, string> data = PrepareData(keys, parts);

            ApplyData(data);
        }

        private string values;

        private static Dictionary<string, string> PrepareData(string[] keys, string[] parts)
        {
            Dictionary<string, string> data = new Dictionary<string, string>();
            for (int i = 0, n = Math.Min(keys.Length, parts.Length); i < n; ++i)
            {
                data[keys[i]] = parts[i];
            }

            return data;
        }

        private void ApplyData(Dictionary<string, string> data)
        {
            DataOffset = Field(data, "#Offset", x => int.Parse(x.Trim('#')));

            int year = Field(data, "Y", int.Parse) + 2000;
            int month = Field(data, "M", int.Parse);
            int day = Field(data, "D", int.Parse);
            int hour = Field(data, "h", int.Parse);
            int minute = Field(data, "m", int.Parse);
            int second = Field(data, "s", int.Parse);
            DateTime = new DateTime(year, month, day, hour, minute, second, DateTimeKind.Utc);

            Longitude = OptField(data, "lon", double.Parse);
            Latitude = OptField(data, "lat", double.Parse);
            Altitude = OptField(data, "alt", int.Parse);

            Type = Field(data, "T", int.Parse);

            Offset = Field(data, "Ofs", double.Parse);
        }

        private T? OptField<T>(Dictionary<string, string> data, string key, Func<string, T> parser) where T : struct
        {
            string value;
            if (data.TryGetValue(key, out value))
            {
                return parser(data[key]);
            }
            return default(T);
        }

        private T Field<T>(Dictionary<string, string> data, string key, Func<string, T> parser) where T : struct
        {
            return OptField(data, key, parser).Value;
        }

        [DisplayName("Data offset, bytes")]
        public int DataOffset { get; set; }

        [Browsable(false)]
        public DateTime DateTime { get; set; }

        [DisplayName("Event time, UTC")]
        public string SDateTime => this.DateTime.ToString("dd-MM-yyyy HH:mm:ss");


        [DisplayName("Event time offset, s")]
        public double Offset { get; set; }

        [Browsable(false)]
        public DateTime DateTimeTotal
        {
            get
            {
                var ticks = (Offset + 0.5e-5) * System.TimeSpan.TicksPerSecond;
                return DateTime.AddTicks((long)ticks);
            }
        }

        [DisplayName("Event time (total)")]
        public string SDateTimeTotal => DateTimeTotal.ToString("dd-MM-yyyy HH:mm:ss.fffff");

        [Browsable(false)]
        public TimeSpan? Delta { get; set; }

        [DisplayName("Delta, s")]
        public string SDelta => Delta?.ToString();

        [DisplayName("Longitude, °")]
        public double? Longitude { get; set; }

        [DisplayName("Latitude, °")]
        public double? Latitude { get; set; }

        [DisplayName("Altitude, m")]
        public int? Altitude { get; set; }

        [Browsable(false)]
        public int Type { get; set; }

        [DisplayName("Event type")]
        public string SType
        {
            get
            {
                switch (Type)
                {
                    case 0:
                        return "Falling";
                    case 1:
                        return "Rising";
                    default:
                        return "";
                }
            }
        }
    }
}
