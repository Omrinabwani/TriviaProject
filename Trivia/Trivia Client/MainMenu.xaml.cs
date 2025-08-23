using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Reflection;
using System.Reflection.Metadata;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Resources;
using System.Windows.Shapes;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenuWindow : Window
    {
        public MainMenuWindow()
        {
            InitializeComponent();
        }
        private void JoinRoomFunction(object sender, RoutedEventArgs e)
        {
            JoinRoomWindow joinRoomWindow = new JoinRoomWindow();
            joinRoomWindow.WindowState = this.WindowState;
            joinRoomWindow.ResizeMode = this.ResizeMode;
            joinRoomWindow.Show();
            this.Close();
        }
        private void CreateRoomFunction(object sender, RoutedEventArgs e)
        {
            CreateRoomWindow createRoomWindow = new CreateRoomWindow();
            createRoomWindow.WindowState = this.WindowState;
            createRoomWindow.ResizeMode = this.ResizeMode;
            createRoomWindow.Show();
            this.Hide();
        }
        private void ExitFunction(object sender, RoutedEventArgs e)
        {
            const byte LOGOUT_REQUEST_CODE = 20;

            try
            {
                var stream = ConnectionManager.GetStream();
                if (stream == null || !stream.CanWrite || !stream.CanRead)
                {
                    MessageBox.Show("Connection error: Stream is invalid.");
                    return;
                }
                byte[] getRoomsRequest = new byte[5];
                getRoomsRequest[0] = LOGOUT_REQUEST_CODE; 

                // Set length to 0 (4 bytes)
                getRoomsRequest[1] = 0;
                getRoomsRequest[2] = 0;
                getRoomsRequest[3] = 0;
                getRoomsRequest[4] = 0;

                stream.Write(getRoomsRequest, 0, getRoomsRequest.Length);

                int responseCode = stream.ReadByte();

                byte[] lengthBytes = new byte[4];

                ReadExact(stream, lengthBytes, 4);

                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);
                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);

                string json = Encoding.UTF8.GetString(contentBytes);
                try
                {
                    using JsonDocument doc = JsonDocument.Parse(json);
                    JsonElement root = doc.RootElement;

                    if (root.TryGetProperty("status", out JsonElement statusElement) && statusElement.GetInt32() == 0)
                    {
                        MainWindow mainWindow = new MainWindow();
                        mainWindow.WindowState = this.WindowState;
                        mainWindow.ResizeMode = this.ResizeMode;
                        mainWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        ServerResponse.Text = "Logout failed... Please try again.";
                    }
                }
                catch (JsonException ex)
                {
                    ServerResponse.Text = $"Failed to parse response: {ex.Message}";
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading rooms: {ex.Message}");
            }
        }

        private void StatisticsFunction(object sender, RoutedEventArgs e)
        {
            StatisticsWindow statisticsWindow = new StatisticsWindow();
            statisticsWindow.WindowState = this.WindowState;
            statisticsWindow.ResizeMode = this.ResizeMode;
            statisticsWindow.Show();
            this.Close();

        }
        private void ReadExact(NetworkStream stream, byte[] buffer, int size)
        {
            int totalRead = 0;
            while (totalRead < size)
            {
                int bytesRead = stream.Read(buffer, totalRead, size - totalRead);
                if (bytesRead == 0)
                    throw new IOException("Disconnected while reading from stream.");
                totalRead += bytesRead;
            }
        }

    }
}
