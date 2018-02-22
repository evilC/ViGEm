using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nefarius.ViGEm.Client.Targets.DualShock4
{
    public class DualShock4Helper : TargetHelperBase
    {
        private readonly DualShock4Report _report = new DualShock4Report();

        private readonly List<DualShock4Buttons> _buttonMappings = new List<DualShock4Buttons>
        {
            DualShock4Buttons.Cross, DualShock4Buttons.Circle, DualShock4Buttons.Share, DualShock4Buttons.Triangle,
            DualShock4Buttons.ShoulderLeft, DualShock4Buttons.ShoulderRight,
            DualShock4Buttons.ThumbLeft, DualShock4Buttons.ThumbRight
        };

        public DualShock4Helper(ViGEmTarget target) : base(target)
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
