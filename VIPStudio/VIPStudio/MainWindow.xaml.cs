using Syncfusion.Windows.Tools.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace VIPStudio
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : RibbonWindow
    {
        public MainWindow()
        {
            InitializeComponent();

            //CheckBoxData.SetValue(CheckBoxData.IsChecked, true);
            CheckBoxData.IsChecked = true;
            CheckBoxView.IsChecked = true;
            CheckBoxRconsole.IsChecked = true;
            CheckBoxScript.IsChecked = true;
            CheckBoxPlot.IsChecked = true;
            CheckBoxDashboard.IsChecked = true;

            Ribbon1.BackStageButton.Visibility = Visibility.Collapsed;
        }

        public DockState IsWindowViewVisible { get; set; }

        public DockState IsWindowRConsoleVisible { get; set; }

        public DockState IsWindowScriptVisible { get; set; }

        public DockState IsWindowPlotVisible { get; set; }

        public DockState IsWindowDashboardVisible { get; set; }


        private void CheckBoxView_Click(object sender, RoutedEventArgs e)
        {
            DockState state = CheckBoxView.IsChecked.Value ? DockState.Dock : DockState.Hidden;
            DockingManager.SetState(WindowView, state);
        }

        private void DockingManager1_DockStateChanged(FrameworkElement sender, DockStateEventArgs e)
        {
            if (e.NewState == DockState.Hidden)
            {
                if (DockingManager.GetState(WindowData) == DockState.Hidden)
                    CheckBoxData.IsChecked = false;

                if (DockingManager.GetState(WindowView) == DockState.Hidden)
                    CheckBoxView.IsChecked = false;

                if (DockingManager.GetState(WindowRConsole) == DockState.Hidden)
                    CheckBoxRconsole.IsChecked = false;

                if (DockingManager.GetState(WindowScript) == DockState.Hidden)
                    CheckBoxScript.IsChecked = false;

                if (DockingManager.GetState(WindowPlot) == DockState.Hidden)
                    CheckBoxPlot.IsChecked = false;

                if (DockingManager.GetState(WindowDashboard) == DockState.Hidden)
                    CheckBoxDashboard.IsChecked = false;
            }
        }

        private void CheckBoxData_Click(object sender, RoutedEventArgs e)
        {
            DockState state = CheckBoxData.IsChecked.Value ? DockState.Dock : DockState.Hidden;
            DockingManager.SetState(WindowData, state);
        }

        private void CheckBoxRconsole_Click(object sender, RoutedEventArgs e)
        {
            DockState state = CheckBoxRconsole.IsChecked.Value ? DockState.Dock : DockState.Hidden;
            DockingManager.SetState(WindowRConsole, state);
        }

        private void CheckBoxScript_Click(object sender, RoutedEventArgs e)
        {
            DockState state = CheckBoxScript.IsChecked.Value ? DockState.Dock : DockState.Hidden;
            DockingManager.SetState(WindowScript, state);
        }

        private void CheckBoxPlot_Click(object sender, RoutedEventArgs e)
        {
            DockState state = CheckBoxPlot.IsChecked.Value ? DockState.Dock : DockState.Hidden;
            DockingManager.SetState(WindowPlot, state);
        }

        private void CheckBoxDashboard_Click(object sender, RoutedEventArgs e)
        {
            DockState state = CheckBoxDashboard.IsChecked.Value ? DockState.Dock : DockState.Hidden;
            DockingManager.SetState(WindowDashboard, state);
        }
    }
}
