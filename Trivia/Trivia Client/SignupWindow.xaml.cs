using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
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
    /// Interaction logic for SignupWindow.xaml
    /// </summary>
    public partial class SignupWindow : Window
    {
        public SignupWindow()
        {
            InitializeComponent();
        }
        private void GetBackFunction(object sender, RoutedEventArgs e)
        {
            MainWindow mainWindow = new MainWindow();
            mainWindow.WindowState = this.WindowState;
            mainWindow.ResizeMode = this.ResizeMode;
            mainWindow.Show();
            this.Close();
        }
        private void SignupFunction(object sender, RoutedEventArgs e)
        {
            const byte SIGNUP_REQUEST_CODE = 12;
            try
            {

                // Get user input
                string username = UsernameInput.Text;
                string password = PasswordInput.Password;
                string email = EmailInput.Text;

                if (string.IsNullOrWhiteSpace(username) ||
                    string.IsNullOrWhiteSpace(password) ||
                    string.IsNullOrWhiteSpace(email))
                {
                    ServerResponse.Text = "All fields are required.";
                    return;
                }

                // Username: only letters and digits, 3-15 characters
                if (username.Length < 3 || username.Length > 15 || !username.All(char.IsLetterOrDigit))
                {
                    ServerResponse.Text = "Username must be 3–15 characters long and contain only letters and digits.";
                    return;
                }

                // Password: at least 8 characters, must contain letters and digits
                if (password.Length < 8 || !password.Any(char.IsLetter) || !password.Any(char.IsDigit))
                {
                    ServerResponse.Text = "Password must be at least 8 characters and contain both letters and digits.";
                    return;
                }

                if (!email.Contains("@"))
                {
                    ServerResponse.Text = "Please enter a valid email address.";
                    return;
                }

                // Build JSON and request
                string loginJson = $"{{\"username\": \"{username}\", \"password\": \"{password}\", \"email\": \"{email}\"}}";
                byte[] request = SerializeRequest(SIGNUP_REQUEST_CODE, loginJson);

                // Connect to server
                ConnectionManager.Connect();
                NetworkStream stream = ConnectionManager.GetStream();

                // Send request
                stream.Write(request, 0, request.Length);

                // Read 1 byte response code
                int responseCode = stream.ReadByte();

                // Read 4-byte response length
                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                // Read response content
                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);

                string content = Encoding.ASCII.GetString(contentBytes);
                if (content.Contains("0"))
                {
                    ServerResponse.Text = content;
                    MainMenuWindow mainMenuWindow = new MainMenuWindow();
                    mainMenuWindow.WindowState = this.WindowState;
                    mainMenuWindow.ResizeMode = this.ResizeMode;
                    mainMenuWindow.Show();
                    this.Close();
                }
                else
                {
                    ServerResponse.Text = "Signup failed... Username or email already taken!";
                }

            }
            catch (SocketException)
            {
                ServerResponse.Text = "Connection error: Unable to reach the server.";
            }
            catch (IOException)
            {
                ServerResponse.Text = "IO error: Connection lost during communication.";
            }
            catch (Exception ex)
            {
                ServerResponse.Text = $"Unexpected error: {ex.Message}";
            }
        }
        private void ReadExact(NetworkStream stream, byte[] buffer, int size)
        {
            int totalRead = 0;
            while (totalRead < size)
            {
                int bytesRead = stream.Read(buffer, totalRead, size - totalRead);
                if (bytesRead == 0)
                    throw new IOException("Disconnected while reading data from stream.");
                totalRead += bytesRead;
            }
        }
        private byte[] SerializeRequest(byte requestCode, string json)
{
    byte[] jsonBytes = Encoding.ASCII.GetBytes(json);
    byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

    if (BitConverter.IsLittleEndian)
        Array.Reverse(lengthBytes); // Convert to big-endian

    byte[] request = new byte[1 + 4 + jsonBytes.Length];
    request[0] = requestCode;
    Array.Copy(lengthBytes, 0, request, 1, 4);
    Array.Copy(jsonBytes, 0, request, 5, jsonBytes.Length);
    return request;
        }
    }
}

