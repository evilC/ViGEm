using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nefarius.ViGEm.Client.Targets.DualShock4
{
    public static class DualShock4ExtensionMethods
    {
        /// <summary>
        /// Sets the state of an individual button in the report
        /// </summary>
        /// <param name="report"></param>
        /// <param name="button">The button to set the state of</param>
        /// <param name="state">The state to set. True = pressed, False = released</param>
        public static void SetButtonState(this DualShock4Report report, DualShock4Buttons button, bool state)
        {
            if (state)
            {
                report.Buttons |= (ushort)button;
            }
            else
            {
                report.Buttons &= (ushort)~button;
            }
        }

        /// <summary>
        /// Sets the state of the PS or Touchpad buttons
        /// </summary>
        /// <param name="report"></param>
        /// <param name="button"></param>
        /// <param name="state"></param>
        public static void SetSpecialButtonState(this DualShock4Report report, DualShock4SpecialButtons button, bool state)
        {
            if (state)
            {
                report.SpecialButtons |= (byte)button;
            }
            else
            {
                report.SpecialButtons &= (byte)~button;
            }
        }
    }
}
