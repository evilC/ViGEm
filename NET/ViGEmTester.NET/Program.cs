using Nefarius.ViGEm.Client;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Threading;
using System.Threading.Tasks;
using Nefarius.ViGEm.Client.Targets;
using Nefarius.ViGEm.Client.Targets.DualShock4;
using Nefarius.ViGEm.Client.Targets.Xbox360;

namespace ViGEmTester.NET
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var client = new ViGEmClient();

#if X360
            var x360 = new Xbox360Controller(client);

            x360.FeedbackReceived +=
                (sender, eventArgs) => Console.WriteLine(
                    $"LM: {eventArgs.LargeMotor}, " +
                    $"SM: {eventArgs.SmallMotor}, " +
                    $"LED: {eventArgs.LedNumber}");

            x360.Connect();

            var report = new Xbox360Report();
            report.SetButtons(Xbox360Buttons.A, Xbox360Buttons.B);
            report.SetAxis(Xbox360Axes.LeftTrigger, 0xFF);
            report.SetAxis(Xbox360Axes.RightTrigger, 0xFF);

            x360.SendReport(report);
#endif

            Console.Title = "ViGEm Benchmark - CLOSE THIS WINDOW TO STOP!";
            Console.WriteLine(Console.Title);

            var controllers = new ObservableCollection<DualShock4Controller>();

            controllers.CollectionChanged += (sender, eventArgs) =>
            {
                switch (eventArgs.Action)
                {
                    case NotifyCollectionChangedAction.Add:
                        Console.WriteLine("Added Controller");
                        break;
                    case NotifyCollectionChangedAction.Remove:
                        Console.WriteLine("Removed Controller");

                        if (((ObservableCollection<DualShock4Controller>) sender).Count == 0)
                        {
                            Console.WriteLine(" > Finished! Press any key to exit!");
                        }

                        break;
                }
            };

            for (int i = 0; i < 500; i++)
            {
                Console.WriteLine($" > Spawning device {i}");

                Task.Run(() =>
                {
                    var ds4 = new DualShock4Controller(client);

                    try
                    {
                        lock (controllers)
                            controllers.Add(ds4);

                        ds4.Connect();

                        var report = new DualShock4Report();
                        report.SetButtons(DualShock4Buttons.Cross);
                        ds4.SendReport(report);

                        Thread.Sleep(1000);
                        ds4.Disconnect();
                    }
                    finally
                    {
                        lock (controllers)
                            controllers.Remove(ds4);
                    }
                });

                Thread.Sleep(20);
            }

            /*
            var x360 = new Xbox360Controller();
            x360.FeedbackReceived +=
                (sender, eventArgs) => Console.WriteLine(
                    $"LM: {eventArgs.LargeMotor}, " +
                    $"SM: {eventArgs.SmallMotor}, " +
                    $"LED: {eventArgs.LedNumber}");
            x360.PlugIn();

            var report = new Xbox360Report();
            report.SetButtons(Xbox360Buttons.A, Xbox360Buttons.B);
            report.SetAxis(Xbox360Axes.LeftTrigger, 0xFF);
            report.SetAxis(Xbox360Axes.RightTrigger, 0xFF);

            x360.SendReport(report);

            Console.ReadKey();

            var ds4 = new DualShock4Controller();
            ds4.FeedbackReceived +=
                (sender, eventArgs) => Console.WriteLine(
                    $"LM: {eventArgs.LargeMotor}, " +
                    $"SM: {eventArgs.SmallMotor}, ");
            ds4.PlugIn();
            */

            Console.ReadKey();
        }
    }
}