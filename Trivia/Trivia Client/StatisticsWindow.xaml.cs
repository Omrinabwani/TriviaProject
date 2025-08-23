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
using System.Windows.Shapes;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for StatisticsWindow.xaml
    /// </summary>
    public partial class StatisticsWindow : Window
    {
        public StatisticsWindow()
        {
            InitializeComponent();
        }

        private void PersonalStatsFunction(object sender, RoutedEventArgs e)
        {
            PersonalStatsWindow personalStatsWindow = new PersonalStatsWindow();
            personalStatsWindow.WindowState = this.WindowState;
            personalStatsWindow.ResizeMode = this.ResizeMode;
            personalStatsWindow.Show();
            this.Close();
        }

        private void HighscoresFunction(object sender, RoutedEventArgs e)
        {
            HighscoresWindow highscoresWindow = new HighscoresWindow();
            highscoresWindow.WindowState = this.WindowState;
            highscoresWindow.ResizeMode = this.ResizeMode;
            highscoresWindow.Show();
            this.Close();
        }
        private void GetBackFunction(object sender, RoutedEventArgs e)
        {
            MainMenuWindow mainMenuWindow = new MainMenuWindow();
            mainMenuWindow.WindowState = this.WindowState;
            mainMenuWindow.ResizeMode = this.ResizeMode;
            mainMenuWindow.Show();
            this.Close();
        }
    }
}
