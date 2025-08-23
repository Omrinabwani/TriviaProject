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
using Wpf_learning;

namespace Trivia_Client
{
    /// <summary>
    /// Interaction logic for GameResultsWindow.xaml
    /// </summary>
    public partial class GameResultsWindow : Window
    {
        public GameResultsWindow(string resultsText)
        {
            InitializeComponent();

            if (!string.IsNullOrWhiteSpace(resultsText))
            {
                ShowResults(resultsText);
            }
            else
            {
                ShowWaitingMessage();
            }
        }

        public void ShowResults(string resultsText)
        {
            WaitingTextBlock.Visibility = Visibility.Collapsed;
            ResultsTextBlock.Text = resultsText;
            ResultsTextBlock.Visibility = Visibility.Visible;
            BackToMenuButton.Visibility = Visibility.Visible;
        }

        public void ShowWaitingMessage()
        {
            WaitingTextBlock.Visibility = Visibility.Visible;
            ResultsTextBlock.Visibility = Visibility.Collapsed;
            BackToMenuButton.Visibility = Visibility.Collapsed;
        }

        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            var menu = new MainMenuWindow();
            menu.WindowState = this.WindowState;
            menu.ResizeMode = this.ResizeMode;
            menu.Show();
            this.Close();
        }

    }

}
