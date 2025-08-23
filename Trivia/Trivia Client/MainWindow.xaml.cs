using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net.Sockets;
using System.IO;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

        }

        private void Login(object sender, RoutedEventArgs e)
        {
            LoginWindow loginWindow = new LoginWindow();
            loginWindow.WindowState = this.WindowState; 
            loginWindow.ResizeMode = this.ResizeMode;
            loginWindow.Show();
            this.Close();
        }
        private void Signup(object sender, RoutedEventArgs e)
        {
            SignupWindow signupWindow = new SignupWindow();
            signupWindow.WindowState = this.WindowState;
            signupWindow.ResizeMode = this.ResizeMode;
            signupWindow.Show();
            this.Close();
        }

    }
}