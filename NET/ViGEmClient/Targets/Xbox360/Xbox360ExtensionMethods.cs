using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Nefarius.ViGEm.Client.Targets.Xbox360
{
    public static class Xbox360ExtensionMethods
    {
        /// <summary>
        /// Sets the state of an individual button in the report
        /// </summary>
        /// <param name="report"></param>
        /// <param name="button">The button to set the state of</param>
        /// <param name="state">The state to set. True = pressed, False = released</param>
        public static void SetButtonState(this Xbox360Report report, Xbox360Buttons button, bool state)
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
    }
}
