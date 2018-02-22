namespace Nefarius.ViGEm.Client.Targets
{
    public abstract class TargetHelperBase
    {
        protected ViGEmTarget Controller;

        protected TargetHelperBase(ViGEmTarget target)
        {
            Controller = target;
        }

        public void Connect()
        {
            Controller.Connect();
        }

        public abstract void SetButtonState(int button, bool state);
        public abstract void SendReport();
    }
}