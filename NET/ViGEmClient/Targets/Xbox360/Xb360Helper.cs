using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Nefarius.ViGEm.Client.Targets.Xbox360
{
    public class Xb360Helper : TargetHelperBase
    {
        private readonly Xbox360Report _report = new Xbox360Report();

        private readonly List<Xbox360Buttons> _buttonMappings = new List<Xbox360Buttons>
        {
            Xbox360Buttons.A, Xbox360Buttons.B, Xbox360Buttons.X, Xbox360Buttons.Y,
            Xbox360Buttons.LeftShoulder, Xbox360Buttons.RightShoulder,
            Xbox360Buttons.LeftThumb, Xbox360Buttons.RightThumb
        };

        public Xb360Helper(ViGEmTarget target) : base(target)
        {
        }

        public override void SetButtonState(int button, bool state)
        {
            if (state)
            {
                _report.Buttons |= (ushort)_buttonMappings[button];
            }
            else
            {
                _report.Buttons &= (ushort)~_buttonMappings[button];
            }
        }

        public override void SendReport()
        {
            Controller.SendReport(_report);
        }
    }
}
