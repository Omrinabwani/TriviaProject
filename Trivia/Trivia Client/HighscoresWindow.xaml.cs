using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
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
using System.Windows.Shapes;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for HighscoresWindow.xaml
    /// </summary>
    public partial class HighscoresWindow : Window
    {
        private void GetBackFunction(object sender, RoutedEventArgs e)
        {
            StatisticsWindow statisticsWindow = new StatisticsWindow();
            statisticsWindow.WindowState = this.WindowState;
            statisticsWindow.ResizeMode = this.ResizeMode;
            statisticsWindow.Show();
            this.Hide();
        }
        public HighscoresWindow()
        {

            InitializeComponent();
            try
            {
                NetworkStream stream = ConnectionManager.GetStream();

                byte GET_HIGHSCORES = 23;
                string json = "{}";
                byte[] request = SerializeRequest(GET_HIGHSCORES, json);
                stream.Write(request, 0, request.Length);

                int responseCode = stream.ReadByte();

                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                // Read JSON content
                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);

                string jsonResponse = Encoding.ASCII.GetString(contentBytes);

                using JsonDocument doc = JsonDocument.Parse(jsonResponse);
                JsonElement root = doc.RootElement;

                if (root.TryGetProperty("statistics", out JsonElement highscores) && highscores.ValueKind == JsonValueKind.Array)
                {
                    int length = highscores.GetArrayLength();
                    for (int i = 0; i < length; i++)
                    {
                        JsonElement entry = highscores[i];
                        if (entry.ValueKind == JsonValueKind.Array && entry.GetArrayLength() == 2)
                        {
                            string username = entry[0].GetString();
                            int score = entry[1].GetInt32();

                            HighscoresPanel.Children.Add(CreateScoreText($"🏅 {username} — {score} pts"));
                        }
                    }
                }
                else
                {
                    HighscoresPanel.Children.Add(CreateScoreText("Failed to extract highscores."));
                }
            }
            catch (Exception ex)
            {
                HighscoresPanel.Children.Add(CreateScoreText("Error: " + ex.Message));
            }

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
        private TextBlock CreateScoreText(string text)
        {
            return new TextBlock
            {
                Text = text,
                FontSize = 18,
                Margin = new Thickness(5),
                Foreground = Brushes.White,
                FontWeight = FontWeights.Bold
            };
        }

        private byte[] SerializeRequest(byte requestCode, string json)
        {
            byte[] jsonBytes = Encoding.ASCII.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes);

            byte[] request = new byte[1 + 4 + jsonBytes.Length];
            request[0] = requestCode;
            Array.Copy(lengthBytes, 0, request, 1, 4);
            Array.Copy(jsonBytes, 0, request, 5, jsonBytes.Length);
            return request;
        }


    }
}
