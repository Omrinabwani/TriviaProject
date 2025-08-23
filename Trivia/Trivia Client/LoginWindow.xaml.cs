using System;
using System.Net.Sockets;
using System.Text;
using System.Windows;
using System.IO;
using System.Text.Json;

namespace Wpf_learning
{
    public partial class LoginWindow : Window
    {
        public LoginWindow()
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

        private void LoginFunction(object sender, RoutedEventArgs e)
        {
            const byte LOGIN_REQUEST_CODE = 11;

            try
            {
                ConnectionManager.Connect();
                NetworkStream stream = ConnectionManager.GetStream();

                string username = UsernameInput.Text;
                string password = PasswordInput.Password;
                string loginJson = $"{{\"username\": \"{username}\", \"password\": \"{password}\"}}";
                byte[] request = SerializeRequest(LOGIN_REQUEST_CODE, loginJson);

                stream.Write(request, 0, request.Length);

                int responseCode = stream.ReadByte();

                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);

                string content = Encoding.ASCII.GetString(contentBytes);

                try
                {
                    using JsonDocument doc = JsonDocument.Parse(content);
                    JsonElement root = doc.RootElement;

                    if (root.TryGetProperty("status", out JsonElement statusElement) && statusElement.GetInt32() == 0)
                    {
                        MainMenuWindow mainMenuWindow = new MainMenuWindow();
                        mainMenuWindow.WindowState = this.WindowState;
                        mainMenuWindow.ResizeMode = this.ResizeMode;
                        mainMenuWindow.Show();
                        this.Close();
                    }
                    else if (statusElement.GetInt32() == 1) // 1 is the code for non exiting user
                    {
                        ServerResponse.Text = "Login failed... Please recheck username and password.";
                    }
                    else 
                    {
                        ServerResponse.Text = "Login failed, User already logged in...";
                    }
                }
                catch (JsonException ex)
                {
                    ServerResponse.Text = $"Failed to parse response: {ex.Message}";
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

        // Helper to serialize request packet
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
