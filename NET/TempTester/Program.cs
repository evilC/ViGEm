using Nefarius.ViGEm.Client;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Nefarius.ViGEm.Client.Targets;
using Nefarius.ViGEm.Client.Targets.DualShock4;
using Nefarius.ViGEm.Client.Targets.Xbox360;

namespace TempTester
{
    class Program
    {
        static void Main(string[] args)
        {
            var client = new ViGEmClient();

            var helpers = new Dictionary<string, TargetHelperBase>()
            {
                {"xb360", new Xb360Helper(new Xbox360Controller(client)) },
                { "ds4", new DualShock4Helper(new DualShock4Controller(client)) }
            };

            foreach (var helper in helpers)
            {
                helper.Value.Connect();
                Console.WriteLine($"Initialized controller {helper.Key}");
                Console.ReadLine();
                helper.Value.SetButtonState(0, true);
                helper.Value.SendReport();
                Console.ReadLine();
                helper.Value.SetButtonState(0, false);
                helper.Value.SendReport();
                Console.ReadLine();
            }

        }
    }
}
